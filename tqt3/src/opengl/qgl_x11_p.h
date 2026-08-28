/****************************************************************************
**
** Definitions needed for resolving GL/GLX symbols using dlopen()
** under X11.
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of the OpenGL module of the TQt GUI Toolkit.
**
** This file may be used under the terms of the GNU General
** Public License versions 2.0 or 3.0 as published by the Free
** Software Foundation and appearing in the files LICENSE.GPL2
** and LICENSE.GPL3 included in the packaging of this file.
** Alternatively you may (at your option) use any later version
** of the GNU General Public License if such license has been
** publicly approved by Trolltech ASA (or its successors, if any)
** and the KDE Free TQt Foundation.
**
** Please review the following information to ensure GNU General
** Public Licensing requirements will be met:
** http://trolltech.com/products/qt/licenses/licensing/opensource/.
** If you are unsure which license is appropriate for your use, please
** review the following information:
** http://trolltech.com/products/qt/licenses/licensing/licensingoverview
** or contact the sales department at sales@trolltech.com.
**
** Licensees holding valid TQt Commercial licenses may use this file in
** accordance with the TQt Commercial License Agreement provided with
** the Software.
**
** This file is provided "AS IS" with NO WARRANTY OF ANY KIND,
** INCLUDING THE WARRANTIES OF DESIGN, MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE. Trolltech reserves all rights not granted
** herein.
**
**********************************************************************/

#ifndef TQGL_P_H
#define TQGL_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the TQt API.  This header file may
// change from version to version without notice, or even be
// removed.
//
// We mean it.
//
//

#ifdef QT_DLOPEN_OPENGL
// resolve the GL symbols we use ourselves
bool tqt_resolve_gl_symbols(bool = true);
extern "C" {
// GL symbols
typedef void (*_glCallLists)( GLsizei n, GLenum type, const GLvoid *lists );
typedef void (*_glClearColor)( GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha );
typedef void (*_glClearIndex)( GLfloat c );
typedef void (*_glColor3ub)( GLubyte red, GLubyte green, GLubyte blue );
typedef void (*_glDeleteLists)( GLuint list, GLsizei range );
typedef void (*_glDrawBuffer)( GLenum mode );
typedef void (*_glFlush)( void );
typedef void (*_glIndexi)( GLint c );
typedef void (*_glListBase)( GLuint base );
typedef void (*_glLoadIdentity)( void );
typedef void (*_glMatrixMode)( GLenum mode );
typedef void (*_glOrtho)( GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble near_val, GLdouble far_val );
typedef void (*_glPopAttrib)( void );
typedef void (*_glPopMatrix)( void );
typedef void (*_glPushAttrib)( GLbitfield mask );
typedef void (*_glPushMatrix)( void );
typedef void (*_glRasterPos2i)( GLint x, GLint y );
typedef void (*_glRasterPos3d)( GLdouble x, GLdouble y, GLdouble z );
typedef void (*_glReadPixels)( GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid *pixels );
typedef void (*_glViewport)( GLint x, GLint y, GLsizei width, GLsizei height );
typedef void (*_glPixelStorei)( GLenum pname, GLint param );
typedef void (*_glBitmap)( GLsizei width, GLsizei height, GLfloat xorig, GLfloat yorig, GLfloat xmove, GLfloat ymove,
			   const GLubyte *bitmap );
typedef void (*_glDrawPixels)( GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *pixels );
typedef void (*_glNewList)( GLuint list, GLenum mode );
typedef void (*_glGetFloatv)( GLenum pname, GLfloat *params );
typedef void (*_glGetIntegerv)( GLenum pname, GLint *params );
typedef void (*_glEndList)( void );


// GLX symbols - should be in the GL lib as well
typedef XVisualInfo* (*_glXChooseVisual)(Display *dpy, int screen, int *attribList);
typedef GLXContext (*_glXCreateContext)(Display *dpy, XVisualInfo *vis, GLXContext shareList, Bool direct);
typedef GLXPixmap (*_glXCreateGLXPixmap)(Display *dpy, XVisualInfo *vis, Pixmap pixmap);
typedef void (*_glXDestroyContext)(Display *dpy, GLXContext ctx);
typedef void (*_glXDestroyGLXPixmap)(Display *dpy, GLXPixmap pix);
typedef const char* (*_glXGetClientString)(Display *dpy, int name );
typedef int (*_glXGetConfig)(Display *dpy, XVisualInfo *vis, int attrib, int *value);
typedef Bool (*_glXIsDirect)(Display *dpy, GLXContext ctx);
typedef Bool (*_glXMakeCurrent)(Display *dpy, GLXDrawable drawable, GLXContext ctx);
typedef Bool (*_glXQueryExtension)(Display *dpy, int *errorBase, int *eventBase);
typedef const char* (*_glXQueryExtensionsString)(Display *dpy, int screen);
typedef const char* (*_glXQueryServerString)(Display *dpy, int screen, int name);
typedef void (*_glXSwapBuffers)(Display *dpy, GLXDrawable drawable);
typedef void (*_glXUseXFont)(Font font, int first, int count, int listBase);
typedef void (*_glXWaitX)(void);

extern _glCallLists tqt_glCallLists;
extern _glClearColor tqt_glClearColor;
extern _glClearIndex tqt_glClearIndex;
extern _glColor3ub tqt_glColor3ub;
extern _glDeleteLists tqt_glDeleteLists;
extern _glDrawBuffer tqt_glDrawBuffer;
extern _glFlush tqt_glFlush;
extern _glIndexi tqt_glIndexi;
extern _glListBase tqt_glListBase;
extern _glLoadIdentity tqt_glLoadIdentity;
extern _glMatrixMode tqt_glMatrixMode;
extern _glOrtho tqt_glOrtho;
extern _glPopAttrib tqt_glPopAttrib;
extern _glPopMatrix tqt_glPopMatrix;
extern _glPushAttrib tqt_glPushAttrib;
extern _glPushMatrix tqt_glPushMatrix;
extern _glRasterPos2i tqt_glRasterPos2i;
extern _glRasterPos3d tqt_glRasterPos3d;
extern _glReadPixels tqt_glReadPixels;
extern _glViewport tqt_glViewport;
extern _glPixelStorei tqt_glPixelStorei;
extern _glBitmap tqt_glBitmap;
extern _glDrawPixels tqt_glDrawPixels;
extern _glNewList tqt_glNewList;
extern _glGetFloatv tqt_glGetFloatv;
extern _glGetIntegerv tqt_glGetIntegerv;
extern _glEndList tqt_glEndList;

extern _glXChooseVisual tqt_glXChooseVisual;
extern _glXCreateContext tqt_glXCreateContext;
extern _glXCreateGLXPixmap tqt_glXCreateGLXPixmap;
extern _glXDestroyContext tqt_glXDestroyContext;
extern _glXDestroyGLXPixmap tqt_glXDestroyGLXPixmap;
extern _glXGetClientString tqt_glXGetClientString;
extern _glXGetConfig tqt_glXGetConfig;
extern _glXIsDirect tqt_glXIsDirect;
extern _glXMakeCurrent tqt_glXMakeCurrent;
extern _glXQueryExtension tqt_glXQueryExtension;
extern _glXQueryExtensionsString tqt_glXQueryExtensionsString;
extern _glXQueryServerString tqt_glXQueryServerString;
extern _glXSwapBuffers tqt_glXSwapBuffers;
extern _glXUseXFont tqt_glXUseXFont;
extern _glXWaitX tqt_glXWaitX;
}; // extern "C"

