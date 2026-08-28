#!/bin/bash
# ┌─ What : 从 tqt3/ 源码头文件中提取全部 TQ 前缀类名，生成 Q→TQ 类名映射表
# │         port/q1compat_map.h（q1compat.h 的自动生成部分）。
# │  Why  : TQt3 把 Qt3 类名全部改为 TQ 前缀（TQString/TQWidget…），KDE 1.1.2 源码
# │         用的是 Qt1 时代的 Q 前缀名。本映射是「迁移脚手架」（strangler fig 模式）
# │         的一部分：让 54.7 万行 KDE1 源码先在 TQt3 上编译通过、系统先跑起来，
# │         之后逐模块摘除——每摘一个模块，就把该模块源码显式 TQ 化（类名直改），
# │         并关闭它对 -include q1compat.h 的依赖。终态 port/ 整体拆除，绝不留在
# │         最终架构里（前车之鉴：TDE 背着 tqtinterface 永久层 18 年迁不成 Qt4；
# │         兼容层的 BUG 是语义暗改型，无处下锚）。
# │  Who  : 本脚本由维护者手工执行（tqt3 版本更新后必须重跑）；产物 q1compat_map.h
# │         随仓库入库（仅脚手架存续期内有效）；尚在脚手架上的模块编译命令经
# │         -include q1compat.h 注入映射。
# │  When : ① 首次建立脚手架时；② tqt3/ 快照被替换为新版本后；③ 全部模块显式化
# │         完成后本脚本随 port/ 一并删除。
# │  Where: 仓库根目录执行 ./port/gen_q1compat.sh；只读 tqt3/，只写 port/。
# │  How  : 伪代码：
# │         1. 扫描 tqt3/src/**/ntq*.h 与 attic 头，正则抓取 "class TQxxx" /
# │            "struct TQxxx" 声明，去重排序得类名全集
# │         2. 过滤黑名单（见下方 BLACKLIST，Qt1→Qt3 语义改名、不能盲映射的类）
# │         3. 对每个 TQXxx 输出 "#define QXxx TQXxx"
# │         4. 追加 Qt1→Qt3 语义改名映射（QList→TQPtrList 等，见 SPECIAL_MAP）
# │         5. 写入 port/q1compat_map.h，带文件头说明
# ─────────────────────────────────────────────────────────────────────────────
# 注：不用 set -e/pipefail——提取管道在 EOF/空匹配下返回非零属正常，严格模式
# 会把产物写成半截文件（曾导致 #endif 缺失、下游编译雪崩）；改在尾部做完整性自检
set -u
cd "$(dirname "$0")/.."
TQT3_SRC=tqt3
OUT=port/q1compat_map.h

# 黑名单：这些名字在 Qt1 与 Qt3 中语义/继承体系不同，盲映射会编译错或静默错位，
# 由 q1compat.h 单独处理（包装类/专门映射/源码适配）：
#   QList/QListIterator   Qt1 节点链表  → Qt3 是 TQPtrList
#   QArray/QArrayIterator Qt1 数组      → Qt3 是 TQMemArray
#   QCollection           Qt1 集合基类  → Qt3 是 TQPtrCollection
#   QFont                 Qt1 带 CharSet 体系 → TQFont 无 charset 概念，
#                         由 q1compat.h 的包装类提供（继承 TQFont + 枚举兼容）
#   QCache/QGDict/QGCache/QGVector/QGList Qt1 内部基类 → Qt3 对应 TQPtr*/TQG* 名
BLACKLIST_RE='^TQ(List|Array|Collection|Font|FontInfo|Cache|GDict|GCache|GVector|GList)$'

# Qt1→Qt3 语义改名映射（黑名单类的正确去向）
SPECIAL_MAP=(
  "QList TQPtrList"
  "QListIterator TQPtrListIterator"
  "QArray TQMemArray"
  "QArrayIterator TQMemArrayIterator"
  "QCollection TQPtrCollection"
  "QStack TQPtrStack"
  "QQueue TQPtrQueue"
)

{
  echo '/*'
  echo ' * q1compat_map.h — Q→TQ 类名映射表（自动生成，勿手改）'
  echo ' *'
  echo ' * 【迁移脚手架·非最终架构】由 port/gen_q1compat.sh 从 tqt3/ 源码提取生成。'
  echo ' * 用途：让尚未显式 TQ 化的 KDE1 模块在 TQt3 上编译通过；每模块显式化后'
  echo ' * 即脱离此表，全部模块摘除完毕后本文件随 port/ 脚手架整体拆除。'
  echo ' * tqt3 快照更新后须重跑生成。黑名单类（QList/QArray 等语义改名者）'
  echo ' * 不在此表，见 q1compat.h 特判段。'
  echo ' */'
  echo '#ifndef Q1COMPAT_MAP_H'
  echo '#define Q1COMPAT_MAP_H'
  echo
  grep -rhoE "(class|struct)( +TQ_EXPORT)? +TQ[A-Za-z0-9_]+" \
      ${TQT3_SRC}/src/*/ntq*.h ${TQT3_SRC}/src/attic/*.h 2>/dev/null \
    | grep -oE "TQ[A-Za-z0-9_]+" | grep -v "^TQ_EXPORT$" | sort -u \
    | grep -vE "${BLACKLIST_RE}" \
    | while read -r cls; do
        # 剥离规则：TQt3 类名 = T + Qt 名（TQString = T + QString），
        # 故剥一个字符 T 得 Qt 名（QString）——映射即 #define QString TQString
        printf '#define %s %s\n' "${cls#T}" "$cls"
      done
  echo
  echo '/* Qt1→Qt3 语义改名映射（Qt1 名 → TQt3 实际类） */'
  for m in "${SPECIAL_MAP[@]}"; do
    set -- $m
    printf '#define %s %s\n' "$1" "$2"
  done
  echo
  echo '#endif // Q1COMPAT_MAP_H'
} > ${OUT}

# 完整性自检（What/Why）：产物必须以 #endif 收尾且含语义改名段——防止管道
# 中途异常把文件写成半截（下游 -include 时雪崩出上百个伪错误）
if ! grep -q "^#endif // Q1COMPAT_MAP_H" ${OUT} || ! grep -q "define QList TQPtrList" ${OUT}; then
    echo "错误：${OUT} 生成不完整（缺 #endif 或语义改名段），已中止" >&2
    exit 1
fi
echo "已生成 ${OUT}：$(grep -c '^#define' ${OUT}) 条映射"
