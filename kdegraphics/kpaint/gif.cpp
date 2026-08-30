//
// GIF write support and transparency support added by Richard J. Moore
// Note that Qt will support gif files internally in version 1.3 so this
// code is not worth spending much time on.
// moorer@cs.man.ac.uk
//
// $Id: gif.cpp,v 1.6 1998/10/19 20:09:16 garbanzo Exp $
//
//   Modified for the KDE1 Revival Project, 2026
//   Maintainer: <维护者姓名> <邮箱>
//   Modifications written with GLM-5.3 (Z.ai)
//   [2026-08-30] 按 AGENTS.md §6.7 以 giflib 5.x 现代 API 全面重写：
//   ① 错误处理改用出参模型（DGifOpenFileName/EGifOpenFileName 等的
//     int* Error 参数与 DGifGetGifError），移除 1999 年全局 GifError /
//     PrintGifError 依赖；② 函数名对齐 giflib5 的 Gif 前缀命名
//     （GifMakeMapObject/GifFreeMapObject）；③ 修正 1999 版多处中途
//     return 造成 GifFile 不关闭、行缓冲不释放的资源泄漏；
//   ④ 读路径支持帧局部色表缺省时回退画布尺寸色表数。

#include <kdebug.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>

#include <qimage.h>

#include "gif.h"

extern "C" {
#include <gif_lib.h>
}

// 统一错误出口：带位置与 giflib5 错误码文本（替代 1999 年 PrintGifError）
static void gif_report_error(const char *where, int errCode)
{
    const char *msg = GifErrorString(errCode);
    fprintf(stderr, "kpaint GIF: %s: %s (%d)\n",
            where, msg ? msg : "unknown error", errCode);
}

// 释放逐行屏幕缓冲
static void gif_free_screen(GifRowType *buf, int height)
{
    if (!buf)
        return;
    for (int i = 0; i < height; i++)
        if (buf[i])
            free(buf[i]);
    free(buf);
}

void read_gif_file(QImageIO * imageio)
{
  int i, j, Size, Row, Col, Width, Height, ExtCode, Count;
  GifRecordType RecordType;
  GifByteType *Extension;
  GifRowType *ScreenBuffer = 0;
  GifFileType *GifFile = 0;
  int gifErr = 0;
  bool ok = false;

  QImage image;
  ColorMapObject *Colourmap;
  int trans = -1;      // 透明色索引（-1 表示无透明色）
  unsigned char *uc_row;
  int InterlacedOffset[] = {0, 4, 2, 1},
      InterlacedJumps[]  = {8, 8, 4, 2};

  // 打开文件并读入首块（giflib5：错误码经出参返回）
  if ((GifFile = DGifOpenFileName(imageio->fileName(), &gifErr)) == NULL) {
      gif_report_error("DGifOpenFileName", gifErr);
      return;
  }

  image.create(GifFile->SWidth, GifFile->SHeight, 8,
               GifFile->SColorMap ? GifFile->SColorMap->ColorCount : 256,
               QImage::BigEndian);

  // 逐行分配屏幕缓冲（GIF 逻辑画布，各帧按 Image.Left/Top 叠加）
  if ((ScreenBuffer = (GifRowType *)
       malloc(GifFile->SHeight * sizeof(GifRowType *))) == NULL)
      goto done;

  Size = GifFile->SWidth * sizeof(GifPixelType);
  if ((ScreenBuffer[0] = (GifRowType) malloc(Size)) == NULL)
      goto done;

  for (i = 0; i < GifFile->SWidth; i++)
      ScreenBuffer[0][i] = GifFile->SBackGroundColor;

  for (i = 1; i < GifFile->SHeight; i++) {
      if ((ScreenBuffer[i] = (GifRowType) malloc(Size)) == NULL)
          goto done;
      memcpy(ScreenBuffer[i], ScreenBuffer[0], Size);
  }

  // 扫描 GIF 记录流并解码
  do {
      if (DGifGetRecordType(GifFile, &RecordType) == GIF_ERROR) {
          gif_report_error("DGifGetRecordType", GifFile->Error);
          goto done;
      }
      switch (RecordType) {
      case IMAGE_DESC_RECORD_TYPE:
          if (DGifGetImageDesc(GifFile) == GIF_ERROR) {
              gif_report_error("DGifGetImageDesc", GifFile->Error);
              goto done;
          }
          Row = GifFile->Image.Top;
          Col = GifFile->Image.Left;
          Width = GifFile->Image.Width;
          Height = GifFile->Image.Height;
          if (GifFile->Image.Left + GifFile->Image.Width > GifFile->SWidth ||
              GifFile->Image.Top + GifFile->Image.Height > GifFile->SHeight) {
              fprintf(stderr, "kpaint GIF: image exceeds screen size\n");
              goto done;
          }
          if (GifFile->Image.Interlace) {
              // 交错存储按 4 遍跳行读取
              for (Count = i = 0; i < 4; i++)
                  for (j = Row + InterlacedOffset[i]; j < Row + Height;
                       j += InterlacedJumps[i]) {
                      if (DGifGetLine(GifFile, &ScreenBuffer[j][Col],
                                      Width) == GIF_ERROR) {
                          gif_report_error("DGifGetLine", GifFile->Error);
                          goto done;
                      }
                  }
          } else {
              for (i = 0; i < Height; i++) {
                  if (DGifGetLine(GifFile, &ScreenBuffer[Row++][Col],
                                  Width) == GIF_ERROR) {
                      gif_report_error("DGifGetLine", GifFile->Error);
                      goto done;
                  }
              }
          }
          break;
      case EXTENSION_RECORD_TYPE:
          // 跳过扩展块，仅提取 Graphic Control Ext 的透明色标志
          if (DGifGetExtension(GifFile, &ExtCode, &Extension) == GIF_ERROR) {
              gif_report_error("DGifGetExtension", GifFile->Error);
              goto done;
          }
          if (ExtCode == 249 && Extension && (Extension[1] & 1))
              trans = Extension[4];
          while (Extension != NULL) {
              if (DGifGetExtensionNext(GifFile, &Extension) == GIF_ERROR) {
                  gif_report_error("DGifGetExtensionNext", GifFile->Error);
                  goto done;
              }
              if (Extension != NULL && ExtCode == 249 && (Extension[1] & 1))
                  trans = Extension[4];
          }
          break;
      case TERMINATE_RECORD_TYPE:
          break;
      default:
          break;
      }
  } while (RecordType != TERMINATE_RECORD_TYPE);

  // 帧局部色表优先于全局色表
  Colourmap = (GifFile->Image.ColorMap ? GifFile->Image.ColorMap
                                       : GifFile->SColorMap);
  if (!Colourmap) {
      fprintf(stderr, "kpaint GIF: no colour map\n");
      goto done;
  }
  for (j = 0; j < Colourmap->ColorCount && j < image.numColors(); j++) {
      image.setColor(j, qRgb(Colourmap->Colors[j].Red,
                             Colourmap->Colors[j].Green,
                             Colourmap->Colors[j].Blue) | 0xff000000);
  }

  // 像素索引写入 QImage
  for (j = 0; j < GifFile->SHeight; j++) {
      uc_row = (unsigned char *) image.scanLine(j);
      for (i = 0; i < GifFile->SWidth; i++) {
          *uc_row++ = ScreenBuffer[j][i];
      }
  }
  if (trans != -1 && trans < image.numColors()) {
      image.setColor(trans, qRgb(Colourmap->Colors[trans].Red,
                                 Colourmap->Colors[trans].Green,
                                 Colourmap->Colors[trans].Blue) & 0x00ffffff);
      image.setAlphaBuffer(TRUE);
  }

  imageio->setImage(image);
  ok = true;

done:
  if (ok)
      imageio->setStatus(0);
  else
      imageio->setStatus(1);
  if (GifFile) {
      if (DGifCloseFile(GifFile, &gifErr) == GIF_ERROR)
          gif_report_error("DGifCloseFile", gifErr);
      if (ScreenBuffer)
          gif_free_screen(ScreenBuffer, GifFile->SHeight);
  }
}


