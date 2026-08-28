#include <ntqlayout.h>
#include <ntqtextview.h>
#include <ntqfont.h>
#include "glinfo.h"

class GLInfoText : public TQWidget
{
public:
    GLInfoText(TQWidget *parent) 
	: TQWidget(parent)
    {
	view = new TQTextView(this);
	view->setFont(TQFont("courier", 10));
	view->setWordWrap(TQTextEdit::NoWrap);
	TQHBoxLayout *l = new TQHBoxLayout(this);
	l->addWidget(view);
	GLInfo info;
	view->setText(info.info());
    }
    
private:
    TQTextView *view;
};
