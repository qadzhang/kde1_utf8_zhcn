#include "pkfiledialog.h"
#include "pkfiledialog-cmd.h"
#include <stdio.h>

PObject *createWidget(CreateArgs &ca)
{
  PKFileDialog *pw = new  PKFileDialog(ca.parent);
  KFileDialog *kfbd;
  if(ca.fetchedObj != 0 && ca.fetchedObj->inherits("KFileDialog") == TRUE){
    kfbd = (KFileDialog *) ca.fetchedObj;
    pw->setDeleteAble(FALSE);
  }
  else // Never takes a parent in Puke
    kfbd = new  KFileDialog("/", 0x0, 0x0, "PukeKFileDialog", TRUE, TRUE);
  pw->setWidget(kfbd);
  pw->setWidgetId(ca.pwI);
  return pw;
}


PKFileDialog::PKFileDialog(PObject *parent)
  : PWidget(parent)
{
  kfbd = 0;  /* TQt3 迁移 */
  setWidget(kfbd);
}

PKFileDialog::~PKFileDialog()
{
  //  tqDebug("PLineEdit: in destructor");
  /*
  delete widget();     // Delete the frame
  kfbd=0;          // Set it to 0
  setWidget(kfbd); // Now set all widget() calls to 0.
  */
}

void PKFileDialog::messageHandler(int fd, PukeMessage *pm)
{
  PukeMessage pmRet;
  if(widget() == 0){
    tqDebug("PKFileDialog: No Widget set");
    return;
  }
  switch(pm->iCommand){
  case PUKE_KBFD_SET_PATH:
    ((KFileBaseDialog *)widget())->setDir((const char *) pm->cArg);
    
    pmRet.iCommand = - pm->iCommand;
    pmRet.iWinId = pm->iWinId;
    pmRet.iArg = 0;
    pmRet.iTextSize = widget()->dirPath().length();
    pmRet.cArg = widget()->dirPath().data();
    emit outputMessage(fd, &pmRet);
    break;
  case PUKE_KBFD_SET_FILTER:
    widget()->setFilter(pm->cArg);
    
    pmRet.iCommand = - pm->iCommand;
    pmRet.iWinId = pm->iWinId;
    pmRet.iArg = 0;
    pmRet.iTextSize = 0;
    pmRet.cArg = 0;
    emit outputMessage(fd, &pmRet);
    break;
  case PUKE_KBFD_SET_SELECTION:
    widget()->setSelection(pm->cArg);
    
    pmRet.iCommand = - pm->iCommand;
    pmRet.iWinId = pm->iWinId;
    pmRet.iArg = 0;
    pmRet.iTextSize = widget()->selectedFile().length();
    pmRet.cArg = widget()->selectedFile().data();
    emit outputMessage(fd, &pmRet);
    break;
  case PUKE_WIDGET_SHOW:
    widget()->exec();
    pmRet.iCommand = PUKE_KBFD_FILE_SELECTED_ACK;
    pmRet.iWinId = pm->iWinId;
    pmRet.iArg = 0;
    /* [2026-08-31] 按 UTF-8 字节数分配/拷贝/上报：原用 length()（UTF-16
       字符数），中文文件名 3 字节/字符——缓冲只拷了前缀，对端拿到断头路径 */
    QCString sel = widget()->selectedFile().utf8();
    pmRet.cArg = new char[sel.length() + 1];
    memcpy(pmRet.cArg, sel.data(), sel.length() + 1);
    pmRet.iTextSize = sel.length();
    emit outputMessage(widgetIden().fd, &pmRet);
    delete pmRet.cArg;
    break;

  default:
    PWidget::messageHandler(fd, pm);
  }
}

void PKFileDialog::setWidget(QObject *_kbfd)
{
  if(_kbfd != 0 && _kbfd->inherits("KFileBaseDialog") == FALSE)
    throw(errorInvalidSet(_kbfd, className()));

  kfbd = (KFileDialog *) _kbfd;
  PWidget::setWidget(kfbd);
}


KFileDialog *PKFileDialog::widget()
{
  return kfbd;
}

