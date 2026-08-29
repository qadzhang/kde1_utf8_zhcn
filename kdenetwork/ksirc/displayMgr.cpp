#include "displayMgr.h"

#include <qobject.h>


DisplayMgr::DisplayMgr()
{
}

DisplayMgr::~DisplayMgr()
{
}

void DisplayMgr::newTopLevel(QWidget *, bool show)
{
  tqWarning("Display Manger: newTopLevel called");
}

void DisplayMgr::removeTopLevel(QWidget *)
{
  tqWarning("Display Manger: removeTopLevel called");
}

void DisplayMgr::show(QWidget *)
{
  tqWarning("Display Manger: show called");
}

void DisplayMgr::raise(QWidget *)
{
  tqWarning("Display Manger: raise called");
}

void DisplayMgr::setCaption(QWidget *w, const char *)
{
  tqWarning("Display Manger: setCaption called");
}

/* TQt3 迁移:displayMgr 无 Q_OBJECT,原 AUTOMOC 残迹 include 已删 */
