// kmmsgbase.cpp

#include "kmmsgbase.h"
#include <mimelib/mimepp.h>
#include <qregexp.h>
#ifndef KRN
#include "kmfolder.h"
#endif

#include <ctype.h>
#include <qtextcodec.h> /* [2026-08-31] RFC2047 头按 charset 转 Unicode */

#define NUM_STATUSLIST 9
static KMMsgStatus sStatusList[NUM_STATUSLIST] =
{
  KMMsgStatusDeleted, KMMsgStatusNew,
  KMMsgStatusUnread,  KMMsgStatusOld,
  KMMsgStatusRead,    KMMsgStatusReplied,
  KMMsgStatusSent,    KMMsgStatusQueued,
  KMMsgStatusUnknown /* "Unknown" must be at the *end* of the list */
};


//-----------------------------------------------------------------------------
KMMsgBase::KMMsgBase(KMFolder* aParent)
{
  mParent  = aParent;
  mDirty   = FALSE;
  mMsgSize = 0;  /* TQt3 迁移 */
  mFolderOffset = 0;  /* TQt3 迁移 */
  mStatus  = KMMsgStatusNew;
  mDate    = 0;
}


//-----------------------------------------------------------------------------
KMMsgBase::~KMMsgBase()
{
}


//-----------------------------------------------------------------------------
void KMMsgBase::assign(const KMMsgBase* other)
{
  mParent = other->mParent;
  mDirty  = other->mDirty;
  mMsgSize = other->mMsgSize;
  mFolderOffset = other->mFolderOffset;
  mStatus = other->mStatus;
  mDate = other->mDate;
}


//-----------------------------------------------------------------------------
KMMsgBase& KMMsgBase::operator=(const KMMsgBase& other)
{
  assign(&other);
  return *this;
}


//-----------------------------------------------------------------------------
bool KMMsgBase::isMessage(void) const
{
  return FALSE;
}


//-----------------------------------------------------------------------------
void KMMsgBase::setStatus(KMMsgStatus aStatus)
{
  if (mParent) mParent->msgStatusChanged( mStatus, aStatus );
  mStatus = aStatus;
  mDirty = TRUE;
#ifndef KRN
  if (mParent) mParent->headerOfMsgChanged(this);
#endif
}


//-----------------------------------------------------------------------------
void KMMsgBase::setStatus(const char* aStatusStr, const char* aXStatusStr)
{
  int i;

  mStatus = KMMsgStatusUnknown;

  // first try to find status from "X-Status" field if given
  if (aXStatusStr) for (i=0; i<NUM_STATUSLIST-1; i++)
  {
    if (strchr(aXStatusStr, (char)sStatusList[i]))
    {
      mStatus = sStatusList[i];
      break;
    }
  }

  // if not successful then use the "Status" field
  if (mStatus == KMMsgStatusUnknown)
  {
    if (aStatusStr && 
        ((aStatusStr[0]=='R' && aStatusStr[1]=='O') ||
	 (aStatusStr[0]=='O' && aStatusStr[1]=='R')))
	mStatus=KMMsgStatusOld;
    else if (aStatusStr && aStatusStr[0]=='R') mStatus=KMMsgStatusRead;
    else if (aStatusStr && aStatusStr[0]=='D') mStatus=KMMsgStatusDeleted;
    else mStatus=KMMsgStatusNew;
  }

  mDirty = TRUE;
#ifndef KRN
  if (mParent) mParent->headerOfMsgChanged(this);
#endif
}


//-----------------------------------------------------------------------------
KMMsgStatus KMMsgBase::status(void) const
{
  return mStatus;
}


//-----------------------------------------------------------------------------
bool KMMsgBase::isUnread(void) const
{
  KMMsgStatus st = status();
  return (st==KMMsgStatusNew || st==KMMsgStatusUnread);
}

//-----------------------------------------------------------------------------
bool KMMsgBase::isNew(void) const
{
  KMMsgStatus st = status();
  return (st==KMMsgStatusNew);
}


//-----------------------------------------------------------------------------
const char* KMMsgBase::statusToStr(KMMsgStatus aStatus)
{
  static char sstr[2];

  sstr[0] = (char)aStatus;
  sstr[1] = '\0';

  return sstr;
}


