// TQt stuff
#include "ntqnp.h"
#include <ntqpainter.h>
#include <ntqmessagebox.h>

class Trivial : public TQNPWidget {
    TQ_OBJECT
public:
    void mouseReleaseEvent(TQMouseEvent* event)
    {
	TQMessageBox::aboutTQt(this);
    }

    void paintEvent(TQPaintEvent* event)
    {
	TQPainter p(this);
	p.setClipRect(event->rect());
	int w = width();
	p.drawRect(rect());
	p.drawText(w/8, 0, w-w/4, height(), AlignCenter|WordBreak, "Trivial!");
    }
};

class TrivialInstance : public TQNPInstance {
    TQ_OBJECT
public:
    TQNPWidget* newWindow()
    {
	return new Trivial;
    }

    void print(TQPainter* p)
    {
	p->drawText(0,0,"Hello");
    }
};

class TrivialPlugin : public TQNPlugin {
public:
    TQNPInstance* newInstance()
    {
	return new TrivialInstance;
    }

    const char* getMIMEDescription() const
    {
	return "trivial/very:xxx:Trivial and useless";
    }

    const char * getPluginNameString() const
    {
	return "Trivial TQt-based Plugin";
    }

    const char * getPluginDescriptionString() const
    {
	return "A TQt-based LiveConnected plug-in that does nothing";
    }

};

TQNPlugin* TQNPlugin::create()
{
    return new TrivialPlugin;
}

#include "trivial.moc"
