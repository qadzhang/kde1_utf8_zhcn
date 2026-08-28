#include <ntqthread.h>
#include <ntqwaitcondition.h>
#include <ntqmutex.h>
#include <ntqapplication.h>
#include <ntqwidget.h>
#include <ntqpushbutton.h>
#include <ntqcheckbox.h>
#include <ntqprogressbar.h>
#include <ntqlayout.h>
#include <ntqevent.h>
#include <ntqlabel.h>
#include <ntqcstring.h>
#include <ntqtextstream.h>
#include <ntqfile.h>

#include <stdio.h>

// 50kb buffer
#define BUFSIZE (100*1000)
#define PRGSTEP (BUFSIZE / 50)
#define BLKSIZE (8)
TQByteArray bytearray;


class ProdEvent : public TQCustomEvent
{
public:
    ProdEvent(long s, bool d)
	: TQCustomEvent(TQEvent::User + 100), sz(s), dn(d)
    { ; }

    long size() const { return sz; }
    bool done() const { return dn; }


private:
    long sz;
    bool dn;
};


class ProdThread : public TQThread
{
public:
    ProdThread(TQObject *r, TQMutex *m, TQWaitCondition *c);

    void stop();
    void run();


private:
    TQObject *receiver;
    TQMutex *mutex;
    TQWaitCondition *condition;

    bool done;
};


ProdThread::ProdThread(TQObject *r, TQMutex *m, TQWaitCondition *c)
    : receiver(r), mutex(m), condition(c), done(false)
{
}


void ProdThread::stop()
{
    mutex->lock();
    done = true;
    mutex->unlock();
}


void ProdThread::run()
{
    bool stop = false;
    done = false;

    uchar *buffer = new uchar[BUFSIZE];
    int pos = 0, oldpos = 0;
    int loop = 1;
    int lastpostedpos = 0;

    ProdEvent *pe = new ProdEvent(pos, done);
    TQApplication::postEvent(receiver, pe);

    while (! stop) {
	oldpos = pos;
	int i;
	for (i = 0; i < BLKSIZE && pos < BUFSIZE; i++) {
	    buffer[pos++] = (loop % 2) ? 'o' : 'e';
	}

	mutex->lock();

	if (pos == BUFSIZE) {
	    done = true;
	}

	while (! bytearray.isNull() && ! stop) {
            condition->wakeOne();
            condition->wait(mutex);

	    stop = done;
	}

	stop = done;
	bytearray.duplicate((const char *) (buffer + oldpos), pos - oldpos);
	condition->wakeOne();

	mutex->unlock();

	if ( pos - lastpostedpos > PRGSTEP || stop ) {
	    lastpostedpos = pos;
	    ProdEvent *pe = new ProdEvent(pos, stop);
	    TQApplication::postEvent(receiver, pe);
	}

	loop++;
    }

    condition->wakeOne();

    delete [] buffer;
}


class ConsEvent : public TQCustomEvent
{
public:
    ConsEvent(long s)
	: TQCustomEvent(TQEvent::User + 101), sz(s)
    { ; }

    long size() const { return sz; }


private:
    long sz;
};


class ConsThread : public TQThread
{
public:
    ConsThread(TQObject *r, TQMutex *m, TQWaitCondition *c);

    void stop();
    void run();


private:
    TQObject *receiver;
    TQMutex *mutex;
    TQWaitCondition *condition;

    bool done;
};


ConsThread::ConsThread(TQObject *r, TQMutex *m, TQWaitCondition *c)
    : receiver(r), mutex(m), condition(c), done(false)
{
}


void ConsThread::stop()
{
    mutex->lock();
    done = true;
    mutex->unlock();
}


void ConsThread::run()
{
    bool stop = false;
    done = false;

    TQFile file("prodcons.out");
    file.open(IO_WriteOnly);

    long size = 0;
    long lastsize = 0;

    ConsEvent *ce = new ConsEvent(size);
    TQApplication::postEvent(receiver, ce);

    while (! stop) {
	mutex->lock();

	while (bytearray.isNull() && ! stop) {
            condition->wakeOne();
            condition->wait(mutex);

	    stop = done;
	}

	if (size < BUFSIZE) {
	    file.writeBlock(bytearray.data(), bytearray.size());
	    size += bytearray.size();
	    bytearray.resize(0);
	}

	stop = done || size >= BUFSIZE;

	mutex->unlock();

	if ( size - lastsize > 1000 || stop ) {
	    lastsize = size;
	    ConsEvent *ce = new ConsEvent(size);
	    TQApplication::postEvent(receiver, ce);
	}
    }

    file.flush();
    file.close();
}


