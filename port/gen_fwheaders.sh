#!/bin/bash
# ┌─ What : 生成 port/include/ 下的 Qt1 缩写头转发集（qapp.h → ntqapp.h →
#           ntqapplication.h）、qkeycode.h 键码映射、qdropsite.h 空 mixin。
# │  Why  : KDE 1.1.2 源码大量 #include Qt1 缩写头（qpushbt.h 264 处、qapp.h 96 处…），
# │         TQt3 只提供 nt 前缀的 compat 转发（ntqapp.h），名字对不上；Qt1 的键码
# │         是全局宏（Key_Escape 0x1000），TQt3 是 TQt 命名空间枚举成员；Qt1 的
# │         QDropSite 拖放 mixin 类在 Qt3 已删除（拖放并入 QWidget）。
# │  Who  : 属迁移脚手架（strangler fig）——服务于尚未显式 TQ 化的 KDE1 模块；
# │         模块显式化后其编译不再经过这些头，全部摘除完毕随 port/ 整体拆除。
# │  When : ① 首次搭建脚手架；② tqt3/ 快照更新后重跑（转发目标可能变化）。
# │  Where: 仓库根目录执行 ./port/gen_fwheaders.sh；读 qt1/include 与 tqt3/src/compat，
# │         只写 port/include/。
# │  How  : 伪代码：
# │         1. 清空 port/include/
# │         2. 对 tqt3/src/compat/ntq*.h 每个转发头 F：
# │              生成 port/include/<去 nt 前缀名>：#include "<F>"（带 include guard）
# │         3. qkeycode.h：从 qt1/include/qkeycode.h 提取全部 "#define Key_XXX"
# │              宏名 → 生成 "#define Key_XXX TQt::Key_XXX"（Qt1/Qt3 键名与值
# │              一脉相承，逐一映射回枚举成员）
# │         4. qdropsite.h：定义空 mixin 类 QDropSite（构造接 QWidget* 但不做
# │              任何事——Qt3 的拖放虚函数已在 QWidget 上，空 mixin 仅补齐
# │              "class X : public QWidget, public QDropSite" 的多继承语法位）
# ─────────────────────────────────────────────────────────────────────────────
set -euo pipefail
cd "$(dirname "$0")/.."
OUT=port/include
mkdir -p ${OUT}
rm -f ${OUT}/*.h

HDR='/*
 * %s — Qt1→TQt3 迁移脚手架转发头（自动生成，勿手改）
 * 由 port/gen_fwheaders.sh 生成；模块显式 TQ 化后随 port/ 脚手架整体拆除。
 */
#ifndef Q1FW_%s
#define Q1FW_%s
#include "%s"
#endif
'

