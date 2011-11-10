///////////////////////////////////////////////////////////////////////
// Wendy OpenGL library
// Copyright (c) 2005 Camilla Berglund <elmindreda@elmindreda.org>
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

#include <wendy/Config.h>

#include <wendy/Bimap.h>

#include <wendy/OpenGL.h>
#include <wendy/GLBuffer.h>
#include <wendy/GLTexture.h>
#include <wendy/GLProgram.h>
#include <wendy/GLContext.h>

#define GLEW_STATIC
#include <GL/glew.h>

#include <internal/GLConvert.h>

///////////////////////////////////////////////////////////////////////

namespace wendy
{
  namespace GL
  {

///////////////////////////////////////////////////////////////////////

namespace
{

GLenum convertToGL(LockType type)
{
  switch (type)
  {
    case LOCK_READ_ONLY:
      return GL_READ_ONLY;
    case LOCK_WRITE_ONLY:
      return GL_WRITE_ONLY;
    case LOCK_READ_WRITE:
      return GL_READ_WRITE;
  }

  logError("Invalid lock type %u", type);
  return 0;
}

GLenum convertToGL(IndexBuffer::Usage usage)
{
  switch (usage)
  {
    case IndexBuffer::STATIC:
      return GL_STATIC_DRAW;
    case IndexBuffer::STREAM:
      return GL_STREAM_DRAW;
    case IndexBuffer::DYNAMIC:
      return GL_DYNAMIC_DRAW;
  }

  logError("Invalid index buffer usage %u", usage);
  return 0;
}

GLenum convertToGL(VertexBuffer::Usage usage)
{
  switch (usage)
  {
    case VertexBuffer::STATIC:
      return GL_STATIC_DRAW;
    case VertexBuffer::STREAM:
      return GL_STREAM_DRAW;
    case VertexBuffer::DYNAMIC:
      return GL_DYNAMIC_DRAW;
  }

  logError("Invalid vertex buffer usage %u", usage);
  return 0;
}

} /*namespace*/

///////////////////////////////////////////////////////////////////////

VertexBuffer::~VertexBuffer()
{
  if (locked)
    logWarning("Vertex buffer destroyed while locked");

  if (bufferID)
    glDeleteBuffers(1, &bufferID);
}

void* VertexBuffer::lock(LockType type)
{
  if (locked)
  {
    logError("Vertex buffer already locked");
    return NULL;
  }

  context.setCurrentVertexBuffer(this);

  void* mapping = glMapBuffer(GL_ARRAY_BUFFER, convertToGL(type));
  if (mapping == NULL)
  {
    logError("Failed to lock vertex buffer: %s",
             gluErrorString(glGetError()));
    return NULL;
  }

  locked = true;
  return mapping;
}

void VertexBuffer::unlock()
{
  if (!locked)
  {
    logWarning("Cannot unlock non-locked vertex buffer");
    return;
  }

  context.setCurrentVertexBuffer(this);

  if (!glUnmapBuffer(GL_ARRAY_BUFFER))
    logWarning("Data for vertex buffer was corrupted");

  locked = false;
}

void VertexBuffer::copyFrom(const void* source, unsigned int sourceCount, unsigned int start)
{
  if (locked)
  {
    logError("Cannot copy data into locked vertex buffer");
    return;
  }

  if (start + sourceCount > count)
  {
    logError("Too many vertices submitted to vertex buffer");
    return;
  }

  context.setCurrentVertexBuffer(this);

  const size_t size = format.getSize();
  glBufferSubData(GL_ARRAY_BUFFER, start * size, sourceCount * size, source);

#if WENDY_DEBUG
  checkGL("Error during copy to vertex buffer");
#endif
}

void VertexBuffer::copyTo(void* target, unsigned int targetCount, unsigned int start)
{
  if (locked)
  {
    logError("Cannot copy data from locked vertex buffer");
    return;
  }

  if (start + targetCount > count)
  {
    logError("Too many vertices requested from vertex buffer");
    return;
  }

  context.setCurrentVertexBuffer(this);

  const size_t size = format.getSize();
  glGetBufferSubData(GL_ARRAY_BUFFER, start * size, targetCount * size, target);

#if WENDY_DEBUG
  checkGL("Error during copy from vertex buffer");
#endif
}

VertexBuffer::Usage VertexBuffer::getUsage() const
{
  return usage;
}

const VertexFormat& VertexBuffer::getFormat() const
{
  return format;
}

unsigned int VertexBuffer::getCount() const
{
  return count;
}

Ref<VertexBuffer> VertexBuffer::create(Context& context,
                                       unsigned int count,
                                       const VertexFormat& format,
				       Usage usage)
{
  Ref<VertexBuffer> buffer(new VertexBuffer(context));

  buffer->format = format;
  buffer->usage = usage;
  buffer->count = count;

  glGenBuffers(1, &buffer->bufferID);

  context.setCurrentVertexBuffer(buffer);

  glBufferData(GL_ARRAY_BUFFER,
	       count * format.getSize(),
	       NULL,
	       convertToGL(usage));

  if (!checkGL("Error during creation of vertex buffer of format \'%s\'",
               format.asString().c_str()))
  {
    context.setCurrentVertexBuffer(NULL);
    return NULL;
  }

  return buffer;
}

VertexBuffer::VertexBuffer(Context& initContext):
  context(initContext),
  locked(false),
  bufferID(0),
  count(0),
  usage(STATIC)
{
}

VertexBuffer::VertexBuffer(const VertexBuffer& source):
  context(source.context)
{
  panic("Vertex buffers may not be copied");
}

VertexBuffer& VertexBuffer::operator = (const VertexBuffer& source)
{
  panic("Vertex buffers may not be assigned");
}

///////////////////////////////////////////////////////////////////////

IndexBuffer::~IndexBuffer()
{
  if (locked)
    logWarning("Index buffer destroyed while locked");

  if (bufferID)
    glDeleteBuffers(1, &bufferID);
}

void* IndexBuffer::lock(LockType type)
{
  if (locked)
  {
    logError("Index buffer already locked");
    return NULL;
  }

  context.setCurrentIndexBuffer(this);

  void* mapping = glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, convertToGL(type));
  if (mapping == NULL)
  {
    logError("Failed to lock index buffer: %s", gluErrorString(glGetError()));
    return NULL;
  }

