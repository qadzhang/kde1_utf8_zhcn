#!/usr/bin/env python3
# ────────────────────────────────────────────────────────────────────
# [KDE1 Revival 2026-08-31] 人工翻译表：标准词之外的应用特有词条
# What : 逐条人工翻译覆盖率分析出的 MISSING/EMPTY/FUZZY 词条并经
#        po-surgery 原位写入对应 po
# Why  : 标准词批补只能覆盖通用菜单/按钮词；长句/错误信息需逐条译
# Who  : 各应用 po；译法对齐 KDE6 zh_CN 官方术语与通行译法
# When : po-standard-terms.py --apply + po-surgery 去重之后运行
# Where: 仓库根，python3 tools/apply-manual-translations.py
# How  : MANUAL_PER_PO: {po 相对路径: {msgid: 译文}} → 逐文件 surgery()
# 注：GPL 许可证全文、版权声明行、无意义拟声词（kodo 的 Goosh）按惯例
#     保持英文不译（KDE 官方 zh_CN 同样如此处理）
# ────────────────────────────────────────────────────────────────────
import sys, importlib.util
spec = importlib.util.spec_from_file_location(
    'po_surgery', '/home/user/debain_deb/kde1/tools/po-surgery.py')
ps = importlib.util.module_from_spec(spec); spec.loader.exec_module(ps)
surgery = ps.surgery

M = {}

M['kdebase/po/zh_CN.UTF-8/kfm.po'] = {
    'Cannot execute request.\n': '无法执行请求。\n',
    'Could not load': '无法加载',
    'ERROR: You dropped some URL over itself': '错误：您把一个 URL 拖放到了它自身上',
}

M['kdebase/po/zh_CN.UTF-8/kdehelp.po'] = {
    '&Find': '查找(&F)',
    'Font Size': '字体大小',
    'Fonts': '字体',
    'Miscellaneous': '杂项',
    'Open': '打开',
    'Stop': '停止',
    'Back': '后退',
}

M['kdebase/po/zh_CN.UTF-8/kcmdisplay.po'] = {
    '&Enter a name for the new color scheme\n': '输入新配色方案的名称(&E)\n',
    'File manager and desktop icons': '文件管理器和桌面图标',
    'Icon style': '图标样式',
    'Icons style': '图标样式',
    'Other': '其他',
    'Please choose a unique name for the new color\n': '请为新配色方案选择一个唯一的名称\n',
    'Sorry, this type of URL is currently unsupported': '抱歉，目前不支持这种类型的 URL',
    'The icon style change will not all be applied until you restart KDE.': '图标样式的更改要在重新启动 KDE 之后才能全部生效。',
    'any': '任意',
}

M['kdebase/po/zh_CN.UTF-8/kcmkeys.po'] = {
    '&Enter a name for the new key scheme\n': '输入新按键方案的名称(&E)\n',
    'Please choose a unique name for the new key\n': '请为新的按键方案选择一个唯一的名称\n',
}

M['kdebase/po/zh_CN.UTF-8/kfontmanager.po'] = {
    'The environment variable HOME\n': '环境变量 HOME\n',
    '~/.kde/share/config/kdefonts exists but\n': '~/.kde/share/config/kdefonts 已存在，但\n',
}

M['kdebase/po/zh_CN.UTF-8/kpager.po'] = {
    '%s\n\n': '%s\n\n',   # 纯格式透传（无自然语言成分，直抄格式串）
}