# 1. 缩写头转发（qapp.h → ntqapp.h）
for f in tqt3/src/compat/ntq*.h; do
    base=$(basename "$f")            # ntqapp.h
    short=${base#nt}                 # qapp.h
    name=${short%.h}                 # qapp
    guard=$(echo "$short" | tr 'a-z.' 'A-Z_')
    printf "$HDR" "$short" "$guard" "$guard" "$base" > ${OUT}/${short}
done

# 1b. 长名头转发（qdir.h → ntqdir.h）
# What/Why：TQt3 源码只有 nt 前缀头（上游 make-symlinks.sh 也只链接 nt 名），
#      长名 q*.h 实体从未存在；KDE1 全库以长名 include（qstring.h 237 处、
#      qwidget.h 187 处…），故对 tqt3/include/ntq*.h 全集剥前缀生成转发。
#      （与缩写转发重复的名字内容等价，后者覆盖前者，无冲突）
for f in tqt3/include/ntq*.h; do
    base=$(basename "$f")            # ntqdir.h
    short=${base#nt}                 # qdir.h
    name=${short%.h}
    guard=$(echo "$short" | tr 'a-z.' 'A-Z_')
    printf "$HDR" "$short" "$guard" "$guard" "$base" > ${OUT}/${short}
done

# 1c. attic 控件转发（qtableview.h → port/src/qttableview.h）
# What/Why：TQt3 r14.1.6 上游删除了 src/attic/（但 include/ 的符号链接
# 残留悬垂——上游遗留 bug）；QTableView 是 KDE1 的核心控件（81 处使用，
# KTreeList/KDateTable/KColorCells/KTabListBox 等继承它），attic 实现从
# tqt3 r14.0.0 搬运保存于 port/src/（原版权头保留，TQ 化形态）。
# 类名映射 #define QTableView TQtTableView 见 q1compat.h。
cat > ${OUT}/qtableview.h <<'EOF'
/*
 * qtableview.h — QTableView 转发头（迁移脚手架）
 * TQt3 r14.1.6 已删 attic；实现搬运自 tqt3 r14.0.0 src/attic/（port/src/）。
 * 模块显式化后随 port/ 脚手架拆除。
 */
#ifndef Q1FW_QTABLEVIEW_H
#define Q1FW_QTABLEVIEW_H
#include "../src/qttableview.h"
#endif
EOF

# 1d. Qt1→Qt3 改名头转发（qlist.h → ntqptrlist.h 等）
# What/Why：QList/QStack 在 Qt3 改名 QPtrList/QPtrStack（头文件相应改名），
#      tqt3 无 ntqlist.h/ntqstack.h，KDE1 的 include 会 404——补转发
#     （后续改名头按需在此追加）
cat > ${OUT}/qlist.h <<'EOF'
/*
 * qlist.h — Qt1 QList → TQt3 TQPtrList 转发头（迁移脚手架）
 * Qt3 里 QList 改名 QPtrList；类名映射见 q1compat_map.h 语义改名段。
 */
#ifndef Q1FW_QLIST_H
#define Q1FW_QLIST_H
#include "ntqptrlist.h"
#endif
EOF
cat > ${OUT}/qstack.h <<'EOF'
/*
 * qstack.h — Qt1 QStack → TQt3 TQPtrStack 转发头（迁移脚手架）
 */
#ifndef Q1FW_QSTACK_H
#define Q1FW_QSTACK_H
#include "ntqptrstack.h"
#endif
EOF
cat > ${OUT}/qpaintd.h <<'EOF'
/*
 * qpaintd.h — Qt1 缩写转发（迁移脚手架）
 */
#ifndef Q1FW_QPAINTD_H
#define Q1FW_QPAINTD_H
#include "ntqpaintdevice.h"
#endif
EOF
cat > ${OUT}/qtablevw.h <<'EOF'
/*
 * qtablevw.h — Qt1 QTableView 短名转发（迁移脚手架，与 qtableview.h 同体）
 */
#ifndef Q1FW_QTABLEVW_H
#define Q1FW_QTABLEVW_H
#include "../src/qttableview.h"
#endif
EOF

# 2. qkeycode.h 键码映射（Qt1 全局宏 → TQt 枚举成员）
{
    echo '/*'
    echo ' * qkeycode.h — Qt1 键码宏 → TQt3 枚举映射（迁移脚手架，自动生成）'
    echo ' * Qt1: #define Key_Escape 0x1000（全局宏）；TQt3: TQt::Key_Escape 枚举。'
    echo ' * 键名与键值 Qt1→Qt3 一脉相承，机械映射；模块显式化后随 port/ 拆除。'
    echo ' */'
    echo '#ifndef Q1FW_QKEYCODE_H'
    echo '#define Q1FW_QKEYCODE_H'
    echo '#include <ntqnamespace.h>'
    grep -oE '#define +Key_[A-Za-z0-9_]+' qt1/include/qkeycode.h \
        | grep -oE 'Key_[A-Za-z0-9_]+' | sort -u \
        | while read -r key; do
            printf '#define %s TQt::%s\n' "$key" "$key"
          done
    echo '#endif'
} > ${OUT}/qkeycode.h

# 3. qdropsite.h 空 mixin（Qt3 已将拖放并入 QWidget）
cat > ${OUT}/qdropsite.h <<'EOF'
/*
 * qdropsite.h — Qt1 QDropSite 空 mixin（迁移脚手架，自动生成配套）
 * Qt1 的拖放靠 "class X : public QWidget, public QDropSite" 多继承混入；
 * Qt3 起拖放（dragEnterEvent 等）已是 QWidget 原生虚函数，QDropSite 被删除。
 * 此处仅保留空壳补齐多继承语法位，行为全部落到 TQWidget 原生实现上。
 * 模块显式化（源码去掉 QDropSite 继承）后随 port/ 脚手架拆除。
 */
#ifndef Q1FW_QDROPSITE_H
#define Q1FW_QDROPSITE_H
#include <ntqwidget.h>
class QDropSite {
public:
    QDropSite( TQWidget * ) {}
};
#endif
EOF

echo "已生成 $(ls ${OUT} | wc -l) 个转发头于 ${OUT}/"
