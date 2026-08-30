# CHANGELOG — 修改日志

> 本文件是全项目**唯一**允许记录修改历史的地方。条目新的在最上，一次工作对应一条。其余所有文档（agent.md、README.md 等）禁止出现过程性/日志式内容，只保留当前最终状态。

## 2026-08-31（第三批：git 历史重写——tqt3-build 路径从全部历史清除）

- **历史重写（filter-branch index-filter）**：上一批只把 tqt3-build 移出了索引，6697 条路径仍存在于 2026-08 底座迁移以来的历史提交中；本批对全部历史重写剔除该路径——历史中 `git log -- tqt3-build` 命中归零，重写后 HEAD 树与重写前逐字节一致（工作区零变化）。全部 commit hash 已变（含上一批两次提交），若已有远端需 force push。
- **陈旧 stash 清理**：删除 2026-08"重写作者身份前的临时暂存"stash（kcharsetsdata/krootwm 等 Qt1 时期改动，已被 TQt3 路线取代）；diff 导出留档于仓库外 `kde1-old-stash-20260831.patch`。重写前完整历史备份 bundle 同在仓库外。
- 说明：tqt3-build 文件内容与 tqt3/ 快照共享同一批对象（delta 存储），本次清理的是历史路径与语义污染（clone/翻历史不再见构建目录），仓库体积基本持平。

## 2026-08-31（第二批：git 仓库卫生整理——tqt3-build 误入库清除 + 忽略规则误伤修复）

