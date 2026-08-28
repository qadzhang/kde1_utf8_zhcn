/****************************************************************************
**
** Implementation of cached TQt SQL result classes
**
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of the sql module of the TQt GUI Toolkit.
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

#include "qsqlcachedresult.h"
#include <ntqdatetime.h>

#ifndef TQT_NO_SQL
static const uint initial_cache_size = 128;

class TQtSqlCachedResultPrivate
{
public:
    TQtSqlCachedResultPrivate();
    bool seek(int i);
    void init(int count, bool fo);
    void cleanup();
    TQtSqlCachedResult::RowCache* next();
    void revertLast();

    TQtSqlCachedResult::RowsetCache *cache;
    TQtSqlCachedResult::RowCache *current;
    int rowCacheEnd;
    int colCount;
    bool forwardOnly;
};

TQtSqlCachedResultPrivate::TQtSqlCachedResultPrivate():
    cache(0), current(0), rowCacheEnd(0), colCount(0), forwardOnly(false)
{
}

void TQtSqlCachedResultPrivate::cleanup()
{
    if (cache) {
	for (int i = 0; i < rowCacheEnd; ++i)
	    delete (*cache)[i];
	delete cache;
	cache = 0;
    }
    if (forwardOnly)
	delete current;
    current = 0;
    forwardOnly = false;
    colCount = 0;
    rowCacheEnd = 0;
}

void TQtSqlCachedResultPrivate::init(int count, bool fo)
{
    cleanup();
    forwardOnly = fo;
    colCount = count;
    if (fo)
	current = new TQtSqlCachedResult::RowCache(count);
    else
	cache = new TQtSqlCachedResult::RowsetCache(initial_cache_size);
}

TQtSqlCachedResult::RowCache *TQtSqlCachedResultPrivate::next()
{
    if (forwardOnly)
	return current;

    Q_ASSERT(cache);
    current = new TQtSqlCachedResult::RowCache(colCount);
    if (rowCacheEnd == (int)cache->size())
	cache->resize(cache->size() * 2);
    cache->insert(rowCacheEnd++, current);
    return current;
}

bool TQtSqlCachedResultPrivate::seek(int i)
{
    if (forwardOnly || i < 0)
	return false;
    if (i >= rowCacheEnd)
	return false;
    current = (*cache)[i];
    return true;
}

void TQtSqlCachedResultPrivate::revertLast()
{
    if (forwardOnly)
	return;
    --rowCacheEnd;
    delete current;
    current = 0;
}

//////////////

TQtSqlCachedResult::TQtSqlCachedResult(const TQSqlDriver * db ): TQSqlResult ( db )
{
    d = new TQtSqlCachedResultPrivate();
}

TQtSqlCachedResult::~TQtSqlCachedResult()
{
    delete d;
}

void TQtSqlCachedResult::init(int colCount)
{
    d->init(colCount, isForwardOnly());
}

bool TQtSqlCachedResult::fetch(int i)
{
    if ((!isActive()) || (i < 0))
	return false;
    if (at() == i)
	return true;
    if (d->forwardOnly) {
	// speed hack - do not copy values if not needed
	if (at() > i || at() == TQSql::AfterLast)
	    return false;
	while(at() < i - 1) {
	    if (!gotoNext(0))
		return false;
	    setAt(at() + 1);
	}
	if (!gotoNext(d->current))
	    return false;
	setAt(at() + 1);
	return true;
    }
    if (d->seek(i)) {
	setAt(i);
	return true;
    }
    setAt(d->rowCacheEnd - 1);
    while (at() < i) {
	if (!cacheNext())
	    return false;
    }
    return true;
}

bool TQtSqlCachedResult::fetchNext()
{
    if (d->seek(at() + 1)) {
	setAt(at() + 1);
	return true;
    }
    return cacheNext();
}

bool TQtSqlCachedResult::fetchPrev()
{
    return fetch(at() - 1);
}

bool TQtSqlCachedResult::fetchFirst()
{
    if (d->forwardOnly && at() != TQSql::BeforeFirst) {
	return false;
    }
    if (d->seek(0)) {
	setAt(0);
	return true;
    }
    return cacheNext();
}

bool TQtSqlCachedResult::fetchLast()
{
    if (at() == TQSql::AfterLast) {
	if (d->forwardOnly)
	    return false;
	else
	    return fetch(d->rowCacheEnd - 1);
    }

    int i = at();
    while (fetchNext())
	i++; /* brute force */
    if (d->forwardOnly && at() == TQSql::AfterLast) {
	setAt(i);
	return true;
    } else {
	return fetch(d->rowCacheEnd - 1);
    }
}

TQVariant TQtSqlCachedResult::data(int i)
{
    if (!d->current || i >= (int)d->current->size() || i < 0)
	return TQVariant();

    return (*d->current)[i];
}

bool TQtSqlCachedResult::isNull(int i)
{
    if (!d->current || i >= (int)d->current->size() || i < 0)
	return true;

    return (*d->current)[i].isNull();
}

void TQtSqlCachedResult::cleanup()
{
    setAt(TQSql::BeforeFirst);
    setActive(false);
    d->cleanup();
}

bool TQtSqlCachedResult::cacheNext()
{
    if (!gotoNext(d->next())) {
	d->revertLast();
	return false;
    }
    setAt(at() + 1);
    return true;
}

int TQtSqlCachedResult::colCount() const
{
    return d->colCount;
}
#endif // TQT_NO_SQL
