/****************************************************************************
** q1compat.h — KDE 1.1.2 → TQt3 迁移脚手架主头（strangler fig 模式）
**
** 【定位·务必先读】这是脚手架，不是建筑。
**   What : 为尚未显式 TQ 化的 KDE1 模块提供编译期名字映射——Qt1 时代的
**          Q 前缀类名 / qt_x* X11 内部函数 / QT_VERSION 等映射到 TQt3 的
**          TQ 前缀现实，使 54.7 万行历史源码无需大爆炸式改写即可编译。
**   Why  : TQt3 把 Qt3 类名全部 TQ 前缀化（TQString/TQWidget…），Qt1 的
**          QList/QArray 等容器在 Qt3 改名（TQPtrList/TQMemArray）；若不用
**          映射，迁移期存在漫长的「全库不可编译中间态」。本头把中间态压成
**          「逐模块短过渡」：某模块源码显式 TQ 化完成后，其编译命令去掉
**          -include q1compat.h 即完成摘除——每摘一块，那块代码就是真正的
**          TQt3 代码。全部模块摘除后，port/ 脚手架整体拆除，不留在终态
**          架构里（前车之鉴：TDE 的 tqtinterface 永久层背上 18 年，
**          Qt4 至今未迁成；兼容层的 BUG 属语义暗改型、无处下锚）。
**   Who  : 各 KDE1 模块的 CMake 编译命令以 -include 注入本头；显式化完成
**          的模块不再注入。本头不进入 tqt3/（底座零修改是章程红线）。
**   When : 编译期最前注入（先于任何 nt 头与 KDE 头）；随 port/ 脚手架消亡。
**   Where: port/q1compat.h；映射表 port/q1compat_map.h（自动生成）；
**          缩写头转发 port/include/（自动生成）。
**   How  : 伪代码：
**     1. define Qt TQt（枚举命名空间）
**     2. 载入 q1compat_map.h：507+ 条 #define QXxx TQXxx 类名映射 +
**        语义改名映射（QList→TQPtrList、QArray→TQMemArray、
**        QCollection→TQPtrCollection、QCache→TQCache…）
**     3. X11 内部函数映射：qt_xdisplay→tqt_xdisplay、qt_xrootwin→tqt_xrootwin、
**        qt_xscreen→tqt_xscreen
**     4. 全局对象/函数映射：qApp→tqApp、qRed/qGreen/qBlue/qGray/qVersion→tq*
**     5. moc 关键字宏映射（仅编译器侧；tqmoc 不展开宏，moc 输入由构建管线
**        预先 sed：Q_OBJECT→TQ_OBJECT 等）：Q_OBJECT/Q_PROPERTY/Q_ENUMS/
**        Q_SETS/Q_CLASSINFO/Q_OVERRIDE → TQ_*
**     6. QT_VERSION→TQT_VERSION（历史代码的 #if QT_VERSION 分支判断保持语义）
**
** 【UTF-8 全局策略（本脚手架最重要的运行期决策，详见 README）】
**   迁移期在 KApplication 构造中执行
**       TQTextCodec::setCodecForCStrings(TQTextCodec::codecForName("UTF-8"));
**   使全部 char*→TQString 隐式转换按 UTF-8 解释——KDE1 源码中 UTF-8 字节
**   字面量经此通道一次性获得正确语义，无需逐点 fromUtf8。不经此通道的
**   例外点（system() 参数、环境变量、QFile 原始字节流、X11 属性、协议
**   字节）由 Qt1 路线勘定的语义地图平移复审。
**
** This file is part of the KDE1 Revival Project's migration scaffold.
** Written with GLM-5.3 (Z.ai) for the KDE1 Revival Project, 2026
** 不含第三方版权内容；可自由随项目分发与修改。
*****************************************************************************/

#ifndef Q1COMPAT_H
#define Q1COMPAT_H

/* TQt3 命名空间与基础（先落位，后续映射可能引用其类型） */
#include <ntqglobal.h>
/* ntqstring 必须最先完整落位（What/Why）：后续 nt 头存在循环引用
 *（ntqfont→…→ntqevent→ntqobject→…→ntqtl→ntqtextstream 回引 TQChar），
 * 若 ntqstring 因中途进入而半完成，下游全链报 incomplete type */