//-----------------------------------------------------------------------------
void KMMsgBase::setDate(const time_t aUnixTime)
{
  mDate  = aUnixTime;
  mDirty = TRUE;
}


//-----------------------------------------------------------------------------
void KMMsgBase::setDate(const char* aDateStr)
{
  DwDateTime dwDate;

  dwDate.FromString(aDateStr);
  dwDate.Parse();
  mDate  = dwDate.AsUnixTime();
  mDirty = TRUE;
}


//-----------------------------------------------------------------------------
time_t KMMsgBase::date(void) const
{
  return mDate;
}


//-----------------------------------------------------------------------------
const QString KMMsgBase::dateStr(void) const
{
  return ctime(&mDate);
}


//-----------------------------------------------------------------------------
const QString KMMsgBase::asIndexString(void) const
{
  int i, len;
  TQString str;  /* TQt3 迁移 */ // Sven is blind.

  // don't forget to change indexStringLength() below !!
  str.sprintf("%c %-.9lu %-.9lu %-.9lu %-3.3s %-100.100s %-100.100s %-100.100s",
	      (char)status(), folderOffset(), msgSize(), (unsigned long)date(),
	      (const char*)xmark(),
	      (const char*)decodeQuotedPrintableString(subject()),
	      (const char*)decodeQuotedPrintableString(from()),
	      (const char*)decodeQuotedPrintableString(to())); //sven
  len = str.length();
  for (i=0; i<len; i++)
    if (str[i] < ' ' && str[i] >= 0)
      str[i] = ' ';

  return str;
}


//-----------------------------------------------------------------------------
int KMMsgBase::indexStringLength(void)
{
  //return 237;
  return 338; //sven (+ 100 chars to + one space, right?
}


//-----------------------------------------------------------------------------
int KMMsgBase::compareByIndex(const KMMsgBase* other) const
{
  return (mFolderOffset - other->mFolderOffset);
}


//-----------------------------------------------------------------------------
int KMMsgBase::compareBySubject(const KMMsgBase* other) const
{
  //const char *subjStr, *otherSubjStr;
  bool hasKeywd, otherHasKeywd;
  int rc;

  QString subjStr = skipKeyword(subject(), ':', &hasKeywd);
  QString otherSubjStr = skipKeyword(other->subject(), ':', &otherHasKeywd);

  rc = stricmp(subjStr, otherSubjStr);
  if (rc) return rc;

  // If both are equal return the one with a keyword (Re: / Fwd: /...)
  // at the beginning as the larger one.
  return (hasKeywd - otherHasKeywd);
}


//-----------------------------------------------------------------------------
int KMMsgBase::compareByStatus(const KMMsgBase* other) const
{
  KMMsgStatus stat;
  int i;

  for (i=NUM_STATUSLIST-1; i>0; i--)
  {
    stat = sStatusList[i];
    if (mStatus==stat || other->mStatus==stat) break;
  }

  return ((mStatus==stat) - (other->mStatus==stat));
}


//-----------------------------------------------------------------------------
int KMMsgBase::compareByDate(const KMMsgBase* other) const
{
  return (mDate - other->mDate);
}


//-----------------------------------------------------------------------------
int KMMsgBase::compareByFrom(const KMMsgBase* other) const
{
  const char *f, *fo;

  f = from();
  fo = other->from();

  if (!f || !fo) return ((!fo) - (!f));

  while (*f && *f<'A') f++;
  while (*fo && *fo<'A') fo++;

  return stricmp(f, fo);
}


//-----------------------------------------------------------------------------
QString KMMsgBase::skipKeyword(const QString aStr, char sepChar,
				   bool* hasKeyword)
{
  int i, maxChars=3;
  const char *pos, *str = aStr.data();

  if (!str) return 0;

  while (*str==' ')
    str++;
  if (hasKeyword) *hasKeyword=FALSE;

  for (i=0,pos=str; *pos && i<maxChars; pos++,i++)
  {
    if (*pos < 'A' || *pos == sepChar) break;
  }

  if (i>1 && *pos == sepChar) // skip following spaces too
  {
    for (pos++; *pos==' '; pos++)
      ;
    if (hasKeyword) *hasKeyword=TRUE;
    return pos;
  }
  return str;
}


