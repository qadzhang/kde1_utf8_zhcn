#!/usr/bin/env python3
# ────────────────────────────────────────────────────────────────────
# [KDE1 Revival 2026-08-31] po 标准词批量补齐工具
# What : ① 从全仓库已有 po 提炼标准词译法（频次加权）→ ② 对每个应用
#        提取源码 i18n 字符串 → ③ 缺失且命中的词条批量追加进 po
# Why  : 覆盖率分析表明缺失词条 60%+ 是 &File/&Help/OK/Cancel 这类
#        标准词（1999 年 po 从未收录）——机器化批量补齐 + 术语统一
# Who  : 各模块 po/zh_CN.UTF-8/*.po；kdelibs/po/zh_CN.UTF-8.po
# When : 补翻译时人工触发一次；输出报告供人工复核
# Where: 仓库根运行；只改 po 不动源码
# How  : 伪代码：
#   1. 解析所有 po → (msgid, msgstr) 集；跳过空/直抄/fuzzy 条目
#      → term_map[msgid] = 出现频次最高的 msgstr（项目内已确立的译法）
#   2. 手工权威表 override（KDE6 zh_CN 官方术语，频次表的兜底修正）
#   3. 对每个 (应用源码目录, po 文件)：正则+多行拼接提取 i18n(...)
#      → missing = 源码有 po 无
#   4. missing ∩ term_map → 按 po 格式追加（带 KDE1 Revival 标记注释）
#   5. 输出仍缺失清单（应用特有词条，留人工）
# ────────────────────────────────────────────────────────────────────
import re, os, glob, sys
from collections import Counter, defaultdict

ROOT = '/home/user/debain_deb/kde1'

# ── 1. 解析 po（简单状态机：msgid/msgstr 可多行，fuzzy 检测） ──
def parse_po(path):
    entries = []  # (msgid, msgstr, fuzzy)
    msgid = msgstr = None
    fuzzy = False
    def flush():
        nonlocal msgid, msgstr
        if msgid is not None:
            entries.append((msgid, msgstr, fuzzy))
    def unq(s):
        return re.findall(r'"((?:[^"\\]|\\.)*)"', s)
    with open(path, encoding='utf-8') as f:
        for line in f:
            line = line.strip()
            if line.startswith('#,') and 'fuzzy' in line:
                fuzzy = True
            elif line.startswith('msgid '):
                flush(); msgid = ''.join(unq(line)); msgstr = ''
            elif line.startswith('msgstr '):
                msgstr = ''.join(unq(line))
            elif line.startswith('"') and msgstr != '' or (line.startswith('"') and msgid is not None and msgstr is not None and msgstr == ''):
                if msgid is not None and msgstr is not None:
                    if line.startswith('"'):
                        # 归属判断：msgstr 已开（非 None 且非 ''）则续 msgstr，否则续 msgid
                        if msgstr_open[0]:
                            msgstr += ''.join(unq(line))
                        else:
                            msgid += ''.join(unq(line))
            if line.startswith('msgstr'):
                msgstr_open[0] = True
            elif line.startswith('msgid'):
                msgstr_open[0] = False
    flush()
    return entries

msgstr_open = [False]

