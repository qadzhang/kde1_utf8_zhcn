/****************************************************************************
**
** Implementation of asynchronous I/O classes
**
** Created : 970617
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of the kernel module of the TQt GUI Toolkit.
**
** This file may be used under the terms of the GNU General
** Public License versions 2.0 or 3.0 as published by the Free
** Software Foundation and appearing in the files LICENSE.GPL2
** and LICENSE.GPL3 included in the packaging of this file.
** Alternatively you may (at your option) use any later version
** of the GNU General Public License if such license has been
** publicly approved by Trolltech ASA (or its successors, if any)
** and the KDE Free TQt Foundation.
**
** Please review the following information to ensure GNU General
** Public Licensing requirements will be met:
** http://trolltech.com/products/qt/licenses/licensing/opensource/.
** If you are unsure which license is appropriate for your use, please
** review the following information:
** http://trolltech.com/products/qt/licenses/licensing/licensingoverview
** or contact the sales department at sales@trolltech.com.
**
** This file may be used under the terms of the Q Public License as
** defined by Trolltech ASA and appearing in the file LICENSE.TQPL
** included in the packaging of this file.  Licensees holding valid TQt
** Commercial licenses may use this file in accordance with the TQt
** Commercial License Agreement provided with the Software.
**
** This file is provided "AS IS" with NO WARRANTY OF ANY KIND,
** INCLUDING THE WARRANTIES OF DESIGN, MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE. Trolltech reserves all rights not granted
** herein.
**
**********************************************************************/

#include "ntqasyncio.h"
#include "ntqiodevice.h"
#include <stdlib.h>

#ifndef TQT_NO_ASYNC_IO

/*!
  \class TQAsyncIO ntqasyncio.h
  \obsolete
  \brief The TQAsyncIO class encapsulates I/O asynchronicity.

  The TQt classes for asynchronous input/output provide a simple
  mechanism to allow large files or slow data sources to be processed
  without using large amounts of memory or blocking the user interface.

  This facility is used in TQt to drive animated images.  See TQImageConsumer.
*/


/*!
  Destroys the async IO object.
*/
TQAsyncIO::~TQAsyncIO()
{
}

/*!
  Ensures that only one object, \a obj and function, \a member, can
  respond to changes in readiness.
*/
void TQAsyncIO::connect(TQObject* obj, const char *member)
{
    signal.disconnect(0, 0);
    signal.connect(obj, member);
}

/*!
  Derived classes should call this when they change from being
  unready to ready.
*/
void TQAsyncIO::ready()
{
    signal.activate();
}



/*!
  \class TQDataSink ntqasyncio.h
  \obsolete
  \brief The TQDataSink class is an asynchronous consumer of data.

  A data sink is an object which receives data from some source in an
  asynchronous manner.  This means that at some time not determined by
  the data sink, blocks of data are given to it from processing.  The
  data sink is able to limit the maximum size of such blocks which it
  is currently able to process.

  \sa TQAsyncIO, TQDataSource, TQDataPump
*/

/*!
  \fn int TQDataSink::readyToReceive()

  The data sink should return a value indicating how much data it is ready
  to consume.  This may be 0.
*/

/*!
  This should be called whenever readyToReceive() might have become non-zero.
  It is merely calls TQAsyncIO::ready() if readyToReceive() is non-zero.
*/
void TQDataSink::maybeReady()
{
    if (readyToReceive()) ready();
}

/*!
  \fn void TQDataSink::receive(const uchar*, int count)

  This function is called to provide data for the data sink.  The \a count
  will be no more than the amount indicated by the most recent call to
  readyToReceive().  The sink must use all the provided data.
*/

/*!
  \fn void TQDataSink::eof()

  This function will be called when no more data is available for
  processing.
*/


/*!
  \class TQDataSource ntqasyncio.h
  \obsolete
  \brief The TQDataSource class is an asynchronous producer of data.

  A data source is an object which provides data from some source in an
  asynchronous manner.  This means that at some time not determined by
  the data source, blocks of data will be taken from it for processing.
  The data source is able to limit the maximum size of such blocks which
  it is currently able to provide.

  \sa TQAsyncIO, TQDataSink, TQDataPump
*/

/*!
  \fn int TQDataSource::readyToSend()

  The data source should return a value indicating how much data it is ready
  to provide.  This may be 0.  If the data source knows it will never be
  able to provide any more data (until after a rewind()), it may return -1.
*/

/*!
  This should be called whenever readyToSend() might have become non-zero.
  It is merely calls TQAsyncIO::ready() if readyToSend() is non-zero.
*/
void TQDataSource::maybeReady()
{
    if (readyToSend()) ready();
}

/*!
  \fn void TQDataSource::sendTo(TQDataSink*, int count)

  This function is called to extract data from the source, by sending
  it to the given data sink.  The \a count will be no more than the amount
  indicated by the most recent call to readyToSend().  The source must
  use all the provided data, and the sink will be prepared to accept at
  least this much data.
*/

/*!
  This function should return true if the data source can be rewound.

  The default returns false.
*/
bool TQDataSource::rewindable() const
{
    return false;
}

