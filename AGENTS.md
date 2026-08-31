# AGENTS.md — 本项目工作章程

> 本文件是所有在本仓库工作的 AI 代理与人类协作者的入口说明。开始任何工作前必须先读完本文件。

## 1. 项目使命

把 **KDE 1.1.2**（KDE 1 系列最终版，1999-09-13 发布）在 **Debian 12 (bookworm)** 上跑起来，并通过**路线乙：UTF-8 现代化补丁**实现完整的简体中文桌面体验。

**总体目标（验收标准，八条缺一不可）：**

1. **流畅稳定运行**：宿主系统保持 UTF-8 不变，zh_CN 环境的 KDE 1 桌面必须流畅、稳定运行——不崩溃、不乱码、不卡顿。
2. **现代输入法稳定接入**：中文输入按现代输入法框架运行——**fcitx5 与 fcitx 两代框架都必须稳定、完美运行**（经 XIM 通道接入，缺一不可）。
3. **浏览器内核跟进**：KDE 1 自带浏览器（kfm）的 HTML 内核过于古老，以 **WebKit2GTK 为新内核**（Debian 12 官方包，完整现代 CSS / JIT JS / HTTPS），与 1999 年原始渲染器构成**新旧双内核可切换**（新内核看现代网页，旧内核保持历史原貌）；CEF（Chromium）为备选路线；Firefox/Gecko 无维护中的嵌入接口，已排除。
4. **全面中文化与英文对齐**：菜单、帮助、提示等**所有**界面内容全面中文化，覆盖率与英文原文对齐——不是"部分翻译"，而是与英文功能完全对等。**kvt 终端内的中文内容（文件名、命令输出等）显示正常；与系统现代应用（浏览器、现代 Qt/GTK 程序）之间可双向复制粘贴中文文本**。**翻译质量硬性要求**：禁止闭门自造译法——每条新增/修订翻译必须 ① 上网检索通行、准确的中文译法；② **对齐当今 KDE 6（Plasma 6 / KDE Gear）的 zh_CN 官方翻译术语**，保证用词与现代 KDE 一致（用户从 KDE 6 切换到本项目不应感到术语跳变）；③ 1999 年自带的 GB2312 旧翻译仅作参考底稿，其术语与现代 KDE 冲突时以现代术语为准。
5. **Debian 软件包交付（唯一正式安装方式）**：正式安装**只经 deb 包**（dpkg / apt）——源码包（sdeb：.dsc + .orig.tar.* + .debian.tar.*）与二进制 .deb 包都要产出，且**分目录存放**（`dist/src/` 与 `dist/deb/`，不得混放）。**.deb 须开箱即用、零手工配置**，包内容至少含：① `/usr/kde1` 全树（各模块经 DESTDIR 暂存收编）；② `/usr/share/xsessions/kde1.desktop` 会话入口；③ **startkde 包装启动脚本**（内部完成 PATH / LD_LIBRARY_PATH / KDEDIR 环境设置后 exec 真正的 startkde——用户不需要写 `.xinitrc`、不需要 export 任何变量）；④ TQt3 运行库与工具（libtqt-mt、tqmoc 等）；⑤ 运行时依赖声明（libxft / fontconfig / libwebkit2gtk、中文字体等）。安装后 lightdm 会话菜单即出现 KDE 1。**安装/卸载走 dpkg 标准生命周期**：`apt install` 安装、`apt remove` 干净移除、不留任何游离于包管理之外的文件；按模块拆分：`kde1-core`（tqt3+kdelibs+kdebase，桌面必需）、`kde1-games`/`kde1-graphics`/`kde1-utils`/`kde1-network`/`kde1-toys` 五个可选应用包、`kde1` 元包（会话入口 + startkde 包装，Recommends 各可选包但可单独不装）。`build.sh` 仅负责开发期构建与 `./staging` 暂存运行验证，**不得 sudo 直接安装到系统**。
6. **现代系统集成**：图形登录由**系统显示管理器管理**（本机为 lightdm，方式与 XFCE 等现代桌面一致）——KDE 1 会话出现在 lightdm 会话菜单、选择后直接进入桌面，**不依赖手工 `startx`**；桌面事件音效**接入现代音频栈**（PipeWire/PulseAudio），1999 年的 `/dev/dsp`（OSS）失效路径必须解决（padsp/osspd 兼容层或原生补丁），提示音正常发声；**XDG 用户目录兼容**——桌面/模板等用户目录与宿主现代桌面共用同一套（经 `~/.config/user-dirs.dirs`，中文系统即 `~/桌面`、`~/模板`），回收站对齐 freedesktop 标准（`~/.local/share/Trash`），不得另建 1999 年的 `~/Desktop` 体系。
7. **打印接入 CUPS**：KDE1 的打印（Qt1 QPrinter 经 `lpr` 命令提交）必须正常接入现代 CUPS 打印体系——经 `cups-bsd` 兼容层（提供 `/usr/bin/lpr`）零代码打通，deb 依赖声明完整；实测打印任务能进入 CUPS 队列并产出。
8. **全面 UTF-8 融合**：多字节感知**不止于编辑部件，而是所有模块全面的接入、融合**——Qt 内核与各 KDE 模块中一切按字节处理文本的路径（光标移动、退格/删除、选区、字符串截断/省略、字符计数与长度限制、宽度测量与对齐、表格/列表列宽、文件名与标题显示、查找替换等），在中文文本上**一律按 UTF-8 字符边界精确操作**：不切断字符、不产生错位、计数与宽度准确。验收口径：KDE 1 作为整体**完美运行于 UTF-8 系统**，用户在任何模块、任何文本交互处都感知不到"这是单字节时代的程序"（该目标 1999 年 CLE 补丁未达成，本项目必须达成）。

