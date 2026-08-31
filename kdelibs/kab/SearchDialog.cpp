/* -*- C++ -*-
 * This file implements the search dialog..
 * 
 * the KDE addressbook
 * copyright:  (C) Mirko Sucker, 1998
 * license:    GNU Public License, Version 2
 * mail to:    Mirko Sucker <mirko.sucker@unibw-hamburg.de>
 * requires:   C++-compiler, STL, string class,
 *             Nana for debugging
 * $Revision: 1.4 $
 */

#include "SearchDialog.h"
#include "SearchDialogMainWidget.h"
#include "debug.h"

const char* SearchDialog::Keys[]= {
  "title",
  "firstname",
  "additionalName",
  "namePrefix",
  "name",
  "fn",
  "address",
  "town",
  "org",
  "role",
  "orgUnit",
  "orgSubUnit",
  "email",
  "email2",
  "email3",
  "telephone",
  "fax",
  "modem",
  "URL",
  "comment"
  /* , "deliveryLabel" */ };
  

SearchDialog::SearchDialog(QWidget* parent, const char* name)
  : DialogBase(parent, name),
    widget(0)
{
  // ############################################################################
  const char* Descriptions[]= {
    i18n("The title"),
    i18n("The first name"),
    i18n("The additional name"),
    i18n("The name prefix"),
    i18n("The name"),
    i18n("The formatted name"),
    i18n("The address"),
    i18n("The town"),
    i18n("The organization"),
    i18n("The role"),
    i18n("The org unit"),
    i18n("The org subunit"),
    i18n("The email address"),
    i18n("The second email address"),
    i18n("The third email address"),
    i18n("The telephone number"),
    i18n("The fax number"),
    i18n("The modem number"),
    i18n("The homepage URL"),
    i18n("The comment") };
  const int Size=sizeof(Descriptions)/sizeof(Descriptions[0]);
  int count;
  // ----- create main widget:
  widget=new SearchDialogMainWidget(this);
  CHECK(widget!=0);
  setMainWidget(widget);
  // ----- configure dialog:
  setCaption(i18n("kab: Search entries"));
  enableButtonApply(false);
  // showMainFrameTile(false);
  // -----
  for(count=0; count<Size; count++)
    {
      widget->comboSelector->insertItem(Descriptions[count]);
    }
  connect(widget->comboSelector, SIGNAL(activated(int)), SLOT(keySelected(int)));
  // [KDE1 Revival 2026] TQLineEdit 只有 textChanged(const TQString&)——const char* 版连接静默失败
  connect(widget->lePattern, SIGNAL(textChanged(const TQString&)), 
	  SLOT(valueChanged(const TQString&)));
  connect(widget, SIGNAL(sizeChanged()), SLOT(initializeGeometry()));
  keySelected(0);
  resize(minimumSize());
  widget->lePattern->setFocus();
  // ############################################################################
}


SearchDialog::~SearchDialog()
{
  // ############################################################################
  // ############################################################################
}

void SearchDialog::keySelected(int index)
{
  REQUIRE(index>=0 && (unsigned)index<(sizeof(Keys)/sizeof(Keys[0])));
  // ############################################################################
  L("SearchDialog::keySelected: key %s selected.\n", Keys[index]);
  key=Keys[index];
  // ############################################################################
}

void SearchDialog::valueChanged(const TQString& value)
{
  // ############################################################################
  L("SearchDialog::valueChanged: new value %s.\n", value.utf8().data());
  // [KDE1 Revival 2026] data 为 std::string——TQString 经 utf8 字节串转换
  { TQCString v = value.utf8(); data = v.data() ? v.data() : ""; }
  // ############################################################################
}

// ##############################################################################
// MOC OUTPUT FILES:
#include "SearchDialog.moc"
// ##############################################################################