- **tqt3-build/ 移出版本库**：该目录是 tqt3 快照的构建副本（build.sh 生成、clean.sh 清理），因 tqt3 上游 .gitignore 只挡了 Makefile/bin 等，6696 个源码副本文件曾被误跟踪；git rm --cached 清出索引并在根 .gitignore 显式声明 tqt3-build/——随之 git archive 导出的源码包不再携带冗余副本。
- **误伤的上游源码文件强制入库（23 个）**：根 .gitignore 撤销 *.deb/*.tar.gz 等全局规则（打包产物只在 dist/，目录级忽略已覆盖；全局规则误伤 kdebase/pics/locolor-icons.tar.gz 上游资源）；kdebase/kdelibs 上游 .gitignore 的 Makefile 规则误伤的 4 个 1999 年原生 Makefile（bsd-port、konsole/tests、mediatool/Documentation）、tqt3/.gitignore 误伤的 18 个快照原生文件（Makefile、bin/tqtrename140、tutorial/t1–t15 的 .pro）均 git add -f 入库——保证 clone 后源码树完整可构建。

## 2026-08-31（版本口径 1.2.0 RC1 + README「缘起」）

- **版本进入 RC：口径统一为 1.2.0 RC1**：kapp.h 的 KDE_VERSION_STRING 改 `1.2.0rc1`（无空格写法——kfm 的 User-Agent 以该宏拼接，UA 串中空格非法）；package.sh 包版本改 Debian 规范写法 `1.2.0~rc1-1`（`~` 保证 dpkg 排序早于将来的正式版 `1.2.0-1`，升级路径正确）；package.sh / debian/control 各包描述、kde1.desktop 会话名（lightdm 菜单显示 KDE 1.2.0 RC1 Revival）、README 版本说明五处同步；kdelibs 起七模块增量重编并全量刷新 staging 与 dist/deb、dist/src 产物（旧 1.2.0-1 产物清除）。
- **README 新增「缘起」一节**：记录项目动机——1999 年初遇 KDE 1 的"白月光"，到 AI 时代（GLM-4.7 用至今）复活白月光；点明"复活不是回到过去，而是把它带到现代"的路线基调。

## 2026-08-30（第三批：包版本口径统一 1.2.0 + 按模块拆包六件套）

- **包版本口径统一为复活版 1.2.0**：kapp.h 的 KDE_VERSION_STRING 上一批已提升为 1.2.0（kcontrol 首页显示随之变化），package.sh/debian/control 的包版本与描述同步（Version 1.2.0-1），Qt 1.44 残留表述清除。
- **拆包方案落地为按模块六件套**：kde1-core（tqt3+kdelibs+kdebase 必需）/ kde1-games / kde1-utils / kde1-network / kde1-toys 四个独立可选包（各 Depends core 版本锁定）/ kde1 元包（Depends core，Recommends 四个可选包与输入法/音效/打印推荐——可选包可单独不装）；kde1-apps 撤销。package.sh、debian/control、.dsc Binary 清单、AGENTS.md §1 目标 5 四处同步。逐包验证内容归属正确无串包（games 无 kcalc 等）。
- **staging 全量刷新 + krn 编译修复**：kdenetwork/krn 补 mimelib 头搜索路径（与 kmail 同款）后编译归零，kdenetwork 随整体刷新入包；staging 120 个二进制与 deb 逐文件 md5 核对一致。
- **实机升级口径确认**：同包名升级 dpkg 按旧清单自动清理旧文件（含 qt1 底座旧版），dpkg -i 三/六包直接覆盖安装。

## 2026-08-30（第二批：控制中心模块拉起 + 背景布局/壁纸三问题闭环）

- **kcontrol 树选模块拉不起（控制中心里点任何设置项无反应）根治**：configlist.cpp 的 execute() 用 KProcess 裸名 execvp 沿 PATH 搜索 kcmdisplay 等模块二进制——非 startkde 包装环境 PATH 不含 /usr/kde1/bin，execve 全部 ENOENT（strace 实证逐路径失败）。修复：KDEDIR 存在且裸名可在 $KDEDIR/bin 命中时直接拼绝对路径，PATH 正常环境行为不变。终验：树选「背景」→ 模块进程拉起 → KWM 吞窗 → 完整背景设置页嵌入控制中心。附带确认 KTreeList 键盘语义（+/− 展开折叠、Return 选中执行，1999 年原生无 Right/Left 分支）。
- **背景设置页布局两处修复（kcmdisplay backgnd.cpp）**：① topLayout 网格构造补显式 spacing 8——TQt3 下默认行距趋零，「颜色」组框标题与上方「桌面」组底边重叠（标题被裁半截）；② 缓存大小 LCD 的 setMinimumHeight(2/3*h) 是 C 整数除法（2/3==0），高度被压零致「1024」段位撕裂，改 h*2/3 并撤掉人为削 20px 宽度。
- **图片壁纸全链路复测通过**（用户报「壁纸打不开/无法切图片壁纸」的闭环）：独立与 kcontrol 嵌入双路实测——壁纸下拉 140 张正常列出 → 选 abnormal_fluid.jpg 预览即变 → 「应用」写 desktop0rc（UseWallpaper=true/Wallpaper=abnormal_fluid.jpg）→ kbgndwm 收 kbgwm_reconfigure 重载 → 桌面整屏上墙；UI 无重置。此链路的 activated(const char*) 信号失效根因已由上一批修复（int 索引槽），本批实测确认。测试配置已还原，调试打点全部清理。

## 2026-08-30（kfontmanager 空列表修复：0 字节 kdefonts 误判"已安装"）

- 早前测试点"应用"生成的 **0 字节 ~/.kde/share/config/kdefonts** 使 loadKDEInstalledFonts() 误判"已有自定义字体列表"——fontconfig 枚举整块被门槛跳过，"可用的X11字体"只剩 X 核心字体、"KDE可用的字体"全空。两处修复：① fontconfig 枚举无条件执行（kdefonts 仅决定"KDE可用"列是否自动填充）；② 文件读到 0 个有效条目时返回 false（空文件视为无自定义列表）。已删测试副产物空文件，双列表实测填满。

## 2026-08-30（全项目 BUG 排查：八个缺陷根治 + 14 处 Qt1 信号静默失效修复 + 30 余应用跑测审计）

- **BUG1 konsole 中文显示（根修）**：VT 默认字体经 fontconfig 按候选序选 CJK 等宽家族（Noto Sans Mono CJK SC 等，konsoleDefaultVTFont）；drawAttrStr 改显式 UTF-8 解码整段绘制（TQString::fromUtf8，规避 char* 隐式转换的字符数截断语义）；font_w 度量改 width('M')（CJK 等宽字体 maxWidth 是全角宽，会使列数减半）。
- **BUG2 kcontrol 树展开不刷新（port 兼容层根修）**：port/src/qttableview 的 setAutoUpdate 原实现联动 setUpdatesEnabled()（置 WState_BlockUpdates），KDE1"setAutoUpdate(FALSE);…;repaint();setAutoUpdate(TRUE)"防闪烁模式的手动 repaint 被静默吞掉——改为纯标志位（Qt1 语义），KTreeList 展开即时刷新，全部 81 处 QTableView 派生控件受益。
- **BUG4 窗口边框/边角拖拉缩放失效**：kwm client.C mouseMoveEvent 光标选择链的 else 在 TQt3 迁移重排时丢失，无条件 set_x_cursor(normal) 覆盖四边/四角光标，mousePressEvent 的 do_resize 恒 0——恢复 else 即愈（实测 frame 708→865 精确跟手）。
- **BUG5 字体选择与 Debian 环境不符**：konsole 字体菜单动态化（TQFontDatabase 家族按实测等宽过滤——TQt3 X11 下 QFontInfo::fixedPitch 恒 0 不可用，改 i/M/W 三字符 advance 相等判定；中文字体自然入列）+ "选择字体..."（KFontDialog）；kfontmanager 字体枚举 fontconfig 化（XListFonts 回退保底）；家族名取 utf8()（latin1 会把 CJK 名变 NULL 致 strcmp 崩溃），utf8() 的 TQCString 临时对象必须保生命周期。
- **BUG6 最大化溢出桌面**：kwm adjustSize 的 PResizeInc 对齐以 geometry_restore 为锚，最大化时 (restore-dx) 为负、整除向零截断致宽度反向膨胀——改 ICCCM 标准锚点（PBaseSize/0）向零取整对齐（实测 kvt 最大化精确铺满工作区）。
- **BUG7 kcmlocale 选简体中文后翻译全丢**：kcmlocale 语言表 zh_CN.GB2312/zh_TW.Big5 更新为 UTF-8 标签；KLocale 语言列表规范化（"C"/空项沉底——kcmlocale 写出的 "C:C:zh_CN.GB2312" 首位 C 使语言循环提前终止）；KLocale 增加 charset 无关最终回退（语言+国家匹配的 zh_CN.UTF-8 目录命中，charset 取目录实际值）。选英文/选简体中文均正常。
- **14 处 Qt1 信号静默失效修复**：TQComboBox 仅 activated(int)（const char* 版不存在），TQListBox 仅 highlighted(int)——KFontDialog×5、KFontChooser×2、kdehelp 字体选项×2、syssound、slidescreen（死 connect 移除，1999 年即未实现）、KFileDialog 位置栏、KFileFilter、kikbd 切换键、backgnd 图案列表、banner 字体、attraction 模式，全部改 activated/highlighted(int)+槽内按索引取文本（局部 combo 经 sender() 取发信对象）。
- **kmail 启动告警弹窗根治**：TQt3 tqWarning 统一加 "[时间戳] " 前缀使 kmail 消息处理器白名单前缀匹配失效；且 Qt 内部告警逐个追补不可持续——Qt 告警一律 stderr+kdebug，不再弹模态框（TQtFatalMsg 路径不变，应用主动 KMsgBox 不受影响）。
- **全应用跑测审计**：zh_CN 沙箱逐应用启动截图——kdebase（kfm/kpanel/kwm/kvt/konsole/kcontrol/kdehelp/kfind/kfontmanager/kmenuedit/kappfinder/krootwm/kikbd）与 kdegames 13 款、kdeutils 8 款、kdenetwork 6 款、kdetoys 3 款全部运行正常；kcontrol 树展开即时刷新、窗口拖拉缩放/最大化恢复正常。翻译补齐：konsole.po 新词条（选择字体）、kfontmanager.po 文案同步、桌面图标中文名（主文件夹/回收站/模板，applnk 源与用户桌面副本双端）。
- **仓库卫生**：删除 port/ 下误存为源码的 404 HTML 残页（src_qttableview.*，真实现位于 port/src/qttableview.*，无引用）；删除 qt1/ 构建残留；README/build.sh/tqt3-patches README 的补丁数口径同步（1→2）。

## 2026-08-30（版本提升 1.2.0 + README 实拍图 + 全线小版本号提升）

- **版本号提升（与 1999 原版区分）**：KDE 主版本 kapp.h 的 KDE_VERSION_STRING/MAJOR/MINOR/RELEASE 统一改为 "1.2.0"/1/2/0（原上游宏 RELEASE=5 与字符串 "1.1.2" 本就不一致，一并修正）；五个模块级 VERSION 提升（kdebase 1.1.2→1.2.0，kdegames/kdeutils/kdenetwork/kdetoys 1.1.2.1→1.2.0.1，源码 config.h.in/common/config.h 与 build 树生成物同步）；实质修复/接入组件的独立版本宏提升——kwm 0.14→0.15（patchlevel 保留）、kreversi 1.0.1→1.1.0、konquest 0.99.1→1.0.0、kmahjongg 0.4.1→0.5.0、kfloppy 1.1.2→1.2.0、karm 0.5→0.6、kljettool 1.1→1.2、karchie 1.1.2→1.2.0。规则统一：次版本 +1、修订位归零、四段版本保留包后缀。全库六模块重编装载。
- **README 加桌面实拍图**：仓库根新增 t0_main.png（staging 桌面 1280x960 实拍），README 标题下以相对路径引用，git 渲染直接显示。

## 2026-08-30（第八批:全应用运行与中文翻译大排查——10 个被注释应用接入 + 13 个新 po + 六个渲染/启动缺陷根治）

- **全应用清点与接入**：清点发现 kdeutils/kdenetwork 有 **10 个应用当年迁移 CMake 时被注释未构建**——ark、kab、karm、klpq、kljettool、karchie、kppp、krn、ksirc（含 7 静态子库+libpuke.so+pws）、ksticker。逐个补写/翻译 CMakeLists（含 KRN 宏、gdbm 兼容头、系统旧库路径隔离——/usr/kde1/lib 残留 Qt1 时代同名库导致链接错库，全部改为仓库 staging 优先并 unset 缓存），并修复约 80 处 Qt1→TQt3 编译错误（iostream.h 族、QString 字节缓冲、setStr/resize、GCI/Bool/Status 宏、带名 new、>? 运算符、CMSGSHDR 弹性成员、C99 inline、QListView updateItem、QTabDialog 重载二义、moc include 补齐等）。ktalkd（talk 守护进程，现代系统无 talk/inetd 生态、非 GUI）保持不接入。
- **13 个应用补齐 zh_CN 翻译（约 680 条）**：kworldwatch/kpm/karchie/kab/kjots/ksokoban/ksmiletris/klipper/kwrite/kbiff/korn/ksirc/konquest——译法上网核实并对齐 KDE6 术语（kill→结束进程、语法高亮、区分大小写、仅整词等，参考 KDE UserBase/apps.kde.org zh-cn 官方页面）。**发现并修复 kdeutils 的 po 子树从未接入 CMake**（上层 add_subdirectory(po) 被注释）——ark/kcalc/kedit 等十余应用翻译自始未随构建安装；现 staging 共 73 个 zh_CN mo 落盘。
- **X11 宏污染连锁根治**：上个会话给 q1compat.h 加的「全局预 include X11」使 Xlib 的 CursorShape/None 等宏击穿全库 TQt3 枚举（ntqnamespace/ntqevent 首轮即炸），已回退；改为定点修复 15 处——kdelibs 三头（kwm/krootprop/ktoolboxmgr）与 kdebase 各 TU 采用「恢复-解析-再摘」护罩（Xutil.h 声明依赖宏形式的 Bool/Status，须在摘宏前解析；shape.h 声明同理），kpanel/kvt/kwm 的 Bool 全局声明 bool 化。教训记入：批量脚本写文件一律临时文件+原子替换（本轮三次编码异常截断源文件，均由 git 恢复）。
- **六个应用运行缺陷逐一视觉定位并根治（gdb+X 窗口树实证）**：① ksirc 启动即退——irclistbox 的 updateTableSize 去掉基类限定后自递归栈溢出，且 TQListBox 已无 QTableView 祖先（改仅置脏标记）；② karchie 启动即退——KAView 三指针成员依赖 BSS 零值，构造早期触碰野指针（显式置空）；③ kmail1 段错误——kmsettings 两处 stricmp(readEntry()) 把 TQString 对象指针当 C 串（改 TQString==）；④ kpat 牌桌全空——dealer 纯虚 show() 遮蔽容器映射、X 窗口 IsUnMapped（pWidget::showEvent 兜底 + 显式 TQWidget::show）；⑤ ktop 列表全空——TaskMan（QTabDialog）同对话框类型位问题 + KTMainWindow::show 不再级联（reparent 重嵌入 + 显式 show）；⑥ kshisen 全英文——KApplication 未传 catalog 名而 CMake 可执行名带 1 后缀找不到 kshisen.mo（补传名）；另 kfloppy 缺辅助程序 kfdformat/kmkdosfs（CMake 启用+修 MAJOR/cdiv 兼容）、knotes 缺 mini 图标+KWM 空 QRect 把便笺压成 1x1（补装+防御）。kfind 的 reparent 修复经重装后验证生效（上批修改从未编译进 staging）。
- **全量视觉回归终验（50+ 应用，视觉模型多轮看图）**：分四批并行截图审计 + 失败项修复后逐个复核——最终全部通过：窗口正常、菜单按钮中文、无乱码方块（ksticker 中文跑马灯、kpat 纸牌、ktop 进程表列名、kworldwatch 昼夜地图+中文日期等均实证上屏）；kljettool 在无 /etc/printcap 的环境按设计退出（环境性，非缺陷）；knotes 为托盘驻留形态。kasteroids HUD（Score/Fuel）等少量游戏内文案与帮助正文英文属「界面全面中文化」路线图后续项。
- **deb 重打包**：全部改动经 package.sh 重新产出 dist/deb 与 dist/src（kde1-core/kde1-apps/kde1 元包 + sdeb）。

## 2026-08-29（第七批：中文目录导航根治——双缺陷叠加 + tqt3 补丁 001 裁决）

- **kfm 中文目录导航根治（用户报告"点击中文目录报服务器上没有这个目录"的完整闭环）**：经 KURL 回环单测、strace 系统调用实证、五层打点（IPC parse/parse_dirEntry/slotDirEntry/manager/newSlave-getSlave）与 gdb 断点，定性为两个叠加缺陷，分别修复并双路终验通过（原始中文 URL 与百分号编码 URL 均正确进入目录列出内容，视觉模型核验）：
  ① **KURL::decodeURL UTF-8 字节流化**（kdelibs/kdecore/kurl.cpp）：原实现逐 QChar 取 latin1()，TQt3 的 TQString 为 UTF-16，中文字符 latin1() 返回 0（NUL），解码产物在第一个中文处截断——KURL("file:/a/公共").path() 只剩 "/a/"（实测），kioslave 因此拿到断头路径。改为对 utf8() 字节流做 %XX 解码后 fromUtf8 还原，顺带修正两个 1999 边界缺陷（孤立 '%' 越界读、'%' 后非十六进制编出假字节，现原样保留）。
  ② **tqt3-patches/002：TQString == const char* 遵循 codecForCStrings**（src/tools/qstring.cpp operator==）：TQt3 原实现逐字节比较（UTF-16 码位 vs (uchar) 单字节），中文经 UTF-8 解码进 TQString 后与其原始字节流永不相等且不受 setCodecForCStrings 影响——kfm 的 KIOJob::slotDirEntry 按 URL 字符串匹配路由目录条目，slave 上报与 lstURL **打印完全相同却比较不等**（实证），条目全部静默失配、窗口内容区空白。补丁使设置了 C 字符串 codec 时先经 codec 解码再比较（fromAscii 内部即 codecForCStrings()->toUnicode），未设 codec 的原生 TQt3 程序走原字节路径行为不变；单测四例（中文等/不等/ASCII/char* 左侧）全过。此为全局性 UTF-8 融合缺陷（验收标准第 8 条范畴），KDE1 代码所有 "TQString == char*" 比较点一并受益。
  - **排查方法教训登记**：中途"slot 未被调用"是误判——不匹配分支的调试输出误用 printf（stdout 重定向到文件时全缓冲），输出憋在缓冲区；stderr（无缓冲）才是打点正确通道。gdb 断点曾证明 slot 实际被调用而日志无输出，两相矛盾才暴露此节。
- **kwm Client::setLabel 的 KWM_WIN_TITLE 字节截断修复（kpanel 任务栏标签 tofu）**：kwm 把窗口名写回 KWM_WIN_TITLE 供模块读取时用 `label.length()+1` 当字节数（同族第三处）——中文 URL 拦腰截断（"公共"→"公"+孤立字节，任务栏标签末尾方框）。gdb 条件断点（XChangeProperty 且 atom==KWM_WIN_TITLE）抓到写入栈为 Client::setLabel，改 `strlen(bytes)+1` 后实测属性完整（公共+斜杠齐整），全量重启终验任务栏标签显示完整 URL。排查附带确认：该属性仅在窗口创建/改名时一次性写入，之后不再刷新。
- **tqt3-patches/001（ntqvariant Bool 宏）删除**：该补丁在枚举后 `#define Bool int` 恢复 X11 宏——宏泄漏后污染 TQt3 自身源码（qvariant.cpp 的 `case Bool:`、`TQVariant::Bool` 全部语法错误），实际从未通过编译验证（时间线考证：staging 库建于补丁打入之前，staging 头文件一直 pristine，六模块全部编译通过——证明 TQt3 构建根本不需要它；C++ 作用域规则下类内枚举器 Bool 与全局 typedef 本就不冲突）。tqt3-build/ 内残留已还原 pristine，全量增量重编通过。
- **libtqt-mt 全量增量重编**（pristine + 002）：qstring.o 重编译并重链接主库，装载 staging；kdelibs（kurl.cpp）与 kdebase kfm 同步重编装载。临时调试打点全部清理（§5.2.3）。
- **文档同步**：AGENTS.md §3 补丁条目、build.sh 注释、tqt3-patches/README.md 由"ntqvariant/空"更新为"当前 1 个：002 qstring UTF-8 比较修复"。
- **遗留登记**：TQString 的 operator</ucstrcmp 系字节比较同样语义存疑（排序场景未见实锤，待审计）；kfm 每窗口约 10 个 X 窗口泄漏（QPopupMenu 构造即建窗/书签子菜单泄漏，上批已记）；`const char*` 信号签名连接失败（KLined::textChanged/setText 等）全库清扫仍待做。

## 2026-08-29（第六批：kfm 目录白屏根治——数据侧三连修）

- **kfm 文件窗口内容区白屏根治（经子代理 gdb 深查定性）**：白屏非渲染缺陷——渲染管线经断点/重绘实证完全健康，根因在数据侧：同 URL 并发多窗口（双击桌面图标连开）时 kioslave 池 FIFO 分发 + KIOJob::slotDirEntry 逐字节 URL 匹配路由，条目整体喂给先绑定的 job，其余 job 零条目零错误"完成"，KFMManager::slotFinished 对零条目写空壳页（writeBeginning+writeEnd）→ 白屏（实证 9 窗 8 白 1 满）。修复三连：① slotFinished 零条目自动重试——opendir 实数确认目标本地目录非空才重发一次 openURL(_refresh)，retryEmptyUsed 防死循环；② slotError 改写最小错误页（原实现静默留白，无从得知出错）；③ KURL::encodeURL UTF-8 化——原逐 QChar 取 latin1()，TQt3 UTF-16 下中文字符得 0 编出 %00%00（实测"下载/"→ file:.../%00%00/），改为对 utf8() 字节流百分号编码（实测"公共"→ %E5%85%AC%E5%85%B1 正确）。
- **实测**：双击桌面图标并发连开两窗全部完整渲染；回收站窗口（freedesktop 路径）正常；诊断过程 kfm 曾被子代理的实验性 gdb inferior call 触发 SIGSEGV 退出（旧空窗口状态已损坏的旁证），按原方式拉起恢复。
- **遗留登记**：中文目录点击导航仍报"服务器上没有这个目录"——编码侧已正确（%E5%85%AC%...），断点在 kioslave 侧对编码 URL 的解码/访问更深处，待下批排查；kioslave 池竞争的治本方案（本地目录进程内枚举，KFMJob::openDir 直出）已在诊断报告中评估，留作备选。

## 2026-08-29（第五批：任务栏中文方块 + 控制中心翻译/可用性三连修）

- **任务栏按钮中文方块根治**：myTaskButton::drawButtonLabel 经 KCharsetConverter 变换字体族后绘制——变换字体在矩形 drawText 排版路径丢字形回退，"文本编辑器"渲染成 5 个方块（ASCII 正常、K 菜单同路径的原字体中文完好）。修复：绕开转换器直接用原字体与原字符串（UTF-8 全局策略下该转换本为恒等变换，仅字体被错误改写）。
- **资源目录运行期解析全面铺开（kapp.cpp 13 个 kde_*dir() 统一改造）**：烧录前缀问题的第二处实锤——kde_localedir() 返回编译期烧录的 /usr/kde1/share/locale/kde1，mo 翻译全部读自机器上旧 deb 的树（旧树有条目的显示中文、没有的显示英文，"大量未翻译"的根因）。新增共享助手 kde_runtime_resource_dir()：优先 $KDEDIR 拼接（剥烧录前缀取后缀），未设置回落烧录值（1999 的 KDEDIR 字面量语义保留）；htmldir/appsdir/icondir/datadir/localedir/cgidir/sounddir/toolbardir/wallpaperdir/bindir/partsdir/configdir/mimedir 全部接入。实测控制中心背景模块界面全面中文（一色背景/壁纸/颜色/预览）。
- **控制中心"点不动"实测已愈**：kcm 模块经 KProcess 启动 + kwm doNotManage 标题豁免 + windowAdd 信号吞入的三段链路在修复后的 kwm（MWM 尊重修复的连带收益）下端到端正常——双击树项模块嵌入显示，无悬空窗口；用户此前遇到的失败与悬空 skcm 窗口系上一轮坏 kwm 所致，本轮会话复测通过。kdelnk 菜单名（本轮前批已译）在树中正常显示中文。

## 2026-08-29（第四批：面板贴边根治 + 标题栏回归修复）

- **面板/任务栏贴屏幕边缘根治（P4 收官）**：kwm 映射期把面板窗口当普通窗口"钳入"工作区——而工作区正是面板自己预留的排除区（0+34+1280x881），顶部任务栏被推到 (0,34)、底部面板被钳到 (0,870)，各向内偏离恰好一个自身高度（kwm 调试打印实测与排除区数值精确吻合）。修复：kwm manage() 的钳制分支增加"边缘贴边窗口豁免"——完全位于屏内且四边之一与屏幕边缘重合的窗口（面板形态）不做钳制（钳制本意是保证可见，贴边窗口本就全可见）。曾试验 kpanel 侧"映射前临时还原全屏工作区"，因 showEvent 触发的 doGeometry 在毫秒间收回区域、纯时序竞态不可靠而放弃（试验记录在 kpanel.C show() 注释）。
- **上一批 kwm 修复的标题栏回归修正**：上批 getMwmHints 按"KWM_WIN_DECORATION 属性存在即跳过 Motif 解读"的判据有误——kwm/libkde 读该属性时对缺失窗口会写回默认值 1，manage 后所有窗口都"带有"属性，kpanel 的 Motif 无边框提示被无视 → 上下面板被画上可点击关闭的标题栏。修正为 manage() 改用裸 XGetWindowProperty 读取（不写回）并记录"客户程序是否自行声明"（Client 新增 kwm_decoration_client_set 成员），getMwmHints 仅对客户声明的窗口豁免——kfm 桌面图标（1280 声明）保持隐藏，kpanel（无声明）继续走 Motif 无边框路径，两者各得其所。
- 终验：顶部任务栏 1280x34+0+0、底部面板 1280x45+0+915（精确贴底），均无边框；任务栏仅真实程序条目；桌面图标正常；临时调试输出全部清理。

## 2026-08-29（第三批：任务栏 kfm 假条目根治——回到 kwm 原生机制）

- **任务栏 "kfm <2>…<9>" 假条目根治（经子代理独立复核修正根因）**：桌面图标窗（KRootIcon）的 KWM 装饰属性 desktopIcon|noFocus 在 manage 时被 kwm 自己覆盖——TQt3 对 WStyle_Customize 窗口一律写 _MOTIF_WM_HINTS(decorations=0)，kwm 的 getMwmHints 据此把 KWM_WIN_DECORATION 回写成 0，回写又触发属性监听链（wants_focus 重算为 true、hidden_for_modules 撤销、WIN_ADD 重发），图标窗被主动推进任务栏。修复双管齐下：①kfm 的 KRootIcon 改为 show() 前写 desktopIcon|noFocus（Qt1 惰性 show 顺序在 TQt3 下失效的时序补偿）；②kwm getMwmHints 增加优先级判定——窗口已带 KWM_WIN_DECORATION 属性时跳过 MWM 解读与回写（KDE 原生提示优先；无 KWM 属性的非 KDE 程序如 StarOffice 行为不变）。活体实验定位：manage 后补写 1280 条目即消失（无覆盖），构造期写入则被覆盖，证明覆盖链在 manage 期。终验：图标窗属性稳定 0x500，任务栏仅剩真实程序，桌面图标显示/点击无回归；kpanel 侧黑名单已撤，仅留自身窗口与 desktopIcon 位双道防御。
- **字符集假设排查（未成立）**：kwm 标题读取路径（getTextProperty/Xmb）经 git diff 零迁移改动、中文标题全程正常、"kfm" 默认窗口名系 TQt3 XStoreName(tqAppName) 行为——任务栏假条目与字符集无关。
- **遗留登记（X 资源卫生，非任务栏问题）**：TQt3 的 QPopupMenu 构造期即建 X 窗（override-redirect 未映射 popup，不进任务栏）+ kfmgui 书签子菜单/析构菜单泄漏 + kapp 通信窗泄漏——每个 kfm 窗口生命周期净漏约 10 个 X 窗口（32 个 640x384 幽灵窗的来源），待后续窗口生命周期清理。

## 2026-08-29（第二批：K 菜单全面汉化 + 翻译格式串乱码根治）

- **K 菜单 applnk 树 167 项全面汉化**：菜单项文字取自 applnk 的 .kdelnk/.directory 的 Name 字段而非 .mo，此前 321 条 po 补译覆盖不到——K 菜单分类与全部应用项因此大面积英文。以运维脚本对 staging 与源码树同步插入 `Name[zh_CN.UTF-8]`/`Name[zh_CN]` 双键（8-28 已验证的策略），译名对齐 KDE6 zh_CN 官方术语（扫雷/黑白棋/空当接龙/四川省/仓库番/便笺/剪贴板/世界时钟/软盘格式化等），KDE6 无对应物者用通行译法（弹珠棋/星际征服/笑脸俄罗斯方块等）。实测 K 菜单两级全中文。
- **kde_appsdir 运行期解析修复（kapp.cpp）**：applnk/locale 等资源目录用编译期烧录的 KDE_APPSDIR 绝对路径，$KDEDIR 环境变量被无视——沙箱（staging 树）与 deb 安装共用二进制时菜单读到烧录前缀下的旧文件（沙箱实测 40 余项菜单读自 /usr/kde1 旧文件）。kde_appsdir 改为优先 `$KDEDIR`（未设置回落烧录前缀，正式 deb 环境行为不变）。
- **翻译格式串乱码根治（kde_sprintf，153 处调用点）**：KDE1 遍布 `sprintf(locale->translate(FMT), …)` 形态——TQString::sprintf 扫描 format 用 const char* 逐字节 Latin1 升位，mo 译文的中文被拆成逐字节假字符（"您"→3 个方框、"电脑"→6 个，kreversi 状态栏/kcalc 组框实测）。在 port/q1compat.h 新增 `kde_sprintf`：format 经 fromUtf8 正确解码、%s 走 fromUtf8、数值字段按 length_mod 取参组 ASCII 子格式串交 vsnprintf；调用点批量改写 153 处（56 文件）。修复过程中发现并修掉初版实现的越界缺陷（fromUtf8 结果的 unicode() 数组未保证 NUL 终止，isNull() 判终止扫进相邻堆块拼出假字符——改用 length() 显式界定）。
- **遗留补充**：kedit 状态栏 "Line/Col" 无中文译文（kedit.mo 缺条目，翻译补齐范畴）；knotes 启动时 mini/knotes.xpm 图标缺失弹错误框（图标资源补齐范畴）；konsole/other/secure.patch 为文档性补丁不参与构建。

## 2026-08-29（沙箱体验周：三连崩溃/死锁根治 + XIM 插件补装）

- **新增 `sandbox.sh`**：Xvfb `:99` + fcitx5（私有会话 DBus，与宿主输入法总线隔离）+ x11vnc（仅 127.0.0.1:5901）+ libpulsedsp 音效转发，一键起停的后台桌面沙箱，Remmina 连入即可体验 staging 版 KDE1 而不扰宿主会话；会话环境与 deb 包装模板 startkde-kde1.in 逐字对齐（补 `TQTDIR` 与 `tqt3/lib` 库路径；音效直接预载 libpulsedsp——Debian 12 的 padsp 命令自带预载路径模板损坏）。AGENTS.md §3/§4 同步。
- **XIM 输入法插件补装（打包回归修复）**：8 月 28 日编译好的 TQt3 输入法插件（libqxim 等四件）一直躺在 `tqt3-build/plugins/inputmethods/` 未被安装——build.sh 的 TQt3 安装清单只拷了 imageformats。补装后 fcitx5 经 XIM 在 TQt3 应用（kedit 实测）恢复激活，`:99` 根窗口 `XIM_SERVERS=@server=fcitx` 注册正常；build.sh 安装清单同步加入 inputmethods。
- **kfm 点击桌面图标必崩（SEGV）根治**：Qt1 的 `QPixmapCache` 是实例级私有缓存，TQt3 的 `TQPixmapCache` 是全进程静态全局缓存且超 1MB 自动回收像素图，而 KRootIcon 等直接持有缓存内裸指针，被回收后点击图标触发 `KRootIcon::init()` 的 `pixmap->width()` 即段错误。改为 kfm 自有 `QDict<QPixmap>`（autoDelete 仅随进程退出生效，运行期永不清理，语义回归 Qt1 私有缓存）。
- **krootwm/kwm 五处 XGrabServer 死锁根治**：桌面空白处左键框选（select_rectangle）、透明拖动、杀窗口模式与注销/任务表/警告三个模态对话框，均为 XGrabServer 冻结全 server 后 XMaskEvent 死等按键释放——1999 年真鼠标的释放走设备直通无事，VNC/XTEST 注入的释放属客户端请求会被 server grab 冻在队列，形成双向死锁，且指针 grab 悬挂后吞掉后续全部点击（"点几下就失效"的真凶）。全部移除 server grab（指针/键盘 grab 保证事件通路，模态性不变）；kwm 其余瞬时竞争保护型 grab 经甄别保留。
- **面板/任务栏贴边定位未遂**：实测确认面板自身 setGeometry 的贴底计算正确，是 kwm 智能摆位把面板框架抬高约一个自身高度；试验 dock 化（KWM_DOCKWINDOW 原子 + manage 分支 XMapWindow 放行）出现"主窗口整体消失 + kbgndwm 隐身模块窗口被误照亮"两个副作用，已整体回退，机理与现状标注在 kpanel.C/main.C/manager.C，待后续以正确途径（kwm 识别面板窗口特征或 kpanel 走 module 协议申报几何）解决。
- **遗留清单（本轮新登记）**：kfm 目录视图内容区空白（文件列表不渲染）；桌面出现 `.directory` 隐藏文件图标（kfm 根桌面未滤点文件）；`const char*` 信号签名在 TQt3 下连接失败（KLined::textChanged/setText、TQLabel::setText 等，Reversi 状态栏等处静默失效）需全库清扫；kwm 拉屏保走烧录绝对路径 `/usr/kde1/bin`（本机旧 Qt1 残留 deb 的二进制，符号不匹配必崩）。

## 2026-08-29（底座更换：Qt1 → TQt3）

- **GUI 底座由 Qt Free Edition 1.44 更换为 TQt3 r14.1.6**（Trinity 维护的 Qt3 分支，GPLv2——解决 Qt1 专有许可证修改分发无授权的合规死结）。`tqt3/` 为 pristine 快照（commit 3a07835），源地址 `https://mirror.git.trinitydesktop.org/gitea/TDE/tqt3`；一切修改走 `tqt3-patches/`（当前 1 个：ntqvariant 的 X11 Bool 宏冲突），构建期打入 `tqt3-build/` 拷贝树。
- **port/ 迁移脚手架**（strangler fig 模式）：`q1compat.h`（600+ 条 Q→TQ 类名映射 + tq 函数族 + QFont/QFontInfo 包装类 + X11 宏防冲突）经 `-include` 注入全部编译单元；331 个转发头（Qt1 短名/缩写名/改名头/attic 控件）；两个生成器脚本可重跑。逐模块显式 TQ 化后摘除，终态整体拆除。
- **六模块全部完成 TQt3 编译迁移并归零**：kdelibs（42 轮轰炸）、kdebase（73 轮）、kdegames（12 轮）、kdeutils（16 轮）、kdenetwork（22 轮）、kdetoys（2 轮）；150 个程序装入 staging。
- **UTF-8 全局策略落地**：`TQTextCodec::setCodecForCStrings(UTF-8)` + `setCodecForLocale(UTF-8)` 于 KApplication::init()——字符串编码语义一个全局决策统一（TQt3 的 ascii()/data() 输出同受此开关驱动，双向边界一致）。
- **运行时崩溃根治三连**：kwm colormapFocus 悬空 else（gdb 定位）、kfm 桌面布局解析 strchr NULL 解引用、kfm SIGPIPE（Qt1 时代 QApplication 默认忽略、TQt3 不再代劳）。
- **staging 实测**（Xvfb :96）：kwm+kfm+kpanel 三件套稳定运行；桌面图标（主文件夹/回收站/模板）中文正常；kfm 文件管理器窗口（中文菜单栏 文件(F)/编辑(E)/查看(V)/转到(G)/书签(B)/选项(O)/帮助(H)、状态栏「文件： 完成」）；kvt 终端（中文菜单 + bash 提示符）。遗留：kpanel pager 按钮字形缺失、Templates/模板双条目、主文件夹图标位图缺失——记录待后续打磨。
- **qt1 彻底移除**：源码删除 + git filter-branch 历史重写（refs/objects 全清，`git rev-list --objects --all` 验证零 qt1 路径）+ 全部文档/脚本/打包配置同步更新（AGENTS/README/clean.sh/package.sh/debian/*）。

## 2026-08-28（第五批：kfm 转到菜单核验）

- 核验 kfm「转到(G)」下拉菜单内容并完成术语统一：向上(U)/后退(B)/前进(F)/主文件夹(H) 四项全中文（Alt+方向键快捷键提示正常），分隔线下动态显示历史记录 URL；「主目录」→「主文件夹」对齐 KDE6/Dolphin 与桌面图标术语。Alt+G 助记键打开菜单视觉核验通过。

## 2026-08-28（第四批：中文边界情况大阅兵与排版引擎 UTF-8 化）

- 边界测试大阅兵（混排/标点/全角/简繁/扩展B区四字节/韩日文/制表符/换行/半遮挡/反显/菜单，全部经视觉模型逐字核验）暴露并根治四类排版缺陷：
  ①**qt_format_text 排版引擎 UTF-8 化**（qpainter.cpp，所有矩形文本排版的中枢）：原按单字节核心字体宽预算断行，UTF-8 中文宽度失真且断点只在空格——改为按 UTF-8 字符粒度（fm.width(p,nbc) Xft 真宽）、多字节余字节零宽 code 编码、**CJK 任意断行 + 英文按空格整词保护**（对齐现代排版）；断点采用**占位槽方案**（多字节字符 code 序列前插 MBPLACE 可牺牲占位，溢出断行时 BEGLINE 行标记覆盖占位而非字符），修复断行丢字；实现**简化避头尾禁则**（，。！？；：、）】》」』…%不得居行首，标准 UTF-8 码点解码判定）。
  ②**裁剪 Region 悬垂**：矩形版 drawText 用过裁剪并 restore 后，基线版 Xft 分支把「空但 isEmpty() 误报非空」的 crgn 传给 XftDrawSetClip 导致后续文本全部被裁空（自绘第 2 行消失）——改为仅 testf(ClipOn) 且非空时传 Region。
  ③**制表符 tofu**：C0 控制字符（\t 等）在 Xft 绘制层经缓冲替换为空格（度量与绘制同源替换），不再画 .notdef 方框。
  ④**QListBox cellHeight 统一** +2（与 QListBoxText::height 一致，缓解视口初始偏移；首行仍有约半行偏移，属 QTableView 深层问题，对 ASCII 同样存在，非中文特有，记遗留）。
- 验证结果：WordBreak 中文任意断行无丢字（400px 三段+QLabel 逐字核对）、英文词完整、混排/标点/全角/简繁/韩日均正常、扩展B区四字节缺字形显示方框不崩溃、半遮挡移回重绘完整、kfm 长文件名折行（apache-maven-3.9.9 三行）无丢字；排版引擎大改后桌面会话四项终验（kfm/图标/面板/无乱码）全过，重打包重装 deb。遗留：QTableView 首行视口偏移、QLabel 超长单行硬裁无省略号（Qt1 原生行为）。

## 2026-08-28（第三批：界面全面中文化推进）

- po 存量缺口批量补译 321 条（msgfmt 全部校验通过，术语对齐现代 KDE：Go→转到、New→新建、Preferences→首选项、Portrait/Landscape→纵向/横向、Traceroute→路由跟踪、Freecell→空当接龙等；专有名词/版权行/数字/尺寸按惯例保留原文）：kfm 的 &Go、konsole 关于与返回码、kdm/klock/kbgndwm、kdegames 十件、kmail+krn 的邮件撰写全链（收件人/抄送/密送/附件/签名/PGP 系列）、kppp 拨号提示、ktop 任务管理器全套、kedit/knotes/kcalc/karm/ark/kljettool/kfinger/knu/ktalkd 等。已重编译 .mo 部署并随 deb 重装。
- 桌面图标标签中文化：①kfm root.cpp 的 initFilename 新增 .kdelnk 显示名读取（1999 原逻辑只对目录图标读 .directory，.kdelnk 文件图标一律显示文件名，Name[zh_CN] 永远无法生效）——只读 KSimpleConfig 取 Name，语言键由 KConfig 按 KLocale::language() 自动匹配；②applnk 的 Home/Trash 追加 Name[zh_CN.UTF-8]/Name[zh_CN] 双键（主文件夹/回收站，Comment 一并补齐）；③kfmpaths 生成 Templates.kdelnk 同步双键（模板）。视觉终验：三个图标显示「主文件夹/回收站/模板」，白字透明无黑框；kfm 菜单「转到(G)」生效，全图无乱码。
- 同批附带：XIM 输入通道补 XNFocusWindow+XSetICFocus（此前 IC 无焦点、fcitx5 从不接管——dbus 确认 Ctrl+Space 后 CurrentInputMethod 可切至 pinyin、按键 XFilterEvent 全部被拦截；候选窗不显示问题挂起待查，经用户指示暂停输入法工作并已恢复物理屏 fcitx5 实例）。

## 2026-08-28（第二批：渲染通道根源修复）

- 控件文字空白问题从根源修复（四个叠加缺陷，全部经视觉模型逐像素复验）：①绘制路径 pointSize 单位错误——qt_xft_font(QFont*) 把 QFont::pointSize() 的磅值当十分之一磅存入 QFontDef，经 /10 后 12pt 字体按 1pt 打开，文字缩成 1 像素墨点；②XftFontOpenName 单字体无按字形回退——实现字形回退链（主字体缺字形的码点自动切换 lang=zh-cn 中文字体与通用兜底字体，度量与绘制同一分段保证量宽=绘制，qmbxft_x11.cpp），Helvetica 等西文字体直接渲染混合中英文；③QFontMetrics 度量基于「实际加载的核心字体」XLFD 写回值（现代系统退化为 bitmap 兜底、字号漂移）——QFontMetrics 增设请求字体副本 xftReq（公共头 ABI 变更，全模块强制重编），spec() 三形态一律返回请求值与绘制路径同源；④height()/leading() 未接 Xft 分支——补齐（链上最大 ascent/descent），修复矩形对齐版 drawText 基线过低导致的中文下半截裁剪。
- KCharsetConverter 在 UTF-8 locale 下强制 NoConversion 透传（kcharsetsdata.cpp 单点修复）：此前 KLocale::charset() 因 UTF-8 不在 1999 字符集注册表而退回 us-ascii，kpanel 桌面按钮/kfm 目录树等所有转换调用点的中文整体变 '?'，透传后一处修复全覆盖，桌面按钮正确显示「一/二/三/四」。
- 1bpp 掩码位图文字通道（qpainter_x11.cpp）：XRender 无 1bpp visual，kpanel 按钮「文字画进 mask」路径回退核心字体导致中文被掩码裁空——新增临时 24bpp 中转绘制+亮度阈值转回+bitBlt 拷回（不重建目标位图防 XID 失效 BadDrawable），qmbxft 增加 qt_xft_draw_release 防短命 pixmap 的 XftDraw 悬垂。
- 桌面图标标签现代化：kfmrc [KFM Root Icons] Style=0→1，标签从 1999 黑底白字块改为透明背景纯文字（视觉确认无黑框、无黑晕）。
- XDG 解析收敛为公共头 xdgdirs.h（kfm/krootwm 共用），修复 krootwm 右键菜单仍指旧 ~/Desktop 的脱节；清空指向 /root 的 1999 桌面布局遗物 config/desktop。
- startkde-kde1 音效预载修正：LD_PRELOAD 由错误的命令名 padsp 改为真实库路径 libpulsedsp.so。
- 字体渲染微调对齐 XFCE（QT_XFT_ANTIALIAS/HINTING/HINTSTYLE/RGBA/DPI 五环境变量，默认 抗锯齿+轻度微调+RGB 次像素+96DPI）：DPI=120 实测字高精确放大 25%、关抗锯齿实测中间灰像素 401→0、三组参数中文均正常。
- 渲染调试探针（QT_XFT_DEBUG/[qtxft]/[xdgdbg]）全部清理；全量重编+重打包重装 deb，最终会话五项验收（kfm 中文/图标标签/面板/无乱码/可交付）经视觉模型全部通过。遗留：kfm "Go" 菜单等 po 存量缺口归入「界面全面中文化」项。

## 2026-08-28

- XDG 桌面图标问题根治（三层叠加因素）：①系统级 kfmrc 的 [Paths] 组在 KConfig 全局合并时覆盖代码默认值——源头移除该四行硬编码（用户级 kfmrc 仍可显式覆盖）；②解析函数两处 1999 年工具链陷阱（多字符常量 '="' 与 QString::length 含终止 0）修复；③补齐 initPaths 尾部的默认桌面图标逻辑（Home/Trash 从 applnk 拷入、生成指向 XDG 模板目录的 Templates 图标，幂等）。终验：全新会话桌面自动生成三个系统图标（房子/垃圾桶/文件夹）+用户文件图标完整、与 XFCE 共用 ~/桌面、~/Desktop 不再创建。

## 2026-08-28

- XDG 用户目录兼容落地并实机验证：KFMPaths 四项默认值改造——桌面/模板经 ~/.config/user-dirs.dirs 解析（中文系统即 ~/桌面、~/模板，kfm 配置显式设置仍优先）、Autostart 挪至 ~/.kde/share/autostart（不再污染桌面目录）、回收站对齐 freedesktop ~/.local/share/Trash/files（现代文件管理器可见 kfm 删除物）；实测 KDE1 桌面与 XFCE 完全共用 ~/桌面（原有文件+测试文件同屏显示）、不再创建 ~/Desktop。验收标准第 6 条扩充 XGD 表述，路线图记已完成项。

## 2026-08-28

- 全语言 po 转 UTF-8：非中文的 31 种语言约 1900 个 po（Latin-1/2/8、KOI8-R、EUC-JP 等）全部按各自 charset 声明转码为 UTF-8——宿主固定 UTF-8 前提下，任何 XX.UTF-8 locale 的用户经 KLocale 的语言回退命中 XX 目录后拿到的均为合法 UTF-8 内容（此前 Latin-1 系语言会出现重音字符乱码）；1999 年遗留的格式符错误条目以 #~ 废弃、损坏的 ja kstart 两文件重写为合法头，全项目 2064 个 po msgfmt 终检通过；法语 mo 重音字符（é/è/ç）抽查验证为合法 UTF-8；deb 已重打包重装。英文环境无需转换（不经 mo，源码原文即 ASCII）。

## 2026-08-28

- 打印接入 CUPS 并闭环实测通过：Qt1 QPrinter 的 lpr 命令路径经 cups-bsd 兼容层（/usr/bin/lpr）零代码接入现代 CUPS；kde1 元包 Recommends 声明 cups-bsd/cups（package.sh 与 debian/control 同步）；实测 lpr -P PDF 提交中文文档成功产出 PDF 且内容正确。验收标准增至八条（打印为第 7 条，原 UTF-8 融合顺延为第 8 条），AGENTS/README/路线图同步。

## 2026-08-28

- 依赖清单补齐：构建过程中实补的 byacc/flex（Qt1 老工具链）、libglu1-mesa-dev（qt1 OpenGL 扩展）、libssl-dev（kdebase）、pkg-config（CMake 探测）全部写入 AGENTS.md §4 与 README.md 构建必需组，并同步 debian/control 与 package.sh 生成 .dsc 的 Build-Depends；新增「运行与测试推荐」组（fonts-noto-cjk、fcitx5/fcitx5-chinese-addons/fcitx、pulseaudio-utils、xvfb、gettext、x11-utils、x11-xserver-utils、imagemagick、xdotool、xterm、dbus-x11）。

## 2026-08-28

- Debian 打包完成并经装/删/重装全流程验证：kde1-core（qt1+kdelibs+kdebase，9.1MB）/ kde1-apps（四应用模块，3.6MB）/ kde1 元包（xsessions 会话入口 + startkde-kde1 环境包装：KDEDIR/PATH/LD 自动设置、fcitx5/fcitx 自动拉起、padsp 音效转发、/usr/bin/moc-qt1 入口）分存 dist/deb，sdeb 源码包（orig/debian/dsc）存 dist/src；apt remove 零残留验证通过，重装为最终交付态。deb 安装后的系统级会话经 lightdm 等价路径实机验证：桌面图标全中文（个人文件/回收站/模板/自动启动/打印机）、fcitx5 托盘自动拉起、Xft 抗锯齿渲染正常。kfm 双内核集成完成（View 菜单/Ctrl+Shift+W/KFM_WEBENGINE 三入口 + kde1-webview XEmbed 嵌入，最小原型已验证 WebKit 渲染现代 HTTPS 站点）。kvt 中文显示、剪贴板 UTF8_STRING 协议、qutf8 编辑精度（17 项单测）此前均已落地。剩余差距如实记录：界面英文与中文的完全对齐仍有源码未标记 i18n 的存量串与 krn/kppp 等 po 缺口；kfm 双内核与 lightdm 真机菜单级交互待用户重启会话后最终确认。

## 2026-08-27

- XIM 输入通道打通：QKeyEvent 新增 text() 承载输入法整段 UTF-8 提交（XmbLookupString 缓冲 16→64 字节），QLineEdit/QMultiLineEdit 整串插入；fcitx5（拼音 nihao→「你好」）与 fcitx 4.2.9.9（zhongwen→「中文」）两代框架经 XIM 向 Qt1 程序提交中文均实测通过。

## 2026-08-27

- 路线乙两大里程碑落地：①Qt1 Xft 多字节渲染（fontconfig 字体管理 + XRender 抗锯齿，中英混排/标点/度量实机验证通过）；②po 全量转 UTF-8（94 个文件，含 1999 年遗留坏转义与格式符修复）并以 zh_CN.UTF-8 目录统一装载，中文桌面实机验证通过——K 菜单「程序/桌面/窗口/帮助/退出」等全部中文上屏。附带：Qt1 剪贴板 UTF8_STRING/CLIPBOARD 补丁与全面 UTF-8 融合补丁（qutf8 基础设施 17 项单元测试全过，QLineEdit/QMultiLineEdit 编辑按字符边界）同批完成。

## 2026-08-27

- 仓库初始化为 git 并完成三次分层提交（历史源码基线 → 项目文档与构建脚本 → 路线乙补丁：Xft 渲染/UTF8_STRING 剪贴板/全面 UTF-8 融合），构建产物（*/build、staging、dist）经 .gitignore 排除；同日完成依赖安装与 7 模块全量编译（byacc/flex/libglu/libssl 补装），Xvfb 英文桌面冒烟测试通过，Xft 中文渲染经实机截图验证（中英混排/标点/度量同源正常）。

