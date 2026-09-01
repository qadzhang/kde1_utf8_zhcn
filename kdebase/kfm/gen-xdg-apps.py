#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#
#   Written with GLM-5.3 (Z.ai) for the KDE1 Revival Project
#
# ┌─ What : 把现代 XDG 桌面条目（/usr/share/applications/*.desktop）转换为
# │        KDE1 的 K 菜单子树（~/.kde/share/applnk/Modern/ 下的 .kdelnk），
# │        K 菜单出现「现代应用」顶层分类，随系统安装/卸载自动增减
# │  Why  : KDE1 1999 年的菜单只认 applnk 树，现代软件（XFCE/GNOME 生态、
# │        apt 装的一切 GUI 程序）注册的是 desktop 文件——两类体系互不相
# │        认，用户在 KDE1 里"找不到系统里已装的软件"。参考现代系统的
# │        Debian menu / XDG menu 聚合思想，在 KDE1 侧做转换适配
# │        （章程 §6.7：KDE1 主动适配现代生态，不改现代软件）
# │  Who  : startkde 在会话启动时调用（python3 缺失则静默跳过）
# │  When : 每次登录一次（幂等：先清空 Modern/ 再重建，反映装卸变化）
# │  Where: 输出 ~/.kde/share/applnk/Modern/；图标复制到 ~/.kde/share/icons/
# │  How  : 伪代码——
# │        1. 收集 desktop：/usr/share/applications 与 ~/.local/share/applications
# │        2. 过滤：NoDisplay/Hidden、OnlyShowIn 指定了非 KDE 桌面、
# │           无 Exec 的条目跳过
# │        3. 字段转换：
# │             Name ← Name[zh_CN]（缺则 Name）；Icon ← 主题图标名在
# │             hicolor 树中解析为实际文件并复制（KIconLoader 只认
# │             $KDEDIR 图标目录里的文件名）；Exec 清掉 KDE1 不认识的
# │             desktop 占位符（%d %D %n %N %k），保留 %f %F %u %U %c %i
# │             （KDE1 kbind 原生支持）；Terminal=true 包装 xterm -e
# │        4. Categories 主分类映射到子目录（互联网/办公/图形/音频和视频/
# │           游戏/开发/系统/实用工具…），无分类者进「其他」
# │        5. 每个子目录写 .directory（中文名双键），每条目写 .kdelnk
# │           （Name[zh_CN.UTF-8]/Name[zh_CN] 双键，K 菜单显示中文）

import os
import re
import shutil
import sys

APPS_DIRS = ["/usr/share/applications",
             "/usr/local/share/applications",
             os.path.expanduser("~/.local/share/applications")]
OUT_BASE = os.path.expanduser("~/.kde/share/applnk/Modern")
ICON_OUT = os.path.expanduser("~/.kde/share/icons")

# Categories 主分类 → 子目录（键为目录名；显示名在 .directory 里中文）
CATEGORY_MAP = [
    (("AudioVideo", "Audio", "Video"), "AudioVideo"),
    (("Network", "Communication"), "Network"),
    (("Office", "WordProcessor", "Spreadsheet", "Presentation",
      "Calendar", "TextEditor"), "Office"),
    (("Graphics", "Photography", "2DGraphics"), "Graphics"),
    (("Game",), "Games"),
    (("Development", "IDE", "Building", "Debugger", "RevisionControl"),
     "Development"),
    (("Science", "Education"), "Science"),
    (("Utility", "TextTools", "Archiving", "FileManager"), "Utilities"),
    (("System", "Settings", "Monitor", "TerminalEmulator"), "System"),
]
CATEGORY_NAMES = {
    "AudioVideo":  ("音频和视频", "Audio & Video"),
    "Network":     ("互联网", "Internet"),
    "Office":      ("办公", "Office"),
    "Graphics":    ("图形", "Graphics"),
    "Games":       ("游戏", "Games"),
    "Development": ("开发", "Development"),
    "Science":     ("教育与科学", "Science & Education"),
    "Utilities":   ("实用工具", "Utilities"),
    "System":      ("系统工具", "System"),
    "Other":       ("其他", "Other"),
}

ICON_ROOTS = ["/usr/share/icons", "/usr/local/share/icons",
              os.path.expanduser("~/.icons"), "/usr/share/pixmaps"]
ICON_GROUPS = ("apps", "status", "places", "devices", "actions")


def find_icon(name):
    """按 XDG 图标主题规范（简化版）把图标名解析为实际文件路径。
    优先取大尺寸 png（平滑缩放交给显示端），svg 无法被 TQt3 加载故跳过。"""
    if not name:
        return None
    if name.startswith("/") and os.path.isfile(name):
        return name
    for root in ICON_ROOTS:
        if root.endswith("pixmaps"):
            for ext in (".png", ".xpm"):
                p = os.path.join(root, name + ext)
                if os.path.isfile(p):
                    return p
            continue
        themedir = os.path.join(root, "hicolor")
        if not os.path.isdir(themedir):
            continue
        best = None          # (尺寸, 路径)，取最大尺寸
        for size in os.listdir(themedir):
            m = re.match(r"^(\d+)x\1$", size)
            if not m:
                continue
            px = int(m.group(1))
            for grp in ICON_GROUPS:
                for ext in (".png", ".xpm"):
                    p = os.path.join(themedir, size, grp, name + ext)
                    if os.path.isfile(p) and (best is None or px > best[0]):
                        best = (px, p)
        if best:
            return best[1]
    return None


