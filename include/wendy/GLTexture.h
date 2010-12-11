///////////////////////////////////////////////////////////////////////
// Wendy OpenGL library
// Copyright (c) 2004 Camilla Berglund <elmindreda@elmindreda.org>
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
#ifndef WENDY_GLTEXTURE_H
#define WENDY_GLTEXTURE_H
///////////////////////////////////////////////////////////////////////

#include <wendy/Core.h>
#include <wendy/Block.h>
#include <wendy/Vector.h>
#include <wendy/Rectangle.h>
#include <wendy/Path.h>
#include <wendy/Resource.h>
#include <wendy/Pixel.h>
#include <wendy/XML.h>
#include <wendy/Image.h>

///////////////////////////////////////////////////////////////////////

namespace wendy
{
  namespace GL
  {

///////////////////////////////////////////////////////////////////////

class Image;
class Texture;
class Context;

///////////////////////////////////////////////////////////////////////

/*! @brief Texture sampler filtering mode.
 *  @ingroup opengl
 */
enum FilterMode
{
  /*! Select the nearest texel of the nearest mipmap level.
   */
  FILTER_NEAREST,
  /*! Select the four nearest texels of the nearest mipmap level and
   *  interpolate.
   */
  FILTER_BILINEAR,
  /*! Select the four nearest texels of the two nearest mipmap levels and
   *  interpolate.
   */
  FILTER_TRILINEAR,
};

///////////////////////////////////////////////////////////////////////

/*! @brief Texture sampler address wrapping mode.
 *  @ingroup opengl
 */
enum AddressMode
{
  /*! Addresses are wrapped to within the texture.
   */
  ADDRESS_WRAP,
  /*! Addresses are clamped to the edges of the texture.
   */
  ADDRESS_CLAMP,
};

///////////////////////////////////////////////////////////////////////

/*! @brief %Texture image object.
 *  @ingroup opengl
 */
class TextureImage : public Image
{
  friend class Texture;
public:
  /*! Updates the area within this texture image, at the specified coordinates
   *  and with a size matching the specified image, with the contents of that
   *  image.
   *  @param[in] source The image to copy pixel data from.
   *  @param[in] x The x-coordinate of the area within this image to update.
   *  @param[in] y The y-coordinate of the area within this image to update.
   */
  bool copyFrom(const wendy::Image& source, unsigned int x, unsigned int y);
  /*! Updates this texture image with the contents of the current color buffer
   *  at the specified coordinates.
   *  @param[in] x The x-coordinate of the desired area within the current color buffer.
   *  @param[in] y The y-coordinate of the desired area within the current color buffer.
   */
  bool copyFromColorBuffer(unsigned int x, unsigned int y);
  bool copyTo(wendy::Image& result) const;
  unsigned int getWidth(void) const;
  unsigned int getHeight(void) const;
  const PixelFormat& getFormat(void) const;
  /*! @return The texture containing this texture image.
   */
  Texture& getTexture(void) const;
private:
  TextureImage(Texture& texture, unsigned int level, unsigned int width, unsigned int height);
  void attach(int attachment);
  void detach(int attachment);
  Texture& texture;
  unsigned int level;
  unsigned int width;
  unsigned int height;
};

///////////////////////////////////////////////////////////////////////

typedef Ref<TextureImage> TextureImageRef;

///////////////////////////////////////////////////////////////////////

/*! @brief %Texture object.
 *  @ingroup opengl
 */
class Texture : public Resource
{
  friend class Sampler;
  friend class TextureImage;
public:
  enum
  {
    /*! The texture may have non-POT dimensions and will be created with the
     *  original image dimensions or as close to them as possible.  Note that
     *  textures created with this flag have an address range of [0..w] and
     *  [0..h], not [0..1], whether or not they have POT dimensions.
     */
    RECTANGULAR = 1,
    /*! The texture will be created with a mipmap chain.
     */
    MIPMAPPED = 2,
    /*! The default texture creation flags.
     */
    DEFAULT = MIPMAPPED,
  };
  /*! Destructor.
   */
  ~Texture(void);
  /*! @return @c true if this texture's dimensions are power of two, otherwise
   *  @c false.
   */
  bool isPOT(void) const;
  /*! @return @c true if this texture is mipmapped, otherwise @c false.
   */
  bool isMipmapped(void) const;
  /*! @return @c true if this texture is a cube map, otherwise @c false.
   */
  bool isCubeMap(void) const;
  /*! @param[in] level The desired mipmap level.
   *  @return The width, in pixels, of the specified mipmap level of this texture.
   */
  unsigned int getWidth(unsigned int level = 0);
  /*! @param[in] level The desired mipmap level.
   *  @return The height, in pixels, of the specified mipmap level of this texture.
   */
  unsigned int getHeight(unsigned int level = 0);
  /*! @return The width, in pixels, of the source image.
   */
  unsigned int getSourceWidth(void) const;
  /*! @return The height, in pixels, of the source image.
   */
  unsigned int getSourceHeight(void) const;
  /*! @return The number of mipmap levels in this texture.
   */
  unsigned int getImageCount(void) const;
  /*! @param[in] level The desired mipmap level.
   *  @return The specified mipmap level.
   */
  TextureImage& getImage(unsigned int level = 0);
  /*! @return The sampler filter mode of this texture.
   */
  FilterMode getFilterMode(void) const;
  /*! Sets the sampler filter mode of this texture.
   *  @param[in] newMode The desired filter mode.
   */
  void setFilterMode(FilterMode newMode);
  /*! @return The sampler address wrapping mode of this texture.
   */
  AddressMode getAddressMode(void) const;
  /*! Sets the sampler address wrapping mode of this texture.
   *  @param[in] newMode The desired address wrapping mode.
   */
  void setAddressMode(AddressMode newMode);
  /*! @return The image format of this texture.
   */
  const PixelFormat& getFormat(void) const;
  /*! @return The context used to create this texture.
   */
  Context& getContext(void) const;
  /*! Creates a texture from the specified image.
   *  @param[in] info The resource info for the texture.
   *  $param[in] context The OpenGL context within which to create the
   *  texture.
   *  @param[in] image The image data to use.
   *  @param[in] flags The creation flags.
   */
  static Ref<Texture> create(const ResourceInfo& info,
                             Context& context,
                             const wendy::Image& source,
                             unsigned int flags);
  /*! Creates a texture from the specified image cube.
   *  @param[in] info The resource info for the texture.
   *  $param[in] context The OpenGL context within which to create the
   *  texture.
   *  @param[in] image The image cube data to use.
   *  @param[in] flags The creation flags.
   */
  static Ref<Texture> create(const ResourceInfo& info,
                             Context& context,
                             const ImageCube& source,
                             unsigned int flags);
  /*! Creates a texture using the specified texture specification file.
   *  @param[in] index The resource index within which to create the texture.
   *  @param[in] path The path of the texture specification file to use.
   */
  static Ref<Texture> read(Context& context, const Path& path);
private:
  Texture(const ResourceInfo& info, Context& context);
  Texture(const Texture& source);
  bool init(const wendy::Image& source, unsigned int flags);
  bool init(const ImageCube& source, unsigned int flags);
  Texture& operator = (const Texture& source);
  typedef std::vector<TextureImageRef> ImageList;
  Context& context;
  unsigned int textureTarget;
  unsigned int textureID;
  unsigned int sourceWidth;
  unsigned int sourceHeight;
  unsigned int flags;
  FilterMode filterMode;
  AddressMode addressMode;
  PixelFormat format;
  ImageList images;
};

///////////////////////////////////////////////////////////////////////

typedef Ref<Texture> TextureRef;

///////////////////////////////////////////////////////////////////////

class TextureReader : public ResourceReader, public XML::Reader
{
public:
  TextureReader(Context& context);
  Ref<Texture> read(const Path& path);
private:
  bool onBeginElement(const String& name);
  bool onEndElement(const String& name);
  Context& context;
  Ref<Texture> texture;
  ResourceInfo info;
};

///////////////////////////////////////////////////////////////////////

  } /*namespace GL*/
} /*namespace wendy*/

///////////////////////////////////////////////////////////////////////
#endif /*WENDY_GLTEXTURE_H*/
///////////////////////////////////////////////////////////////////////