M['kdebase/po/zh_CN.UTF-8/konsole.po'] = {
    '&Color Schemes': '配色方案(&C)',
    '&General': '常规(&G)',
    '&Sessions': '会话(&S)',
    'Brightness': '亮度',
    'Color Schema Management': '配色方案管理',
    'Color Schemas define a palette of colors together with further specifications of the rendering.':
        '配色方案定义一组调色板以及进一步的渲染规格。',
    'Colourness': '彩色度',
    'Contrast': '对比度',
    'General Konsole settings': 'Konsole 常规设置',
    'Konsole Settings': 'Konsole 设置',
    'Session Management': '会话管理',
    'Sessions are actually commands that can be executed from within konsole.':
        '会话实际上是可以在 konsole 内执行的命令。',
    'This is work in progress.': '此功能仍在开发中。',
    'color schema selection': '配色方案选择',
    'intensive background color': '加亮背景色',
    'intensive foreground color': '加亮前景色',
    'intensive rgb color palette': '加亮 RGB 调色板',
    'regular background color': '普通背景色',
    'regular foreground color': '普通前景色',
    'regular rgb color palette': '普通 RGB 调色板',
    'usage:': '用法：',
    "{summary on konsole's general attributes.}": '{关于 konsole 常规属性的摘要。}',
}

M['kdeutils/po/zh_CN.UTF-8/ktop.po'] = {
    'Buffer (MB)': '缓冲 (MB)',
    'Cache (MB)': '缓存 (MB)',
    'Program (MB)': '程序 (MB)',
    'Swap (MB)': '交换 (MB)',
}

M['kdeutils/po/zh_CN.UTF-8/kedit.po'] = {
    'A Document with this Name exists already\n': '已存在同名文档\n',
    'Beginning of document reached.\n': '已到达文档开头。\n',
    'Could not pipe the contents': '无法通过管道输出内容',
    'Could not save the document.\n': '无法保存文档。\n',
    'Could not save the file.\n': '无法保存文件。\n',
    'End of document reached.\n': '已到达文档末尾。\n',
    'KEdit is already waiting\n': 'KEdit 已在等待\n',
    'The current Document has been modified.\n': '当前文档已被修改。\n',
    'The current document has been modified.\n': '当前文档已被修改。\n',
    'There are windows with modified content open.\n': '还有内容已被修改的窗口处于打开状态。\n',
    'This Document has been modified.\n': '文档已被修改。\n',
    'You do not have write permission ': '您没有写权限 ',
}

M['kdeutils/po/zh_CN.UTF-8/kwrite.po'] = {
    'KWrite is already waiting\nfor an internet job to finish\n':
        'KWrite 已在等待\n一个网络任务完成\n',
    'Color:': '颜色：',
}

M['kdeutils/po/zh_CN.UTF-8/karm.po'] = {
    '&New': '新建(&N)',
}

M['kdeutils/po/zh_CN.UTF-8/kcalc.po'] = {
    "Due to broken glibc's everywhere, ": '由于各处的 glibc 都有缺陷，',
}

M['kdeutils/po/zh_CN.UTF-8/kfloppy.po'] = {
    'Cannot access floppy or floppy drive\n': '无法访问软盘或软盘驱动器\n',
    'Cannot format: ': '无法格式化：',
    'The floppy was sucessfully formatted.\n': '软盘格式化成功。\n',
}

M['kdeutils/po/zh_CN.UTF-8/kjots.po'] = {
    '&Delete Page': '删除页(&D)',
    'Add current book to hotlist': '将当前书加入热点列表',
    'Are you sure you want to delete the current book ?': '确定要删除当前的书吗？',
    'Delete current book': '删除当前的书',
    'File already exists. \n Do you want to overwrite it ?': '文件已存在。\n要覆盖它吗？',
    'Font for editor:': '编辑器字体：',
    'Remove current book from hotlist': '从热点列表移除当前的书',
    'Save current book': '保存当前的书',
}

M['kdeutils/po/zh_CN.UTF-8/knotes.po'] = {
    'A KNotes note with this name already exists\n': '已存在同名的 KNotes 便签\n',
    'Are you sure you ': '确定要',   # 拼接句前半段
    'Could not save a KNote.\n': '无法保存便签。\n',
    'Could not save the KNotes.\n': '无法保存 KNotes。\n',
    'KNotes is already waiting\n': 'KNotes 已在等待\n',
    'The name "xyalarms" is reserved for internal usage.\n': '名称 "xyalarms" 保留为内部使用。\n',
}