路线乙（2026-08 底座已换 TQt3）：宿主系统保持 UTF-8 不变；**GUI 底座为 TQt3 r14.1.6**（Trinity 维护的 Qt3 分支，GPLv2——彻底解决 Qt1 专有许可证不可修改分发的问题）。TQt3 原生 Unicode（TQString 内部 UTF-16）+ fontconfig/Xft 抗锯齿渲染 + 原生 XIM 支持，与当今 KDE/XFCE 同一套字体体系。KDE1 六模块经 `port/` 迁移脚手架（strangler fig 模式：编译期 -include 注入 Q→TQ 名字映射，逐模块显式 TQ 化后摘除）完成编译迁移；`TQTextCodec::setCodecForCStrings(UTF-8)` 全局开关统一字符串编码语义。po 翻译文件 UTF-8。目标效果：菜单、按钮、对话框正常显示中文，系统其余部分零改动。

## 2. 代码来历（详见 README.md「代码来历」）

- 上游源头：KDE 团队 1999 年发布的 KDE 1.1.2；GUI 底座为 TQt3（Trinity Desktop 对 Qt 3.3.8 的维护性分支，GPLv2）。
- 直接来源：KDE1 源码来自 GitHub `NishiOwO/kde1` 元仓库；TQt3 来自 Trinity 官方 Gitea（地址与版本见 README「底座」节）。
- 本仓库于 2026-08 移除原 git 历史与子模块结构整合为普通目录树；同年 8 月底完成 Qt1→TQt3 底座更换（历史已重写清除 qt1）。

## 3. 目录结构

