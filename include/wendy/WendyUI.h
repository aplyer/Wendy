///////////////////////////////////////////////////////////////////////
// Wendy user interface library
// Copyright (c) 2011 Camilla Berglund <elmindreda@elmindreda.org>
//
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any
// damages arising from the use of this software.
//
// Permission is granted to anyone to use this software for any
// purpose, including commercial applications, and to alter it and
// redistribute it freely, subject to the following restrictions:
//
//  1. The origin of this software must not be misrepresented; you
//     must not claim that you wrote the original software. If you use
//     this software in a product, an acknowledgment in the product
//     documentation would be appreciated but is not required.
//
//  2. Altered source versions must be plainly marked as such, and
//     must not be misrepresented as being the original software.
//
//  3. This notice may not be removed or altered from any source
//     distribution.
//
///////////////////////////////////////////////////////////////////////
#ifndef WENDY_WENDYUI_H
#define WENDY_WENDYUI_H
///////////////////////////////////////////////////////////////////////

/*! @defgroup ui User interface API
 *
 *  These classes provide a simple 2D graphical user interface (GUI). The
 *  drawing is layered on top of the OpenGL renderer and using the input
 *  functionality of the context singleton, so the environment must be set
 *  up before the user interface can be drawn.
 *
 *  The user interface classes make heavy use of signals, and its design is in
 *  many ways similar to the gtkmm library. One notable difference is that,
 *  since we are working on top of OpenGL, usually together with 3D rendering,
 *  we need to redraw the entire interface each frame.
 */

///////////////////////////////////////////////////////////////////////

#if WENDY_INCLUDE_UI_SYSTEM

#include <wendy/UIDrawer.h>
#include <wendy/UILayer.h>
#include <wendy/UIWidget.h>
#include <wendy/UIScroller.h>
#include <wendy/UIBook.h>
#include <wendy/UICanvas.h>
#include <wendy/UILayout.h>
#include <wendy/UILabel.h>
#include <wendy/UIProgress.h>
#include <wendy/UIButton.h>
#include <wendy/UISlider.h>
#include <wendy/UIEntry.h>
#include <wendy/UIItem.h>
#include <wendy/UIList.h>
#include <wendy/UIMenu.h>
#include <wendy/UIPopup.h>

#else
#error "UI module not enabled"
#endif

///////////////////////////////////////////////////////////////////////
#endif /*WENDY_WENDYUI_H*/
///////////////////////////////////////////////////////////////////////