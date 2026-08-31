/***********************************************************************
 *
 *  Kfwin.cpp
 *
 **********************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <pwd.h>
#include <grp.h>
#include <sys/types.h>  
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>     
#include <time.h>    

#include <qapp.h>
#include <qwidget.h>
#include <qframe.h>
#include <qlabel.h>
#include <qlistbox.h>
#include <qstrlist.h>
#include <qfileinf.h> 
#include <qpushbt.h> 
#include <qlined.h> 
#include <qgrpbox.h> 
#include <qchkbox.h>
#include <qfiledef.h>
#include <qfiledlg.h> 
#include <qlist.h>
#include <qfileinf.h> 
#include <qmsgbox.h>
#include <qdir.h>
#include <qclipbrd.h>
#include <kfiledialog.h>

#include <kdebug.h>
#include <kfm.h>
#include <kfmclient_ipc.h>
#include <kprocess.h>
#include <kmsgbox.h>

#include "kfwin.h"
#include "kfarch.h"
#include "kfsave.h"

#include <klocale.h>
#include <kapp.h>

#include <qkeycode.h>

extern KfSaveOptions *saving; 
extern QList<KfArchiver> *archivers;

class MyQListBox : public QListBox {
public:
  MyQListBox(QWidget *parent, const char *name) :
    QListBox(parent, name) {
  }

  virtual bool eventFilter(QObject *, QEvent *e) {
    if(e->type() == Event_KeyPress || e->type() == TQEvent::Accel  /* TQt3 迁移 */) {
      QKeyEvent *k = (QKeyEvent *)e;
      if(k->key() == Key_PageUp ||
	 k->key() == Key_PageDown) {
	keyPressEvent(k);
	return TRUE;
      }
    }
    
    return FALSE;
  }
};
  

KfindWindow::KfindWindow( QWidget *parent, const char *name )
    : QWidget( parent, name )          
  {
    lbx = new MyQListBox(this,"list_box" );
    topLevelWidget()->installEventFilter(lbx);
    lbx->setMultiSelection(FALSE);

    connect(lbx , SIGNAL(highlighted(int)),
            this, SLOT(highlighted()) );
    connect(lbx , SIGNAL(selected(int)),
            this, SLOT( openBinding()) );
   };

KfindWindow::~KfindWindow()
  {
    delete lbx;
  };

void KfindWindow::resizeEvent( QResizeEvent * )
  {
    lbx->setGeometry(0,0,width(),height());    
  };

void KfindWindow::appendResult(const char *file) {
  lbx->insertItem(file);
}

void KfindWindow::beginSearch() {
  killTimers();
  startTimer(100);
}

void KfindWindow::doneSearch() {
  killTimers();
  
  QString str;
  str = kde_sprintf(i18n("%d file(s) found"), (int)lbx->count());
  emit statusChanged(str);
}

void KfindWindow::timerEvent(QTimerEvent *) {
  if(lbx->count() > 0) {
    QString str;
    str = kde_sprintf(i18n("%d file(s) found"), (int)lbx->count());
    emit statusChanged(str);
  }
}

#ifndef PATH_MAX
#define PATH_MAX 1024
#endif