| 目录 | 内容 |
|---|---|
| `tqt3/` | TQt3 r14.1.6 pristine 快照（Trinity 的 Qt3 分支，GPLv2；**绝不直接修改**，修改一律走 `tqt3-patches/`） |
| `tqt3-patches/` | 对 tqt3 的补丁（构建时打入 `tqt3-build/`；当前 2 个：qstring 的 TQString==const char* 遵循 codecForCStrings 语义修复 UTF-8 中文比较、启用内建 GIF 编解码；数量与清单以 `tqt3-patches/README.md` 为准） |
| `port/` | KDE1→TQt3 迁移脚手架（strangler fig 模式）：`q1compat.h`（Q→TQ 映射 + tq 函数族）、331 个转发头、两个生成器脚本；全部模块显式 TQ 化后整体拆除 |
| `kdelibs/` | KDE 1.1.2 基础库（kdecore、kdeui 等） |
| `kdebase/` | KDE 1.1.2 基础应用（kwm、kpanel、kfm、kvt、kdm 等） |
| `kdegames/` `kdegraphics/` `kdeutils/` `kdenetwork/` `kdetoys/` | 其余应用模块（kdegraphics 为 2026-08 新纳入：ksnapshot/kview/kpaint/kiconedit/kfract/kdvi/kfax/kghostview） |
| `build.sh` | 一键构建脚本（按 tqt3 → kdelibs → kdebase → 各应用模块 顺序） |
| `clean.sh` | 清理脚本（删除各模块 `build/` 目录与 `staging/` 暂存区；模块清单必须与 `build.sh` 保持同步） |
| `sandbox.sh` | 开发期后台桌面沙箱（Xvfb `:99` + fcitx5 私有总线 + x11vnc 仅 127.0.0.1:5901），Remmina 连入即可体验 staging 版桌面，不扰宿主会话与 lightdm |
| `staging/` | 开发期安装暂存区（`build.sh` 以 DESTDIR 重定向 `make install` 所得，布局对应最终 `/usr/kde1`；由 `clean.sh` 清理） |
| `dist/src/` | 打包产物：Debian 源码包 sdeb（`.dsc` / `.orig.tar.*` / `.debian.tar.*`，由打包脚本生成，分目录存放见 §1 目标 5） |
| `dist/deb/` | 打包产物：二进制 `.deb` 包（由打包脚本生成） |
| `README.md` | 项目说明（来历、版权、构建、中文支持方案） |
| `CHANGELOG.md` | **全项目唯一的修改历史记录处** |

## 4. 构建与运行（Debian 12）

```bash
# 依赖——构建必需（libglu 为 tqt3 OpenGL 扩展；
# libssl 为 kdebase 所需；pkg-config 为各模块 CMake 探测所需；
# libxft/libfontconfig/libfreetype 为路线乙 fontconfig+Xft 字体栈；
# libwebkit2gtk 为 kfm 新内核）
sudo apt install build-essential cmake git pkg-config \
     libx11-dev libxext-dev libxmu-dev libxpm-dev libjpeg-dev libpng-dev \
     zlib1g-dev libtiff-dev libssl-dev libglu1-mesa-dev \
     libxft-dev libfontconfig1-dev libfreetype-dev libwebkit2gtk-4.1-dev

# 依赖——运行与测试推荐（中文字体/输入法、padsp 音效转发、无头验证工具）
sudo apt install fonts-noto-cjk fcitx5 fcitx5-chinese-addons \
     fcitx pulseaudio-utils \
     xvfb gettext x11-utils x11-xserver-utils imagemagick xdotool \
     xterm dbus-x11

# 构建：只编译并把安装结果经 DESTDIR 重定向到 ./staging 暂存区
# （零提权、不写入任何系统目录；正式安装一律走 deb 包，见 §1 目标 5）
./build.sh --prefix=/usr/kde1

# 开发期运行（直接从暂存区起，无需安装）：
#   export PATH=<仓库>/staging/usr/kde1/bin:$PATH
#   export LD_LIBRARY_PATH=<仓库>/staging/usr/kde1/lib:<仓库>/staging/usr/kde1/tqt3/lib
#   export KDEDIR=<仓库>/staging/usr/kde1
#   （~/.xinitrc 末行 exec startkde，控制台 startx）

# 后台桌面沙箱（不注销宿主会话即可体验/验证；Remmina 等客户端 VNC 连 127.0.0.1:5901）：
./sandbox.sh start     # 起沙箱（Xvfb :99 + fcitx5 + x11vnc + 音效转发）
./sandbox.sh status    # 查看各组件状态
./sandbox.sh stop      # 停止并按 PID 定点清理
#
# 正式安装：只使用 dist/deb/ 下的 deb 包（dpkg -i / apt install），
# 安装后在 lightdm 会话菜单选择 KDE 1 登录。
```

