#!/bin/sh
# ─────────────────────────────────────────────────────────────────────────────
# sandbox.sh — KDE1 后台测试沙箱（Xvfb + fcitx5 + x11vnc）
#
# Written with GLM-5.3 (Z.ai) for the KDE1 Revival Project
#
# ┌─ What : 在虚拟显示 :99 上后台拉起一套完整 KDE1 桌面（kwm/kpanel/kfm/krootwm
# │        + fcitx5 输入法 + padsp 音效转发），并经 x11vnc 暴露为本机 VNC 服务，
# │        供开发者在 XFCE 等宿主会话里用 Remmina 随时进入体验，不必注销切换。
# │  Why  : 正式体验须从 lightdm 注销换会话，打断工作流；沙箱让"边开发边试"
# │        随时可做，且完全不触碰 lightdm 与宿主桌面。
# │  Who  : 开发者手工运行；依赖 staging/ 构建产物、Xvfb、x11vnc（apt 安装）、
# │        fcitx5（+ chinese-addons 拼音）、padsp；缺组件会在输出里明确提示。
# │  When : `start` 手动拉起，常驻后台直至 `stop` 或关机；`status` 查看状态；
# │        `stop` 整组清理，不留游离进程。
# │  Where: 显示 :99 / VNC 127.0.0.1:5901（仅本机可连）；日志与 PID 文件在
# │        /tmp/kde1-sandbox/；二进制一律取自本仓库 staging 树（开发版），
# │        与 deb 安装版 /usr/kde1 互不影响。
# │  How  : 伪代码——
# │        start:
# │          1. 健康检查：staging 产物、外部工具齐备；:99 未被占用（锁文件
# │             存在且属主进程存活则拒绝重复启动）
# │          2. 起 Xvfb :99（1280x960x24）→ 为 fcitx5 自起一条私有会话 DBus
# │             （与宿主总线隔离，避免 org.fcitx.Fcitx5 总线名冲突）→
# │             起 fcitx5（为 :99 提供 XIM 服务，KDE1/TQt3 经 XMODIFIERS 接入）
# │             → 以 setsid 拉起 startkde（kwm 经 exec 链成为进程组锚点）；
# │             会话环境与 deb 安装版包装模板 staging-integration/startkde-kde1.in
# │             完全一致：KDEDIR/TQTDIR/PATH + LD_LIBRARY_PATH（含 tqt3/lib）；
# │             音效经 LD_PRELOAD 预载 libpulsedsp.so 把 /dev/dsp 的 OSS 调用
# │             转发到 PipeWire/PulseAudio（不调用 padsp 命令——其自带的预载
# │             路径模板在 Debian 12 上损坏，且 LD_PRELOAD 必须写库文件路径）→
# │             起 x11vnc（仅监听 127.0.0.1，免密码，-forever 支持断开重连）
# │          3. 各组件 PID 落盘；就绪探测：等待 startkde 的 exec 链走完、
# │             锚点进程变为 kwm 即视为会话可用
# │        stop: 逆序定点清理——x11vnc → startkde 整个进程组（kill 负 PID，
# │              覆盖 kpanel/kfm/krootwm/kwmsound/kbgndwm 等全部子进程）→
# │              kaudioserver（daemon() 自守护会脱离进程组，故单独记 PID）→
# │              fcitx5 → 私有 DBus → Xvfb；最后清 PID 文件与 X 锁文件。
# │              全程只按记录的 PID 精确清理，严禁按名字全局 pkill——
# │              宿主会话里可能存在同名进程（如用户自己的 fcitx5）。
# ─────────────────────────────────────────────────────────────────────────────

set -u

REPO=$(cd "$(dirname "$0")" && pwd)
BIN="$REPO/staging/usr/kde1/bin"
RUN=/tmp/kde1-sandbox
DISP=:99
VNCPORT=5901

# is_alive PID文件: 文件存在且其中记录的进程仍存活则为真
is_alive() { [ -f "$1" ] && kill -0 "$(cat "$1" 2>/dev/null)" 2>/dev/null; }