## 2026-08-27

- 细化 deb 交付内容与安装/卸载要求（AGENTS.md 目标 5、路线图、README 打包交付节）：.deb 开箱即用零手工配置——含 /usr/kde1 全树、xsessions 会话入口、startkde 环境包装脚本（自动设置 PATH/LD_LIBRARY_PATH/KDEDIR，用户无需 .xinitrc 与 export）、moc-qt1 入口、运行时依赖声明；安装卸载走 dpkg 标准生命周期（apt install / apt remove 干净移除）；按模块拆分核心包与可选包；并修正 README 打包节残留的「与 build.sh 直装等价」旧表述。

## 2026-08-27

- 交付模式变革：正式安装只经 deb 包，取消脚本 sudo 直装——build.sh 改为「构建 + DESTDIR ./staging 暂存」模式，整体删除提权体系（rootcmd / doas-sudo-su 探测 / /usr/bin moc-qt1 软链），改以 PATH / LD_LIBRARY_PATH / CMAKE_PREFIX_PATH 指向暂存区供模块间依赖查找；clean.sh 同步清理 staging/；AGENTS.md §1 目标 5（唯一正式安装方式）/§3 目录表/§4 构建运行说明与 README.md 目标、构建、运行节全部同步更新。

## 2026-08-27

- kfm 新内核技术路线定案：主选 WebKit2GTK（Debian 12 官方包，kfm 侧手写 XEmbed 容器挂载 WebKit 视图，与 1999 原始渲染器构成双内核可切换，先做最小嵌入原型再动 kfm 本体），CEF 备选，Firefox/Gecko 排除；AGENTS.md §1 目标 3/§4 依赖/§7 路线图与 README.md 同步，依赖列表新增 libwebkit2gtk-4.1-dev。