M['kdegames/po/zh_CN.UTF-8/kpat.po'] = {
    'This will end the current game\n': '这将结束当前游戏\n',
}

M['kdegames/po/zh_CN.UTF-8/konquest.po'] = {
    # 版权行保留英文，仅译描述部分不适用（整条均为版本版权块）——保持英文
}

M['kdegames/po/zh_CN.UTF-8/kreversi.po'] = {
    # 纯版权行——保持英文
}

M['kdegraphics/po/zh_CN.UTF-8/kdvi.po'] = {
    'File status changed.': '文件状态已改变。',
    'Preferences ...': '首选项...',
    'Small text button set to shrink factor ': '小文字按钮设为缩放系数 ',
}

M['kdegraphics/po/zh_CN.UTF-8/kfax.po'] = {
    'About KFax': '关于 KFax',
    "Failure in 'copy file()'\n": "执行 'copy file()' 失败\n",
    'KFax is already waiting\n': 'KFax 已在等待\n',
    'You must enter a file name if you wish to ': '如果您想要',
    'You must enter a print command such as "lpr"\n ': '您必须输入打印命令，例如 "lpr"\n ',
    'Invalid page dimensions:\nWidth %.2f Height %.2f\n': '页面尺寸无效：\n宽 %.2f 高 %.2f\n',
    'Can\'t print to "%s"\n': '无法打印到 "%s"\n',
    "Couldn't create %s\n": '无法创建 %s\n',
    "Failure in 'copy file()'\nCouldn't save file!": "执行 'copy file()' 失败\n无法保存文件！",
    'KFax is already waiting\nfor an internet job to finish\nPlease wait until has finished\n':
        'KFax 已在等待\n一个网络任务完成\n请稍候直至其结束\n',
    'A file with this name already exists\nDo you want to overwrite it?\n\n':
        '同名文件已存在\n要覆盖它吗？\n\n',
    'Unable to open:\n%s\n': '无法打开：\n%s\n',
    'You must enter a print command such as "lpr"\n if you wish to print to a printer.':
        '如果您想要打印到打印机，必须输入打印命令，例如 "lpr"\n。',
    'Go to end': '转到末尾',
    'Zoom in': '放大',
}

M['kdegraphics/po/zh_CN.UTF-8/kfract.po'] = {
    'Fract::calcPoint() is an abstract method. You MUST\nre-write it!\n':
        'Fract::calcPoint() 是抽象方法。您必须\n重写它！\n',
}

M['kdegraphics/po/zh_CN.UTF-8/kghostview.po'] = {
    'Already waiting\n': '已在等待\n',
    'Error opening file': '打开文件出错',
    'No file name was given so\n': '未给出文件名，因此\n',
    'No file name was specified.\n': '未指定文件名。\n',
    'Printing failed because the list of\n': '打印失败，因为',
    'See KDE and Qt documentation for\n': '详见 KDE 与 Qt 文档，',
    'The range of pages entered for printing\n': '输入的打印页范围',
    'Set Environment variable (to PRINTER or LPDEST for instance)for Printer name to be taken into account.':
        '设置环境变量（例如 PRINTER 或 LPDEST），使打印机名称生效。',
    '&Zoom factor': '缩放系数(&Z)',
    'Remove page marks': '移除页面标记',
    'Usage: ': '用法：',
    # GPL 全文/版权块按 KDE 官方惯例保持英文原文
}

M['kdegraphics/po/zh_CN.UTF-8/kpaint.po'] = {
    'Image Depth:': '图像深度：',
    '1 (2 Colours)': '1（2 色）',
    '4 (16 Colours)': '4（16 色）',
    '8 (256 Colours)': '8（256 色）',
    '15 (32k Colours)': '15（32k 色）',
    '24 (True Colour)': '24（真彩色）',
    'Roundangle': '圆角',
}