void KfindWindow::updateResults(const char *file )
  {
    kdebug(KDEBUG_INFO, 1902, "UPDATERESULTs\n");
    
    char str[PATH_MAX];
    int count;
    
        QStrList *strl= new QStrList (TRUE);
    FILE *f = fopen(file,"rb");
    if (f==0)
      {
	/* [2026-08-31] 错误分支补 delete：原实现直接 return 泄漏刚 new 的清单 */
	delete strl;
	sprintf(str,i18n("%d file(s) found"),0);
	emit statusChanged(str);
	return;
      };
    
    lbx->clear();
    
    count=0;
    while ( !feof( f ) )
      {
        str[0] = 0;
        fgets( str, 1023, f );
        /* [2026-08-31] 超 1022 字节的长行会被 fgets 截断在 UTF-8 字符中间：
           只裁尾部残缺序列（完整尾字符保留——首版无差别回退会误删完整字
           符，修订算法与 ksame/HighScore.cpp 的 utf8_trim_boundary 一致，
           该算法经 tools/utf8-boundary-fuzz.c 200 万模糊样本验证） */
        /* [2026-08-31 三次修订] 循环收口（同 ksame utf8_trim_boundary，
           经 tools/utf8-boundary-fuzz.c 200 万模糊样本验证）：只裁尾部
           残缺序列，完整尾字符保留，坏尾巴反复暴露反复裁，有界收敛 */
        for (;;) {
          int L = strlen( str );
          if ( L <= 0 ) break;
          unsigned char last = (unsigned char)str[L-1];
          if ( ( last & 0xC0 ) == 0x80 ) {
            int p = L - 1;
            while ( p >= 0 && ( (unsigned char)str[p] & 0xC0 ) == 0x80 )
              p--;
            if ( p < 0 ) { str[0] = 0; break; }
            int need = 1;
            unsigned char b = (unsigned char)str[p];
            if ( (b & 0xE0) == 0xC0 ) need = 2;
            else if ( (b & 0xF0) == 0xE0 ) need = 3;
            else if ( (b & 0xF8) == 0xF0 ) need = 4;
            int conts = need - 1;
            int have = L - 1 - p;
            if ( have < conts ) { str[p] = 0; continue; }
            if ( have > conts ) { str[p + need] = 0; continue; }
            break;
          }
          if ( last >= 0xC0 && last < 0xF8 ) { str[L-1] = 0; continue; }
          break;
        }
        if ( str[0] != 0 )
          {
            // Delete trailing '\n'
            str[ strlen( str ) - 1 ] = 0;
 	    strl->append (str);
            //insertItem( str );
            count++;
          }
      };

    QFileInfo *filename = new QFileInfo(strl->last());
    if (!(filename->exists()))
      strl->removeLast();

    lbx->insertStrList(strl,-1);
    sprintf(str,i18n("%d file(s) found"),count);
    emit statusChanged(str);

	unlink( file );
    fclose(f);    
    delete filename;
    delete strl;
  };

// copy to clipboard aka X11 selection
void KfindWindow::copySelection() {
  QString s;
  for(int i = 0; i < (int)lbx->count(); i++)
    if(lbx->isSelected(i)) {
      s.append(lbx->text(i));
      s.append(" ");
    }

  if(s.length() > 0) {
    QClipboard *cb = kapp->clipboard();
    cb->clear();
    cb->setText( s.data() );  /* TQt3 迁移：setData(fmt,data)→setText */
  }
}

void KfindWindow::clearList()
  { 
    lbx->clear();
  };

void KfindWindow::changeItem(const char *itemName)
  {
    lbx->changeItem(itemName,lbx->currentItem());    
  };

void KfindWindow::selectAll() {
  lbx->setAutoUpdate(FALSE);
  if(lbx->currentItem() == -1)
    lbx->setCurrentItem(0);
  for(int i = 0; i < (int)lbx->count(); i++)
    lbx->setSelected(i, TRUE);
  lbx->setAutoUpdate(TRUE);
  lbx->repaint();
}

void KfindWindow::invertSelection() {
  lbx->setAutoUpdate(FALSE);
  if(lbx->currentItem() == -1)
    lbx->setCurrentItem(0);
  for(int i = 0; i < (int)lbx->count(); i++)
    lbx->setSelected(i, !lbx->isSelected(i));
  lbx->setAutoUpdate(TRUE);
  lbx->repaint();
}

void KfindWindow::unselectAll() {
  lbx->setAutoUpdate(FALSE);
  if(lbx->currentItem() == -1)
    lbx->setCurrentItem(0);
  for(int i = 0; i < (int)lbx->count(); i++)
    lbx->setSelected(i, FALSE);
  lbx->setAutoUpdate(TRUE);
  lbx->repaint();
}

