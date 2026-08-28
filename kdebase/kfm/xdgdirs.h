//
//   Written with GLM-5.3 (Z.ai) for the KDE1 Revival Project
//
// xdgdirs.h —— XDG 用户目录解析（kdebase 内部公共头）
//
// ┌─ What : 解析 ~/.config/user-dirs.dirs，按 KEY 取现代桌面（XFCE/KDE6 等）
//           建立的标准用户目录路径，未命中时返回调用方给的 fallback
// │  Why  : KDE1 体系里 kfm 与 krootwm 各有一份 1999 年的路径默认值
//           （~/Desktop 等），曾出现「只修 kfm 一处、krootwm 仍在旧目录」
//           的教训——解析逻辑必须收敛为唯一实现，各处共用（AGENTS §6.7
//           「同一事实多处出现须同步」的代码形态）
// │  Who  : kdebase/kfm/kfmpaths.cpp（桌面/模板/回收站/自启动）、
//           kdebase/krootwm/krootwm.C（右键菜单的桌面/模板）
// │  When : 各自初始化时调用一次
// │  Where: ~/.config/user-dirs.dirs（XDG_USER_DIRS 标准，xdg-user-dirs 生成）
// │  How  : 伪代码：
//           1. 路径 = $XDG_CONFIG_HOME 或 ~/.config，拼 /user-dirs.dirs
//           2. 打不开（文件不存在）→ 直接返回 fallback
//           3. 逐行找 "KEY=\"" 子串 → 跳过键名与引号取值到下一个引号
//           4. 值形如 $HOME/xxx → 展开 home；空值 → fallback
//           5. 返回展开结果（UTF-8 原样透传，QString Latin-1 往返保真）
//
#ifndef KDE1_XDGDIRS_H
#define KDE1_XDGDIRS_H

#include <qdir.h>
#include <qstring.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static QString xdg_home_dir( const char *key, const QString & fallback )
{
    const char *cfg = getenv( "XDG_CONFIG_HOME" );
    QString path = cfg && *cfg ? QString(cfg) : QDir::homeDirPath() + "/.config";
    path += "/user-dirs.dirs";
    FILE *f = fopen( path, "r" );
    if ( !f )
        return fallback;
    QString kwd = QString(key) + "=\"";
    char line[512];
    while ( fgets( line, sizeof(line), f ) ) {
        char *p = strstr( line, kwd );
        if ( !p )
            continue;
        p += strlen(key) + 2;   /* 跳过键名与开引号（QString::length 含终止 0，勿用） */
        char *q = strchr( p, '"' );
        if ( !q )
            continue;
        *q = 0;
        QString val( p );
        fclose( f );
        int vlen = strlen( (const char *)val );
        if ( val.left(6) == "$HOME/" )
            val = QDir::homeDirPath() + "/" + val.mid( 6, vlen-6 );
        else if ( val.left(5) == "$HOME" )
            val = QDir::homeDirPath() + val.mid( 5, vlen-5 );
        if ( val.isEmpty() )
            return fallback;
        return val;
    }
    fclose( f );
    return fallback;
}

#endif // KDE1_XDGDIRS_H