## 5. 编码与文档规范

### 5.1 统一中文 UTF-8

- 所有**新增或完全改写**的源码文件、配置文件、脚本、文档一律以 **UTF-8（无 BOM）、LF 行尾**保存。
- C/C++ 源码不需要 Python 式 `coding` 声明行：GCC 默认按 UTF-8 处理输入源码，无需任何等价物；不要往源文件里加编码声明注释。
- **历史文件禁止转码**：`kde*/` 原始源码为 ASCII/Latin-1（个别为 Latin-1 带高位字节），po 翻译文件转 UTF-8 已完成——一律保持现状编码，禁止任何形式的批量转码重存（会产生无意义巨型 diff，且违反对历史代码的最小改动原则）。`tqt3/` 为上游 pristine 快照，除 `tqt3-patches/` 补丁外禁止任何修改。
- 所有**注释、提交说明（git commit message）、构建脚本输出信息、诊断日志文本**均**使用中文**。
- 中文字符串字面量的边界：GUI 可见的中文字面串依赖路线乙渲染补丁生效后才能正确上屏——补丁落地前，新增代码中的中文只允许出现在**注释与 stderr 日志**里（现代终端按 UTF-8 显示 stderr，不受 Qt 1 渲染限制）。
- 控制台输出中文时确保终端为 UTF-8：本机已生成 `zh_CN.utf8` locale，必要时运行前执行 `export LANG=zh_CN.UTF-8`。构建 27 年前的工具链组件（lex/yacc/老 configure）如遇 locale 相关报错，构建命令前缀 `LC_ALL=C`（仅限构建期，不影响运行期 UTF-8）。
- 日志输出规范（本项目各层的"logging 模块"等价物，禁止用裸 printf 散落各处）：
  - **Qt 层**：TQt3 自带 `tqDebug()` / `tqWarning()` / `tqFatal()`（printf 风格，stderr）。KDE1 代码中的裸 `debug()/warning()/fatal()` 调用已在迁移中显式化为 tq 前缀。示例：
    ```cpp
    tqWarning("KWM 通信失败: %s", cmd);
    ```
  - **KDE 应用层**：`kdelibs/kdecore/kdebug.h` 提供 `kdebug( level, area, fmt, ... )` 与 `KDEBUG*/KASSERT*` 宏。注意：KDE 1.1.2 **没有** `kdDebug`（那是 KDE 2 之后的接口），不要写错年代。
  - **脚本层**：`echo "..." >&2`，排查时可临时 `set -x`。
  - 原则：错误信息必须包含**失败位置与原因**；临时调试输出交付前清理（见 §5.2.3）。

### 5.2 注释规范：5W1H + 伪代码（强制）

所有由 Agent 编写的代码，注释必须同时满足**两条硬性要求**：① 关键逻辑用 **5W1H** 格式详写；② 非平凡函数/算法在实现前先写**伪代码**说明思路。这两条与 §5.1「统一中文 UTF-8」并列，缺一不可。

#### 5.2.1 注释用 5W1H 格式（强制）

- **What（做什么）**：本段代码的功能/目标。
- **Why（为什么）**：设计理由、背景、为什么选这个方案（不是另一个）。
- **Who（谁/对谁）**：调用方/被调用方/责任模块/适用对象。
- **When（何时）**：执行时机、触发条件、生命周期（初始化/每次调用/退出时）。
- **Where（在哪）**：所在文件、所属模块、作用范围、数据来源/去向。
- **How（怎么做）**：实现步骤、关键算法、边界处理、关键函数的数据运行轨迹及函数内部流程。