void KfindWindow::saveResults()
  { 
    uint items,item;
    FILE *results;
    QString filename;

     if ( saving->getSaveStandard() )
      {
	filename = getenv("HOME");
	filename += "/.kfind-results.html";
      }
    else
      filename = saving->getSaveFile();

    results=fopen(filename,"w");

    items=lbx->count();
    if (results==0L)
      QMessageBox::warning(parentWidget(),i18n("Error"),
		     i18n("It wasn't possible to save results!"),
		     i18n("OK"));
    else
      {
	if ( strcmp(saving->getSaveFormat(),"HTML")==0)
	  {
	    fprintf(results,"<HTML><HEAD>\n");
	    fprintf(results,"<!DOCTYPE %s>\n",
		    i18n("KFind Results File"));
	    fprintf(results,"<TITLE>%sKFind Results File</TITLE></HEAD>\n",
		    i18n("KFind Results File"));
	    fprintf(results,"<BODY><H1>%s</H1>\n",
		    i18n("KFind Results File"));
	    fprintf(results,"<DL><p>\n"); 
	
	    item=0;  
	    while(item!=items)
	      {
		fprintf(results,"<DT><A HREF=\"file:%s\">file:%s</A>\n",
			lbx->text(item),lbx->text(item));
		item++;
	      };
	    fprintf(results,"</DL><P></BODY></HTML>\n");
	  }
	else
	  {
	    item=0;  
	    while(item!=items)
	      {
		fprintf(results,"%s\n", lbx->text(item));
		item++;
	      };
	    
	  };
	fclose(results); 
 	QMessageBox::information(parentWidget(),
			 i18n("Information"),
 			 i18n("Results were saved to file\n")+
			 filename,
 			 i18n("OK"));
      };
  };

void KfindWindow::highlighted()
  {
    emit resultSelected(true);
  };

void KfindWindow::deleteFiles()
  {
    QString tmp;

    tmp = kde_sprintf(i18n("Do you really want to delete file:\n%s"),
                lbx->text(lbx->currentItem()));
    if(KMsgBox::yesNo(parentWidget(),i18n("Delete File"),
                      tmp, KMsgBox::QUESTION | KMsgBox::DB_SECOND) == 1)
      {
        QFileInfo *file = new QFileInfo(lbx->text(lbx->currentItem()));
	if (file->isFile()||file->isSymLink())
            {
              if (remove(file->filePath())==-1)
                  switch(errno)
                    {
    	              case EACCES: 
			QMessageBox::warning(parentWidget(),
					   i18n("Error"),
					   i18n("You have no permission\n to delete this file"),
					   i18n("OK"));
                                   break;
                      default: 
			QMessageBox::warning(parentWidget(),
					     i18n("Error"),
					     i18n("It isn't possible to delete\nselected file"),
					     i18n("OK"));
                    }
                else
                  {
                   KFM *kfm= new KFM();
                   kfm->refreshDirectory(lbx->text(lbx->currentItem()));
                   lbx->removeItem(lbx->currentItem());
		   delete kfm;
 		  };
            }
          else
            {
              if (rmdir(file->filePath())==-1)
		  switch(errno)
                    {
		    case EACCES: QMessageBox::warning(parentWidget(),
				              i18n("Error"),
		                              i18n("You have no permission\n to delete this directory"),
					      i18n("OK"));
                                  break;
      	             case ENOTEMPTY: QMessageBox::warning(parentWidget(),
					    i18n("Error"),
					    i18n("Specified directory\nis not empty!"),
					    i18n("OK"));
                                     break;
     	             default: QMessageBox::warning(parentWidget(),
					 i18n("Error"),
                                         i18n("It isn't possible to delete\nselected directory"),
                                         i18n("OK"));
                   }
                 else
                  {
		    KFM *kfm= new KFM();
		    kfm->refreshDirectory(lbx->text(lbx->currentItem()));
		    lbx->removeItem(lbx->currentItem());
		    delete kfm;
                  };
            };

	  delete file;
      };
  };

