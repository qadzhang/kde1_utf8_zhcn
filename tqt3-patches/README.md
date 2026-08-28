# tqt3-patches — TQt3 底座补丁目录

- 本目录存放对 `tqt3/` 快照的全部修改，`.patch` 后缀、**按文件名序**在构建时
  由 `build.sh` 打入 `tqt3-build/`（拷贝出的构建树）；`tqt3/` 本体保持 pristine。
- 当前状态：**空**——TQt3 r14.1.6（commit 3a07835，2026-04-11）在 Debian 12
  上经构建配置项适配后零补丁编译通过。Trinity 本身就是补丁维护完备的 Qt3
  fork（持续收录安全修复、新工具链/新 X 兼容、输入法与双向文本增强），
  发行版层面无需再叠补丁。
- 快照更新流程（见 README.md「底座」节）：从官方 Gitea 重拉新 tag →
  替换 `tqt3/` → 重跑 `port/gen_q1compat.sh` 与 `port/gen_fwheaders.sh` →
  删除 `tqt3-build/` 重建 → 若上游改动导致编译失败，在此目录新增补丁修复。