#include <ntqstring.h>
/* 高频容器兜底 include（What/Why）：Qt1 头互相传递 include 的习惯（qwidget.h
 * 曾带出 qstrlist.h 等）在 TQt3 的干净头下不复存在，而 KDE1 的头文件
 *（kconfigbase.h 等）裸用 QStrList/QFile 却不自 include——q1compat 是全部
 * 编译单元的最前注入点，在此统一兜底，免逐头修补（显式化阶段自然消亡） */
#include <ntqstrlist.h>
#include <ntqfile.h>
/* 常用部件/基础类兜底（同上理由）：TQCursor/TQTimer/TQStyle 在 KDE1 头里
 * 裸用而不自 include（Qt1 时代靠传递），补进兜底集 */
#include <ntqpainter.h>
/* 常用部件兜底（KDE1 头裸用作成员/前向而未 include，靠 Qt1 时代传递） */
#include <ntqcombobox.h>
#include <ntqslider.h>
#include <ntqlabel.h>
#include <ntqcheckbox.h>
#include <ntqpushbutton.h>
#include <ntqlineedit.h>
#include <ntqgroupbox.h>
#include <ntqradiobutton.h>
#include <ntqcursor.h>
#include <ntqtimer.h>
#include <ntqstyle.h>

/* Qt1 qwindefs.h 的定宽整数 typedef（TQt3 已无此头；KDE1 的 krootprop 等
 * 以 UINT8 书写） */
typedef unsigned char  UINT8;
typedef unsigned short UINT16;
typedef unsigned int   UINT32;
typedef signed char    INT8;
typedef short          INT16;
typedef int            INT32;

/* ── 1. 枚举命名空间与布尔字面量 ─────────────────────────────────── */
#define Qt TQt
/* TQt3 r14.1.x 删除了 Qt1/Qt3 时代的 FALSE/TRUE 大写宏（现代化清理）；
 * KDE1 全库以 FALSE/TRUE 书写——映射到 C++ 布尔字面量（Qt1 的 FALSE 即 0，
 * bool 语义等价；C++98 下 int↔bool 隐转兼容历史代码的所有用法） */
#define FALSE false
#define TRUE true

/* ── 2. 类名映射（自动生成表：全量 QXxx→TQXxx + 语义改名） ────────── */
#include "q1compat_map.h"

/* ── 2b. QFont 包装类（Qt1 CharSet 体系兼容） ────────────────────────
 * What : 继承 TQFont 的薄包装，补回 Qt1 的 QFont::CharSet 枚举与
 *        setCharSet()/charSet() 接口（枚举值沿 Qt1 qfont.h 原定义）。
 * Why  : TQt3 无 charset 概念（TQString 即 Unicode，渲染按码点找字形），
 *        KDE1 全库 94 处 QFont::Latin 系列、ISO_8859 系列、AnyCharSet 引用与
 *        kcharsets 体系的 QFont::CharSet 签名依赖这些名字；包装类让它们
 *        全部编译通过且行为统一为「无操作 / 恒 AnyCharSet」——UTF-8 世界
 *        里 charset 转换层失去存在意义（对应 Qt1 路线的透传补丁）。
 * Who  : 所有以 QFont 名字使用字体的 KDE1 代码（因 QFont 不在映射表、
 *        解析到此包装类）；对 TQt3 API 透明——派生引用自动绑定基类。
 * When : 编译期；摘除脚手架时随源码显式化（charset 调用点删除）消亡。
 * How  : 伪代码：
 *   1. class QFont : public TQFont——继承全部真行为
 *   2. enum CharSet { Latin1…KOI8R }（Qt1 原值照抄）
 *   3. 构造族：默认/四参（char* 家族经 codecForCStrings 隐转 TQString）/
 *      五参（末位 CharSet 丢弃）/ 基类拷贝（承接 widget->font() 等返回值）
 *   4. setCharSet(CharSet) 空实现；charSet() 恒返 AnyCharSet
 */
