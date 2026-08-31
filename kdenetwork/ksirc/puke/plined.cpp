#include "plined.h"
#include <stdio.h>

PObject *createWidget(CreateArgs &ca)
{
  PLineEdit *pw = new  PLineEdit(ca.parent);
  QLineEdit *le;
  if(ca.parent != 0 && ca.parent->widget()->isWidgetType() == TRUE)
    le = new  QLineEdit((QWidget *) ca.parent->widget());
  else
    le = new  QLineEdit();
  pw->setWidget(le);
  pw->setWidgetId(ca.pwI);
  return pw;
}


PLineEdit::PLineEdit(PObject *parent)
  : PWidget(parent)
{
  //  tqDebug("PLineEdit PLineEdit called");
  lineedit = 0;  /* TQt3 迁移 */
  setWidget(lineedit);
}

PLineEdit::~PLineEdit()
{
  //  tqDebug("PLineEdit: in destructor");
  /*
  delete widget();     // Delete the frame
  lineedit=0;          // Set it to 0
  setWidget(lineedit); // Now set all widget() calls to 0.
  */
}

void PLineEdit::messageHandler(int fd, PukeMessage *pm)
{
  PukeMessage pmRet;
  switch(pm->iCommand){
  case PUKE_LINED_SET_MAXLENGTH:
    if(widget() == 0){
      tqDebug("PLineEdit: No Widget set");
      return;
    }
    widget()->setMaxLength(pm->iArg);
    pmRet.iCommand = - pm->iCommand;
    pmRet.iWinId = pm->iWinId;
    pmRet.iArg = widget()->maxLength();
    pmRet.cArg = 0;
    emit outputMessage(fd, &pmRet);
    break;
  case PUKE_LINED_SET_ECHOMODE:
    if(widget() == 0){
      tqDebug("PLineEdit: No Widget set");
      return;
    }
    widget()->setEchoMode((QLineEdit::EchoMode) pm->iArg);
    pmRet.iCommand = - pm->iCommand;
    pmRet.iWinId = pm->iWinId;
    pmRet.iArg = widget()->echoMode();
    pmRet.cArg = 0;
    emit outputMessage(fd, &pmRet);
    break;
  case PUKE_LINED_SET_TEXT:
    if(widget() == 0){
      tqDebug("PLineEdit: No Widget set");
      return;
    }
    tqDebug("PukeLine Edit: Got: %s", pm->cArg);
    widget()->setText(pm->cArg);
    pmRet.iCommand = - pm->iCommand;
    pmRet.iWinId = pm->iWinId;
    pmRet.iArg = 0;
    pmRet.iTextSize = strlen(widget()->text());
    pmRet.cArg = new char[strlen(widget()->text())+1];
    strcpy(pmRet.cArg, widget()->text());
    emit outputMessage(fd, &pmRet);
    delete[] pmRet.cArg;
    break;
  case PUKE_LINED_GET_TEXT:
    if(widget() == 0){
      tqDebug("PLineEdit: No Widget set");
      return;
    }
    pmRet.iCommand = - pm->iCommand;
    pmRet.iWinId = pm->iWinId;
    pmRet.iArg = 0;
    pmRet.iTextSize = strlen(widget()->text());
    pmRet.cArg = new char[strlen(widget()->text())+1];
    strcpy(pmRet.cArg, widget()->text());
    emit outputMessage(fd, &pmRet);
    delete[] pmRet.cArg;
    break;
  default:
    PWidget::messageHandler(fd, pm);
  }
}

void PLineEdit::setWidget(QObject *_le)
{
  if(_le != 0 && _le->inherits("QLineEdit") == FALSE)
    throw(errorInvalidSet(_le, className()));

  lineedit = (QLineEdit *) _le;
  if(lineedit != 0){
    // [KDE1 Revival 2026] TQt3 无 textChanged(const char*) 信号，改接 TQString 版（updateText 槽签名同步）
    connect(lineedit, SIGNAL(textChanged(const TQString&)),
	    this, SLOT(updateText(const TQString&)));
    connect(lineedit, SIGNAL(returnPressed()),
	    this, SLOT(returnPress()));
  }
  PWidget::setWidget(_le);

}


QLineEdit *PLineEdit::widget()
{
  return lineedit;
}

// [KDE1 Revival 2026] 槽签名 TQString 化：先落 UTF-8 C 串（TQCString 局部变量保生命周期）再量长/拷贝
void PLineEdit::updateText(const TQString &text){
  PukeMessage pmRet;
  QCString cstr(text.utf8());

  pmRet.iCommand = PUKE_LINED_GET_TEXT_ACK;
  pmRet.iWinId = widgetIden().iWinId;
  pmRet.iArg = 0;
  pmRet.iTextSize = strlen(cstr);
  pmRet.cArg = new char[strlen(cstr)+1];
  strcpy(pmRet.cArg, cstr);
  emit outputMessage(widgetIden().fd, &pmRet);
  delete[] pmRet.cArg;
}

void PLineEdit::returnPress() {
  PukeMessage pmRet;

  pmRet.iCommand = PUKE_LINED_RETURN_PRESSED_ACK;
  pmRet.iWinId = widgetIden().iWinId;
  pmRet.iArg = 0;
  pmRet.cArg = 0;
  emit outputMessage(widgetIden().fd, &pmRet);
}

#include "plined.moc"

