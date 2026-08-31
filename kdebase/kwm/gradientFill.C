//   Modified for the KDE1 Revival Project, 2026
//   Maintainer: <维护者姓名> <邮箱>
//   Modifications written with GLM-5.3 (Z.ai)
//     （渐变生成由 Qt1 字节级写入改为 qRgb 值级写入，修复 TQt3 下红蓝对调）

#include "gradientFill.h"
#include <dither.h>
#include <qimage.h>
#include <stdio.h>

void kwm_gradientFill(KPixmap &pm, QColor ca, QColor cb, bool upDown) {
  if(upDown == FALSE && QColor::numBitPlanes() >= 15) {    
    int w = pm.width();
    int h = pm.height();
    
    int c_red_a = ca.red() << 16;
    int c_green_a = ca.green() << 16;
    int c_blue_a = ca.blue() << 16;

    int c_red_b = cb.red() << 16;
    int c_green_b = cb.green() << 16;
    int c_blue_b = cb.blue() << 16;
    
    int d_red = (c_red_b - c_red_a) / w;
    int d_green = (c_green_b - c_green_a) / w;
    int d_blue = (c_blue_b - c_blue_a) / w;

    QImage img(w, h, 32);

    // ┌─ What : 逐列计算渐变色并以 qRgb() 值级写入 32bpp 图像行缓冲
    // │  Why  : Qt1 时代按 R,G,B,X 逐字节写入 little-endian 分支的字节流，在
    // │         TQt3 下是红蓝对调（TQt3 32bpp 内存序为 B,G,R,A，QRgb=0xFFRRGGBB）
    // │         ——蓝白标题栏变红黑的根因；值级写入不依赖字节序，两代语义一致
    // │  When : kwm 标题条 H_SHADED/V_SHADED 装饰构造期（client.C 两处调用）
    // │  How  : 固定点 16.16 累加通道值 → 右移 16 取 8 位 → qRgb(r,g,b) 组合
    // │         alpha 0xff 由 qRgb 自动置位（convertFromImage 按 0xFFRRGGBB 解读）
    uint *p = (uint *)img.scanLine(0);

    int r = c_red_a, g = c_green_a, b = c_blue_a;

    for(int x = 0; x < w; x++) {
      *p++ = qRgb(r >> 16, g >> 16, b >> 16);

      r += d_red;
      g += d_green;
      b += d_blue;
    }

    uchar *src = img.scanLine(0);
    for(int y = 1; y < h; y++)
      memcpy(img.scanLine(y), src, 4*w);

    pm.convertFromImage(img);
  } else
    pm.gradientFill(ca, cb, upDown);
}
