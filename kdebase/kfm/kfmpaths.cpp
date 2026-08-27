#include "kfmpaths.h"
/* [KDE1 Revival 2026] XDG 用户目录兼容（Modified for the KDE1 Revival Project）
   Maintainer: <维护者姓名> <邮箱>， Modifications written with GLM-5.3 (Z.ai) */
#include <stdlib.h>
#include <string.h>
#include <kconfig.h>
#include <qdir.h>
#include <qfileinfo.h>
#include <kapp.h>

#include <unistd.h>

#include <cstdlib>

#ifdef HAVE_PATHS_H
#include <paths.h>
#endif

#ifndef _PATH_TMP
#define _PATH_TMP "/tmp/"
#endif

QString* KFMPaths::desktopPath = 0L;
QString* KFMPaths::templatePath = 0L;
QString* KFMPaths::autostartPath = 0L;
QString* KFMPaths::trashPath = 0L;
QString* KFMPaths::cachePath = 0L;

void KFMPaths::initPaths() 
{
  if ( desktopPath == 0L )
    desktopPath = new QString;
  if ( templatePath == 0L )
    templatePath = new QString;
  if ( autostartPath == 0L )
    autostartPath = new QString;
  if ( trashPath == 0L )
    trashPath = new QString;
  if ( cachePath == 0L )
    cachePath = new QString;
  
  KConfig *config = kapp->getConfig();
  config->setGroup( "Paths" );

  /* [KDE1 Revival 2026] XDG 用户目录兼容：
     What: 解析 ~/.config/user-dirs.dirs 取现代桌面（XFCE/KDE6 等）建立
           的标准用户目录，使 KDE1 与宿主桌面共用同一套目录（同一桌面、
           同一模板、同一回收站），不再另建 1999 年的 ~/Desktop 体系
     Why : 宿主为 UTF-8 中文系统时 XDG 桌面是 ~/桌面，而 KDE1 原逻辑硬编码
           ~/Desktop——两套桌面目录并存即用户所见的不兼容
     Who : 仅作为下述四项路径的“默认值”；用户在 kfm 配置 Paths 组中的
           显式设置仍然优先（readEntry 覆盖逻辑保持不变）
     When : KFMPaths::initPaths 初始化时读取一次
     Where: ~/.config/user-dirs.dirs（XDG_USER_DIRS 标准，xdg-user-dirs 生成）
     How : 逐行找 KEY="$HOME/值" 形式，展开 $HOME；无配置/无该键时用 fallback */
  QString xdg_home_dir( const char *key, const QString & fallback ); // 见文件尾

  // Desktop Path：默认取 XDG_DESKTOP_DIR（如 ~/桌面），回退 ~/Desktop
  *desktopPath = xdg_home_dir( "XDG_DESKTOP_DIR", QDir::homeDirPath() + "/Desktop" );
  *desktopPath = config->readEntry( "Desktop", *desktopPath);
  *desktopPath = QDir::cleanDirPath( *desktopPath );
  if ( desktopPath->right(1) != "/")
    *desktopPath += "/";
  
  // Templates Path：默认取 XDG_TEMPLATES_DIR（如 ~/模板），回退原相对路径
  *templatePath = xdg_home_dir( "XDG_TEMPLATES_DIR", (*desktopPath + "Templates") );
  *templatePath = config->readEntry( "Templates" , *templatePath);
  *templatePath = QDir::cleanDirPath( *templatePath );
  if ( templatePath->right(1) != "/")
    *templatePath += "/";

  // Autostart Path：挪出桌面目录（避免 XDG 桌面上出现 Autostart 文件夹），
  // 收进 KDE1 自身的数据区 ~/.kde/share/autostart（kdelnk 与 XDG .desktop
  // 格式不兼容，保持 KDE1 私有语义）
  *autostartPath = QDir::homeDirPath() + "/.kde/share/autostart";
  *autostartPath = config->readEntry( "Autostart" , *autostartPath);
  *autostartPath = QDir::cleanDirPath( *autostartPath );
  if ( autostartPath->right(1) != "/")
    *autostartPath += "/";

  // Trash Path：对齐 freedesktop 回收站 ~/.local/share/Trash/files——
  // kfm 删除的文件现代文件管理器可见（info 元数据暂不生成，恢复功能
  // 仍按目录内容工作）
  *trashPath = QDir::homeDirPath() + "/.local/share/Trash/files";
  *trashPath = config->readEntry( "Trash" , *trashPath);
  *trashPath = QDir::cleanDirPath( *trashPath );
  if ( trashPath->right(1) != "/")
    *trashPath += "/";
  
  cachePath->sprintf(_PATH_TMP"/kfm-cache-%i", (int)getuid() );

  if (!QFileInfo(_PATH_TMP).isWritable())
  {
      QString s;
      s.sprintf("ERROR ! kfm needs write permissions to %s\n",_PATH_TMP);
      execlp("kfmwarn","kfmwarn",s.data(), 0);
      fprintf(stderr, s.data()); // in case kfmwarn didn't work
      exit( 1 );
  }
}

QString KFMPaths::DesktopPath()
{
  return *desktopPath;
}
 
QString KFMPaths::TemplatesPath()
{
  return *templatePath;
}

QString KFMPaths::AutostartPath()
{
  return *autostartPath;
}

QString KFMPaths::TrashPath()
{
  return *trashPath;
}

QString KFMPaths::CachePath()
{
  return *cachePath;
}

/* [KDE1 Revival 2026] XDG user-dirs.dirs 解析（说明见 initPaths 内注释）：
   伪代码：读 $XDG_CONFIG_HOME/user-dirs.dirs（默认 ~/.config/）→
   逐行匹配 KEY="$HOME/值" 或 KEY="/绝对/路径" → 展开 $HOME →
   命中返回该目录；否则返回 fallback */
QString xdg_home_dir( const char *key, const QString & fallback )
{
    const char *cfg = getenv( "XDG_CONFIG_HOME" );
    QString path = cfg && *cfg ? QString(cfg) : QDir::homeDirPath() + "/.config";
    path += "/user-dirs.dirs";
    FILE *f = fopen( path, "r" );
    if ( !f )
        return fallback;
    QString kwd = QString(key) + '="';
    char line[512];
    while ( fgets( line, sizeof(line), f ) ) {
        char *p = strstr( line, kwd );
        if ( !p )
            continue;
        p += kwd.length();
        char *q = strchr( p, '"' );
        if ( !q )
            continue;
        *q = 0;
        QString val( p );
        fclose( f );
        if ( val.left(6) == "$HOME/" )
            val = QDir::homeDirPath() + "/" + val.mid( 6, val.length()-6 );
        else if ( val.left(5) == "$HOME" )
            val = QDir::homeDirPath() + val.mid( 5, val.length()-5 );
        if ( val.isEmpty() )
            return fallback;
        return val;
    }
    fclose( f );
    return fallback;
}