#include <ntqfont.h>
class QFont : public TQFont {
public:
    enum CharSet {
        Latin1, ISO_8859_1 = Latin1, AnyCharSet,
        Latin2, ISO_8859_2 = Latin2,
        Latin3, ISO_8859_3 = Latin3,
        Latin4, ISO_8859_4 = Latin4,
        Latin5, ISO_8859_5 = Latin5,
        Latin6, ISO_8859_6 = Latin6,
        Latin7, ISO_8859_7 = Latin7,
        Latin8, ISO_8859_8 = Latin8,
        Latin9, ISO_8859_9 = Latin9,
        KOI8R
    };
    QFont() : TQFont() {}
    QFont( const char *family, int pointSize = 12,
           int weight = Normal, bool italic = FALSE )
        : TQFont( family, pointSize, weight, italic ) {}
    // TQString 家族版（What/Why）：基类 TQFont 有 (const TQString&,…) 构造且
    // 非显式，TQString 实参经它会与 const char* 版产生转换序列歧义——
    // 包装类提供精确匹配版以消歧（char* 与 TQString 两条路都直达基类构造）
    QFont( const TQString &family, int pointSize = 12,
           int weight = Normal, bool italic = FALSE )
        : TQFont( family, pointSize, weight, italic ) {}
    QFont( const char *family, int pointSize,
           int weight, bool italic, CharSet )
        : TQFont( family, pointSize, weight, italic ) {}
    QFont( const TQFont &f ) : TQFont( f ) {}
    QFont &operator=( const TQFont &f ) { TQFont::operator=( f ); return *this; }
    void setCharSet( CharSet ) {}
    CharSet charSet() const { return AnyCharSet; }
};

/* QFontInfo 包装（同 QFont：补 charSet() 查询，恒 AnyCharSet——
 * kcharsets 体系的 QFontInfo::charSet() 调用点零改动编译） */
#include <ntqfontinfo.h>
class QFontInfo : public TQFontInfo {
public:
    QFontInfo( const QFont &f ) : TQFontInfo( f ) {}
    QFontInfo( const TQFontInfo &fi ) : TQFontInfo( fi ) {}
    QFont::CharSet charSet() const { return QFont::AnyCharSet; }
};

/* ── 2c. attic 控件映射（QTableView → TQtTableView） ────────────────
 * What : Qt1 的 QTableView 在 TQt3 r14.1.6 已随 attic 目录删除（上游
 *        include/ 符号链接悬垂为遗留 bug）；attic 实现自 tqt3 r14.0.0
 *        搬运至 port/src/qttableview.{h,cpp}，类名为 TQtTableView。
 * Why  : KDE1 有 81 处 QTableView 使用（KTreeList/KDateTable/KColorCells/
 *        KTabListBox 直接继承）——宏映射 + port/include/qtableview.h
 *        转发头让它们零改动编译。
 * When : 编译期；模块显式化（改用 TQListView 等现代控件）后消亡。
 */
#define QTableView    TQtTableView
#define QtTableView   TQtTableView

/* ── 3. X11 内部函数映射（Qt1 qt_x* → TQt3 tqt_x*） ────────────────
 * Who : kwm/kfm/kvt/kpanel 等直接混用 Xlib 的模块（qt_xdisplay 全库 618 处）
 * Why : TQt3 把这些 X11  internals 全部改名 tqt_ 前缀且签名未变，机械映射
 */
#define qt_xdisplay   tqt_xdisplay
#define qt_xrootwin   tqt_xrootwin
#define qt_xscreen    tqt_xscreen
#define qt_xget_temp_gc        tqt_xget_temp_gc
#define qt_xget_readonly_gc    tqt_xget_readonly_gc

/* ── 4. 全局对象与工具函数映射 ───────────────────────────────────── */
#define qApp      tqApp
#define qRed      tqRed
#define qGreen    tqGreen
#define qBlue     tqBlue
#define qGray     tqGray
#define qVersion  tqVersion