  locked = true;
  return mapping;
}

void IndexBuffer::unlock()
{
  if (!locked)
  {
    logWarning("Cannot unlock non-locked index buffer");
    return;
  }

  context.setCurrentIndexBuffer(this);

  if (!glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER))
    logWarning("Data for index buffer was corrupted");

  locked = false;
}

void IndexBuffer::copyFrom(const void* source, unsigned int sourceCount, unsigned int start)
{
  if (locked)
  {
    logError("Cannot copy data into locked index buffer");
    return;
  }

  if (start + sourceCount > count)
  {
    logError("Too many indices submitted to index buffer");
    return;
  }

  context.setCurrentIndexBuffer(this);

  const size_t size = getTypeSize(type);
  glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, start * size, sourceCount * size, source);

#if WENDY_DEBUG
  checkGL("Error during copy to index buffer");
#endif
}

void IndexBuffer::copyTo(void* target, unsigned int targetCount, unsigned int start)
{
  if (locked)
  {
    logError("Cannot copy data from locked index buffer");
    return;
  }

  if (start + targetCount > count)
  {
    logError("Too many indices requested from index buffer");
    return;
  }

  context.setCurrentIndexBuffer(this);

  const size_t size = getTypeSize(type);
  glGetBufferSubData(GL_ELEMENT_ARRAY_BUFFER, start * size, targetCount * size, target);

#if WENDY_DEBUG
  checkGL("Error during copy from index buffer");
#endif
}

IndexBuffer::Type IndexBuffer::getType() const
{
  return type;
}

IndexBuffer::Usage IndexBuffer::getUsage() const
{
  return usage;
}

unsigned int IndexBuffer::getCount() const
{
  return count;
}

Ref<IndexBuffer> IndexBuffer::create(Context& context,
                                     unsigned int count,
				     Type type,
				     Usage usage)
{
  Ref<IndexBuffer> buffer(new IndexBuffer(context));

  buffer->type = type;
  buffer->usage = usage;
  buffer->count = count;

  glGenBuffers(1, &buffer->bufferID);

  context.setCurrentIndexBuffer(buffer);

  glBufferData(GL_ELEMENT_ARRAY_BUFFER,
	       count * getTypeSize(type),
	       NULL,
	       convertToGL(usage));

  if (!checkGL("Error during creation of index buffer of element size %u",
               getTypeSize(type)))
  {
    context.setCurrentIndexBuffer(NULL);
    return NULL;
  }

  return buffer;
}

size_t IndexBuffer::getTypeSize(Type type)
{
  switch (type)
  {
    case IndexBuffer::UINT8:
      return sizeof(GLubyte);
    case IndexBuffer::UINT16:
      return sizeof(GLushort);
    case IndexBuffer::UINT32:
      return sizeof(GLuint);
    default:
      logError("Invalid index buffer type %u", type);
      return 0;
  }
}

IndexBuffer::IndexBuffer(Context& initContext):
  context(initContext),
  locked(false),
  type(UINT32),
  usage(STATIC),
  bufferID(0),
  count(0)
{
}

IndexBuffer::IndexBuffer(const IndexBuffer& source):
  context(source.context)
{
  panic("Index buffers may not be copied");
}

IndexBuffer& IndexBuffer::operator = (const IndexBuffer& source)
{
  panic("Index buffers may not be assigned");
}

