# tqt3-patches — TQt3 底座补丁目录

- 本目录存放对 `tqt3/` 快照的全部修改，`.patch` 后缀、**按文件名序**在构建时
  由 `build.sh` 打入 `tqt3-build/`（拷贝出的构建树）；`tqt3/` 本体保持 pristine。
- 当前状态：**3 个**——`002-qstring-eq-codec-for-cstrings.patch`：TQt3 原生
  `operator==(const TQString&, const char*)` 逐字节比较（UTF-16 码位 vs 单字节），
  中文等非 ASCII 内容经 codec 解码进 TQString 后与原始字节流永不相等且不受
  setCodecForCStrings 影响；补丁使其在设置了 C 字符串 codec 时先经 codec 解码
  再比较（未设置 codec 的原生程序行为不变）。
- `003-enable-builtin-gif.patch`：打开 TQt3 内置 GIF 解码器（Qt 历史上因
- `004-no-saveunder-for-popups.patch`：弹出菜单（QPopupMenu 等覆盖重定向窗）与
  WStyle_Tool 工具窗不再请求 X11 save-under——X.Org 官方声明 save-under 与
  backing-store 从非保证且有实现缺陷，现代工具链（GTK4 等）已弃用；部分驱动
  （vmware）上 OR 窗卸载的恢复路径产生黑块。暴露区一律交客户端 Expose 重绘
  （TQt3 完整处理），与 XFCE 行为一致。
  LZW 专利默认禁用；专利早已过期，KDE1 大量资源依赖 GIF 解码）。KDE1 代码大量该形态比较（kfm
  目录条目按 URL 匹配路由是首个实锤受害点）。TQt3 r14.1.6（commit 3a07835，
  2026-04-11）在 Debian 12 上除此之外零补丁编译通过——Trinity 本身就是补丁
  维护完备的 Qt3 fork，发行版层面无需再叠其他补丁。
- 快照更新流程（见 README.md「底座」节）：从官方 Gitea 重拉新 tag →
  替换 `tqt3/` → 重跑 `port/gen_q1compat.sh` 与 `port/gen_fwheaders.sh` →
  删除 `tqt3-build/` 重建 → 若上游改动导致编译失败，在此目录新增补丁修复。
