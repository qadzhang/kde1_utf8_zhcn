#ifndef TQISCIICODEC_H
#define TQISCIICODEC_H

#ifndef QT_H
#include "ntqtextcodec.h"
#endif // QT_H

#ifndef TQT_NO_CODECS

class TQIsciiCodec : public TQTextCodec {
public:
    TQIsciiCodec(int i);

    virtual int         mibEnum() const;
    virtual const char* mimeName () const;
    const   char*       name() const;

#if !defined(Q_NO_USING_KEYWORD)
    using TQTextCodec::fromUnicode;
#endif

    TQCString fromUnicode(const TQString& uc, int& len_in_out) const;
    TQString  toUnicode(const char* chars, int len) const;

    int heuristicContentMatch(const char* chars, int len) const;
    int heuristicNameMatch(const char* hint) const;

private:
    int idx;
};

#endif // TQT_NO_CODECS
#endif // TQISCIIDEVCODEC_H
