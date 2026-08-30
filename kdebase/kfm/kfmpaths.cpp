#include "kfmpaths.h"
/* [KDE1 Revival 2026] XDG 用户目录兼容（Modified for the KDE1 Revival Project）
   Maintainer: <维护者姓名> <邮箱>， Modifications written with GLM-5.3 (Z.ai) */
#include <stdlib.h>
#include <string.h>
#include <kconfig.h>
#include "xdgdirs.h"	/* [KDE1 Revival 2026] XDG 解析公共头（krootwm 共用）*/
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

  /* [KDE1 Revival 2026] 默认桌面图标补齐
     What: 桌面缺 Home/Trash 时从安装树 applnk 拷入；并生成指向 XDG
           模板目录的 Templates 图标（含中文显示名）
     Why : 1999 年由发行版脚本一次性初始化，现代 deb 装到全新 XDG 桌面
           （如 ~/桌面）不自带，桌面将只有文字没有图标
     When : initPaths 是 kfm 启动必经早期路径（先于 main 中段 IPC 检测）
     How : 幂等——存在即跳过；源路径经 KDEDIR 拼接；Trash 经 trash:/
           协议指向 TrashPath 免改写 */
  {
      const char *def_icons[] = { "Home.kdelnk", "Trash.kdelnk", 0 };
      const char *kdedir_env = getenv( "KDEDIR" );
      QString iconsrc( kdedir_env && *kdedir_env ? kdedir_env : "/usr/kde1" );
      iconsrc += "/share/applnk/";
      for ( int di = 0; def_icons[di]; di++ ) {
          QString dst = *desktopPath + def_icons[di];
          if ( access( dst, F_OK ) != 0 ) {
              /* [KDE1 Revival 2026] 原 cmd.sprintf("%s%s%s") 弃用：
               * TQt3 的 QString::sprintf 不遵循 codecForCStrings——
               * desktopPath 为中文 XDG 目录（~/桌面）时 UTF-8 字节被按
               * latin-1 拼装，cp 拿到乱码路径静默失败、桌面图标缺失。
               * 改 QString 拼接（全程 UTF-16，data() 输出 UTF-8）。 */
              QString cmd = QString( "cp '" ) + iconsrc + def_icons[di]
                          + QString( "' '" ) + dst + QString( "'" );
              system( cmd.data() );
          }
      }
      QString tpl = *desktopPath + "Templates.kdelnk";
      if ( access( tpl, F_OK ) != 0 ) {
          FILE *tf = fopen( tpl, "w" );
          if ( tf ) {
              fprintf( tf,
                  "# KDE Config File\n"
                  "[KDE Desktop Entry]\n"
                  "Type=Application\n"
                  "Exec=kfmclient1 folder %%u file:$HOME/模板\n"
                  "Icon=folder.xpm\n"
                  "MiniIcon=folder.xpm\n"
                  "Terminal=0\n"
                  "Name=Templates\n"
                  /* 双键冗余：KConfig 语言键匹配 KLocale::language() 返回值，
                     zh_CN.UTF-8 环境匹配前者，无编码后缀环境匹配后者 */
                  "Name[zh_CN.UTF-8]=模板\n"
                  "Name[zh_CN]=模板\n" );
              fclose( tf );
          }
      }
  }

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