/* ── 4b. Qt1 全局函数/调试宏映射（TQt3 已 tq 化或删除者） ───────────
 * What : warning()/debug() 是 Qt1 全局输出函数（TQt3 改名 tqWarning/tqDebug，
 *        KDE1 的 ksock/kprocess 等直接调用）；CHECK_PTR/ASSERT 调试宏在
 *        TQt3 改为 TQ_CHECK_PTR/TQ_ASSERT。
 * Why  : KDE1 六模块以 Qt1 名字书写——全局映射让它们零改动编译。
 *        误伤排查已做：全库无 void warning(/void debug( 裸声明
 *        （debugC/debugT/debugM 等带后缀名不受整词宏影响）。
 */
/* warning/debug/fatal 不设宏（曾设后误伤 TQMessageBox::warning 等限定
 * 调用）——裸调用点已显式化为 tqWarning/tqDebug/tqFatal */
#define CHECK_PTR TQ_CHECK_PTR  // TQt3 实际存在的是 TQ_CHECK_PTR
/* ASSERT：ntqglobal.h 在部分配置下自带兼容定义——先清再定，避免重定义 */
#undef ASSERT
#define ASSERT    Q_ASSERT

/* Qt1 工具宏/函数 → TQt3 对应（QRgb 是 typedef 不经生成表；QMIN/QMAX/QABS
 * 宏名 tq 化；stricmp 是 Qt1 提供的 MSVC 风格比较，POSIX 对应 strcasecmp；
 * qstrdup → tqstrdup） */
#define QRgb      TQRgb
#define QMIN(a,b) TQMIN(a,b)
#define QMAX(a,b) TQMAX(a,b)
#define QABS(a)   TQABS(a)
#define stricmp   strcasecmp
#define strnicmp  strncasecmp
#define qstrdup   tqstrdup
#define qRgb      tqRgb
#define qAddPostRoutine tqAddPostRoutine
#define qSetRgb   tqSetRgb

/* ── 4e. Qt1 全局事件枚举映射（Event_Xxx → TQEvent::Xxx） ──────────
 * What/Why：Qt1 的 QEvent 枚举成员以全局 Event_ 前缀可见（Event_KeyPress 等），
 * TQt3 收敛为 TQEvent::KeyPress 命名空间形态；KDE1 六模块 40+ 处直接使用。
 */
#include <ntqevent.h>
#define Event_Timer                      TQEvent::Timer
#define Event_MouseButtonPress           TQEvent::MouseButtonPress
#define Event_MouseButtonRelease         TQEvent::MouseButtonRelease
#define Event_MouseButtonDblClick        TQEvent::MouseButtonDblClick
#define Event_MouseMove                  TQEvent::MouseMove
#define Event_KeyPress                   ((TQEvent::Type)6)  // KeyPress 撞 X11 宏，用 ABI 数值
#define Event_KeyRelease                 ((TQEvent::Type)7)  // KeyRelease 撞 X11 宏
#define Event_FocusIn                    ((TQEvent::Type)8)  // FocusIn 撞 X11 宏
#define Event_FocusOut                   ((TQEvent::Type)9)  // FocusOut 撞 X11 宏
#define Event_Enter                      TQEvent::Enter
#define Event_Leave                      TQEvent::Leave
#define Event_Move                       TQEvent::Move
#define Event_Resize                     TQEvent::Resize
#define Event_Show                       TQEvent::Show
#define Event_Hide                       TQEvent::Hide
#define Event_Paint                      TQEvent::Paint
#define Event_Close                      TQEvent::Close
#define Event_Quit                       TQEvent::Quit
#define Event_ChildInserted              TQEvent::ChildInserted
#define Event_ChildRemoved               TQEvent::ChildRemoved
#define Event_LayoutHint                 TQEvent::LayoutHint
#define Event_ActivateControl            TQEvent::ActivateControl
#define Event_Drop                        TQEvent::Drop
#define Event_DragEnter                   TQEvent::DragEnter
#define Event_DragLeave                   TQEvent::DragLeave
#define Event_DragMove                    TQEvent::DragMove
#define Event_DeactivateControl          TQEvent::DeactivateControl

