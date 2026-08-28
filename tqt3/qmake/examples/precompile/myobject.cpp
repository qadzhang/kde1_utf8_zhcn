#include <iostream>
#include <ntqobject.h>
#include "myobject.h"

MyObject::MyObject()
    : TQObject()
{
    std::cout << "MyObject::MyObject()\n";
}

MyObject::~MyObject()
{
    tqDebug("MyObject::~MyObject()");
}