void write_gif_file(QImageIO *imageio)
{
  int i, status;
  GifFileType *GifFile;
  ColorMapObject *screenColourmap;
  ColorMapObject *imageColourmap;
  int gifErr = 0;

  imageColourmap = GifMakeMapObject(256, NULL);
  screenColourmap = GifMakeMapObject(256, NULL);
  if (!imageColourmap || !screenColourmap) {
      fprintf(stderr, "kpaint GIF: could not create colour maps\n");
      if (imageColourmap) GifFreeMapObject(imageColourmap);
      if (screenColourmap) GifFreeMapObject(screenColourmap);
      return;
  }

  for (i = 0; i < 256; i++) {
      GifColorType *dst = &imageColourmap->Colors[i];
      if (i < imageio->image().numColors()) {
          dst->Red   = qRed(imageio->image().color(i));
          dst->Green = qGreen(imageio->image().color(i));
          dst->Blue  = qBlue(imageio->image().color(i));
      } else {
          dst->Red = dst->Green = dst->Blue = 0;
      }
  }
  memcpy(screenColourmap->Colors, imageColourmap->Colors,
         256 * sizeof(GifColorType));

  // giflib5：写模式标志后移为错误出参（WriteMode 恒真由 EGif 语义承担）
  if ((GifFile = EGifOpenFileName(imageio->fileName(), 1, &gifErr)) == NULL) {
      gif_report_error("EGifOpenFileName", gifErr);
      GifFreeMapObject(imageColourmap);
      GifFreeMapObject(screenColourmap);
      return;
  }

  status = EGifPutScreenDesc(GifFile,
                             imageio->image().width(),
                             imageio->image().height(),
                             256, 0, screenColourmap);
  if (status == GIF_ERROR) {
      gif_report_error("EGifPutScreenDesc", GifFile->Error);
      goto wfail;
  }

  status = EGifPutImageDesc(GifFile,
                            0, 0,
                            imageio->image().width(),
                            imageio->image().height(),
                            0, imageColourmap);
  if (status == GIF_ERROR) {
      gif_report_error("EGifPutImageDesc", GifFile->Error);
      goto wfail;
  }

  for (i = 0; i < imageio->image().height(); i++) {
      if (EGifPutLine(GifFile,
                      (GifPixelType *) imageio->image().scanLine(i),
                      imageio->image().bytesPerLine()) == GIF_ERROR) {
          gif_report_error("EGifPutLine", GifFile->Error);
          goto wfail;
      }
  }

  if (EGifCloseFile(GifFile, &gifErr) == GIF_ERROR) {
      gif_report_error("EGifCloseFile", gifErr);
      imageio->setStatus(1);
  } else {
      imageio->setStatus(0);
  }
  GifFreeMapObject(imageColourmap);
  GifFreeMapObject(screenColourmap);
  return;

wfail:
  EGifCloseFile(GifFile, &gifErr);
  GifFreeMapObject(imageColourmap);
  GifFreeMapObject(screenColourmap);
  imageio->setStatus(1);
}