**适用范围**：新增文件的头注释、类/函数块注释、复杂逻辑段（多字节解析循环、XIM 事件路径、字体回退分支、构建脚本的平台探测等）的段头注释。**简单一行赋值/取值函数不强制 5W1H**，但也不能零注释。

**历史代码的原有英文注释保持原样**——不翻译、不改写、不删除；中文 5W1H 注释只加在**新增或实质改动的代码段**上。

**C++ 合规示例**（贴合底座迁移真实场景）：

```cpp
static QString xdg_home_dir( const char *key, const QString & fallback )
// ┌─ What : 解析 ~/.config/user-dirs.dirs，按 KEY 取现代桌面标准的用户目录路径
// │  Why  : KDE1 体系里 kfm 与 krootwm 各有 1999 年的路径默认值（~/Desktop 等），
// │         须收敛为唯一实现并优先读 XDG 约定（中文系统即 ~/桌面、~/模板）
// │  Who  : kfmpaths.cpp（桌面/模板/回收站）、krootwm（右键菜单）共用
// │  When : 每个 QFontInternal 生命周期内至多一次（结果缓存），程序退出时随 XftFont 关闭
// │  Where: 任意 KDE1 迁移代码；示例为 tqt3-patches 补丁中的实际用法
// │  How  : ① 以当前字族/字号组装 fontconfig 模式名 → ② XftFontOpenName 打开
// │         → ③ 失败则改用 fallback_pattern 重试 → ④ 仍失败返回 0，调用方回退单字节路径
{
    ...
}
```

**反例（不合规）**：只写 `// 打开中文字体` 或零注释——缺 Why/When/How，看不出为什么用 Xft、什么时候触发、失败怎么回退。

#### 5.2.2 实现前先写伪代码（强制）

- **非平凡**函数/算法/流程（含多步逻辑、条件分支、跨平台探测、IO/X11 调用、对历史代码行为的任何改变），**必须先用中文伪代码注释把思路写清楚，再写真实代码**。
- 伪代码用**贴近自然语言 + 缩进**的结构化中文表达，**不纠结具体语法**，但要把步骤、分支、循环、异常路径覆盖全。
- 伪代码注释**保留在最终代码里**（作为块头说明），**不删**——它是给后来者（含 Agent 自己）快速理解逻辑的路标。
- **平凡逻辑**（单行赋值、直接 return、简单取值函数）**不强制**写伪代码，避免过度注释。

**C++ 合规示例**（UTF-8 宽度计算，路线乙核心难点之一）：

```cpp
int qt_mb_text_width( const char *s, int len )
{
    // 伪代码：
    //   1. 逐字节扫描 [s, s+len)：
    //        a. 字节 < 0x80        → ASCII，宽度累加该字符实测宽度，前进 1 字节
    //        b. 字节是 UTF-8 前导字节 → 按前导字节确定本字符长度 k，
    //                                 宽度累加 XftTextExtentsUtf8 实测值，前进 k 字节
    //        c. 字节是孤立的续字节（0x80-0xBF）→ 按替换符 U+FFFD 计宽，
    //                                 同一字符串只告警一次，前进 1 字节
    //   2. 返回累计宽度（单位：像素）
    ...
}
```

#### 5.2.3 注释密度与同步原则

- **宁可多注释，不可零注释**。复杂/安全相关/跨平台/核心渲染逻辑（本项目的多字节渲染、字体回退、XIM 事件处理均属此类），注释行数可与代码行数持平甚至更多。
- 注释要随代码一起改——**改动代码而不更新注释，等同于误导**，比没注释更糟。
- 改动历史代码时：该区域**原有英文注释保留**；若原注释因改动而失真，在其下一行补中文更正说明，不得直接删改原句（保留历史可追溯性）。
- 临时调试用的注释（`// TODO 调试用，待删`、注释掉的旧代码、临时 debug() 输出）**交付前必须清理**。
- git 提交说明用中文：一行主题概括 What，正文（可选）说明 Why 与影响面。