class ProdCons : public TQWidget
{
    TQ_OBJECT

public:
    ProdCons();
    ~ProdCons();

    void customEvent(TQCustomEvent *);


public slots:
    void go();
    void stop();


private:
    TQMutex mutex;
    TQWaitCondition condition;

    ProdThread *prod;
    ConsThread *cons;

    TQPushButton *startbutton, *stopbutton;
    TQCheckBox *loopcheckbox;
    TQProgressBar *prodbar, *consbar;
    bool stopped;
    bool redraw;
};


ProdCons::ProdCons()
    : TQWidget(0, "producer consumer widget"),
      prod(0), cons(0), stopped(false), redraw(true)
{
    startbutton = new TQPushButton("&Start", this);
    connect(startbutton, TQ_SIGNAL(clicked()), TQ_SLOT(go()));

    stopbutton = new TQPushButton("S&top", this);
    connect(stopbutton, TQ_SIGNAL(clicked()), TQ_SLOT(stop()));
    stopbutton->setEnabled(false);

    loopcheckbox = new TQCheckBox("Loop", this);
    loopcheckbox->setChecked(false);

    prodbar = new TQProgressBar(BUFSIZE, this);
    consbar = new TQProgressBar(BUFSIZE, this);

    TQVBoxLayout *vbox = new TQVBoxLayout(this, 8, 8);
    vbox->addWidget(new TQLabel(TQString("Producer/Consumer using %1 byte buffer").
			       arg(BUFSIZE), this));
    vbox->addWidget(startbutton);
    vbox->addWidget(stopbutton);
    vbox->addWidget(loopcheckbox);
    vbox->addWidget(new TQLabel("Producer progress:", this));
    vbox->addWidget(prodbar);
    vbox->addWidget(new TQLabel("Consumer progress:", this));
    vbox->addWidget(consbar);
}


ProdCons::~ProdCons()
{
    stop();

    if (prod) {
	delete prod;
	prod = 0;
    }

    if (cons) {
	delete cons;
	cons = 0;
    }
}


void ProdCons::go()
{
    stopped = false;

    mutex.lock();

    if ( redraw ) {
	startbutton->setEnabled(false);
	stopbutton->setEnabled(true);
    }

    // start the consumer first
    if (! cons)
        cons = new ConsThread(this, &mutex, &condition);
    cons->start();

    // wait for consumer to signal that it has started
    condition.wait(&mutex);

    if (! prod)
        prod = new ProdThread(this, &mutex, &condition);
    prod->start();
    mutex.unlock();
}


void ProdCons::stop()
{
    if (prod && prod->running()) {
	prod->stop();
        condition.wakeAll();
	prod->wait();
    }

    if (cons && cons->running()) {
	cons->stop();
        condition.wakeAll();
	cons->wait();
    }

    if ( redraw ) {
	// no point in repainting these buttons so many times is we are looping...
	startbutton->setEnabled(true);
	stopbutton->setEnabled(false);
    }

    stopped = true;
}


void ProdCons::customEvent(TQCustomEvent *e)
{
    switch (e->type()) {
    case TQEvent::User + 100:
	{
	    // ProdEvent
	    ProdEvent *pe = (ProdEvent *) e;

	    if (pe->size() == 0 ||
		pe->size() == BUFSIZE ||
		pe->size() - prodbar->progress() >= PRGSTEP)
		prodbar->setProgress(pe->size());

	    // reap the threads
	    if (pe->done()) {
		bool loop = (loopcheckbox->isChecked() && ! stopped);
		bool save_redraw = redraw;
		redraw = !loop;

		stop();

		if (loop)
		    go();

		redraw = save_redraw;
	    }

	    break;
	}

    case TQEvent::User + 101:
	{
	    // ConsEvent
	    ConsEvent *ce = (ConsEvent *) e;

	    if (ce->size() == 0 ||
		ce->size() == BUFSIZE ||
		ce->size() - consbar->progress() >= PRGSTEP)
		consbar->setProgress(ce->size());

	    break;
	}

    default:
	{
	    ;
	}
    }
}


int main(int argc, char **argv)
{
    TQApplication app(argc, argv);
    ProdCons prodcons;
    app.setMainWidget(&prodcons);
    prodcons.show();
    return app.exec();
}


#include "prodcons.moc"
