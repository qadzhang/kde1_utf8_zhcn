/****************************************************************
**
** TQt threading tutorial
** (c) 2012 Timothy Pearson <kb9vqf@pearsoncomputing.net>
**
** This tutorial is released into the Public Domain and
** can therefore be modified and/or used for any purpose
**
****************************************************************/

#ifndef _MAIN_H_
#define _MAIN_H_

#include <ntqapplication.h>
#include <ntqobject.h>
#include <ntqpushbutton.h>
#include <ntqthread.h>

class MainObject;

class WorkerObject : public TQObject
{
	TQ_OBJECT

	public slots:
		void run();
		void timerHandler();

	signals:
		void displayMessage(TQString, TQString);

	public:
		TQString threadFriendlyName;
};

class MainObject : public TQObject
{
	TQ_OBJECT

	public slots:
		void emitMessage(TQString, TQString);
		void buttonClicked();
};

#endif // _MAIN_H_