#define glCallLists tqt_glCallLists
#define glClearColor tqt_glClearColor
#define glClearIndex tqt_glClearIndex
#define glColor3ub tqt_glColor3ub
#define glDeleteLists tqt_glDeleteLists
#define glDrawBuffer tqt_glDrawBuffer
#define glFlush tqt_glFlush
#define glIndexi tqt_glIndexi
#define glListBase tqt_glListBase
#define glLoadIdentity tqt_glLoadIdentity
#define glMatrixMode tqt_glMatrixMode
#define glOrtho tqt_glOrtho
#define glPopAttrib tqt_glPopAttrib
#define glPopMatrix tqt_glPopMatrix
#define glPushAttrib tqt_glPushAttrib
#define glPushMatrix tqt_glPushMatrix
#define glRasterPos2i tqt_glRasterPos2i
#define glRasterPos3d tqt_glRasterPos3d
#define glReadPixels tqt_glReadPixels
#define glViewport tqt_glViewport
#define glPixelStorei tqt_glPixelStorei
#define glBitmap tqt_glBitmap
#define glDrawPixels tqt_glDrawPixels
#define glNewList tqt_glNewList
#define glGetFloatv tqt_glGetFloatv
#define glGetIntegerv tqt_glGetIntegerv
#define glEndList tqt_glEndList

#define glXChooseVisual tqt_glXChooseVisual
#define glXCreateContext tqt_glXCreateContext
#define glXCreateGLXPixmap tqt_glXCreateGLXPixmap
#define glXDestroyContext tqt_glXDestroyContext
#define glXDestroyGLXPixmap tqt_glXDestroyGLXPixmap
#define glXGetClientString tqt_glXGetClientString
#define glXGetConfig tqt_glXGetConfig
#define glXIsDirect tqt_glXIsDirect
#define glXMakeCurrent tqt_glXMakeCurrent
#define glXQueryExtension tqt_glXQueryExtension
#define glXQueryExtensionsString tqt_glXQueryExtensionsString
#define glXQueryServerString tqt_glXQueryServerString
#define glXSwapBuffers tqt_glXSwapBuffers
#define glXUseXFont tqt_glXUseXFont
#define glXWaitX tqt_glXWaitX

#else
inline bool tqt_resolve_gl_symbols(bool = true) { return true; }
#endif // QT_DLOPEN_OPENGL
#endif // TQGL_P_H
