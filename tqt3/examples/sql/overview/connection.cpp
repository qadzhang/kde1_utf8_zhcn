/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/
#include <ntqsqldatabase.h>
#include "connection.h"

bool createConnections()
{

    TQSqlDatabase *defaultDB = TQSqlDatabase::addDatabase( DB_SALES_DRIVER );
    defaultDB->setDatabaseName( DB_SALES_DBNAME );
    defaultDB->setUserName( DB_SALES_USER );
    defaultDB->setPassword( DB_SALES_PASSWD );
    defaultDB->setHostName( DB_SALES_HOST );
    if ( ! defaultDB->open() ) {
	tqWarning( "Failed to open sales database: " + defaultDB->lastError().text() );
	return false;
    }

    TQSqlDatabase *oracle = TQSqlDatabase::addDatabase( DB_ORDERS_DRIVER, "ORACLE" );
    oracle->setDatabaseName( DB_ORDERS_DBNAME );
    oracle->setUserName( DB_ORDERS_USER );
    oracle->setPassword( DB_ORDERS_PASSWD );
    oracle->setHostName( DB_ORDERS_HOST );
    if ( ! oracle->open() ) {
	tqWarning( "Failed to open orders database: " + oracle->lastError().text() );
	return false;
    }

    TQSqlQuery q(TQString::null, defaultDB);
    q.exec("create table people (id integer primary key, name char(40))");
    q.exec("create table staff (id integer primary key, forename char(40), "
           "surname char(40), salary float, statusid integer)");
    q.exec("create table status (id integer primary key, name char(30))");
    q.exec("create table creditors (id integer primary key, forename char(40), "
           "surname char(40), city char(30))");
    q.exec("create table prices (id integer primary key, name char(40), price float)");
    q.exec("create table invoiceitem (id integer primary key, "
           "pricesid integer, quantity integer, paiddate date)");

    TQSqlQuery q2(TQString::null, oracle);
    q2.exec("create table people (id integer primary key, name char(40))");

    return true;
}