## 2026-08-27

- 验收标准第 7 条从「多字节编辑精度」升级为「全面 UTF-8 融合」：多字节感知覆盖 Qt 内核所有字节级文本处理与全部 KDE 模块的自绘/字符串路径，KDE 1 整体完美运行于 UTF-8 系统；路线图对应条目同步改写；README.md 第 7 条同步。

## 2026-08-27

- 验收标准扩为七条并新增路线图三项：图形登录由 lightdm 等显示管理器管理（xsessions 会话入口，不依赖 startx）、事件音效接入现代音频栈（PipeWire/PulseAudio，解决 /dev/dsp 失效）、Qt1 编辑部件多字节感知（光标/退格/选区按 UTF-8 字符边界，不再作为已知限制）；Xwayland 兼容验证明确列为暂缓项；README.md 目标列表、路线乙说明与运行方式同步更新。

## 2026-08-27

- 验收标准第 4 条扩充中文完善范围（kvt 终端中文显示、与现代应用双向中文剪贴板），路线图新增两个功能项：Qt1 剪贴板 UTF8_STRING 补丁、kvt 终端 UTF-8 渲染（kfm/kdehelp 自绘路径一并确认）；README.md 总体目标同步。

## 2026-08-27

- 新增翻译质量规范并入验收标准第 4 条：翻译禁止闭门自造，每条译法须上网检索核实，且术语对齐当今 KDE 6（Plasma 6 / KDE Gear）的 zh_CN 官方翻译，1999 年自带 GB2312 旧翻译仅作参考底稿、与现代术语冲突时以现代为准；AGENTS.md §1/§7 与 README.md 总体目标同步更新。

