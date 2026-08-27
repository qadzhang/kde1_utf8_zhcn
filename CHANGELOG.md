# CHANGELOG — 修改日志

> 本文件是全项目**唯一**允许记录修改历史的地方。条目新的在最上，一次工作对应一条。其余所有文档（agent.md、README.md 等）禁止出现过程性/日志式内容，只保留当前最终状态。

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
