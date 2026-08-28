#ifndef SETDATAFORM_H
#define SETDATAFORM_H

#include "element.h"

#include <ntqdialog.h>

class TQHBoxLayout;
class TQPushButton;
class TQTable;
class TQVBoxLayout;


class SetDataForm: public TQDialog
{
    TQ_OBJECT
public:
    SetDataForm( ElementVector *elements, int decimalPlaces,
		 TQWidget *parent = 0, const char *name = "set data form",
		 bool modal = true, WFlags f = 0 );
    ~SetDataForm() {}

public slots:
    void setColor();
    void setColor( int row, int col );
    void currentChanged( int row, int col );
    void valueChanged( int row, int col );

protected slots:
    void accept();

private:
    TQTable *table;
    TQPushButton *colorPushButton;
    TQPushButton *okPushButton;
    TQPushButton *cancelPushButton;

protected:
    TQVBoxLayout *tableButtonBox;
    TQHBoxLayout *buttonBox;

private:
    ElementVector *m_elements;
    int m_decimalPlaces;
};

#endif