## 2026-08-27

- 新增验收标准第 5 条「Debian 软件包交付」：最终产出源码包（sdeb）与二进制 .deb 包，分目录存放（dist/src/ 与 dist/deb/），安装效果与 build.sh 直装等价；AGENTS.md §1/§3/§7 与 README.md 总体目标、打包交付说明同步更新。

## 2026-08-27

- 路线乙字体方案调整：放弃 ISO10646 点阵字体（XmbDrawString + XFontSet 老路线），改为与当今 KDE/XFCE 相同的字体管理与显示（fontconfig 管理匹配 + Xft/XRender 抗锯齿渲染 UTF-8，直接使用现代 TTF/OTF 中文字体）；AGENTS.md §1/§4/§5 注释示例/§7 路线图与 README.md 路线乙描述、依赖列表（新增 libxft-dev / libfontconfig1-dev / libfreetype-dev）全部同步更新。

## 2026-08-27

- AGENTS.md §1 项目使命新增四条总体目标（验收标准）：UTF-8 系统上流畅稳定运行 zh_CN 桌面、fcitx5 与 fcitx 两代输入法框架均须稳定运行、kfm 浏览器内核升级或新旧双内核可切换、界面全面中文化与英文对齐；§7 路线图相应新增三个待办项；README.md 首节与「中文支持」一节同步该目标口径。