## 6. 工作约定（必须遵守）

1. **禁止日志式记录**：任何 `.md` 文档（AGENTS.md、README.md 及未来新增文档）一律不得出现过程性/流水账内容（"今日完成了…"、"第 X 步…"、"已修复…"等）。所有修改历史只写入 `CHANGELOG.md`，其余文档永远只呈现当前最终状态。
2. **CHANGELOG.md 条目格式**：`日期 + 一行摘要`，新的在最上；一次工作对应一条，不要逐文件罗列。
3. **版权合规**：各源文件头部原有的 Troll Tech / KDE Team 版权声明与许可条款**不得删除、不得改写**（许可文件：`tqt3/LICENSE.GPL2`（TQt3 三选一许可证中我们选择 GPLv2 侧）、各模块 `COPYING` / `COPYING.LIB`、`tqt3-patches/` 补丁头）。对源码的实质性修改，在文件头部原有声明**之后**追加标注，模板：

   ```
   //   Modified for the KDE1 Revival Project, 2026
   //   Maintainer: <维护者姓名> <邮箱>
   //   Modifications written with GLM-5.3 (Z.ai)
   ```

4. **新增文件的作者归属**：由 GLM-5.3 编写的新增文件，在文件头注明 `Written with GLM-5.3 (Z.ai) for the KDE1 Revival Project`。
5. **语言与编码**：文档用简体中文；代码的编码与注释规范详见 §5（新增代码用中文注释，历史代码注释保持原样）。
6. **路线图推进**：完成一项即在 §7"路线图"把 `[ ]` 改为 `[x]`，并在 CHANGELOG.md 记一条，不在其他任何地方留痕。
7. **KDE1 主动适配现代软件生态（强制）**：现代软件（fcitx5、Electron 应用、GTK/Qt 现代程序等）**不做任何修改或降级配置**来迁就 KDE1——KDE1 一侧实现现代软件依赖的通行协议与标准。已实现：XEmbed 系统托盘（`_NET_SYSTEM_TRAY`/XEMBED，kpanel 指示栏）；实施中：StatusNotifierItem（SNI，DBus 协议，fcitx5 5.x 默认托盘路径）host 角色。凡是"改现代软件配置以适配 KDE1"的做法一律视为错误方向并回退。
8. **外部库一律用现代现行版本与现行 API（强制）**：凡项目代码依赖系统第三方库之处（图像编解码 libpng/libjpeg/libtiff/giflib、音频、X11 扩展等），一律使用 Debian 12 所载现代版本的**现行 API** 直接实现，**禁止**为 1999 年老接口写兼容 shim/宏桥/内嵌库副本。历史代码因旧库产生的缺陷（如 1999 年 PNG 读取不含 gamma/ICC 处理导致色彩错误）一律以现代库能力重写修正——应用二十余年积累的编解码成果，而非迁就 1999 年的调用方式。随源码携带的旧版库副本（如 kfax/libtiffax）同规则逐步淘汰为系统库。
7. **内容同步更新**：同一事实在多处出现时（模块清单、构建顺序、安装前缀、版本号、约定口径、代码与其注释/文档的对应关系等），修改任何一处必须**同步更新其余各处**——代码与文档不一致、章程与实际做法脱节，均视为本次修改未完成。发现存量不一致时，随当前工作一并修正并记入 CHANGELOG.md。

## 7. 路线图（当前状态）