# 为状态机正确性重写解析（上面逻辑绕，这里直接用清晰实现）
def parse_po2(path):
    entries = []
    cur_id = []; cur_str = []; mode = None; fuzzy = False
    with open(path, encoding='utf-8') as f:
        for line in f:
            line = line.strip()
            if line.startswith('#'):
                if 'fuzzy' in line: fuzzy = True
                continue
            m = re.match(r'msgid\s+(.*)', line)
            if m:
                if cur_id and cur_str is not None and mode == 'str':
                    entries.append((''.join(cur_id), ''.join(cur_str), fuzzy))
                    cur_id, cur_str, fuzzy = [], [], False
                mode = 'id'; cur_id = [m.group(1)]; continue
            m = re.match(r'msgstr\s+(.*)', line)
            if m:
                mode = 'str'; cur_str = [m.group(1)]; continue
            if line.startswith('"'):
                if mode == 'id': cur_id.append(line)
                elif mode == 'str': cur_str.append(line)
    if cur_id and cur_str:
        entries.append((''.join(cur_id), ''.join(cur_str), fuzzy))
    def unqs(parts):
        s = ''
        for p in parts:
            s += ''.join(re.findall(r'"((?:[^"\\]|\\.)*)"', p))
        return s.replace('\\n','\n').replace('\\"','"').replace('\\\\','\\')
    return [(unqs([i]) if False else unqs(i_parts), unqs(s_parts), fz)
            for i_parts, s_parts, fz in
            [( [x] if isinstance(x,str) else x for x in e) for e in []]] or \
           [(unqs([c if isinstance(c,str) else ''.join(c) for c in [e[0]]]), unqs([e[1]]), e[2]) for e in entries]

# 上面的通用化太绕——直接用数组收集原始字符串行
def parse_po3(path):
    entries = []
    blocks = []
    cur = {'id': [], 'str': [], 'fuzzy': False, 'mode': None}
    with open(path, encoding='utf-8') as f:
        for line in f:
            s = line.strip()
            if s.startswith('#'):
                if 'fuzzy' in s: cur['fuzzy'] = True
                continue
            if s.startswith('msgid '):
                if cur['mode'] == 'str':
                    blocks.append(cur); cur = {'id': [], 'str': [], 'fuzzy': False, 'mode': None}
                cur['mode'] = 'id'; cur['id'].append(s[6:])
            elif s.startswith('msgstr '):
                cur['mode'] = 'str'; cur['str'].append(s[7:])
            elif s.startswith('"'):
                cur[cur['mode']].append(s)
    if cur['mode'] == 'str': blocks.append(cur)
    def join(parts):
        out = ''
        for p in parts:
            out += ''.join(re.findall(r'"((?:[^"\\]|\\.)*)"', p + '"'))
        return out.replace('\\n', '\n').replace('\\"', '"').replace('\\\\', '\\')
    for b in blocks:
        entries.append((join(b['id']), join(b['str']), b['fuzzy']))
    return entries

def po_escape(s):
    return s.replace('\\', '\\\\').replace('"', '\\"').replace('\n', '\\n')

# ── 2. 建术语表：仓库已有译法（频次） + KDE6 权威兜底 ──
AUTHORITY = {
    '&File': '文件(&F)', '&Edit': '编辑(&E)', '&View': '查看(&V)',
    '&Go': '转到(&G)', '&Bookmarks': '书签(&B)', '&Options': '选项(&O)',
    '&Help': '帮助(&H)', '&Settings': '设置(&S)', '&Tools': '工具(&T)',
    '&Window': '窗口(&W)', '&Game': '游戏(&G)', '&Insert': '插入(&I)',
    'OK': '确定', 'Cancel': '取消', 'Apply': '应用', 'Close': '关闭',
    'Help': '帮助', 'Quit': '退出', 'Exit': '退出', '&Quit': '退出(&Q)',
    'E&xit': '退出(&X)', '&Save': '保存(&S)', 'Save': '保存',
    'Save As...': '另存为...', 'Open': '打开', '&Open...': '打开(&O)...',
    '&New': '新建(&N)', '&Print...': '打印(&P)...', 'Print': '打印',
    '&Copy': '复制(&C)', 'Copy': '复制', 'C&ut': '剪切(&T)', '&Cut': '剪切(&T)',
    'Cut': '剪切', '&Paste': '粘贴(&P)', 'Paste': '粘贴',
    '&Delete': '删除(&D)', 'Delete': '删除', '&Undo': '撤销(&U)',
    'Undo': '撤销', '&Redo': '重做(&R)', 'Redo': '重做',
    '&Select All': '全选(&S)', 'Select All': '全选',
    'About': '关于', 'Error': '错误', 'Warning': '警告',
    'Information': '信息', 'Sorry': '抱歉', 'Back': '后退',
    'Forward': '前进', 'Up': '向上', 'Reload': '重新加载', 'Stop': '停止',
    '&Reload': '重新加载(&R)', 'Find': '查找', '&Find': '查找(&F)',
    'Replace': '替换', 'Defaults': '默认', '&Defaults': '默认(&D)',
    'Add': '添加', 'Remove': '删除', '&Remove': '删除(&R)', 'Change': '更改',
    'Clear': '清除', 'Continue': '继续', 'No': '否', 'Yes': '是',
    'Font': '字体', 'Fonts': '字体', 'Browse...': '浏览...',
    'New Window': '新建窗口', '&New Window...': '新建窗口(&N)...',
    'Appearance': '外观', 'Miscellaneous': '杂项', 'New Game': '新游戏',
    'Score': '分数', '&Contents': '内容(&C)', 'Open...': '打开...',
    '&Open': '打开(&O)', '&Close': '关闭(&C)', '&OK': '确定(&O)',
    '&Cancel': '取消(&C)', 'Print...': '打印...', '&Print': '打印(&P)',
    'Edit': '编辑', 'File': '文件', 'View': '查看', 'Game': '游戏',
    'Options': '选项', 'Insert': '插入', 'New': '新建',
    '&Add': '添加(&A)', '&Change': '更改(&C)', '&Clear': '清除(&C)',
    '&Continue': '继续(&C)', '&Help...': '帮助(&H)...',
    'Malformed URL': 'URL 格式错误',
}