## 2026-08-27

- 落实「内容同步更新」要求并新增为 AGENTS.md 工作约定第 7 条：build.sh 补中文 5W1H/伪代码注释、输出信息中文化、失败分支补含位置与原因的错误输出（构建逻辑经 diff 验证零变化）；clean.sh 模块清单由 3 个补齐至与 build.sh 一致的 7 个并补注释头；文档引用统一为实际文件名 AGENTS.md，其 §3 目录表补 clean.sh 条目，README.md 构建一节补 clean.sh 用法。

## 2026-08-27

- agent.md 新增 §5「编码与文档规范」：统一中文 UTF-8 与历史文件禁转码、5W1H + 伪代码强制注释规范（含 C++ 合规/反例）、注释密度与同步原则、各层日志设施用法（Qt1 debug/warning、KDE1 kdebug、脚本 stderr）；全文改为编号章节结构。
- 项目初始化：建立 `agent.md`（工作章程）、重写 `README.md`（代码来历、版权与维护者、GLM-5.3 作者声明、中文支持考证结论与路线乙方案、构建指南）、建立本文件为唯一修改日志。
- 移除全部 git 历史（根目录 `.git`、7 个子仓库各自的 `.git`、根目录 `.gitmodules`），仓库转为普通目录树，准备接入维护者自有 git 仓库。
- 前置工作备档：源码取自 GitHub `NishiOwO/kde1` 元仓库及其 `qt1` / `kde1-*` 子仓库（即 KDE 1.1.2 + Qt Free Edition 1.44 历史源码的现代构建适配版），取回后完成版本考证（1.1.2 确认为 KDE 1 系列终版）与中文支持源码分析（结论固化于 README.md「中文支持」一节）。
