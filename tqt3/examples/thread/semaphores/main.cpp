/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/
#include <ntqapplication.h>
#include <ntqwidget.h>
#include <ntqpushbutton.h>
#include <ntqmultilineedit.h>
#include <ntqthread.h>
#include <ntqsemaphore.h>
#include <ntqmutex.h>
#include <ntqlayout.h>
#include <ntqmessagebox.h>
#include <ntqlabel.h>

#if defined(TQT_NO_THREAD)
#  error Thread support not enabled.
#endif

// Use pointers to create semaphores after TQApplication object!
TQSemaphore* yellowSem, *greenSem;


class YellowThread : public TQThread
{
public:
    YellowThread(TQWidget *o)
	: receiver(o), stopped(false)
    { ; }

    void run();
    void stop();


private:
    TQWidget *receiver;
    TQMutex mutex;
    bool stopped;
};


void YellowThread::run()
{
    for (int i = 0; i < 20; i++) {
	(*yellowSem)++;

	TQCustomEvent *event = new TQCustomEvent(12345);
     	event->setData(new TQString("Yellow!"));
	TQApplication::postEvent(receiver, event);
	msleep(200);

	(*greenSem)--;

	mutex.lock();
	if (stopped) {
	    stopped = false;
	    mutex.unlock();
	    break;
	}
	mutex.unlock();
    }

    (*yellowSem)++;

    TQCustomEvent *event = new TQCustomEvent(12346);
    event->setData(new TQString("Yellow!"));
    TQApplication::postEvent(receiver, event);

    (*greenSem)--;
}

void YellowThread::stop()
{
    mutex.lock();
    stopped = true;
    mutex.unlock();
}


class GreenThread: public TQThread
{
public:
   GreenThread(TQWidget *o)
	: receiver(o), stopped( false )
    { ; }

    void run();
    void stop();


private:
    TQWidget *receiver;
    TQMutex mutex;
    bool stopped;
};


void GreenThread::run()
{
    for (int i = 0; i < 20; i++) {
	(*greenSem)++;

	TQCustomEvent *event = new TQCustomEvent(12345);
     	event->setData(new TQString("Green!"));
	TQApplication::postEvent(receiver, event);
	msleep(200);

	(*yellowSem)--;

	mutex.lock();
	if (stopped) {
	    stopped = false;
	    mutex.unlock();
	    break;
	}
	mutex.unlock();
    }

    (*greenSem)++;

    TQCustomEvent *event = new TQCustomEvent(12346);
    event->setData(new TQString("Green!"));
    TQApplication::postEvent(receiver, event);
    msleep(10);

    (*yellowSem)--;
}

void GreenThread::stop()
{
    mutex.lock();
    stopped = true;
    mutex.unlock();
}



class SemaphoreExample : public TQWidget
{
    TQ_OBJECT
public:
    SemaphoreExample();
    ~SemaphoreExample();

    void customEvent(TQCustomEvent *);


public slots:
    void startExample();


protected:


private:
    TQMultiLineEdit *mlineedit;
    TQPushButton *button;
    TQLabel *label;

    YellowThread yellowThread;
    GreenThread greenThread;
};


SemaphoreExample::SemaphoreExample()
    : TQWidget(), yellowThread(this), greenThread(this)
{
    yellowSem = new TQSemaphore(1);
    greenSem = new TQSemaphore(1);

    button = new TQPushButton("&Ignition!", this);
    connect(button, TQ_SIGNAL(clicked()), TQ_SLOT(startExample()));

    mlineedit = new TQMultiLineEdit(this);
    label = new TQLabel(this);

    TQVBoxLayout *vbox = new TQVBoxLayout(this, 5);
    vbox->addWidget(button);
    vbox->addWidget(mlineedit);
    vbox->addWidget(label);
}


SemaphoreExample::~SemaphoreExample()
{
    bool stopYellow = yellowThread.running(),
	  stopGreen = greenThread.running();
    if (stopYellow)
	yellowThread.stop();
    if (greenThread.running())
	greenThread.stop();
    if (stopYellow)
	yellowThread.wait();
    if (stopGreen)
	greenThread.wait();
    delete yellowSem;
    delete greenSem;
}


void SemaphoreExample::startExample()
{
    if (yellowThread.running() || greenThread.running()) {
	TQMessageBox::information(this, "Sorry",
				 "The threads have not completed yet, and must finish before "
				 "they can be started again.");

	return;
    }

    mlineedit->clear();

    while (yellowSem->available() < yellowSem->total()) (*yellowSem)--;
    (*yellowSem)++;

    yellowThread.start();
    greenThread.start();
}


void SemaphoreExample::customEvent(TQCustomEvent *event) {
    switch (event->type()) {
    case 12345:
	{
	    TQString *s = (TQString *) event->data();

	    mlineedit->append(*s);

	    if (*s == "Green!")
		label->setBackgroundColor(green);
	    else
		label->setBackgroundColor(yellow);
	    label->setText(*s);

	    delete s;

	    break;
	}

    case 12346:
	{
	    TQString *s = (TQString *) event->data();

	    TQMessageBox::information(this, (*s) + " - Finished",
				     "The thread creating the \"" + *s +
				     "\" events has finished.");
	    delete s;

	    break;
	}

    default:
	{
	    tqWarning("Unknown custom event type: %d", event->type());
	}
    }
}


int main(int argc, char **argv)
{
    TQApplication app(argc, argv);
    SemaphoreExample se;
    app.setMainWidget(&se);
    se.show();
    return app.exec();
}


#include "main.moc"