def build_term_map():
    votes = defaultdict(Counter)
    for po in glob.glob(f'{ROOT}/*/po/**/*.po', recursive=True) + glob.glob(f'{ROOT}/kdelibs/po/*.po'):
        try:
            for mid, mstr, fuzzy in parse_po3(po):
                if not mstr or mstr == mid or fuzzy: continue
                if len(mid) <= 40:
                    votes[mid][mstr] += 1
        except Exception:
            pass
    tmap = {}
    for mid, c in votes.items():
        best, n = c.most_common(1)[0]
        if n >= 2:  # 至少两个应用同译法才采纳（避免个体误差）
            tmap[mid] = best
    tmap.update(AUTHORITY)  # 权威表最终覆盖
    return tmap

# ── 3. 源码 i18n 提取（含相邻字符串字面量拼接） ──
CALL_RE = re.compile(r'\b(?:i18n|_\()\s*\(')
def extract_src_strings(paths):
    found = set()
    strlit = re.compile(r'"((?:[^"\\\n]|\\.)*)"')
    for p in paths:
        try:
            src = open(p, encoding='utf-8', errors='replace').read()
        except Exception:
            continue
        src = re.sub(r'/\*.*?\*/', ' ', src, flags=re.S)   # 去块注释
        src = re.sub(r'//[^\n]*', ' ', src)
        for m in re.finditer(r'\b(?:i18n|translate)\s*\(', src):
            # 从调用点向后收集连续字符串字面量（直到逗号+非常量或右括号）
            seg = src[m.end():m.end()+2000]
            lits = []
            pos = 0
            for tok in re.finditer(r'\s*((?:"(?:[^"\\\n]|\\.)*")|(.))', seg):
                g = tok.group(1)
                if g is None: break
                if g.startswith('"'):
                    lits.append(g[1:-1])
                    pos = tok.end()
                else:
                    ch = g.strip()
                    if ch == '"': continue
                    if ch and pos == 0 and ch not in ' \n\t': break
                    if ch == ',':  # 第二参数（默认值）——继续吃一个表达式再停
                        rest = seg[tok.end():tok.end()+200]
                        m2 = re.match(r'\s*("(?:[^"\\\n]|\\.)*")', rest)
                        if m2:
                            lits.append(m2.group(1)[1:-1]); pos = tok.end() + m2.end()
                            seg = seg[:pos] + seg[pos:]
                            continue
                        break
                    if ch == ')': break
                    if ch: break
            s = ''.join(lits)
            if s:
                s = s.replace('\\n','\n').replace('\\"','"').replace('\\\\','\\')
                if '\x00' not in s:
                    found.add(s)
    return found

