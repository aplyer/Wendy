///////////////////////////////////////////////////////////////////////
// Wendy core library
// Copyright (c) 2006 Camilla Berglund <elmindreda@elmindreda.org>
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
#ifndef WENDY_FONT_H
#define WENDY_FONT_H
///////////////////////////////////////////////////////////////////////

#include <vector>

///////////////////////////////////////////////////////////////////////

namespace wendy
{

///////////////////////////////////////////////////////////////////////

class Stream;
class Image;

///////////////////////////////////////////////////////////////////////

class FontGlyph
{
public:
  Vec2 bearing;
  float advance;
  Ref<Image> image;
};

///////////////////////////////////////////////////////////////////////

class Font : public Resource
{
public:
  typedef std::vector<FontGlyph> GlyphList;
  Font(const ResourceInfo& info);
  Font(const Font& source);
  Font& operator = (const Font& source);
  GlyphList glyphs;
  FontGlyph* characters[256];
};

///////////////////////////////////////////////////////////////////////

class FontReader : public ResourceReader, public XML::Reader
{
public:
  FontReader(ResourceIndex& index);
  Ref<Font> read(const Path& path);
private:
  bool extractGlyphs(const Image& image, const String& characters);
  bool onBeginElement(const String& name);
  bool onEndElement(const String& name);
  Ptr<Font> font;
};

///////////////////////////////////////////////////////////////////////

} /*namespace wendy*/

///////////////////////////////////////////////////////////////////////
#endif /*WENDY_FONT_H*/
///////////////////////////////////////////////////////////////////////