/* ── 4g.（已撤）光标对象不设宏——撞 KCursor::arrowCursor() 等静态方法名，
 * 全局光标调用点直接写 TQt::arrowCursor 形式 */

/* ── 4h. Qt1 全局颜色对象（Qt3 已删；各翻译单元内部链接副本，ODR 安全） ──
 * 值取 Qt 时代标准色（与 Qt1 qcolor.cpp 一致） */
#include <ntqcolor.h>
#include <ntqcolor.h>
static const TQColor k1c_white(255,255,255);
static const TQColor k1c_black(0,0,0);
static const TQColor k1c_red(255,0,0);
static const TQColor k1c_green(0,255,0);
static const TQColor k1c_blue(0,0,255);
static const TQColor k1c_cyan(0,255,255);
static const TQColor k1c_magenta(255,0,255);
static const TQColor k1c_yellow(255,255,0);
static const TQColor k1c_darkRed(128,0,0);
static const TQColor k1c_darkGreen(0,128,0);
static const TQColor k1c_darkBlue(0,0,128);
static const TQColor k1c_darkCyan(0,128,128);
static const TQColor k1c_darkMagenta(128,0,128);
static const TQColor k1c_darkYellow(128,128,0);
static const TQColor k1c_lightGray(192,192,192);
static const TQColor k1c_gray(128,128,128);
static const TQColor k1c_darkGray(64,64,64);
#define white        k1c_white
#define black        k1c_black
#define cyan         k1c_cyan
#define magenta      k1c_magenta
#define yellow       k1c_yellow
#define darkRed      k1c_darkRed
#define darkGreen    k1c_darkGreen
#define darkBlue     k1c_darkBlue
#define darkCyan     k1c_darkCyan
#define darkMagenta  k1c_darkMagenta
#define darkYellow   k1c_darkYellow
#define lightGray    k1c_lightGray
#define gray         k1c_gray
#define darkGray     k1c_darkGray
#define white     k1c_white
#define black     k1c_black
#define cyan      k1c_cyan
#define magenta   k1c_magenta
#define yellow    k1c_yellow
/* red/green/blue 不设宏：与 TQColor::red()/green()/blue() 方法名冲突，
 * 用全局颜色对象的调用点直接写 k1c_red/k1c_green/k1c_blue（内部链接对象） */
#define darkRed   k1c_darkRed
#define darkGreen k1c_darkGreen
#define darkBlue  k1c_darkBlue
#define darkCyan  k1c_darkCyan
#define darkMagenta k1c_darkMagenta
#define darkYellow  k1c_darkYellow
#define lightGray k1c_lightGray
#define gray      k1c_gray
#define darkGray  k1c_darkGray

/* ── 4i. Qt1 全局杂项宏/枚举 ─────────────────────────────────────── */
typedef int QCOORD;  // Qt1 坐标类型（TQt3 已并入 int）
/* ── 4j. Qt1 全局对齐/文本标志枚举（AlignLeft 等 → TQt 命名空间） ── */
#define AlignLeft      TQt::AlignLeft
#define AlignRight     TQt::AlignRight
#define AlignHCenter   TQt::AlignHCenter
#define AlignTop       TQt::AlignTop
#define AlignBottom    TQt::AlignBottom
#define AlignVCenter   TQt::AlignVCenter
#define AlignCenter    TQt::AlignCenter
#define AlignAuto      TQt::AlignAuto
#define AlignJustify   TQt::AlignJustify
#define WordBreak      TQt::WordBreak
#define BreakAnywhere  TQt::BreakAnywhere
#define DontClip       TQt::DontClip
#define ShowPrefix     TQt::ShowPrefix
#define SingleLine     TQt::SingleLine
#define DontPrint      TQt::DontPrint
#define ExpandTabs     TQt::ExpandTabs
#define NoAccel        TQt::NoAccel
#define LeftButton     TQt::LeftButton
#define RightButton    TQt::RightButton
#define MidButton      TQt::MidButton
#define ShiftButton    TQt::ShiftButton
#define ControlButton  TQt::ControlButton
#define AltButton      TQt::AltButton
#define KeyButtonMask  TQt::KeyButtonMask
#define PlainText      TQt::PlainText
#define RichText       TQt::RichText
#define AutoText       TQt::AutoText
#define LogText        TQt::LogText