M['kdegraphics/po/zh_CN.UTF-8/ksnapshot.po'] = {
    'KSnapshot was unable to save the image to\n%s.':
        'KSnapshot 无法将图像保存到\n%s。',
}

M['kdenetwork/po/zh_CN.UTF-8/kmail.po'] = {
    ', %_=space': '，%_=空格',
    'Automatically send receive- and read confirmations': '自动发送接收和已读确认',
    'Compact all folders': '压缩全部文件夹',
    'Compact all folders on exit': '退出时压缩全部文件夹',
    "Failed to create temporary attachment directory '%s': %s": '创建临时附件目录 "%s" 失败：%s',
    'execute': '执行',
    'forward to': '转发到',
    '&Qute unknonw characters': '引用未知字符(&Q)',   # 源码拼写错误保留
}

M['kdenetwork/po/zh_CN.UTF-8/knu.po'] = {
    'TQt': 'TQt',
    'TQt3 runtime': 'TQt3 运行时',
}

M['kdenetwork/po/zh_CN.UTF-8/kppp.po'] = {
    'You have launched pppd before the remote server ': '您在远程服务器',
    'Hayes(tm) compatible modem': 'Hayes(tm) 兼容调制解调器',
}

M['kdenetwork/po/zh_CN.UTF-8/krn.po'] = {
    'Decode All': '全部解码',
    'Destination directory:': '目标目录：',
    'ID': 'ID',
    'KRN - Decoder': 'KRN - 解码器',
    'KRN-Decoder': 'KRN 解码器',
}

M['kdenetwork/po/zh_CN.UTF-8/kfinger.po'] = {
    'Output parameters ': '输出参数 ',
}

M['kdetoys/po/zh_CN.UTF-8/kworldwatch.po'] = {
    '&Add a flag': '添加旗标(&A)',
    '&Delete nearest flag': '删除最近的旗标(&D)',
    'KDE World Wide Watch': 'KDE 世界时钟',
    'Toggle &Illumination': '切换日照(&I)',
    'Toggle &Simulation': '切换模拟(&S)',
    '  GMT Time: %x, %H:%M:%S': '  格林威治时间：%x, %H:%M:%S',
    'Local time: %x, %H:%M:%S': '本地时间：%x, %H:%M:%S',
    'Annotate the nearest flag': '注释最近的旗标',
    'Annotation': '注释',
    'A&nnotate flag...': '注释旗标(&N)...',
    'About &World Watch...': '关于世界时钟(&W)...',
    'About KDE World Watch': '关于 KDE 世界时钟',
}

if __name__ == '__main__':
    ROOT = '/home/user/debain_deb/kde1/'
    for porel, table in M.items():
        if not table:
            continue
        path = ROOT + porel
        ndup, nhit = surgery(path, table)
        # surgery 只原位替换——po 里尚不存在的 MISSING 词条在此追加
        with open(path, encoding='utf-8') as f:
            content = f.read()
        import re as _re
        def _unq(s):
            return s.replace('\\n', '\n').replace('\\"', '"').replace('\\\\', '\\')
        def _esq(s):
            return s.replace('\\', '\\\\').replace('"', '\\"').replace('\n', '\\n')
        have = set(_unq(m) for m in _re.findall(r'^msgid\s+"((?:[^"\\]|\\.)*)"', content, _re.M))
        missing = {k: v for k, v in table.items() if k not in have}
        if missing:
            with open(path, 'a', encoding='utf-8') as f:
                f.write('\n# [KDE1 Revival 2026-08-31] 应用特有词条人工补译\n')
                for k, v in missing.items():
                    f.write(f'msgid "{_esq(k)}"\nmsgstr "{_esq(v)}"\n\n')
        print(f'{porel}: 原位替换{nhit}/{len(table)} 追加{len(missing)}')
