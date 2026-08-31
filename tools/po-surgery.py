#!/usr/bin/env python3
# ────────────────────────────────────────────────────────────────────
# [KDE1 Revival 2026-08-31] po 修复手术：去重 + 原位翻译替换 + fuzzy 清理
# What : ① 修复标准词批补脚本造成的重复 msgid（EMPTY 条目被追加而非替换）
#        ② 应用人工翻译表（应用特有词条/长句）——原位改写、顺带清 fuzzy
# Why  : msgfmt 对重复 msgid 报错；fuzzy 条目默认不编入 mo（运行时仍英文）
# Who  : 各模块 po/zh_CN.UTF-8/*.po
# When : po-standard-terms.py --apply 之后运行一次
# Where: 仓库根；翻译表由 apply_manual.py 提供（本文件只做机制）
# How  : 伪代码：
#   1. 逐行状态机解析（mode: id/str 精确跟踪；注释行归属下一条目）
#   2. 重复 msgid：保留首次出现位置，msgstr 取最后一次非空值（批补所加）
#   3. MANUAL 命中：替换 msgstr、去掉 fuzzy 标记行
#   4. 重新序列化写回（保留各条目原有注释与折行结构）
# ────────────────────────────────────────────────────────────────────
import re, sys

def po_escape(s):
    return s.replace('\\', '\\\\').replace('"', '\\"').replace('\n', '\\n')

def po_unescape(s):
    # 先换行、再引号、最后反斜杠（与转义顺序互逆）
    return s.replace('\\n', '\n').replace('\\"', '"').replace('\\t','\t').replace('\\\\', '\\')

LIT = re.compile(r'"((?:[^"\\]|\\.)*)"')

class Entry:
    __slots__ = ('c', 'id', 's', 'fz')
    def __init__(self):
        self.c = []; self.id = []; self.s = []; self.fz = False
    def mid(self):
        return po_unescape(''.join(LIT.findall('\n'.join(self.id))))
    def mstr(self):
        return po_unescape(''.join(LIT.findall('\n'.join(self.s))))

def parse_lines(lines):
    entries = []
    cur = Entry()
    mode = None            # 'id' | 'str' | None
    def flush():
        nonlocal cur, mode
        if cur.id:
            entries.append(cur)
            carry = []
        else:
            carry = cur.c  # 尚未成条目的注释（文件头/条目前导注释）顺延给下一条
        cur = Entry()
        cur.c = carry
        mode = None
    for ln in lines:
        if ln.startswith('#'):
            if 'fuzzy' in ln: cur.fz = True
            cur.c.append(ln)
            continue
        if ln.startswith('msgid '):
            flush()
            cur.id.append(ln[6:]); mode = 'id'
        elif ln.startswith('msgstr '):
            cur.s.append(ln[7:]); mode = 'str'
        elif ln.startswith('"'):
            (cur.id if mode == 'id' else cur.s).append(ln)
        else:
            # 空行/其他：条目结束
            flush()
    flush()
    # 末尾 flush 后残留的注释丢弃（本仓库 po 无尾注释）
    return entries

def serialize(entries):
    buf = []
    for e in entries:
        for c in e.c: buf.append(c)
        buf.append('msgid ' + (e.id[0] if e.id else '""'))
        buf.extend(e.id[1:])
        buf.append('msgstr ' + (e.s[0] if e.s else '""'))
        buf.extend(e.s[1:])
        buf.append('')
    return '\n'.join(buf)

def drop_conflicting_obsolete(lines):
    """[2026-08-31] gettext 0.21 的 msgfmt 把 `#~ msgid X` 与活动 `msgid X`
    重名判为致命错误（1999 年 po 不适应新工具链）。本函数删掉与活动条目
    冲突的废弃块——其译法已在术语表提炼阶段被吸收，无信息损失。"""
    active = set()
    for ln in lines:
        m = re.match(r'msgid\s+"((?:[^"\\]|\\.)*)"', ln)
        if m:
            active.add(po_unescape(m.group(1)))
    out = []
    i = 0
    n = len(lines)
    while i < n:
        ln = lines[i]
        if ln.startswith('#~ msgid '):
            # 废弃区是一整段连续 #~ 行，内含多个 (msgid,msgstr) 对——
            # 按对拆开逐个判定，只保留不冲突的（整段保留会漏检后段冲突）
            j = i
            while j < n and lines[j].startswith('#~'):
                j += 1
            region = lines[i:j]
            kept = []
            k = 0
            while k < len(region):
                rln = region[k]
                if rln.startswith('#~ msgid '):
                    m = re.match(r'#~ msgid\s+"((?:[^"\\]|\\.)*)"', rln)
                    mid = po_unescape(m.group(1)) if m else None
                    # 吸收该条的 msgstr/续行
                    k2 = k + 1
                    while k2 < len(region) and not region[k2].startswith('#~ msgid '):
                        k2 += 1
                    if mid not in active:
                        kept.extend(region[k:k2])
                    k = k2
                else:
                    kept.append(rln)   # 孤儿 #~ msgstr 等注释行原样保留
                    k += 1
            out.extend(kept)
            i = j
            continue
        out.append(ln); i += 1
    return out

def surgery(path, manual):
    with open(path, encoding='utf-8') as f:
        lines = f.read().split('\n')
    lines = drop_conflicting_obsolete(lines)
    entries = parse_lines(lines)
    # ① 去重（含注释归属修正）
    seen = {}
    out = []
    ndup = 0
    for e in entries:
        mid = e.mid()
        if mid == '' and not out:
            out.append(e); continue        # 头部
        if mid in seen:
            first = seen[mid]
            if e.mstr():
                first.s = e.s
                first.fz = False           # 新译直接生效
            ndup += 1
            continue
        seen[mid] = e
        out.append(e)
    # ② 人工表
    nhit = 0
    for e in out:
        mid = e.mid()
        if mid in manual and manual[mid]:
            e.s = ['"%s"' % po_escape(manual[mid])]
            e.c = [c for c in e.c if 'fuzzy' not in c]
            e.fz = False
            nhit += 1
    with open(path, 'w', encoding='utf-8') as f:
        f.write(serialize(out) + '\n')
    return ndup, nhit

if __name__ == '__main__':
    print(surgery(sys.argv[1], {}))
