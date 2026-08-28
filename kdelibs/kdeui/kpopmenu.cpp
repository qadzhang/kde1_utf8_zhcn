#include "kpopmenu.h"
#include "kpopmenu.h"
#include <qpainter.h>
#include <klocale.h>
#include <kapp.h>

KPopupMenu::KPopupMenu(QWidget *parent, const char *name)
    : QPopupMenu(parent, name)
{
    initialize(klocale->translate("Untitled"));
}

KPopupMenu::KPopupMenu(const char *title, QWidget *parent, const char *name)
    : QPopupMenu(parent, name)
{
    initialize(title);
}


KPopupMenu::~KPopupMenu()
{
}


const char *KPopupMenu::title() const
{
    return text(0);
}

void KPopupMenu::setTitle(const char *title)
{
    changeItem(title, 0);
}
    
void KPopupMenu::initialize(const char *title)
{
    insertItem(title);
    insertSeparator();
    insertSeparator();
}
   
//   Modified for the KDE1 Revival Project, 2026（TQt3 底座迁移）
//   原 paintCell 覆盖（Qt1 QPopupMenu 的表格绘制路径）绘制菜单首项标题；
//   TQt3 的 TQPopupMenu 无 paintCell/cellHeight/cellWidth 概念，标题绘制
//   改用 TQPopupMenu 内建 insertTitle() 项——此处覆盖实现移除，标题功能
//   由内建标题项承接（调用方插入标题时经 insertTitle 路径）
    
#include "kpopmenu.moc"
