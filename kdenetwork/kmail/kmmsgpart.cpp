// kmmsgpart.cpp

#include <qdir.h>

#include "kmmsgpart.h"
#include "kmmessage.h"
#include "kmimemagic.h"
#include <kapp.h>
#include <ksimpleconfig.h>

#include <mimelib/enum.h>
#include <mimelib/body.h>
#include <mimelib/bodypart.h>
#include <mimelib/utility.h>


static KMimeMagic* sMagic = NULL;


//-----------------------------------------------------------------------------
KMMessagePart::KMMessagePart() : 
  mType("text"), mSubtype("plain"), mCte("7bit"), mContentDescription(),
  mContentDisposition(), mBody(), mName()
{
  mBodySize = 0;  /* TQt3 迁移 */
}


//-----------------------------------------------------------------------------
KMMessagePart::~KMMessagePart()
{
}


//-----------------------------------------------------------------------------
int KMMessagePart::size(void) const
{
  if (mBodySize < 0)
  {
    ((KMMessagePart*)this)->mBodySize = 
      bodyDecoded().length() - 1;
  }
  return mBodySize;
}


//-----------------------------------------------------------------------------
void KMMessagePart::setBody(const QString aStr)
{
  int encoding = contentTransferEncoding();

  mBody = aStr;

  if (encoding!=DwMime::kCteQuotedPrintable &&
      encoding!=DwMime::kCteBase64)
  {
    mBodySize = mBody.length() - 1;
  }
  else mBodySize = -1;
}


//-----------------------------------------------------------------------------
void KMMessagePart::setBodyEncoded(const QString aStr)
{
  DwString dwResult, dwSrc;
  int encoding = contentTransferEncoding();
  int len;

  mBodySize = aStr.length() - 1;

  switch (encoding)
  {
  case DwMime::kCteQuotedPrintable:
    dwSrc = DwString(aStr.data(), aStr.length()-1);
    DwEncodeQuotedPrintable(dwSrc, dwResult);
    len = dwResult.length();
    mBody.truncate(len);
    memcpy(mBody.data(), dwResult.c_str(), len+1);
    break;
  case DwMime::kCteBase64:
    dwSrc = DwString(aStr.data(), aStr.length()-1);
    DwEncodeBase64(dwSrc, dwResult);
    len = dwResult.length();
    mBody.truncate(len);
    memcpy(mBody.data(), dwResult.c_str(), len+1);
    break;
    len = aStr.length()-1;
    dwSrc = DwString(aStr.data(), len);
    DwEncodeBase64(dwSrc, dwResult);
    mBody = TQString::fromLatin1(dwResult.c_str(), dwResult.length());  /* TQt3 迁移 */
    break;
  default:
    tqDebug("WARNING -- unknown encoding `%s'. Assuming 8bit.", 
	  (const char*)cteStr());
  case DwMime::kCte7bit:
  case DwMime::kCte8bit:
  case DwMime::kCteBinary:
    mBody = aStr;
    break;
  }
}


//-----------------------------------------------------------------------------
const QString KMMessagePart::bodyDecoded(void) const
{
  DwString dwResult, dwSrc;
  QString result;
  int encoding = contentTransferEncoding();
  int len;

  switch (encoding)
  {
  case DwMime::kCteQuotedPrintable:
    dwSrc = DwString(mBody.data(), mBody.length());
    DwDecodeQuotedPrintable(dwSrc, dwResult);
    len = dwResult.length() + 1;
    result.truncate(len);
    memcpy((void*)result.data(), (void*)dwResult.c_str(), len);
#if 0
    result = dwResult.c_str();
#endif
    break;
  case DwMime::kCteBase64:
    dwSrc = DwString(mBody.data(), mBody.length());
    DwDecodeBase64(dwSrc, dwResult);
    len = dwResult.length() + 1;
    result.truncate(len);
    memcpy((void*)result.data(), (void*)dwResult.c_str(), len);
    break;
  default:
    tqDebug("WARNING -- unknown encoding `%s'. Assuming 8bit.", 
	  (const char*)cteStr());
  case DwMime::kCte7bit:
  case DwMime::kCte8bit:
  case DwMime::kCteBinary:
    len = mBody.length();
    result.truncate(len+1);
    memcpy((void*)result.data(), (void*)mBody.data(), len);
    result[len] = '\0';
    break;
  }

  return result;
}


//-----------------------------------------------------------------------------
void KMMessagePart::magicSetType(bool aAutoDecode)
{
  QString mimetype, bod;
  int sep;

  if (!sMagic)
  {
    // initialize mime magic
    sMagic = new KMimeMagic(kapp->kde_mimedir() + "/magic");
    sMagic->setFollowLinks(TRUE);
  }

  if (aAutoDecode) bod = bodyDecoded();
  else bod = mBody;

  mimetype = sMagic->findBufferType(bod, bod.length()-1)->getContent();
  sep = mimetype.find('/');
  mType = mimetype.left(sep);
  mSubtype = mimetype.mid(sep+1, 64);
}