/* ── 4k. Qt1 窗口旗标枚举（WStyle_* 等 → TQt 命名空间） ─────────── */
#define WStyle_Customize      TQt::WStyle_Customize
#define WStyle_NormalBorder   TQt::WStyle_NormalBorder
#define WStyle_DialogBorder   TQt::WStyle_DialogBorder
#define WStyle_NoBorder       TQt::WStyle_NoBorder
#define WStyle_Title          TQt::WStyle_Title
#define WStyle_SysMenu        TQt::WStyle_SysMenu
#define WStyle_Minimize       TQt::WStyle_Minimize
#define WStyle_Maximize       TQt::WStyle_Maximize
#define WStyle_MinMax         TQt::WStyle_MinMax
#define WStyle_Tool           TQt::WStyle_Tool
#define WStyle_StaysOnTop     TQt::WStyle_StaysOnTop
#define WStyle_Dialog         TQt::WStyle_Dialog
#define WType_TopLevel        TQt::WType_TopLevel
#define WType_Dialog          TQt::WType_Dialog
#define WType_Popup           TQt::WType_Popup
#define WType_Desktop         TQt::WType_Desktop
#define WType_Modal           TQt::WType_Modal
#define WDestructiveClose     TQt::WDestructiveClose
#define WPaintDesktop         TQt::WPaintDesktop
#define WPaintUnclipped       TQt::WPaintUnclipped
#define WPaintClever          TQt::WPaintClever
#define WResizeNoErase        TQt::WResizeNoErase
#define WNorthWestGravity     TQt::WNorthWestGravity
#define WRepaintNoErase       TQt::WRepaintNoErase
#define WX11BypassWM          TQt::WX11BypassWM
#define WGroupLeader          TQt::WGroupLeader
#define WShowModal            TQt::WShowModal
#define NoBackground          TQt::NoBackground
#define FixedColors           TQt::FixedColors
#define X11ParentRelative     TQt::X11ParentRelative

#define QCOORD_MAX TQCOORD_MAX
#define QCOORD_MIN TQCOORD_MIN
#define NoBrush    TQt::NoBrush
#define SolidLine  TQt::SolidLine
#define DotLine    TQt::DotLine
#define DashLine   TQt::DashLine
#define DashDotLine TQt::DashDotLine
#define DashDotDotLine TQt::DashDotDotLine
#define NoPen      TQt::NoPen
#define CopyROP    TQt::CopyROP
#define XorROP     TQt::XorROP
#define OrROP      TQt::OrROP
#define NotCopyROP TQt::NotCopyROP
#define AndROP     TQt::AndROP
#define NotROP     TQt::NotROP
#define ClearROP   TQt::ClearROP
#define SetROP     TQt::SetROP
#define NopROP     TQt::NopROP

/* ── 4f. QFileInfoList typedef（Qt1 的 QList<QFileInfo> 惯用别名） ── */
#include <ntqfileinfo.h>
typedef TQPtrList<TQFileInfo> QFileInfoList;
typedef TQPtrListIterator<TQFileInfo> QFileInfoListIterator;
/* typedef 形态的 Qt1 类名（自动生成表只抓 class/struct 声明） */
#include <ntqdragobject.h>
typedef TQUriDrag QUrlDrag;
typedef TQPtrListIterator<TQFileInfo> QFileInfoListIterator;

/* ── 4c. 信号槽宏映射（TQt3 改名 TQ_SIGNAL/TQ_SLOT） ─────────────── */
#define SIGNAL TQ_SIGNAL
#define SLOT   TQ_SLOT

/* ── 4d. 修饰键常量映射（Qt1 全局 const uint → TQt 命名空间枚举） ───
 * Qt1: const uint CTRL=0x4000（qkeycode.h）；TQt3: TQt::CTRL 枚举基值。
 * KDE1 的 CTRL|Key_X 组合形态在位不重叠时与 TQt3 的加法语义等值。
 */