do_start() {
    mkdir -p "$RUN"

    # 健康检查①：staging 产物与外部工具
    if [ ! -x "$BIN/startkde" ]; then
        echo "错误: staging 未构建（缺 $BIN/startkde），请先运行 ./build.sh" >&2
        exit 1
    fi
    for tool in Xvfb x11vnc fcitx5; do
        command -v "$tool" >/dev/null 2>&1 || {
            echo "错误: 缺少 $tool，请先安装（x11vnc 为 apt 包；fcitx5 见 README 依赖清单）" >&2
            exit 1
        }
    done

    # 健康检查②：:99 占用情况——X 锁文件存在且其记录的 PID 存活，说明已有 X server
    if [ -e /tmp/.X99-lock ] && kill -0 "$(cat /tmp/.X99-lock 2>/dev/null)" 2>/dev/null; then
        echo "错误: 显示 :99 已被占用，沙箱可能已在运行（./sandbox.sh status 查看）" >&2
        exit 1
    fi
    rm -f /tmp/.X99-lock /tmp/.X99-unix/X99 2>/dev/null

    # ① Xvfb：沙箱的"屏幕"。-nolisten tcp 只留本机 unix 套接字，不对外网开放
    Xvfb $DISP -screen 0 1280x960x24 -dpi 96 -nolisten tcp >"$RUN/xvfb.log" 2>&1 &
    echo $! > "$RUN/xvfb.pid"
    sleep 1
    if ! is_alive "$RUN/xvfb.pid"; then
        echo "错误: Xvfb 启动失败，详见 $RUN/xvfb.log" >&2
        exit 1
    fi

    # ② fcitx5 专属私有会话 DBus：若复用宿主总线，与宿主 fcitx5 抢
    #    org.fcitx.Fcitx5 总线名会互踢；私有总线彻底隔离。停机时按记录 PID 回收。
    #    注意必须先清掉继承来的总线地址——dbus-launch 一旦检测到
    #    DBUS_SESSION_BUS_ADDRESS 已设置就会直接复用既有总线而不新建。
    unset DBUS_SESSION_BUS_ADDRESS DBUS_SESSION_BUS_PID 2>/dev/null
    if command -v dbus-launch >/dev/null 2>&1; then
        eval "$(dbus-launch --sh-syntax 2>/dev/null)"
        [ -n "${DBUS_SESSION_BUS_PID:-}" ] && echo "$DBUS_SESSION_BUS_PID" > "$RUN/dbus.pid"
        echo "${DBUS_SESSION_BUS_ADDRESS:-}" > "$RUN/dbus.addr"
    fi

    # ③ fcitx5：为 :99 注册 XIM 服务器（前台运行 + setsid，$! 即其真实 PID）。
    #    DISPLAY 必须显式指向 :99——否则继承外层 shell 的 :0 会去宿主显示器
    #    注册 XIM，与宿主输入法冲突而永远 "Failed to open xim"。
    #    中文切换默认快捷键 Ctrl+Space；拼音来自 fcitx5-chinese-addons。
    # [KDE1 Revival 2026] 延迟 4 秒 + 替换陈旧实例：fcitx5 的托盘图标
    # 停靠申请需在 kpanel 的 _NET_SYSTEM_TRAY manager 上线后发出
    # （fcitx5 不重试），而 kpanel 由 startkde（下方④）拉起；-r 为
    # fcitx5 官方的 replace 机制，替换上一轮沙箱可能残留的 :99 实例。
    ( sleep 4
      DISPLAY=$DISP XMODIFIERS=@im=fcitx setsid fcitx5 -r >"$RUN/fcitx.log" 2>&1 &
      echo $! > "$RUN/fcitx.pid" ) &

    # 音效兼容：libpulsedsp 把 OSS /dev/dsp 调用转发到 PulseAudio/PipeWire，
    # 探测顺序与 startkde-kde1.in 相同；未找到则跳过预载（无音效但不影响画面）
    PULSEDSP=""
    for _pd in /usr/lib/x86_64-linux-gnu/pulseaudio/libpulsedsp.so \
               /usr/lib/pulseaudio/libpulsedsp.so; do
        [ -f "$_pd" ] && PULSEDSP="$_pd" && break
    done

    # ④ KDE1 会话本体。setsid 使其自成进程组（PGID = 该 PID），stop 时
    #    kill 负 PID 一组全收。环境变量与 deb 安装版包装模板
    #    staging-integration/startkde-kde1.in 保持一字不差（staging 前缀替换）：
    #    TQt3 运行库在 tqt3/lib 下，LD_LIBRARY_PATH 少了它必然报
    #    "libtqt-mt.so.3: cannot open"——这是本脚本第一版踩过的坑。
    KDEDIR="$REPO/staging/usr/kde1"
    if [ -n "$PULSEDSP" ]; then
        setsid env KDEDIR="$KDEDIR" TQTDIR="$KDEDIR/tqt3" \
            PATH="$KDEDIR/bin:$PATH" \
            LD_LIBRARY_PATH="$KDEDIR/lib:$KDEDIR/tqt3/lib${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}" \
            LANG=zh_CN.UTF-8 DISPLAY=$DISP XMODIFIERS=@im=fcitx \
            LD_PRELOAD="$PULSEDSP" \
            "$BIN/startkde" >"$RUN/session.log" 2>&1 &
    else
        setsid env KDEDIR="$KDEDIR" TQTDIR="$KDEDIR/tqt3" \
            PATH="$KDEDIR/bin:$PATH" \
            LD_LIBRARY_PATH="$KDEDIR/lib:$KDEDIR/tqt3/lib${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}" \
            LANG=zh_CN.UTF-8 DISPLAY=$DISP XMODIFIERS=@im=fcitx \
            "$BIN/startkde" >"$RUN/session.log" 2>&1 &
    fi
    echo $! > "$RUN/session.pid"

    # ⑤ x11vnc：把 :99 映射为本机 VNC。-forever 保证 Remmina 断开后服务不退。
    #    不用 -bg（自守护后拿不到 PID）——直接前台进程丢后台跑，$! 即真实 PID。
    x11vnc -display $DISP -rfbport $VNCPORT -localhost -shared -forever -nopw \
        -quiet -o "$RUN/vnc.log" &
    echo $! > "$RUN/vnc.pid"
    sleep 1
    if ! is_alive "$RUN/vnc.pid"; then
        echo "错误: x11vnc 启动失败，详见 $RUN/vnc.log" >&2
        do_stop
        exit 1
    fi

    # ⑥ kaudioserver 用 daemon() 自守护、脱离了会话进程组，单独取最新实例的 PID
    #    （最新即本次沙箱所起；-n 取 pid 序号最大者）
    sleep 3
    pgrep -n -x kaudioserver > "$RUN/kaudioserver.pid" 2>/dev/null

    # 就绪探测：startkde 内部有约 9 秒分级 sleep，末尾 exec kwm；锚点进程
    # 变为 kwm 即会话成型（最多等 30 秒，超时仅提示不失败——可能仍在启动中）
    i=0
    while [ "$i" -lt 30 ]; do
        if [ "$(ps -o comm= -p "$(cat "$RUN/session.pid" 2>/dev/null)" 2>/dev/null)" = "kwm" ]; then
            break
        fi
        sleep 1
        i=$((i + 1))
    done

    echo "── KDE1 沙箱已启动 ──────────────────────────────────"
    echo "  进入方式 : Remmina → 协议 VNC → 地址 127.0.0.1:$VNCPORT （免密码）"
    echo "  中文输入 : 窗口内 Ctrl+Space 切换 fcitx5 拼音"
    echo "  会话状态 : $(do_status | tr '\n' ' ')"
    echo "  日志目录 : $RUN/"
    echo "  停止沙箱 : ./sandbox.sh stop"
    echo "──────────────────────────────────────────────────────"
}

