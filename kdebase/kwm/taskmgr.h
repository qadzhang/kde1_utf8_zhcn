
#ifndef K1GUARD_kdebase_kwm_taskmgr_h
#define K1GUARD_kdebase_kwm_taskmgr_h
// ktask 
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

class KListBoxItem_Desktop : public QListBoxItem {
public:
  KListBoxItem_Desktop(const char *text);
  virtual void paint(QPainter *);
  virtual int height(const QListBox *) const;
  virtual int width(const QListBox *) const;

  TQString text() const;  // TQt3 迁移：基类签名 TQString，覆盖返回类型对齐

private:
  QString _text;
};

class KListBoxItem_Program : public QListBoxItem {
public:
  KListBoxItem_Program(QPixmap &, const char *text);
  virtual void paint(QPainter *);
  virtual int height(const QListBox *) const;
  virtual int width(const QListBox *) const;

  TQString text() const;  // TQt3 迁移：基类签名 TQString，覆盖返回类型对齐

private:
  QString _text;
  QPixmap _pm;
};

class Ktask : public QDialog {
  Q_OBJECT
public:
  Ktask( QWidget *parent=0, const char *name=0, WFlags f=0);
  bool do_grabbing();
  bool eventFilter( QObject *, QEvent * );
  void prepareToShow(QStrList* strlist, int active);

public slots:
  void cleanup();

signals:
  void changeToClient(QString label);

protected:
  void    resizeEvent( QResizeEvent * );

private slots:
  void logout();
  void listboxSelect(int index);
  void buttonSelect();
private:
  QPushButton* button;
  QPushButton* button_logout;
  QPushButton* button_cancel;
  QListBox* listbox;
  QFrame *frame;
  QLabel* label;
  void SetPointerGrab(QPoint);
  Client* reactive;
};
#endif /* K1GUARD_kdebase_kwm_taskmgr_h */