- [x] 源码就位与版本考证（确认 KDE 1.1.2 为系列终版）
- [x] 中文支持源码分析（结论固化于 README.md「中文支持」一节）
- [x] 项目文档体系建立（AGENTS.md / README.md / CHANGELOG.md）
- [x] 依赖安装与全量编译跑通（英文桌面）
- [x] **底座更换 Qt1→TQt3**（2026-08 完成）：TQt3 r14.1.6 GPLv2，port/ 脚手架迁移六模块，setCodecForCStrings 全局 UTF-8；渲染/剪贴板/XIM/字符边界由 TQt3 原生 Unicode 体系提供，原 Qt1 逐点补丁路线随之作废
- [x] 剪贴板：TQt3 原生 UTF8_STRING 支持，与浏览器/现代 Qt/GTK 双向粘贴中文
- [x] 全面 UTF-8 融合：TQString 内部 UTF-16 + 全局 codec 开关，字符边界天然正确（见 §1 目标 8）
- [x] po 翻译文件转 UTF-8 并验证显示
- [x] XIM 输入通道验证
- [x] fcitx5 / fcitx 输入法稳定运行验证（经 XIM 通道接入，两代框架均须通过，见 §1 目标 2）
- [x] kvt 终端中文化：kvt 不经 Qt 渲染路径（基于 xvt 自绘），须单独实现 UTF-8 终端渲染；kfm/kdehelp 中的自绘文本路径一并确认被渲染补丁覆盖
- [x] 图形登录接入：注册 `/usr/share/xsessions/kde1.desktop` 会话入口并适配 startkde 在显示管理器环境下运行，lightdm 会话菜单可直接进入 KDE 1，不依赖手工 startx（见 §1 目标 6）
- [x] 事件音效接入现代音频栈：解决 `/dev/dsp`（OSS）在 Debian 12 上失效的问题（padsp/osspd 兼容层或原生补丁），提示音经 PipeWire/PulseAudio 正常发声（见 §1 目标 6）
- [x] kfm 浏览器内核升级：以 WebKit2GTK 为新内核——kfm 窗口内实现 XEmbed 容器（kfm 侧手写，约一两百行 C）挂载 WebKit 视图（GtkPlug 一侧现成），与 1999 原始渲染器构成新旧双内核可切换；CEF（Chromium）备选，Firefox/Gecko 已排除；实施时先做最小嵌入原型验证，再动 kfm 本体（见 §1 目标 3）
- [ ] 界面全面中文化：补齐菜单/帮助/提示翻译，覆盖率与英文对齐；译法须上网核实通行译法并与当今 KDE 6 的 zh_CN 官方术语一致（见 §1 目标 4）
- [x] 打印接入 CUPS：经 cups-bsd 兼容层打通 Qt1 的 lpr 打印路径，deb 依赖声明，实测打印任务进入 CUPS 队列（见 §1 目标 7）
- [x] XDG 用户目录兼容：KFMPaths 默认值改为经 ~/.config/user-dirs.dirs 解析（桌面=~/桌面、模板=~/模板），Autostart 挪至 ~/.kde/share/autostart，回收站对齐 ~/.local/share/Trash/files；实测 KDE1 桌面与 XFCE 共用 ~/桌面 且不再创建 ~/Desktop（见 §1 目标 6）
- [x] Debian 打包：按模块拆分核心包与可选包，产出源码包与二进制 .deb 包并分目录存放（dist/src/ 与 dist/deb/）；.deb 开箱即用——含 xsessions 会话入口、startkde 环境包装脚本（PATH/LD_LIBRARY_PATH/KDEDIR 自动设置）、TQt3 运行库、运行时依赖声明；apt install 即装、lightdm 即现、apt remove 干净移除（见 §1 目标 5）

**暂缓项（明确延后，不纳入当前验收）：**

- Xwayland 兼容验证：当前验收环境为 Xorg 会话；Wayland 会话（经 Xwayland 运行 KDE 1）的验证延后处理。

## 8. 维护者

- <维护者姓名> <邮箱>（待填）
- 新增修改由 GLM-5.3（Z.ai）编写，维护者审核入库。
