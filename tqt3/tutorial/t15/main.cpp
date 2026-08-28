/****************************************************************
**
** TQt threading tutorial
** (c) 2012 Timothy Pearson <kb9vqf@pearsoncomputing.net>
**
** This tutorial is released into the Public Domain and
** can therefore be modified and/or used for any purpose
**
****************************************************************/

#include "main.h"

#include <unistd.h>

#include <ntqtimer.h>
#include <ntqeventloop.h>

void WorkerObject::run()
{
	tqDebug( "[%s] thread: %p event loop: %p", threadFriendlyName.ascii(), TQThread::currentThreadObject(), TQApplication::eventLoop() );

	TQEventLoop* eventLoop = TQApplication::eventLoop();
	if (!eventLoop) return;

	TQTimer *t = new TQTimer(this);
	connect( t, TQ_SIGNAL(timeout()), TQ_SLOT(timerHandler()) );
	t->start( 1000, false );

	for( int count = 0; count < 5; count++ ) {
		sleep( 1 );
		tqDebug( "[%s] Ping!", threadFriendlyName.ascii() );
		displayMessage("Hi", "There!");
		eventLoop->processEvents(TQEventLoop::AllEvents);
	}

	delete t;

	eventLoop->exit(0);
}

void WorkerObject::timerHandler()
{
	tqDebug( "[%s] Timer fired!", threadFriendlyName.ascii() );
}

void MainObject::emitMessage(TQString str1, TQString str2)
{
	tqDebug( "%s", ("[MainObject]    emitMessage: " + str1 + " " + str2).ascii() );
}

void MainObject::buttonClicked()
{
	tqDebug( "[MainObject]    Button clicked!" );

	TQEventLoop* eventLoop = TQApplication::eventLoop();
	if (!eventLoop) return;
	eventLoop->exit(0);
}

#define SET_UP_WORKER(x, y, z)														\
	WorkerObject x;															\
	x.threadFriendlyName = y;													\
	x.moveToThread(&z);														\
	TQObject::connect(&x, TQ_SIGNAL(displayMessage(TQString,TQString)), &mainobject, TQ_SLOT(emitMessage(TQString,TQString)));		\
	TQTimer::singleShot(0, &x, TQ_SLOT(run()));

int main( int argc, char **argv )
{
	TQApplication a( argc, argv );

	tqDebug( "[MainObject]    thread: %p event loop: %p", TQThread::currentThreadObject(), TQApplication::eventLoop() );

	TQPushButton hello( "Exit", 0 );
	hello.resize( 100, 30 );

	MainObject mainobject;

	TQEventLoopThread workerthread0;
	TQEventLoopThread workerthread1;
	TQEventLoopThread workerthread2;
	TQEventLoopThread workerthread3;
	TQEventLoopThread workerthread4;
	TQEventLoopThread workerthread5;
	TQEventLoopThread workerthread6;
	TQEventLoopThread workerthread7;
	TQEventLoopThread workerthread8;
	TQEventLoopThread workerthread9;

	TQEventLoopThread workerthread10;
	TQEventLoopThread workerthread11;
	TQEventLoopThread workerthread12;
	TQEventLoopThread workerthread13;
	TQEventLoopThread workerthread14;
	TQEventLoopThread workerthread15;
	TQEventLoopThread workerthread16;
	TQEventLoopThread workerthread17;
	TQEventLoopThread workerthread18;
	TQEventLoopThread workerthread19;

	SET_UP_WORKER(workerobject0, "WorkerObject0", workerthread0)
	SET_UP_WORKER(workerobject1, "WorkerObject1", workerthread1)
	SET_UP_WORKER(workerobject2, "WorkerObject2", workerthread2)
	SET_UP_WORKER(workerobject3, "WorkerObject3", workerthread3)
	SET_UP_WORKER(workerobject4, "WorkerObject4", workerthread4)
	SET_UP_WORKER(workerobject5, "WorkerObject5", workerthread5)
	SET_UP_WORKER(workerobject6, "WorkerObject6", workerthread6)
	SET_UP_WORKER(workerobject7, "WorkerObject7", workerthread7)
	SET_UP_WORKER(workerobject8, "WorkerObject8", workerthread8)
	SET_UP_WORKER(workerobject9, "WorkerObject9", workerthread9)

	SET_UP_WORKER(workerobject10, "WorkerObjec10", workerthread10)
	SET_UP_WORKER(workerobject11, "WorkerObjec11", workerthread11)
	SET_UP_WORKER(workerobject12, "WorkerObjec12", workerthread12)
	SET_UP_WORKER(workerobject13, "WorkerObjec13", workerthread13)
	SET_UP_WORKER(workerobject14, "WorkerObjec14", workerthread14)
	SET_UP_WORKER(workerobject15, "WorkerObjec15", workerthread15)
	SET_UP_WORKER(workerobject16, "WorkerObjec16", workerthread16)
	SET_UP_WORKER(workerobject17, "WorkerObjec17", workerthread17)
	SET_UP_WORKER(workerobject18, "WorkerObjec18", workerthread18)
	SET_UP_WORKER(workerobject19, "WorkerObjec19", workerthread19)

	workerthread0.start();
	workerthread1.start();
	workerthread2.start();
	workerthread3.start();
	workerthread4.start();
	workerthread5.start();
	workerthread6.start();
	workerthread7.start();
	workerthread8.start();
	workerthread9.start();

	workerthread10.start();
	workerthread11.start();
	workerthread12.start();
	workerthread13.start();
	workerthread14.start();
	workerthread15.start();
	workerthread16.start();
	workerthread17.start();
	workerthread18.start();
	workerthread19.start();

	a.setMainWidget( &hello );
	TQObject::connect(&hello, TQ_SIGNAL(clicked()), &mainobject, TQ_SLOT(buttonClicked()));
	hello.show();
	a.exec();
	hello.hide();

	tqDebug( "[MainObject]    Waiting for thread completion..." );

	workerthread0.wait();
	workerthread1.wait();
	workerthread2.wait();
	workerthread3.wait();
	workerthread4.wait();
	workerthread5.wait();
	workerthread6.wait();
	workerthread7.wait();
	workerthread8.wait();
	workerthread9.wait();

	workerthread10.wait();
	workerthread11.wait();
	workerthread12.wait();
	workerthread13.wait();
	workerthread14.wait();
	workerthread15.wait();
	workerthread16.wait();
	workerthread17.wait();
	workerthread18.wait();
	workerthread19.wait();

	tqDebug( "[MainObject]    Finished!" );
}
