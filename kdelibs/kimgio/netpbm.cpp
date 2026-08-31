////////////////////////////////////////////////////
//
// Transparent support for several files formats in Qt Pixmaps,
// using the NetPBM tools.
//
// Dirk Schoenberger, Jul 1997.
//

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <qimage.h>

#define CMDBUFLEN     4096

//////
// the real filter.
//

void import_graphic (char *filter, QImageIO *image)
{
  char * tmpFileName;
  QImage myimage;

  char cmdBuf [CMDBUFLEN];

  tmpFileName = tmpnam(NULL);

  /* [KDE1 Revival 2026] fileName() 返回 TQString 对象——varargs 直传把对象指针
     当 char* 解引用出垃圾命令行；先转 utf8() 字节串（TQCString 局部保生命周期） */
  {
    TQCString fname = image->fileName().utf8();
    snprintf (cmdBuf, CMDBUFLEN, "%s %s > %s", filter, fname.data(), tmpFileName);
  }
//  printf (cmdBuf);
//  fflush (stdout);

  system (cmdBuf);
  myimage.load (tmpFileName);

  unlink (tmpFileName);

  image->setImage (myimage);
  image->setStatus (0);
}

//////
// PCX IO handlers for QImage.
//

void read_pcx (QImageIO *image)
{
  import_graphic ("pcxtoppm", image);
}

//////
// IFF IO handlers for QImage.
//

void read_ilbm (QImageIO *image)
{
  import_graphic ("ilbmtoppm", image);
}

//////
// TGA IO handlers for QImage.
//

void read_tga (QImageIO *image)
{
  import_graphic ("tgatoppm", image);
}