/*!
  If this function is called with \a on set to true, and rewindable()
  is true, then the data source must take measures to allow the rewind()
  function to subsequently operate as described.  If rewindable() is false,
  the function should call TQDataSource::enableRewind(), which aborts with
  a tqFatal() error.

  For example, a network connection may choose to use a disk cache
  of input only if rewinding is enabled before the first buffer-full of
  data is discarded, returning false in rewindable() if that first buffer
  is discarded.
*/
void TQDataSource::enableRewind( bool /* on */ )
{
    tqFatal( "Attempted to make unrewindable TQDataSource rewindable" );
}

/*!
  This function rewinds the data source.  This may only be called if
  enableRewind(true) has been previously called.
*/
void TQDataSource::rewind()
{
    tqFatal("Attempted to rewind unrewindable TQDataSource");
}

/*!
  \class TQIODeviceSource ntqasyncio.h
  \obsolete
  \brief The TQIODeviceSource class is a TQDataSource that draws data from a TQIODevice.

  This class encapsulates retrieving data from a TQIODevice (such as a TQFile).
*/

/*!
  Constructs a TQIODeviceSource from the TQIODevice \a device.  The TQIODevice
  \e must be dynamically allocated, becomes owned by the TQIODeviceSource,
  and will be deleted when the TQIODeviceSource is destroyed. \a buffer_size
  determines the size of buffering to use between asynchronous operations.
  The higher the \a buffer_size, the more efficient, but the less interleaved
  the operation will be with other processing.
*/
TQIODeviceSource::TQIODeviceSource(TQIODevice* device, int buffer_size) :
    buf_size(buffer_size),
    buffer(new uchar[buf_size]),
    iod(device),
    rew(false)
{
}

/*!
  Destroys the TQIODeviceSource, deleting the TQIODevice from which it was
  constructed.
*/
TQIODeviceSource::~TQIODeviceSource()
{
    delete iod;
    delete [] buffer;
}

/*!
  Ready until end-of-file.
*/
int TQIODeviceSource::readyToSend()
{
    if ( iod->status() != IO_Ok || !(iod->state() & IO_Open) )
	return -1;

    int n = TQMIN((uint)buf_size, iod->size()-iod->at()); // ### not 64-bit safe
                                                         // ### not large file safe
    return n ? n : -1;
}

/*!
  Reads a block of data and sends up to \a n bytes to the \a sink.
*/
void TQIODeviceSource::sendTo(TQDataSink* sink, int n)
{
    iod->readBlock((char*)buffer, n);
    sink->receive(buffer, n);
}

/*!
  All TQIODeviceSource's are rewindable.
*/
bool TQIODeviceSource::rewindable() const
{
    return true;
}

/*!
  If \a on is set to true then rewinding is enabled.  
  No special action is taken.  If \a on is set to 
  false then rewinding is disabled.
*/
void TQIODeviceSource::enableRewind(bool on)
{
    rew = on;
}

/*!
  Calls reset() on the TQIODevice.
*/
void TQIODeviceSource::rewind()
{
    if (!rew) {
	TQDataSource::rewind();
    } else {
	iod->reset();
	ready();
    }
}


/*!
  \class TQDataPump ntqasyncio.h
  \obsolete
  \brief The TQDataPump class moves data from a TQDataSource to a TQDataSink during event processing.

  For a TQDataSource to provide data to a TQDataSink, a controller must exist
  to examine the TQDataSource::readyToSend() and TQDataSink::readyToReceive()
  methods and respond to the TQASyncIO::activate() signal of the source and
  sink.  One very useful way to do this is interleaved with other event
  processing.  TQDataPump provides this - create a pipe between a source
  and a sink, and data will be moved during subsequent event processing.

  Note that each source can only provide data to one sink and each sink
  can only receive data from one source (although it is quite possible
  to write a multiplexing sink that is multiple sources).
*/

/*!
  Constructs a TQDataPump to move data from a given \a data_source
  to a given \a data_sink.
*/
TQDataPump::TQDataPump(TQDataSource* data_source, TQDataSink* data_sink) :
    source(data_source), sink(data_sink)
{
    source->connect(this, TQ_SLOT(kickStart()));
    sink->connect(this, TQ_SLOT(kickStart()));
    connect(&timer, TQ_SIGNAL(timeout()), this, TQ_SLOT(tryToPump()));
    timer.start(0, true);
}

void TQDataPump::kickStart()
{
    if (!timer.isActive()) {
	interval = 0;
	timer.start(0, true);
    }
}

void TQDataPump::tryToPump()
{
    int supply, demand;

    supply = source->readyToSend();
    demand = sink->readyToReceive();
    if (demand <= 0) {
	return;
    }
    interval = 0;
    if (supply < 0) {
	// All done (until source signals change in readiness)
	sink->eof();
	return;
    }
    if (!supply)
	return;
    source->sendTo(sink, TQMIN(supply, demand));

    timer.start(0, true);
}

#endif // TQT_NO_ASYNC_IO