def parse_desktop(path):
    """解析 desktop 文件的主组字段（INI 简化版：只读 [Desktop Entry] 组）。"""
    fields = {}
    in_main = False
    try:
        with open(path, encoding="utf-8", errors="replace") as f:
            for line in f:
                line = line.strip()
                if line.startswith("["):
                    in_main = (line == "[Desktop Entry]")
                    continue
                if not in_main or "=" not in line or line.startswith("#"):
                    continue
                k, v = line.split("=", 1)
                fields[k.strip()] = v.strip()
    except OSError:
        return None
    return fields


def clean_exec(exec_str):
    """去掉 KDE1 kbind 不认识的 desktop 占位符；%f %F %u %U %c %i 与
    KDE1 语义兼容故保留（kfm 双击打开/菜单启动的替换逻辑原生支持）。"""
    for tok in ("%d", "%D", "%n", "%N", "%k"):
        exec_str = exec_str.replace(" " + tok + " ", " ")
        exec_str = re.sub(re.escape(tok) + r"\s*$", "", exec_str)
    return exec_str.strip()


def escape_ini(s):
    """kdelnk 值里的换行/分号无碍，但保留前后空白剥离。"""
    return s.replace("\n", " ").strip()


def main():
    # 幂等重建：先清空上次的生成树
    if os.path.isdir(OUT_BASE):
        shutil.rmtree(OUT_BASE, ignore_errors=True)
    os.makedirs(ICON_OUT, exist_ok=True)

    made_dirs = set()
    count = 0
    for apps_dir in APPS_DIRS:
        if not os.path.isdir(apps_dir):
            continue
        for fn in sorted(os.listdir(apps_dir)):
            if not fn.endswith(".desktop"):
                continue
            f = parse_desktop(os.path.join(apps_dir, fn))
            if f is None:
                continue
            if f.get("NoDisplay", "").lower() == "true":
                continue
            if f.get("Hidden", "").lower() == "true":
                continue
            if f.get("Type", "Application") != "Application":
                continue
            only = f.get("OnlyShowIn", "")
            if only and "KDE" not in only.split(";"):
                continue            # 只面向特定桌面（GNOME-;XFCE; 等）的跳过
            exe = f.get("Exec", "").strip()
            if not exe:
                continue
            exe = clean_exec(exe)
            if f.get("Terminal", "").lower() == "true":
                exe = "xterm -e " + exe

            # 主分类 → 子目录
            cats = set(f.get("Categories", "").split(";"))
            subdir = "Other"
            for keys, target in CATEGORY_MAP:
                if cats & set(keys):
                    subdir = target
                    break

            # 名称：zh_CN 优先
            name = f.get("Name[zh_CN]") or f.get("Name") or fn[:-8]
            comment = (f.get("Comment[zh_CN]") or f.get("Comment")
                       or f.get("GenericName[zh_CN]") or f.get("GenericName") or "")

            # 图标：解析→复制到 KDE1 图标目录（防重名加条目名前缀）
            icon_field = ""
            icon_src = find_icon(f.get("Icon", ""))
            if icon_src:
                base = fn[:-8].replace("/", "-") + "-" + os.path.basename(icon_src)
                dst = os.path.join(ICON_OUT, base)
                try:
                    shutil.copyfile(icon_src, dst)
                    icon_field = base
                except OSError:
                    pass

            outdir = os.path.join(OUT_BASE, subdir)
            os.makedirs(outdir, exist_ok=True)
            made_dirs.add(subdir)

            kdelnk = os.path.join(outdir, fn[:-8] + ".kdelnk")
            with open(kdelnk, "w", encoding="utf-8") as out:
                out.write("# KDE Config File\n[KDE Desktop Entry]\n")
                out.write("# Generated by gen-xdg-apps.py (KDE1 Revival, 2026)\n")
                # Type=Application 必写：kfm 的 KDELnkMimeType::run() 按 Type
                # 分派，缺失时走"config file has no Type"错误框而非启动程序
                # （2026-09-01 修复：此前生成的条目点击只弹错误框）
                out.write("Type=Application\n")
                out.write("Exec=%s\n" % escape_ini(exe))
                if icon_field:
                    out.write("Icon=%s\n" % icon_field)
                if comment:
                    out.write("Comment=%s\n" % escape_ini(comment))
                out.write("Terminal=0\n")
                out.write("Name=%s\n" % escape_ini(name))
                out.write("Name[zh_CN.UTF-8]=%s\n" % escape_ini(name))
                out.write("Name[zh_CN]=%s\n" % escape_ini(name))
            count += 1

    # 各子目录与根的 .directory（K 菜单分类显示名，中英双键）
    for d in made_dirs | {"ROOT"}:
        zh, en = CATEGORY_NAMES.get(
            d, CATEGORY_NAMES["Other"]) if d != "ROOT" else ("现代应用",
                                                             "Modern Apps")
        outdir = OUT_BASE if d == "ROOT" else os.path.join(OUT_BASE, d)
        with open(os.path.join(outdir, ".directory"), "w",
                  encoding="utf-8") as out:
            out.write("# KDE Config File\n[KDE Desktop Entry]\n")
            out.write("Name=%s\n" % en)
            out.write("Name[zh_CN.UTF-8]=%s\n" % zh)
            out.write("Name[zh_CN]=%s\n" % zh)

    print("gen-xdg-apps: %d 个现代应用已发布到 K 菜单「现代应用」" % count,
          file=sys.stderr)


if __name__ == "__main__":
    main()
