
#include "debug.h"
#include <kmsgbox.h>
#include <kapp.h>

void myMessageOutput( TQtMsgType type, const char *msg )    
{
  switch ( type )
  {
     case TQtDebugMsg:
#ifdef DEBUG
       fprintf( stderr, "Debug: %s\n", msg );
#endif
       break;
     case TQtWarningMsg:
       KMsgBox::message(0, i18n("Warning"), msg );
       fprintf( stderr, "Warning: %s\n", msg );
       break;
     case TQtFatalMsg:
       KMsgBox::message(0, i18n("Fatal!"), msg );
       fprintf( stderr, "Fatal: %s\n", msg );
       abort();
       // dump core on purpose
  }
}


