
#ifndef K1GUARD_kdebase_kwm_warning_h
#define K1GUARD_kdebase_kwm_warning_h
// klogout 
// Copyright (C) 1997 Matthias Ettrich

#include <qmsgbox.h>
#include <qlined.h>
#include <qlabel.h>
#include <qframe.h>
#include <qdialog.h>
#include <qpushbt.h>
#include <qlistbox.h>
#include <qstring.h>

#include <qapp.h>
#include <qwindefs.h>
/* 预解析含 Bool/Status 枚举的 TQt3 头：必须在下方 X11 头的
 * #define Bool int / #define Status int 生效前完成解析，否则枚举被宏击穿
 * （ntqvariant.h 的 enum Type 含 Bool、ntqmovie.h 的 enum Status）。 */
#include <ntqvariant.h>
#include <ntqmovie.h>
#include <X11/Xlib.h>
#include <stdlib.h>

#include "client.h"


class KWarning : public QDialog{
  Q_OBJECT
public:
  KWarning( QWidget *parent=0, const char *name=0, WFlags f=0);
  bool do_grabbing();
  void setText(const char* text, bool with_button);
  bool eventFilter( QObject *, QEvent * );
public slots:
  void ok();
  void release();
private:
  QFrame *frame;
  QPushButton* button;
  QLabel* label;
  void SetPointerGrab(QPoint);
  Client* reactive;
};
#endif /* K1GUARD_kdebase_kwm_warning_h */
