/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include <ntqvaluelist.h>
#include <ntqstring.h>
#include <ntqwindowdefs.h>
#include <stdio.h>


class Employee
{
public:
    Employee(): s(0) {}
    Employee( const TQString& name, int salary )
	: n(name), s(salary) {}

    TQString name() const { return n; }

    int salary() const { return s; }
    void setSalary( int salary ) { s = salary; }

private:
    TQString n;
    int s;
};


int main( int, char** )
{
    typedef TQValueList<Employee> EmployeeList;
    EmployeeList list;

    list.append( Employee("Bill", 50000) );
    list.append( Employee("Steve",80000) );
    list.append( Employee("Ron",  60000) );

    Employee joe( "Joe", 50000 );
    list.append( joe );
    joe.setSalary( 4000 );

    EmployeeList::ConstIterator it = list.begin();
    while( it != list.end() ) {
	printf( "%s earns %d\n", (*it).name().latin1(), (*it).salary() );
	++it;
    }

    return 0;
}
