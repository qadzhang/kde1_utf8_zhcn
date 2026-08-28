/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you wish to add, delete or rename slots use TQt Designer which will
** update this file, preserving your code. Create an init() slot in place of
** a constructor, and a destroy() slot in place of a destructor.
*****************************************************************************/

#include <ntqsqldatabase.h>

void ConnectDialog::init()
{
    comboDriver->clear();
    comboDriver->insertStringList( TQSqlDatabase::drivers() );
}

void ConnectDialog::destroy()
{

}
