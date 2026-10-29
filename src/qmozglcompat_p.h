/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

// Desktop OpenGL headers lack the GLES names used to import EGL images.
#ifndef QMOZGLCOMPAT_P_H
#define QMOZGLCOMPAT_P_H

#include <qopengl.h>

#ifndef GL_TEXTURE_EXTERNAL_OES
#define GL_TEXTURE_EXTERNAL_OES 0x8D65
#endif

#if !defined(QT_OPENGL_ES_2) && !defined(GL_OES_EGL_image)
typedef void *GLeglImageOES;
typedef void (*PFNGLEGLIMAGETARGETTEXTURE2DOESPROC)(GLenum target, GLeglImageOES image);
#endif

#endif
