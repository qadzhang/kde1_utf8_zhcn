#include <ntqobject.h>
#include <ntqdialog.h>

class Receiver : public TQObject
{
    TQ_OBJECT
public:
    void setParent( TQDialog *parent );
public slots:
    void setAmount();
private:
    TQDialog *p;
};