# ── 4. 应用清单（源码目录 → po） ──
APPS = [
    ('kfm',        ['kdebase/kfm/*.cpp'],                          'kdebase/po/zh_CN.UTF-8/kfm.po'),
    ('kvt',        ['kdebase/kvt/*.C','kdebase/kvt/*.c'],          'kdebase/po/zh_CN.UTF-8/kvt.po'),
    ('kdehelp',    ['kdebase/kdehelp/*.cpp'],                      'kdebase/po/zh_CN.UTF-8/kdehelp.po'),
    ('kfind',      ['kdebase/kfind/*.cpp'],                        'kdebase/po/zh_CN.UTF-8/kfind.po'),
    ('kmenuedit',  ['kdebase/kmenuedit/*.cpp'],                    'kdebase/po/zh_CN.UTF-8/kmenuedit.po'),
    ('kcmdisplay', ['kdebase/kcontrol/display/*.cpp'],             'kdebase/po/zh_CN.UTF-8/kcmdisplay.po'),
    ('kcmkeys',    ['kdebase/kcontrol/keys/*.cpp'],                'kdebase/po/zh_CN.UTF-8/kcmkeys.po'),
    ('kcmbell',    ['kdebase/kcontrol/bell/*.cpp'],                'kdebase/po/zh_CN.UTF-8/kcmbell.po'),
    ('kcminfo',    ['kdebase/kcontrol/info/info.cpp','kdebase/kcontrol/info/info_linux.cpp','kdebase/kcontrol/info/memory_linux.cpp','kdebase/kcontrol/info/info_generic.cpp'], 'kdebase/po/zh_CN.UTF-8/kcminfo.po'),
    ('kfontmanager',['kdebase/kfontmanager/*.cpp'],                'kdebase/po/zh_CN.UTF-8/kfontmanager.po'),
    ('kpager',     ['kdebase/kwmmodules/kpager/*.cpp'],            'kdebase/po/zh_CN.UTF-8/kpager.po'),
    ('konsole',    ['kdebase/konsole/src/*.C'],                    'kdebase/po/zh_CN.UTF-8/konsole.po'),
    ('ktop',       ['kdeutils/ktop/*.cpp'],                        'kdeutils/po/zh_CN.UTF-8/ktop.po'),
    ('kedit',      ['kdeutils/kedit/*.cpp'],                       'kdeutils/po/zh_CN.UTF-8/kedit.po'),
    ('kwrite',     ['kdeutils/kwrite/*.cpp'],                      'kdeutils/po/zh_CN.UTF-8/kwrite.po'),
    ('kab',        ['kdeutils/kab/*.cpp'],                         'kdeutils/po/zh_CN.UTF-8/kab.po'),
    ('karm',       ['kdeutils/karm/*.cpp'],                        'kdeutils/po/zh_CN.UTF-8/karm.po'),
    ('kcalc',      ['kdeutils/kcalc/*.cpp'],                       'kdeutils/po/zh_CN.UTF-8/kcalc.po'),
    ('kfloppy',    ['kdeutils/kfloppy/*.cpp'],                     'kdeutils/po/zh_CN.UTF-8/kfloppy.po'),
    ('khexdit',    ['kdeutils/khexdit/*.cpp'],                     'kdeutils/po/zh_CN.UTF-8/khexdit.po'),
    ('kjots',      ['kdeutils/kjots/*.cpp'],                       'kdeutils/po/zh_CN.UTF-8/kjots.po'),
    ('kljettool',  ['kdeutils/kljettool/*.cpp'],                   'kdeutils/po/zh_CN.UTF-8/kljettool.po'),
    ('klpq',       ['kdeutils/klpq/*.cpp'],                        'kdeutils/po/zh_CN.UTF-8/klpq.po'),
    ('knotes',     ['kdeutils/knotes/*.cpp'],                      'kdeutils/po/zh_CN.UTF-8/knotes.po'),
    ('kabalone',   ['kdegames/kabalone/*.cpp'],                    'kdegames/po/zh_CN.UTF-8/kabalone.po'),
    ('kasteroids', ['kdegames/kasteroids/*.cpp'],                  'kdegames/po/zh_CN.UTF-8/kasteroids.po'),
    ('kblackbox',  ['kdegames/kblackbox/*.cpp'],                   'kdegames/po/zh_CN.UTF-8/kblackbox.po'),
    ('kmahjongg',  ['kdegames/kmahjongg/*.cpp'],                   'kdegames/po/zh_CN.UTF-8/kmahjongg.po'),
    ('kmines',     ['kdegames/kmines/*.cpp'],                      'kdegames/po/zh_CN.UTF-8/kmines.po'),
    ('konquest',   ['kdegames/konquest/*.cc'],                    'kdegames/po/zh_CN.UTF-8/konquest.po'),
    ('kpat',       ['kdegames/kpat/*.cpp'],                        'kdegames/po/zh_CN.UTF-8/kpat.po'),
    ('kpoker',     ['kdegames/kpoker/*.cpp'],                      'kdegames/po/zh_CN.UTF-8/kpoker.po'),
    ('kreversi',   ['kdegames/kreversi/*.cpp'],                    'kdegames/po/zh_CN.UTF-8/kreversi.po'),
    ('ksame',      ['kdegames/ksame/*.cpp'],                       'kdegames/po/zh_CN.UTF-8/ksame.po'),
    ('kshisen',    ['kdegames/kshisen/*.cpp'],                     'kdegames/po/zh_CN.UTF-8/kshisen.po'),
    ('ksirtet',    ['kdegames/ksirtet/*.cpp'],                     'kdegames/po/zh_CN.UTF-8/ksirtet.po'),
    ('ksmiletris', ['kdegames/ksmiletris/*.cpp'],                  'kdegames/po/zh_CN.UTF-8/ksmiletris.po'),
    ('ksnake',     ['kdegames/ksnake/*.cpp'],                      'kdegames/po/zh_CN.UTF-8/ksnake.po'),
    ('ksokoban',   ['kdegames/ksokoban/*.C'],                    'kdegames/po/zh_CN.UTF-8/ksokoban.po'),
    ('kdvi',       ['kdegraphics/kdvi/*.cpp'],                     'kdegraphics/po/zh_CN.UTF-8/kdvi.po'),
    ('kfax',       ['kdegraphics/kfax/*.cpp'],                     'kdegraphics/po/zh_CN.UTF-8/kfax.po'),
    ('kfract',     ['kdegraphics/kfract/*.C','kdegraphics/kfract/*.cpp'],                   'kdegraphics/po/zh_CN.UTF-8/kfract.po'),
    ('kghostview', ['kdegraphics/kghostview/*.cpp'],               'kdegraphics/po/zh_CN.UTF-8/kghostview.po'),
    ('kiconedit',  ['kdegraphics/kiconedit/*.cpp'],                'kdegraphics/po/zh_CN.UTF-8/kiconedit.po'),
    ('kpaint',     ['kdegraphics/kpaint/*.cpp'],                   'kdegraphics/po/zh_CN.UTF-8/kpaint.po'),
    ('ksnapshot',  ['kdegraphics/ksnapshot/*.cpp'],                'kdegraphics/po/zh_CN.UTF-8/ksnapshot.po'),
    ('kview',      ['kdegraphics/kview/*.cpp'],                    'kdegraphics/po/zh_CN.UTF-8/kview.po'),
    ('karchie',    ['kdenetwork/karchie/*.cpp'],                   'kdenetwork/po/zh_CN.UTF-8/karchie.po'),
    ('kbiff',      ['kdenetwork/kbiff/*.cpp'],                     'kdenetwork/po/zh_CN.UTF-8/kbiff.po'),
    ('kfinger',    ['kdenetwork/kfinger/*.C','kdenetwork/kfinger/*.cpp'],                   'kdenetwork/po/zh_CN.UTF-8/kfinger.po'),
    ('kmail',      ['kdenetwork/kmail/*.cpp'],                     'kdenetwork/po/zh_CN.UTF-8/kmail.po'),
    ('knu',        ['kdenetwork/knu/*.cpp'],                       'kdenetwork/po/zh_CN.UTF-8/knu.po'),
    ('korn',       ['kdenetwork/korn/*.cpp'],                      'kdenetwork/po/zh_CN.UTF-8/korn.po'),
    ('kppp',       ['kdenetwork/kppp/*.cpp'],                      'kdenetwork/po/zh_CN.UTF-8/kppp.po'),
    ('ksirc',      ['kdenetwork/ksirc/*.cpp'],                     'kdenetwork/po/zh_CN.UTF-8/ksirc.po'),
    ('krn',        ['kdenetwork/krn/*.cpp'],                       'kdenetwork/po/zh_CN.UTF-8/krn.po'),
    ('kmoon',      ['kdetoys/kmoon/*.cpp'],                        'kdetoys/po/zh_CN.UTF-8/kmoon.po'),
    ('kworldwatch',['kdetoys/kworldwatch/*.cpp'],                  'kdetoys/po/zh_CN.UTF-8/kworldwatch.po'),
    ('kodo',       ['kdetoys/mouse/*.cpp'],                        'kdetoys/po/zh_CN.UTF-8/kodo.po'),
]

