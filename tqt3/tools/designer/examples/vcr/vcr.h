#ifndef VCR_H
#define VCR_H
#include <ntqwidget.h>

class Vcr : public TQWidget
{
    TQ_OBJECT
public:
    Vcr( TQWidget *parent = 0, const char *name = 0 );
    ~Vcr() {}
signals:
    void rewind();
    void play();
    void next();
    void stop();
};
#endif
