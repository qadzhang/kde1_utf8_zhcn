// kfileio.cpp
// Author: Stefan Taferner <taferner@kde.org>

#include <kapp.h>
#include <kapp.h>
#include <kmsgbox.h>
#include <qmsgbox.h>
#include <qstring.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <qfile.h>
#include <qfileinf.h>

#include "kfileio.h"


//-----------------------------------------------------------------------------
static void msgDialog(const char* msg, const char* arg=NULL)
{
  TQString str;  /* TQt3 迁移 */

  if (arg) str.sprintf(msg, arg);
  else str = msg;

  KMsgBox::message(NULL, i18n("File I/O Error"), str,
		   KMsgBox::STOP, i18n("OK"));
}


//-----------------------------------------------------------------------------
QString kFileToString(const char* aFileName, bool aEnsureNL, bool aVerbose)
{
  QString result;
  QFileInfo info(aFileName);
  unsigned int readLen;
  unsigned int len = info.size();
  QFile file(aFileName);

  //assert(aFileName!=NULL);
  if( aFileName == NULL)
    return "";

  if (!info.exists())
  {
    if (aVerbose)
      msgDialog(i18n("The specified file does not exist:\n%s"),
		aFileName);
    return 0;
  }
  if (info.isDir())
  {
    if (aVerbose)
      msgDialog(i18n("This is a directory and not a file:\n%s"),
		aFileName);
    return 0;
  }
  if (!info.isReadable())
  {
    if (aVerbose)
      msgDialog(i18n("You do not have read permissions "
				   "to the file:\n%s"), aFileName);
    return 0;
  }
  if (len <= 0) return 0;

  if (!file.open(IO_Raw|IO_ReadOnly))
  {
    if (aVerbose) switch(file.status())
    {
    case IO_ReadError:
      msgDialog(i18n("Could not read file:\n%s"), aFileName);
      break;
    case IO_OpenError:
      msgDialog(i18n("Could not open file:\n%s"), aFileName);
      break;
    default:
      msgDialog(i18n("Error while reading file:\n%s"),aFileName);
    }
    return 0;
  }

  /* TQt3 迁移:Qt1 的 QString 字节缓冲已死;TQCString 中转,返回经 UTF-8 codec */
  TQCString buf(len + (int)aEnsureNL + 1);
  readLen = file.readBlock(buf.data(), len);
  if (aEnsureNL && buf[(int)len-1]!='\n')
  {
    buf[(int)len++] = '\n';
    readLen++;
  }
  buf[(int)len] = '\0';
  result = TQString(buf);

  if (readLen < len)
  {
    TQString msg;  /* TQt3 迁移 */
    msg = kde_sprintf(i18n("Could only read %u bytes of %u."),
		readLen, len);
    msgDialog(msg);
    return 0;
  }

  tqDebug("kFileToString: %d bytes read", readLen);
  return result;
}


//-----------------------------------------------------------------------------
bool kStringToFile(const QString aBuffer, const char* aFileName, 
		   bool aAskIfExists, bool aBackup, bool aVerbose)
{
  QFile file(aFileName);
  QFileInfo info(aFileName);
  int writeLen, len, rc;

  //assert(aFileName!=NULL);
  if(aFileName == NULL)
    return "";

  if (info.exists())
  {
    if (aAskIfExists)
    {
      TQString str;  /* TQt3 迁移 */
      str = kde_sprintf(i18n(
		  "File %s exists.\nDo you want to replace it ?"),
		  aFileName);
      rc = QMessageBox::information(NULL, i18n("Information"),
	   str, i18n("&OK"), i18n("&Cancel"),
	   0, 1);
      if (rc != 0) return FALSE;
    }
    if (aBackup)
    {
      // make a backup copy
      QString bakName = aFileName;
      bakName += '~';
      unlink(bakName);
      rc = rename(aFileName, bakName);
      if (rc)
      {
	// failed to rename file
	if (!aVerbose) return FALSE;
	rc = QMessageBox::warning(NULL, i18n("Warning"),
	     i18n(
             "Failed to make a backup copy of %s.\nContinue anyway ?"),
	     i18n("&OK"), i18n("&Cancel"), 0, 1);
	if (rc != 0) return FALSE;
      }
    }
  }

  if (!file.open(IO_Raw|IO_WriteOnly))
  {
    if (aVerbose) switch(file.status())
    {
    case IO_WriteError:
      msgDialog(i18n("Could not write to file:\n%s"), aFileName);
      break;
    case IO_OpenError:
      msgDialog(i18n("Could not open file for writing:\n%s"),
		aFileName);
      break;
    default:
      msgDialog(i18n("Error while writing file:\n%s"),aFileName);
    }
    return FALSE;
  }

  /* TQt3 迁移:utf8() 取字节串(size 含 NUL,语义同 Qt1 的 size()) */
  TQCString out = aBuffer.utf8();
  len = out.size() - 1;
  tqDebug("kStringToFile: writing %d bytes", len);
  writeLen = file.writeBlock(out.data(), len);

  if (writeLen < 0) 
  {
    msgDialog(i18n("Could not write to file:\n%s"), aFileName);
    return FALSE;
  }
  else if (writeLen < len)
  {
    TQString msg;  /* TQt3 迁移 */
    msg = kde_sprintf(i18n("Could only write %d bytes of %d."),
		writeLen, len);
    msgDialog(msg);
    return FALSE;
  } 

  return TRUE;
}