//-----------------------------------------------------------------------------
const QString KMMessagePart::iconName(void) const
{
  QString fileName, icon;
  QDir dir;

  fileName = KApplication::kde_mimedir() + "/" + mType + "/" + 
             mSubtype + ".kdelnk";

  if (dir.exists(fileName))
  {
    KConfig config(fileName);
    config.setGroup("KDE Desktop Entry");
    icon = config.readEntry("Icon");
  }
  else //Try harder
  {
    QString mime;
    mime = mType + "/" + mSubtype;

    dir.setPath(KApplication::kde_mimedir() + "/" + mType);
    if (dir.exists())
    {
      unsigned int i;
      QString fqn;
      for (i=0; i<dir.count(); i++)
      {
        if (strcmp (dir[i], ".") == 0 || strcmp (dir[i], "..") == 0)
          continue;
        fqn = dir.path();
        fqn += "/";
        fqn += dir[i];
        //tqDebug("try: %s", fqn.data());
        KSimpleConfig conf(fqn.data(), true);
        conf.setGroup("KDE Desktop Entry");
        if (conf.readEntry ("MimeType") == mime)
          icon = conf.readEntry("Icon");
      }
    }
  }

  if(icon.isEmpty()) // If no icon specified.
    if (mType == "message")
      icon = "kmail.xpm";
    else
      icon = "unknown.xpm";
  return KApplication::kde_icondir() + "/" + icon;
}


//-----------------------------------------------------------------------------
const QString KMMessagePart::typeStr(void) const
{
  return mType;
}


//-----------------------------------------------------------------------------
int KMMessagePart::type(void) const
{
  int type = DwTypeStrToEnum(DwString(mType));
  return type;
}


//-----------------------------------------------------------------------------
void KMMessagePart::setTypeStr(const QString aStr)
{
  mType = aStr;
}


//-----------------------------------------------------------------------------
void KMMessagePart::setType(int aType)
{
  DwString dwType;
  DwTypeEnumToStr(aType, dwType);
  mType = dwType.c_str();
}



//-----------------------------------------------------------------------------
const QString KMMessagePart::subtypeStr(void) const
{
  return mSubtype;
}


//-----------------------------------------------------------------------------
int KMMessagePart::subtype(void) const
{
  int subtype = DwSubtypeStrToEnum(DwString(mSubtype));
  return subtype;
}


//-----------------------------------------------------------------------------
void KMMessagePart::setSubtypeStr(const QString aStr)
{
  mSubtype = aStr;
}


//-----------------------------------------------------------------------------
void KMMessagePart::setSubtype(int aSubtype)
{
  DwString dwSubtype;
  DwSubtypeEnumToStr(aSubtype, dwSubtype);
  mSubtype = dwSubtype.c_str();
}


//-----------------------------------------------------------------------------
const QString KMMessagePart::contentTransferEncodingStr(void) const
{
  return mCte;
}


//-----------------------------------------------------------------------------
int KMMessagePart::contentTransferEncoding(void) const
{
  int cte = DwCteStrToEnum(DwString(mCte));
  return cte;
}


//-----------------------------------------------------------------------------
void KMMessagePart::setContentTransferEncodingStr(const QString aStr)
{
  mCte = aStr;
}


//-----------------------------------------------------------------------------
void KMMessagePart::setContentTransferEncoding(int aCte)
{
  DwString dwCte;
  DwCteEnumToStr(aCte, dwCte);
  mCte = dwCte.c_str();
}


//-----------------------------------------------------------------------------
const QString KMMessagePart::contentDescription(void) const
{
  return mContentDescription;
}


//-----------------------------------------------------------------------------
void KMMessagePart::setContentDescription(const QString aStr)
{
  mContentDescription = aStr;
}


//-----------------------------------------------------------------------------
const QString KMMessagePart::fileName(void) const
{
  int i, j, len;
  QString str;

  i = mContentDisposition.find("filename=", 0, FALSE);
  if (i < 0) return 0;
  j = mContentDisposition.find(';', i+9);

  if (j < 0) j = 32767;
  str = mContentDisposition.mid(i+9, j-i-9).stripWhiteSpace();

  len = str.length();
  if (str[0]=='"' && str[len-1]=='"') return str.mid(1, len-2);
  return str;
}


//-----------------------------------------------------------------------------
const QString KMMessagePart::contentDisposition(void) const
{
  return mContentDisposition;
}


//-----------------------------------------------------------------------------
void KMMessagePart::setContentDisposition(const QString aStr)
{
  mContentDisposition = aStr;
}

 
//-----------------------------------------------------------------------------
const QString KMMessagePart::body(void) const
{
  return mBody;
}


//-----------------------------------------------------------------------------
const QString KMMessagePart::name(void) const
{
  return mName;
}


//-----------------------------------------------------------------------------
void KMMessagePart::setName(const QString aStr)
{
  mName = aStr;
}
#if defined CHARSETS
//-----------------------------------------------------------------------------
const QString KMMessagePart::charset(void) const
{

   return mCharset;
}

//-----------------------------------------------------------------------------
void KMMessagePart::setCharset(const QString aStr)
{

  mCharset=aStr;
}
#endif