//-----------------------------------------------------------------------------
// 十六进制字符 → 数值（非法返回 -1）；decodeRFC1522String 的前置辅助（Q 解码 =XX 用）
static int km_hex4( TQChar c )
{
  int v = c.latin1();
  if ( v >= '0' && v <= '9' ) return v - '0';
  if ( v >= 'A' && v <= 'F' ) return v - 'A' + 10;
  if ( v >= 'a' && v <= 'f' ) return v - 'a' + 10;
  return -1;
}

const QString KMMsgBase::decodeRFC1522String(const QString aStr)
{
  /* ┌──────────────────────────────────────────────────────────────────┐
   │ [2026-08-31] 全量重写（KDE1 Revival）                              │
   │ What : 解码 RFC2047 编码头（"=?charset?Q|B?text?="）为 Unicode      │
   │ Why  : 原实现是 Qt1「QString::data() 可写缓冲」惯用法——TQt3 下三连
   │        锁失效：null 串 data()=NULL（首封含编码头的邮件即段错误）、
   │        写 ascii 缓冲与 UTF-16 视图失步、truncate(字符数) 与字节缓
   │        冲错配。且 charset 记号被解析后弃置——GB2312 邮件头从未按
   │        charset 转换。改为纯 TQString 扫描 + 字节级解码 + 按 charset
   │        经 TQTextCodec 转 Unicode（未知 charset 回退 UTF-8）
   │ Who  : kmmsginfo/kmmessage 的 subject/from/to 显示路径（krn 同款拷贝）
   │ When : 读取任一含 "=?…?=" 的邮件头字段时
   │ Where: kmmsgbase.cpp（kmail 与 krn 各一份，保持同步修改）
   │ How  : 伪代码：
   │   1. 无 "=?" 直接原串返回
   │   2. 逐字符扫描：普通字符直接入结果
   │   3. 遇 "=?" 尝试解析 =?charset?Q|B?text?=：
   │      a. charset 名取 "?" 前的 ASCII 段（限长 200，至少 1 字符），
   │         记住该 "?" 的位置 qMark
   │      b. 编码符只认 Q/B（大小写均可），后必须跟 "?"
   │      c. 编码文本取到 "?=" 止（限长 200）
   │      d. 任一步不合法 → 把 "=?" 当普通两字符吐出，从其后继续
   │   4. 合法段：Q 按字节解（'_'→空格、=XX→字节）；B 用 DwDecodeBase64
   │   5. 解出的字节流按 charset[csStart, qMark) 找 codec 转 Unicode
   │      （找不到 codec 按 UTF-8），拼入结果；跳过 "?=" 继续
   └──────────────────────────────────────────────────────────────────┘ */
  if ( aStr.find("=?") < 0 )
    return aStr;

  const uint maxLen = 200;
  QString result;
  uint pos = 0;

  while ( pos < aStr.length() )
  {
    if ( !( aStr[pos] == '=' && pos + 1 < aStr.length() && aStr[pos+1] == '?' ) )
    {
      result += aStr[pos];
      pos++;
      continue;
    }

    /* 在 pos 处尝试解析一个完整编码字 */
    uint save = pos;
    pos += 2;
    uint csStart = pos;
    int qMark = -1;
    while ( pos < aStr.length() && aStr[pos] != '?' && pos - csStart < maxLen )
      pos++;
    bool valid = FALSE;
    char encoding = 0;
    if ( pos < aStr.length() && aStr[pos] == '?' &&
         pos - csStart >= 1 && pos - csStart < maxLen )
    {
      qMark = (int)pos;
      valid = TRUE;
    }
    if ( valid )
    {
      if ( pos + 2 >= aStr.length() )
        valid = FALSE;
      else
      {
        encoding = aStr[pos+1].latin1();
        if ( aStr[pos+2] != '?' ||
             ( encoding != 'Q' && encoding != 'q' &&
               encoding != 'B' && encoding != 'b' ) )
          valid = FALSE;
        else
          pos += 3;
      }
    }
    int end = -1;
    if ( valid )
    {
      uint scan = pos;
      while ( scan + 1 < aStr.length() && scan - pos < maxLen )
      {
        if ( aStr[scan] == '?' && aStr[scan+1] == '=' )
        {
          end = scan;
          break;
        }
        scan++;
      }
      if ( end < 0 )
        valid = FALSE;
    }

    if ( !valid )
    {
      /* 不是合法编码字："=?" 按普通文本吐出，从其后继续扫描 */
      result += '=';
      result += '?';
      pos = save + 2;
      continue;
    }

    /* 编码文本 → 字节串（Q/B 均产出原始字节，不经 Unicode 通道） */
    QString encText = aStr.mid( pos, end - pos );
    QCString bytes;
    if ( encoding == 'Q' || encoding == 'q' )
    {
      for ( uint i = 0; i < encText.length(); i++ )
      {
        TQChar c = encText[i];
        if ( c == '_' )
        {
          bytes += (char)' ';
        }
        else if ( c == '=' && i + 2 < encText.length() )
        {
          int h1 = km_hex4( encText[i+1] );
          int h2 = km_hex4( encText[i+2] );
          if ( h1 >= 0 && h2 >= 0 )
          {
            bytes += (char)( h1 * 16 + h2 );
            i += 2;
          }
          else
            bytes += (char)c.latin1();
        }
        else
          bytes += (char)c.latin1();   /* Q 文本应为纯 ASCII */
      }
    }
    else
    {
      DwString dwsrc( encText.latin1() );
      DwString dwdest;
      DwDecodeBase64( dwsrc, dwdest );
      bytes = QCString( dwdest.data(), dwdest.size() + 1 );
      bytes[(int)dwdest.size()] = '\0';
    }

    /* 按 charset 转 Unicode；未知/缺失 codec 回退 UTF-8 */
    QString charset = aStr.mid( csStart, qMark - (int)csStart );
    TQTextCodec *codec = TQTextCodec::codecForName( charset.latin1() );
    if ( codec )
      result += codec->toUnicode( bytes );
    else
      result += TQString::fromUtf8( bytes );

    pos = end + 2;
  }

  return result;
}