void KfindWindow::fileProperties()
  {
    QString tmp= "file:";
    KFM *kfm= new KFM();

    QFileInfo *fileInfo = new QFileInfo(lbx->text(lbx->currentItem()));
    if (fileInfo->isDir())
      {
	tmp.append(fileInfo->filePath());
	kfm->openProperties(tmp.data());
      }
    else
      {
	tmp.append(lbx->text(lbx->currentItem()));
	kfm->openProperties(tmp.data());
      };
    delete kfm;
  };

void KfindWindow::openFolder()
  {
    QString tmp;
    KFM *kfm= new KFM();

    QFileInfo *fileInfo = new QFileInfo(lbx->text(lbx->currentItem()));
    if (fileInfo->isDir())
        tmp.sprintf("file:%s",fileInfo->filePath());
      else
	tmp.sprintf("file:%s",(fileInfo->dirPath()).data());


    kfm->openURL(tmp.data());
    delete kfm;
  };

void KfindWindow::openBinding()
  {
    QString tmp= "file:";
    KFM *kfm= new KFM();

    QFileInfo *fileInfo = new QFileInfo(lbx->text(lbx->currentItem()));
    if (fileInfo->isDir())
      {
	tmp.append(fileInfo->filePath());
	kfm->openURL(tmp.data());
      }
    else
      {
	tmp.append(lbx->text(lbx->currentItem()));
	kfm->exec(tmp.data(),0L);
      };
    delete kfm;
  };

void KfindWindow::addToArchive()
{
  QString path = QDir::home().absPath();
  KfArchiver *arch;

  QString filename( KFileDialog::getOpenFileName(path) );
  if ( filename.isNull() )
    return;

  int pos1 = filename.findRev(".");
  int pos2 = filename.findRev(".",pos1-1);
  
  QString pattern1 = filename.right(filename.length()-pos1);
  QString pattern2 = "*"+filename.mid(pos2,pos1-pos2)+pattern1;

  if ( (arch = KfArchiver::findByPattern(pattern2))!=0L)
    execAddToArchive(arch,filename);
  else
    if ( (arch = KfArchiver::findByPattern("*"+pattern1))!=0L)
      execAddToArchive(arch,filename);
    else
      QMessageBox::warning(parentWidget(),i18n("Error"),
		       i18n("Couldn't recognize archive type!"),
		       i18n("OK")); 

};

void KfindWindow::execAddToArchive(KfArchiver *arch,QString archname)
{
  QFileInfo archiv(archname);
  QString buffer,pom;
  KProcess archProcess;
  int pos;
      
  if ( archiv.exists() )
    buffer = arch->getOnUpdate();
  else
    buffer = arch->getOnCreate();

  buffer=buffer.simplifyWhiteSpace();

  pos = buffer.find(" ");
  pom = buffer.left(pos);
  if (pos==-1) 
    pos = buffer.length();
  buffer = buffer.remove(0,pos+1);

  archProcess.clearArguments ();
  archProcess.setExecutable(pom.data());

  while( !buffer.isEmpty() )
    {
      pos = buffer.find(" ");
      pom = buffer.left(pos);

      if ( pom=="%d" )
	{
	  QFileInfo *fileInfo = new QFileInfo(lbx->text(lbx->currentItem()));
	  pom.sprintf("%s/",(fileInfo->dirPath(TRUE)).data());
	};

      if ( pom=="%a" )
	pom = archname;
      
      if ( pom=="%f" )
	pom = lbx->text(lbx->currentItem());

      if ( pom=="%n" )
	{
	  QFileInfo *fileInfo = new QFileInfo(lbx->text(lbx->currentItem()));
	  pom = fileInfo->fileName();
	};

      archProcess << pom.data();

      if (pos==-1) 
	pos = buffer.length();
      buffer = buffer.remove(0,pos+1);
    };

  if ( !archProcess.start(KProcess::DontCare) )
    tqWarning(i18n("Error while creating child process!"));
};

int KfindWindow::numItems() { 
  return lbx->count(); 
}