#define CTRL  TQt::CTRL
#define SHIFT TQt::SHIFT
#define ALT   TQt::ALT

/* ── Qt1 事件对象转换宏(ksirc/puke 生态使用)───────────────────────────
 * What : Q_TIMER_EVENT(e) 等把 TQEvent* 强转为具体事件类型
 * Why  : Qt1 的 qobject.h 提供这族 cast 宏,TQt3 未保留;ksirc 的 puke
 *       子系统(pwidget.cpp)按 Qt1 习惯大量使用,逐点改写不如整族映射
 * When : 任何使用这些宏的 TU 编译期(全局注入点)
 * How  : 展开为 C 风格强转,语义与 Qt1 原宏一字不差
 * ────────────────────────────────────────────────────────────────────*/
#define Q_TIMER_EVENT(x)  ((TQTimerEvent*)(x))
#define Q_MOUSE_EVENT(x)  ((TQMouseEvent*)(x))
#define Q_KEY_EVENT(x)    ((TQKeyEvent*)(x))
#define Q_FOCUS_EVENT(x)  ((TQFocusEvent*)(x))
#define Q_MOVE_EVENT(x)   ((TQMoveEvent*)(x))
#define Q_RESIZE_EVENT(x) ((TQResizeEvent*)(x))

/* ── 5. moc 关键字宏映射（编译器侧；moc 侧由构建管线 sed） ──────────
 * When : tqmoc 自带词法器不展开 #define，故 moc 输入前由 CMake 的
 *        wrap 阶段把 Q_OBJECT 等替换为 TQ_OBJECT；此宏只服务编译器
 */
#define Q_OBJECT     TQ_OBJECT
#define Q_PROPERTY   TQ_PROPERTY
#define Q_ENUMS      TQ_ENUMS
#define Q_SETS       TQ_SETS
#define Q_CLASSINFO  TQ_CLASSINFO
#define Q_OVERRIDE   TQ_OVERRIDE

/* ── 6. 版本宏（历史 #if QT_VERSION 判断保持 Qt3 语义） ──────────── */
#define QT_VERSION      TQT_VERSION
#define QT_VERSION_STR  TQT_VERSION_STR

/* ── 7. kde_sprintf：翻译格式串的 UTF-8 安全 printf ──────────────────
 * ┌─ What : 与 TQString::sprintf 同语义的 printf 风格格式化，但其
 * │        格式串与 %s 参数按 UTF-8 解码（返回 UTF-16 TQString）
 * │  Why  : TQString::sprintf 扫描 format 用 const char* 逐字节
 * │        Latin1 升位——KDE1 的调用形态 sprintf(locale->translate(FMT),…)
 * │        中 FMT 是 mo 里的 UTF-8 译文，其中文被拆成逐字节假字符
 * │        （实测"您"→3 个 .notdef 方框，"电脑"→6 个）。%s 参数链路
 * │        （fromUtf8）本就正确，坏的只是 format 明文段。
 * │        TQt3 二进制不可重编（内含源码已失传的 8-28 渲染修复），
 * │        故在脚手架层提供本函数，调用点批量改写。
 * │  Who  : 被 port 改写器批量替换的 sprintf(translate/i18n( 调用点；
 * │        本头文件经 -include 注入全部编译单元，随取随用
 * │  How  : ① format 经 fromUtf8 正确解码为 UTF-16；② 明文段直入结果；
 * │        ③ '%' 字段解析 flags/width/precision/length_mod 后按转换符
 * │        分派——s 走 fromUtf8（%ls 逐 wchar 收敛到 BMP），d/i/u/x/X/o
 * │        按 length_mod 取参后组 ASCII 小格式串交 vsnprintf，f/F/e/E/
 * │        g/G 取 double 同法，c 按 int 提升直转；%% 与不完整转义按明文。
 * │        整数统一提升到 long long 再打印，避免 va_arg 类型失配。
 * │  When : 编译期替换、运行期每次调用即格式化
 */
