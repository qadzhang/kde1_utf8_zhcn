#ifndef _DEVICES_H_
#define _DEVICES_H_

static const char *devices[] = {
#ifdef __FreeBSD__
  "/dev/cuaa0",
  "/dev/cuaa1",
  "/dev/cuaa2",
  "/dev/cuaa3",
#else
  /* [2026-08-31] 现代化设备清单：/dev/cua0-3 自内核 2.2 起废除、
     /dev/modem 符号链接在现代发行版默认不存在——以实际存在的
     ttyS*（传统串口）与 ttyUSB*（USB 串口适配器，现代外设主流）为主 */
  "/dev/ttyS0",
  "/dev/ttyS1",
  "/dev/ttyS2",
  "/dev/ttyS3",
  "/dev/ttyUSB0",
  "/dev/ttyUSB1",
  "/dev/ttyUSB2",
  "/dev/ttyUSB3",
  "/dev/modem",

#ifdef ISDNSUPPORT
  "/dev/ttyI0",
  "/dev/ttyI1",
  "/dev/ttyI2",
  "/dev/ttyI3",
#endif
#endif
  0};

// default device number from the list above
const int DEV_DEFAULT = 0;

#endif
