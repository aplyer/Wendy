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
#include <wendy/Rect.h>
#include <wendy/Path.h>
#include <wendy/Resource.h>
#include <wendy/Pixel.h>
#include <wendy/Image.h>

///////////////////////////////////////////////////////////////////////

namespace wendy
{
  namespace GL
  {

///////////////////////////////////////////////////////////////////////

class Texture;
class Context;

///////////////////////////////////////////////////////////////////////

/*! @brief %Texture sampler filtering mode enumeration.
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
  FILTER_TRILINEAR
};

///////////////////////////////////////////////////////////////////////

/*! @brief %Texture sampler address wrapping mode enumeration.
 *  @ingroup opengl
 */
enum AddressMode
{
  /*! Addresses are wrapped to within the texture.
   */
  ADDRESS_WRAP,
  /*! Addresses are clamped to the edges of the texture.
   */
  ADDRESS_CLAMP
};

///////////////////////////////////////////////////////////////////////

/*! @brief %Texture type enumeration.
 *  @ingroup opengl
 */
enum TextureType
{
  /*! %Texture has one-dimensional images with power-of-two dimensions.
   */
  TEXTURE_1D,
  /*! %Texture has two-dimensional images with power-of-two dimensions.
   */
  TEXTURE_2D,
  /*! %Texture has three-dimensional images with power-of-two dimensions.
   */
  TEXTURE_3D,
  /*! %Texture has a single two-dimensional image without power-of-two
   *  dimension restrictions.
   */
  TEXTURE_RECT,
  /*! %Texture has a cube of two-dimensional, square images with power-of-two
   *  dimensions.
   */
  TEXTURE_CUBE
};

///////////////////////////////////////////////////////////////////////

/*! @ingroup opengl
 */
enum CubeFace
{
  CUBE_POSITIVE_X,
  CUBE_NEGATIVE_X,
  CUBE_POSITIVE_Y,
  CUBE_NEGATIVE_Y,
  CUBE_POSITIVE_Z,
  CUBE_NEGATIVE_Z,
  NO_CUBE_FACE
};

///////////////////////////////////////////////////////////////////////

/*! @brief Texture creation parameters.
 *  @ingroup opengl
 */
class TextureParams
{
public:
  TextureParams(TextureType type);
  TextureType type;
  bool mipmapped;
  bool sRGB;
};

///////////////////////////////////////////////////////////////////////

/*! @brief %Texture image object.
 *  @ingroup opengl
 */
class TextureImage : public RefObject
{
  friend class Texture;
  friend class TextureFramebuffer;
public:
  /*! Updates an area within this texture image, at the specified coordinates
   *  and with a size matching the specified image, with the contents of that
   *  image.
   *  @param[in] source The image to copy pixel data from.
   *  @param[in] x The x-coordinate of the area within this image to update.
   *  @param[in] y The y-coordinate of the area within this image to update.
   *  @param[in] z The z-coordinate of the area within this image to update.
   */
  bool copyFrom(const Image& source, uint x = 0, uint y = 0, uint z = 0);
  /*! Returns a copy the contents of this texture image.
   *  @return An image object containing the image data.
   */
  Ref<Image> getData() const;
  uint getWidth() const;
  uint getHeight() const;
  uint getDepth() const;
  /*! @return The size, in bytes, of the data in this image.
   */
  size_t getSize() const;
  /*! @return The cube face this image represents, or @c NO_CUBE_FACE if this
   *  image is not part of a cube map.
   */
  CubeFace getFace() const;
  /*! @return The texture containing this texture image.
   */
  Texture& getTexture() const;
private:
  TextureImage(Texture& texture,
               uint level,
               uint width,
               uint height,
               uint depth,
               CubeFace face = NO_CUBE_FACE);
  void attach(int attachment, uint z);
  void detach(int attachment);
  Texture& texture;
  uint level;
  uint width;
  uint height;
  uint depth;
  CubeFace face;
};

///////////////////////////////////////////////////////////////////////

/*! @brief %Texture object.
 *  @ingroup opengl
 */
class Texture : public Resource
{
  friend class Context;
  friend class TextureImage;
public:
  /*! Destructor.
   */
  ~Texture();
  /*! Generates mipmaps based on the top-level image.
   */
  void generateMipmaps();
  /*! @return @c true if this texture is one-dimensional, otherwise @c false.
   */
  bool is1D() const;
  /*! @return @c true if this texture is two-dimensional, otherwise @c false.
   */
  bool is2D() const;
  /*! @return @c true if this texture is three-dimensional, otherwise @c false.
   */
  bool is3D() const;
  /*! @return @c true if this texture's dimensions are power of two, otherwise
   *  @c false.
   */
  bool isPOT() const;
  /*! @return @c true if this texture is a cubemap, otherwise @c false.
   */
  bool isCube() const;
  /*! @return @c true if this texture is mipmapped, otherwise @c false.
   */
  bool hasMipmaps() const;
  /*! @return The type of this texture.
   */
  TextureType getType() const;
  /*! @param[in] level The desired mipmap level.
   *  @return The width, in pixels, of the specified mipmap level of this texture.
   */
  uint getWidth(uint level = 0) const;
  /*! @param[in] level The desired mipmap level.
   *  @return The height, in pixels, of the specified mipmap level of this texture.
   */
  uint getHeight(uint level = 0) const;
  /*! @param[in] level The desired mipmap level.
   *  @return The depth, in pixels, of the specified mipmap level of this texture.
   */
  uint getDepth(uint level = 0) const;
  /*! @return The number of mipmap levels of this texture.
   */
  uint getLevelCount() const;
  /*! @param[in] level The desired mipmap level.
   *  @param[in] face The desired cube map face if this texture is a cubemap,
   *  or @c NO_CUBE_FACE otherwise.
   */
  TextureImage& getImage(uint level = 0, CubeFace face = NO_CUBE_FACE);
  /*! @param[in] level The desired mipmap level.
   *  @param[in] face The desired cube map face if this texture is a cubemap,
   *  or @c NO_CUBE_FACE otherwise.
   */
  const TextureImage& getImage(uint level = 0, CubeFace face = NO_CUBE_FACE) const;
  /*! @return The sampler filter mode of this texture.
   */
  FilterMode getFilterMode() const;
  /*! Sets the sampler filter mode of this texture.
   *  @param[in] newMode The desired filter mode.
   */
  void setFilterMode(FilterMode newMode);
  /*! @return The sampler address wrapping mode of this texture.
   */
  AddressMode getAddressMode() const;
  /*! Sets the sampler address wrapping mode of this texture.
   *  @param[in] newMode The desired address wrapping mode.
   */
  void setAddressMode(AddressMode newMode);
  /*! @return The maximum anisotropy of this texture.
   */
  float getMaxAnisotropy() const;
  /*! Sets the maximum anisotropy of this texture.
   *  @param[in] newMax The desired value.
   */
  void setMaxAnisotropy(float newMax);
  /*! @return The image format of this texture.
   */
  const PixelFormat& getFormat() const;
  /*! @return The size, in bytes, of the data in all images of this texture.
   */
  size_t getSize() const;
  /*! @return The context used to create this texture.
   */
  Context& getContext() const;
  /*! Creates a texture from the specified image.
   *  @param[in] context The OpenGL context within which to create the
   *  texture.
   *  @param[in] params The creation parameters for the texture.
   *  @param[in] data The image data to use.
   *  @return The newly created texture object.
   */
  static Ref<Texture> create(const ResourceInfo& info,
                             Context &context,
                             const TextureParams& params,
                             const Image& data);
  static Ref<Texture> read(Context& context,
                           const TextureParams& params,
                           const String& imageName);
private:
  Texture(const ResourceInfo& info, Context& context);
  Texture(const Texture& source);
  bool init(const TextureParams& params, const Image& data);
  void retrieveImages();
  uint retrieveTargetImages(uint target, CubeFace face);
  void applyDefaults();
  Texture& operator = (const Texture& source);
  Context& context;
  TextureType type;
  uint textureID;
  uint levels;
  FilterMode filterMode;
  AddressMode addressMode;
  float maxAnisotropy;
  PixelFormat format;
  std::vector<Ref<TextureImage>> images;
};

///////////////////////////////////////////////////////////////////////

/*! @ingroup opengl
 */
typedef std::vector<Ref<Texture>> TextureList;

///////////////////////////////////////////////////////////////////////

  } /*namespace GL*/
} /*namespace wendy*/

///////////////////////////////////////////////////////////////////////
#endif /*WENDY_GLTEXTURE_H*/
///////////////////////////////////////////////////////////////////////
