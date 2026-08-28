/****************************************************************************
** Form interface generated from reading ui file 'creditformbase.ui'
**
** Created: Fri Aug 10 09:48:10 2001
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#ifndef CREDITFORMBASE_H
#define CREDITFORMBASE_H

#include <ntqvariant.h>
#include <ntqdialog.h>
class TQVBoxLayout; 
class TQHBoxLayout; 
class TQGridLayout; 
class TQButtonGroup;
class TQPushButton;
class TQRadioButton;
class TQSpinBox;

class CreditFormBase : public TQDialog
{ 
    TQ_OBJECT

public:
    CreditFormBase( TQWidget* parent = 0, const char* name = 0, bool modal = false, WFlags fl = 0 );
    ~CreditFormBase();

    TQButtonGroup* creditButtonGroup;
    TQRadioButton* stdRadioButton;
    TQRadioButton* noneRadioButton;
    TQRadioButton* specialRadioButton;
    TQSpinBox* amountSpinBox;
    TQPushButton* okPushButton;
    TQPushButton* cancelPushButton;


public slots:
    virtual void setAmount();

protected:
    TQVBoxLayout* CreditFormBaseLayout;
    TQVBoxLayout* creditButtonGroupLayout;
    TQHBoxLayout* Layout4;
};

#endif // CREDITFORMBASE_H