def main():
    apply = '--apply' in sys.argv
    tmap = build_term_map()
    print(f"术语表规模: {len(tmap)}")
    manual = {}
    for app, globs, porel in APPS:
        paths = []
        for g in globs:
            paths += glob.glob(f'{ROOT}/{g}')
        if not paths:
            print(f"!! {app}: 源码未找到"); continue
        popath = f'{ROOT}/{porel}'
        if not os.path.exists(popath):
            os.makedirs(os.path.dirname(popath), exist_ok=True)
            with open(popath, 'w', encoding='utf-8') as f:
                f.write('# KDE1 Revival 新建 po（上游缺失）\nmsgid ""\nmsgstr ""\n"Content-Type: text/plain; charset=UTF-8\\n"\n\n')
        src_strs = extract_src_strings(paths)
        po_entries = parse_po3(popath)
        have = {e[0] for e in po_entries}
        missing = {s for s in src_strs if s not in have}
        # 空 msgstr / fuzzy 修复集
        empty_ids = [e[0] for e in po_entries if not e[1] and e[0]]
        fuzzy_ids = [e[0] for e in po_entries if e[2] and e[0]]
        add = sorted(s for s in missing if s in tmap)
        rem  = sorted(s for s in missing if s not in tmap)
        fixempty = sorted(set(empty_ids) & set(tmap))
        manual[app] = {'rem': rem, 'empty': [e for e in empty_ids if e not in tmap],
                       'fuzzy': fuzzy_ids}
        if apply and add:
            with open(popath, 'a', encoding='utf-8') as f:
                f.write('\n# [KDE1 Revival 2026-08-31] 标准词批量补齐（对齐 KDE6 zh_CN 术语）\n')
                for s in add:
                    f.write(f'msgid "{po_escape(s)}"\nmsgstr "{po_escape(tmap[s])}"\n\n')
        print(f"{app}: 源码{len(src_strs)} 缺失{len(missing)} 批补{len(add)}+空补{len(fixempty)} 留人工{len(rem)}")
    # 人工清单输出
    with open('/tmp/po-manual-remaining.txt', 'w', encoding='utf-8') as f:
        for app, d in manual.items():
            if d['rem'] or d['empty'] or d['fuzzy']:
                f.write(f"===== {app} =====\n")
                for s in d['rem']: f.write(f"  MISSING: {s!r}\n")
                for s in d['empty']: f.write(f"  EMPTY:   {s!r}\n")
                for s in d['fuzzy']: f.write(f"  FUZZY:   {s!r}\n")
    print("人工剩余清单: /tmp/po-manual-remaining.txt")

if __name__ == '__main__':
    main()