///////////////////////////////////////////////////////////////////////

VertexRange::VertexRange():
  vertexBuffer(NULL),
  start(0),
  count(0)
{
}

VertexRange::VertexRange(VertexBuffer& initVertexBuffer):
  vertexBuffer(&initVertexBuffer),
  start(0),
  count(0)
{
  count = vertexBuffer->getCount();
}

VertexRange::VertexRange(VertexBuffer& initVertexBuffer,
                         unsigned int initStart,
                         unsigned int initCount):
  vertexBuffer(&initVertexBuffer),
  start(initStart),
  count(initCount)
{
  assert(vertexBuffer->getCount() >= start + count);
}

void* VertexRange::lock(LockType type) const
{
  if (!vertexBuffer || count == 0)
  {
    logError("Cannot lock empty vertex buffer range");
    return NULL;
  }

  Byte* vertices = (Byte*) vertexBuffer->lock(type);
  if (!vertices)
    return NULL;

  return vertices + start * vertexBuffer->getFormat().getSize();
}

void VertexRange::unlock() const
{
  if (!vertexBuffer)
  {
    logError("Cannot unlock non-locked vertex buffer");
    return;
  }

  vertexBuffer->unlock();
}

void VertexRange::copyFrom(const void* source)
{
  if (!vertexBuffer)
    return;

  vertexBuffer->copyFrom(source, count, start);
}

void VertexRange::copyTo(void* target)
{
  if (!vertexBuffer)
    return;

  vertexBuffer->copyTo(target, count, start);
}

VertexBuffer* VertexRange::getVertexBuffer() const
{
  return vertexBuffer;
}

unsigned int VertexRange::getStart() const
{
  return start;
}

unsigned int VertexRange::getCount() const
{
  return count;
}

///////////////////////////////////////////////////////////////////////

IndexRange::IndexRange():
  indexBuffer(NULL),
  start(0),
  count(0)
{
}

IndexRange::IndexRange(IndexBuffer& initIndexBuffer):
  indexBuffer(&initIndexBuffer),
  start(0),
  count(0)
{
  count = indexBuffer->getCount();
}

IndexRange::IndexRange(IndexBuffer& initIndexBuffer,
                       unsigned int initStart,
                       unsigned int initCount):
  indexBuffer(&initIndexBuffer),
  start(initStart),
  count(initCount)
{
  assert(indexBuffer->getCount() >= start + count);
}

void* IndexRange::lock(LockType type) const
{
  if (!indexBuffer || count == 0)
  {
    logError("Cannot lock empty index buffer range");
    return NULL;
  }

  Byte* indices = (Byte*) indexBuffer->lock(type);
  if (!indices)
    return NULL;

  return indices + start * IndexBuffer::getTypeSize(indexBuffer->getType());
}

void IndexRange::unlock() const
{
  if (!indexBuffer)
  {
    logError("Cannot unlock non-locked index buffer");
    return;
  }

  indexBuffer->unlock();
}

void IndexRange::copyFrom(const void* source)
{
  if (!indexBuffer)
    return;

  indexBuffer->copyFrom(source, count, start);
}

void IndexRange::copyTo(void* target)
{
  if (!indexBuffer)
    return;

  indexBuffer->copyTo(target, count, start);
}

IndexBuffer* IndexRange::getIndexBuffer() const
{
  return indexBuffer;
}

unsigned int IndexRange::getStart() const
{
  return start;
}

unsigned int IndexRange::getCount() const
{
  return count;
}

///////////////////////////////////////////////////////////////////////

PrimitiveRange::PrimitiveRange():
  type(TRIANGLE_LIST),
  vertexBuffer(NULL),
  indexBuffer(NULL),
  start(0),
  count(0)
{
}

PrimitiveRange::PrimitiveRange(PrimitiveType initType,
	                       VertexBuffer& initVertexBuffer):
  type(initType),
  vertexBuffer(&initVertexBuffer),
  indexBuffer(NULL),
  start(0),
  count(0)
{
  count = vertexBuffer->getCount();
}

PrimitiveRange::PrimitiveRange(PrimitiveType initType,
                               const VertexRange& vertexRange):
  type(initType),
  vertexBuffer(NULL),
  indexBuffer(NULL),
  start(0),
  count(0)
{
  vertexBuffer = vertexRange.getVertexBuffer();
  start = vertexRange.getStart();
  count = vertexRange.getCount();
}

PrimitiveRange::PrimitiveRange(PrimitiveType initType,
	                       VertexBuffer& initVertexBuffer,
	                       IndexBuffer& initIndexBuffer):
  type(initType),
  vertexBuffer(&initVertexBuffer),
  indexBuffer(&initIndexBuffer),
  start(0),
  count(0)
{
  count = indexBuffer->getCount();
}

