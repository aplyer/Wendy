///////////////////////////////////////////////////////////////////////
// Wendy OpenGL library
// Copyright (c) 2010 Camilla Berglund <elmindreda@elmindreda.org>
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
#ifndef WENDY_OPENGL_H
#define WENDY_OPENGL_H
///////////////////////////////////////////////////////////////////////

#include <wendy/Core.h>
#include <wendy/Pixel.h>

///////////////////////////////////////////////////////////////////////

namespace wendy
{
  namespace GL
  {

///////////////////////////////////////////////////////////////////////

/*! @brief Primitive type enumeration.
 *  @ingroup opengl
 */
enum PrimitiveType
{
  POINT_LIST,
  LINE_LIST,
  LINE_STRIP,
  LINE_LOOP,
  TRIANGLE_LIST,
  TRIANGLE_STRIP,
  TRIANGLE_FAN,
};

///////////////////////////////////////////////////////////////////////

bool checkGL(const char* format, ...);

///////////////////////////////////////////////////////////////////////

/*! @ingoup opengl
 */
class Image : public RefObject
{
  friend class ImageCanvas;
public:
  virtual ~Image(void);
  virtual unsigned int getWidth(void) const = 0;
  virtual unsigned int getHeight(void) const = 0;
  virtual const PixelFormat& getFormat(void) const = 0;
protected:
  virtual void attach(int attachment) = 0;
  virtual void detach(int attachment) = 0;
};

///////////////////////////////////////////////////////////////////////

/*! @ingroup opengl
 */
typedef Ref<Image> ImageRef;

///////////////////////////////////////////////////////////////////////

  } /*namespace GL*/
} /*namespace wendy*/

///////////////////////////////////////////////////////////////////////
#endif /*WENDY_OPENGL_H*/
///////////////////////////////////////////////////////////////////////