//-----------------------------------------------------------------------------
const QString KMMsgBase::decodeQuotedPrintableString(const QString aStr)
{
#ifdef BROKEN
  static QString result;
  int start, beg, mid, end;
  end = 0; // Remove compiler warning;

  start = 0;  /* TQt3 迁移 */
  end = 0;  /* TQt3 迁移 */
  result = "";

  while (1)
  {
    beg = aStr.find("=?", start);
    if (beg < 0)
    {
      // no more suspicious string parts found -- done
      result += aStr.mid(start, 32767);
      break;
    }

    if (beg > start) result += aStr.mid(start, beg-start);
    mid = aStr.find("?Q?", beg+2);
    if (mid>beg) end = aStr.find("?=", mid+3);
    if (mid < 0 || end < 0)
    {
      // no quoted printable part -- skip it
      result += "=?";
      start += 2;
      continue;
    }
    if (aStr[mid+3]=='_' )
    {
      result += ' ';
      mid++;
    }
    else if (aStr[mid+3]==' ') mid++;

    if (end-mid-3 > 0)
      result += decodeQuotedPrintable(aStr.mid(mid+3, end-mid-3).data());
    start = end+2;
  }
  return result;
#else
  return decodeRFC1522String(aStr);
#endif
}


//-----------------------------------------------------------------------------
const QString KMMsgBase::decodeQuotedPrintable(const QString aStr)
{
  DwString dwsrc(aStr.data());
  DwString dwdest;

  DwDecodeQuotedPrintable(dwsrc, dwdest);
  return dwdest.c_str();
}


//-----------------------------------------------------------------------------
const QString KMMsgBase::encodeQuotedPrintable(const QString aStr)
{
  DwString dwsrc(aStr.data(), aStr.length());
  DwString dwdest;
  QString result;

  DwEncodeQuotedPrintable(dwsrc, dwdest);
  result = dwdest.c_str();
  return result;
}


//-----------------------------------------------------------------------------
const QString KMMsgBase::decodeBase64(const QString aStr)
{
  DwString dwsrc(aStr.data(), aStr.length());
  DwString dwdest;
  QString result;

  DwDecodeBase64(dwsrc, dwdest);
  result = dwdest.c_str();
  return result;
}


//-----------------------------------------------------------------------------
const QString KMMsgBase::encodeBase64(const QString aStr)
{
  DwString dwsrc(aStr.data(), aStr.length()-1);
  DwString dwdest;
  QString result;

  DwEncodeBase64(dwsrc, dwdest);
  result = dwdest.c_str();
  return result;
}