PrimitiveRange::PrimitiveRange(PrimitiveType initType,
	                       VertexBuffer& initVertexBuffer,
	                       const IndexRange& indexRange):
  type(initType),
  vertexBuffer(&initVertexBuffer),
  indexBuffer(NULL),
  start(0),
  count(0)
{
  indexBuffer = indexRange.getIndexBuffer();
  start = indexRange.getStart();
  count = indexRange.getCount();
}

PrimitiveRange::PrimitiveRange(PrimitiveType initType,
	                       VertexBuffer& initVertexBuffer,
	                       unsigned int initStart,
	                       unsigned int initCount):
  type(initType),
  vertexBuffer(&initVertexBuffer),
  indexBuffer(NULL),
  start(initStart),
  count(initCount)
{
}

PrimitiveRange::PrimitiveRange(PrimitiveType initType,
	                       VertexBuffer& initVertexBuffer,
			       IndexBuffer& initIndexBuffer,
	                       unsigned int initStart,
	                       unsigned int initCount):
  type(initType),
  vertexBuffer(&initVertexBuffer),
  indexBuffer(&initIndexBuffer),
  start(initStart),
  count(initCount)
{
}

bool PrimitiveRange::isEmpty() const
{
  if (vertexBuffer == NULL)
    return true;

  return count == 0;
}

PrimitiveType PrimitiveRange::getType() const
{
  return type;
}

VertexBuffer* PrimitiveRange::getVertexBuffer() const
{
  return vertexBuffer;
}

IndexBuffer* PrimitiveRange::getIndexBuffer() const
{
  return indexBuffer;
}

unsigned int PrimitiveRange::getStart() const
{
  return start;
}

unsigned int PrimitiveRange::getCount() const
{
  return count;
}

///////////////////////////////////////////////////////////////////////

template <>
IndexRangeLock<uint8>::IndexRangeLock(IndexRange& initRange):
  range(initRange),
  indices(NULL)
{
  if (IndexBuffer* indexBuffer = range.getIndexBuffer())
  {
    if (indexBuffer->getType() != IndexBuffer::UINT8)
      panic("Index buffer is not of type UINT8");
  }

  indices = (uint8*) range.lock();
  if (!indices)
    panic("Failed to lock index buffer");
}

template <>
IndexRangeLock<uint16>::IndexRangeLock(IndexRange& initRange):
  range(initRange),
  indices(NULL)
{
  if (IndexBuffer* indexBuffer = range.getIndexBuffer())
  {
    if (indexBuffer->getType() != IndexBuffer::UINT16)
      panic("Index buffer is not of type UINT16");
  }

  indices = (uint16*) range.lock();
  if (!indices)
    panic("Failed to lock index buffer");
}

template <>
IndexRangeLock<uint32>::IndexRangeLock(IndexRange& initRange):
  range(initRange),
  indices(NULL)
{
  if (IndexBuffer* indexBuffer = range.getIndexBuffer())
  {
    if (indexBuffer->getType() != IndexBuffer::UINT32)
      panic("Index buffer is not of type UINT32");
  }

  indices = (uint32*) range.lock();
  if (!indices)
    panic("Failed to lock index buffer");
}

///////////////////////////////////////////////////////////////////////

RenderBuffer::~RenderBuffer()
{
  if (bufferID)
    glDeleteRenderbuffersEXT(1, &bufferID);
}

unsigned int RenderBuffer::getWidth() const
{
  return width;
}

unsigned int RenderBuffer::getHeight() const
{
  return height;
}

unsigned int RenderBuffer::getDepth() const
{
  return 1;
}

const PixelFormat& RenderBuffer::getFormat() const
{
  return format;
}

Ref<RenderBuffer> RenderBuffer::create(const PixelFormat& format,
                                       unsigned int width,
                                       unsigned int height)
{
  Ref<RenderBuffer> buffer(new RenderBuffer());

  buffer->format = format;
  buffer->width = width;
  buffer->height = height;

  glGenRenderbuffersEXT(1, &buffer->bufferID);
  glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, buffer->bufferID);
  glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT,
                           convertToGL(format.getSemantic()),
                           width,
                           height);

  if (!checkGL("Error during creation of render buffer of format \'%s\'",
               format.asString().c_str()))
  {
    return NULL;
  }

  return buffer;
}

RenderBuffer::RenderBuffer():
  bufferID(0)
{
}

void RenderBuffer::attach(int attachment, unsigned int z)
{
  glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT,
                               attachment,
                               GL_RENDERBUFFER_EXT,
                               bufferID);
}

void RenderBuffer::detach(int attachment)
{
  glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT,
                               attachment,
                               GL_RENDERBUFFER_EXT,
                               0);
}

///////////////////////////////////////////////////////////////////////

  } /*namespace GL*/
} /*namespace wendy*/

///////////////////////////////////////////////////////////////////////
