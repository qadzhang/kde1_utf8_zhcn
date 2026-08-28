# CHANGELOG — 修改日志

> 本文件是全项目**唯一**允许记录修改历史的地方。条目新的在最上，一次工作对应一条。其余所有文档（agent.md、README.md 等）禁止出现过程性/日志式内容，只保留当前最终状态。

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
