/* -*- C++ -*-
 * Source file for removing Nana macros and managing kab's debugging system.
 * Mirko Sucker, 1998.
 * $Id: debug.h,v 1.5 1998/12/22 21:45:59 mirko Exp $
 *
 * The following is provided to remove Nana-assertions from the
 * sourcecode. I strongly recommend using Nana if you are sear-
 * ching for bugs  because the assertions will catch mostly all
 * forbidden program states. Additionally the checks for object
 * invariants using the 
 *         bool invariant();
 * element function are used that watch the state of each part-
 * icular object. Nana is available at 
 *         http://wattle.cs.ntu.edu.au/homepages/pjm/nana-home/
 * For compiling final binaries  Nana  is no more needed due to 
 * this defines. The macro REMOVE_NANA should be defined in the
 * Makefile when compiling a final binary.
 */

#ifndef KAB_DEBUG_H
#define	KAB_DEBUG_H

// ##############################################################################
// include C headers: (some are included conditionally later)
extern "C" {
#include <stdio.h> // Nana needs this
#include <errno.h>
}
// C++ headers:
#include "stl_headers.h"
#include <kapp.h>

#if defined USE_NAMESPACES
using namespace std;
#endif

// ##############################################################################
// use version without Nana per default, define KAB_DEBUG to enable logging:
#ifndef KAB_DEBUG
#define REMOVE_NANA
#endif

#if defined REMOVE_NANA
// ------------------------------------------------------------------------------
#define EIFFEL_CHECK CHECK_NO 
#define L_LEVEL 0 
#define I_LEVEL 0
inline void L(...) {}
inline void LG(...) {}
#define CHECK(a)
#define I(a)
#define REQUIRE(a)
#define ENSURE(a)
// ------------------------------------------------------------------------------
#else 
// ------------------------------------------------------------------------------
#define EIFFEL_CHECK CHECK_ALL
#include <nana.h>
#include <eiffel.h> // currently, we use only Nanas logging support
#include <Qstl.h>
// ------------------------------------------------------------------------------
#endif // defined REMOVE_NANA
// ##############################################################################
// kab's (libkab's) own assertions, overriding some Nana keywords:
#ifdef assert
#undef assert
#endif
#ifdef CHECK
#undef CHECK
#endif
#ifdef REQUIRE
#undef REQUIRE
#endif
#ifdef ENSURE
#undef ENSURE
#endif
// ##############################################################################
// a static string that contains the authors email address, must be defined by 
// the application:
extern string AuthorEmailAddress;
// ##############################################################################
// we use our own kind of assertions here: colorful, cute and impressive bugs!
/* [KDE1 Revival 2026-08-31] 断言开关从 `!NDEBUG || DEBUG` 改为
   `defined(KAB_DEBUG)`：TQt3 的 ntqglobal.h 在非 QT_NO_DEBUG 构建里
   全局 #define DEBUG（TQt3 内部调试开关），与本文件的 kab 调试开关
   撞名——1999 年 Qt1 无此宏，迁移后 NDEBUG 因此永久失效，空库启动
   断言恒假导致 kab 每次启动都弹"发现了一个缺陷"对话框。改用本文件
   专属的 KAB_DEBUG 显式开启，生产构建默认关闭 */
#if defined(KAB_DEBUG)
#define assert(x) evaluate_assertion(x, __FILE__, __LINE__, #x)
#define CHECK(x)  evaluate_assertion(x, __FILE__, __LINE__, #x)
#define REQUIRE(x) evaluate_assertion(x, __FILE__, __LINE__, #x)
#define ENSURE(x) evaluate_assertion(x, __FILE__, __LINE__, #x)
#else
#define assert(x)
#define CHECK(x)
#define REQUIRE(x)
#define ENSURE(x)
#endif
// the function that pops up a dialog when an assertion failes (in libkab):
void evaluate_assertion(bool cond, const char* file, int line, const char* text);
// ##############################################################################

#endif // KAB_DEBUG_H











