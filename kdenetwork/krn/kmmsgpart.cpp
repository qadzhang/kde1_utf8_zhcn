// kmmsgpart.cpp

#include <qdir.h>

#include "kmmsgpart.h"
#include "kmmessage.h"
#include "kmimemagic.h"
#include <kapp.h>
#include <kconfig.h>

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
      bodyDecoded().length() - 1;  /* TQt3 迁移 */
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
    mBodySize = mBody.length() - 1;  /* TQt3 迁移 */
  }
  else mBodySize = -1;
}


//-----------------------------------------------------------------------------
void KMMessagePart::setBodyEncoded(const QString aStr)
{
  /* [2026-08-31] 重写（KDE1 Revival）：
   * What : 把 Unicode 正文按 CTE（QP/base64/8bit）编码存入 mBody
   * Why  : 原实现是 Qt1「truncate 后 memcpy 写 data()」惯用法——TQt3 下
   *        data() 是 UTF-8 字节缓存：null 串返回 NULL 直接段错误；写缓存
   *        后 UTF-16 视图长度/内容永久失步；且 aStr.length()（字符数）被
   *        当字节数用。改为显式 utf8() 取字节串，编码结果经 fromUtf8 构串
   * How  : aStr.utf8() 取 UTF-8 字节 → DwString(bytes) → QP/B 编码 →
   *        mBody = fromUtf8(编码结果)（编码产物恒为 ASCII，双通道等价）
   * Who/When/Where : 撰写/保存邮件部件时（kmail 与 krn 同款拷贝同步改） */
  DwString dwResult, dwSrc;
  int encoding = contentTransferEncoding();
  QCString bytes = aStr.utf8();
  bytes.detach();
  mBodySize = bytes.length();

  switch (encoding)
  {
  case DwMime::kCteQuotedPrintable:
    dwSrc = DwString(bytes.data(), bytes.length());
    DwEncodeQuotedPrintable(dwSrc, dwResult);
    mBody = TQString::fromUtf8(dwResult.c_str(), dwResult.length());
    break;
  case DwMime::kCteBase64:
    dwSrc = DwString(bytes.data(), bytes.length());
    DwEncodeBase64(dwSrc, dwResult);
    mBody = TQString::fromUtf8(dwResult.c_str(), dwResult.length());
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
  /* [2026-08-31] 重写（KDE1 Revival）：
   * What : 按 CTE 解码部件体为 Unicode
   * Why  : 原实现 memcpy 写 data()（null 段错误 + 缓存失步 + 字符数/字节
   *        数错配三连锁，见 setBodyEncoded 注释）。解码产物是按 charset
   *        编码的原始字节——经 fromUtf8 统一入 TQString（8bit 邮件按其
   *        声明 charset 的正确转换在 kmmessage 层处理）
   * How  : mBody.utf8() 字节 → DwString → QP/B 解码 → fromUtf8 构串 */
  DwString dwResult, dwSrc;
  int encoding = contentTransferEncoding();

  switch (encoding)
  {
  case DwMime::kCteQuotedPrintable:
    {
      QCString bytes = mBody.utf8();
      dwSrc = DwString(bytes.data(), bytes.length());
      DwDecodeQuotedPrintable(dwSrc, dwResult);
      return TQString::fromUtf8(dwResult.c_str(), dwResult.length());
    }
  case DwMime::kCteBase64:
    {
      QCString bytes = mBody.utf8();
      dwSrc = DwString(bytes.data(), bytes.length());
      DwDecodeBase64(dwSrc, dwResult);
      return TQString::fromUtf8(dwResult.c_str(), dwResult.length());
    }
  default:
    tqDebug("WARNING -- unknown encoding `%s'. Assuming 8bit.", 
	  (const char*)cteStr());
  case DwMime::kCte7bit:
  case DwMime::kCte8bit:
  case DwMime::kCteBinary:
    return mBody;
  }
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

  mimetype = sMagic->findBufferType(bod.latin1(), bod.length()-1)->getContent();  /* TQt3 迁移 */
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
    if(icon.isEmpty()) // If no icon specified.
      icon = "unknown.xpm";
  }
  else
  {
    // not found, use default
    icon = "unknown.xpm";
  }

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