#ifndef Q1COMPAT_NO_KDE_SPRINTF
#include <stdio.h>
#include <stdarg.h>
inline TQString kde_sprintf(const char* kdeFmt, ...)
{
    va_list ap;
    va_start(ap, kdeFmt);
    TQString result;
    const TQString fmt = TQString::fromUtf8(kdeFmt);
    /* 边界用 length() 显式界定（Why：实测 staging 库的 fromUtf8 结果
     * unicode() 数组未保证 NUL 终止——isNull() 判终止会越界扫进相邻
     * 堆块（实测扫出 locale 路径串"-8/LC_MESSAGES/kde.mo"），尾部
     * 拼出 12 个假字符。length() 是唯一可靠边界。） */
    const TQChar* c = fmt.unicode();
    const TQChar* end = c + fmt.length();
    while (c < end) {
	if (*c != '%') { result += *c++; continue; }
	const TQChar* esc = c;
	++c;
	if (c >= end) { result += '%'; break; }
	if (*c == '%') { result += '%'; ++c; continue; }

	/* 重建 ASCII 子格式串头：flags + width + .precision + length */
	TQString head = "%";
	while (c < end && (*c == '#' || *c == '0' || *c == '-' || *c == ' ' ||
	       *c == '+' || *c == '\'')) { head += *c; ++c; }
	if (c >= end) { while (esc != c) result += *esc++; break; }
	if (*c == '*') { head += TQString::number(va_arg(ap, int)); ++c; }
	else while (c < end && c->isDigit()) { head += *c; ++c; }
	if (c < end && *c == '.') {
	    head += '.'; ++c;
	    if (*c == '*') { head += TQString::number(va_arg(ap, int)); ++c; }
	    else while (c < end && c->isDigit()) { head += *c; ++c; }
	}
	if (c >= end) { while (esc != c) result += *esc++; break; }
	bool isL = false, isLL = false;
	for (;;) {
	    if (c < end && *c == 'l') { if (isL) isLL = true; isL = true; ++c; }
	    else if (c < end && *c == 'h') { ++c; }
	    else break;
	}
	if (c >= end) { while (esc != c) result += *esc++; break; }
	const char conv = c->latin1();
	++c;

	switch (conv) {
	case 's': {
	    if (isL) {                     /* %ls：宽字符收敛到 BMP */
		const wchar_t* w = va_arg(ap, const wchar_t*);
		while (w && *w) { result += TQChar((ushort)*w); ++w; }
	    } else {
		result += TQString::fromUtf8(va_arg(ap, const char*));
	    }
	    break;
	}
	case 'c':
	    result += TQChar((ushort)va_arg(ap, int));   /* 整数提升，无需长度符 */
	    break;
	case 'd': case 'i': case 'u': case 'o': case 'x': case 'X': {
	    /* 整数族：按 length_mod 取参（va_arg 类型必须与实参匹配），
	     * 统一提升为 long long 后用带 ll 的子格式串打印 */
	    long long v;
	    if (isLL)      v = va_arg(ap, long long);
	    else if (isL)  v = va_arg(ap, long);
	    else           v = va_arg(ap, int);
	    TQString sub = head;
	    if (!isLL) sub += "ll";
	    sub += conv;
	    char buf[128];
	    snprintf(buf, sizeof buf, sub.latin1(), v);
	    result += TQString::fromLatin1(buf);
	    break;
	}
	case 'f': case 'F': case 'e': case 'E': case 'g': case 'G': {
	    double v = va_arg(ap, double);
	    TQString sub = head;
	    sub += conv;
	    char buf[256];
	    snprintf(buf, sizeof buf, sub.latin1(), v);
	    result += TQString::fromLatin1(buf);
	    break;
	}
	default:                          /* 未知转换符：整段按明文回填 */
	    { const TQChar* p = esc; while (p != c) result += *p++; }
	    break;
	}
    }
    va_end(ap);
    return result;
}
#endif /* Q1COMPAT_NO_KDE_SPRINTF */

#endif /* Q1COMPAT_H */
