
#ifndef VIEWER_H
#define VIEWER_H     


#include <ntqwidget.h>
#include <ntqfont.h>

class TQTextView;
class TQPushButton;

class Viewer : public TQWidget
{
TQ_OBJECT

public:   
    Viewer();

private slots:
    void setDefault();
    void setSansSerif();
    void setItalics();

private:
    void setFontSubstitutions();
    void layout();
    void showFontInfo( TQFont & );

    TQTextView * greetings; 
    TQTextView * fontInfo;

    TQPushButton * defaultButton;
    TQPushButton * sansSerifButton;
    TQPushButton * italicsButton;
};

#endif
