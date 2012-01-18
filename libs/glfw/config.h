//========================================================================
// GLFW - An OpenGL library
// Platform:    Any
// API version: 3.0
// WWW:         http://www.glfw.org/
//------------------------------------------------------------------------
// Copyright (c) 2010 Camilla Berglund <elmindreda@elmindreda.org>
//
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would
//    be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such, and must not
//    be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source
//    distribution.
//
//========================================================================
// As config.h.in, this file is used by CMake to produce the config.h shared
// configuration header file.  If you are adding a feature requiring
// conditional compilation, this is the proper place to add the macros.
//========================================================================
// As config.h, this file defines compile-time build options and macros for
// all platforms supported by GLFW.  As this is a generated file, don't modify
// it.  Instead, you should modify the config.h.in file.
//========================================================================

// Define this to 1 if building GLFW for X11/GLX
#define _GLFW_X11_GLX
// Define this to 1 if building GLFW for Win32/WGL
/* #undef _GLFW_WIN32_WGL */
// Define this to 1 if building GLFW for Cocoa/NSOpenGL
/* #undef _GLFW_COCOA_NSGL */

// Define this to 1 if XRandR is available
#define _GLFW_HAS_XRANDR      1
// Define this to 1 if Xf86VidMode is available
#define _GLFW_HAS_XF86VIDMODE 1

// Define this to 1 if Xkb is available
#define _GLFW_HAS_XKB         1

// Define this to 1 if glXGetProcAddress is available
#define _GLFW_HAS_GLXGETPROCADDRESS    1
// Define this to 1 if glXGetProcAddressARB is available
/* #undef _GLFW_HAS_GLXGETPROCADDRESSARB */
// Define this to 1 if glXGetProcAddressEXT is available
/* #undef _GLFW_HAS_GLXGETPROCADDRESSEXT */

// Define this to 1 if the Linux joystick API is available
#define _GLFW_USE_LINUX_JOYSTICKS 1

// Define this to 1 to not load gdi32.dll dynamically
/* #undef _GLFW_NO_DLOAD_GDI32 */
// Define this to 1 to not load winmm.dll dynamically
/* #undef _GLFW_NO_DLOAD_WINMM */

// The GLFW version as used by glfwGetVersionString
#define _GLFW_VERSION_FULL "3.0.0"