do_stop() {
    # 逆序定点清理；kill 负 PID = 回收整个进程组（kwm 及其兄弟子进程）
    if is_alive "$RUN/vnc.pid"; then kill "$(cat "$RUN/vnc.pid")" 2>/dev/null; fi
    if [ -f "$RUN/session.pid" ]; then
        kill -- -"$(cat "$RUN/session.pid")" 2>/dev/null
    fi
    if is_alive "$RUN/kaudioserver.pid"; then kill "$(cat "$RUN/kaudioserver.pid")" 2>/dev/null; fi
    if is_alive "$RUN/fcitx.pid"; then kill "$(cat "$RUN/fcitx.pid")" 2>/dev/null; fi
    if is_alive "$RUN/dbus.pid"; then kill "$(cat "$RUN/dbus.pid")" 2>/dev/null; fi
    if is_alive "$RUN/xvfb.pid"; then kill "$(cat "$RUN/xvfb.pid")" 2>/dev/null; fi
    rm -f "$RUN"/vnc.pid "$RUN"/session.pid "$RUN"/kaudioserver.pid \
          "$RUN"/fcitx.pid "$RUN"/dbus.pid "$RUN"/dbus.addr "$RUN"/xvfb.pid
    rm -f /tmp/.X99-lock /tmp/.X99-unix/X99 2>/dev/null
    echo "沙箱已停止（日志保留于 $RUN/）"
}

do_status() {
    for name in session fcitx xvfb vnc kaudioserver dbus; do
        pidfile="$RUN/$name.pid"
        if [ ! -f "$pidfile" ]; then
            echo "$name=未启动"
        elif is_alive "$pidfile"; then
            comm=$(ps -o comm= -p "$(cat "$pidfile")" 2>/dev/null)
            echo "$name=运行中($comm)"
        else
            echo "$name=已退出"
        fi
    done
}

case "${1:-}" in
    start)  do_start ;;
    stop)   do_stop ;;
    status) do_status ;;
    *)
        echo "用法: $0 {start|stop|status}" >&2
        echo "  start  在 :99 后台拉起 KDE1 沙箱，VNC 端口 127.0.0.1:$VNCPORT" >&2
        echo "  stop   定点清理沙箱全部进程" >&2
        echo "  status 查看各组件状态" >&2
        exit 1
        ;;
esac
