#include <ntqapplication.h>
#include "regexptester.h"

int main(int argc, char **argv)
{
    TQApplication app(argc, argv);
    RegexpTester form;
    form.show();
    app.connect(&app, TQ_SIGNAL(lastWindowClosed()), &app, TQ_SLOT(quit()));
    return app.exec();
}
