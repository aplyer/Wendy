///////////////////////////////////////////////////////////////////////
// Wendy default renderer
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

#include <moira/Moira.h>

#include <wendy/Config.h>

#include <wendy/GLContext.h>
#include <wendy/GLTexture.h>
#include <wendy/GLVertex.h>
#include <wendy/GLBuffer.h>
#include <wendy/GLShader.h>
#include <wendy/GLRender.h>
#include <wendy/GLState.h>

#include <wendy/RenderStyle.h>
#include <wendy/RenderIO.h>

#include <algorithm>
#include <cstdlib>

///////////////////////////////////////////////////////////////////////

namespace wendy
{
  namespace render
  {
  
///////////////////////////////////////////////////////////////////////

using namespace moira;

///////////////////////////////////////////////////////////////////////

namespace
{

Mapper<String, GL::CullMode> cullModeMap;
Mapper<String, GL::BlendFactor> blendFactorMap;
Mapper<String, GL::Function> functionMap;
/*
Mapper<String, GLenum> operationMap;
Mapper<String, GLint> filterMap;
Mapper<String, GLint> addressModeMap;
*/

const unsigned int RENDER_STYLE_XML_VERSION = 4;

}

///////////////////////////////////////////////////////////////////////

StyleCodec::StyleCodec(void):
  ResourceCodec<Style>("XML render style codec")
{
  addSuffix("style");
}

Style* StyleCodec::read(const Path& path, const String& name)
{
  return ResourceCodec<Style>::read(path, name);
}

Style* StyleCodec::read(Stream& stream, const String& name)
{
  if (cullModeMap.isEmpty())
  {
    cullModeMap["none"] = GL::CULL_NONE;
    cullModeMap["front"] = GL::CULL_FRONT;
    cullModeMap["back"] = GL::CULL_BACK;
    cullModeMap["both"] = GL::CULL_BOTH;
  }

  if (blendFactorMap.isEmpty())
  {
    blendFactorMap["zero"] = GL::BLEND_ZERO;
    blendFactorMap["one"] = GL::BLEND_ONE;
    blendFactorMap["src color"] = GL::BLEND_SRC_COLOR;
    blendFactorMap["dst color"] = GL::BLEND_DST_COLOR;
    blendFactorMap["src alpha"] = GL::BLEND_SRC_ALPHA;
    blendFactorMap["dst alpha"] = GL::BLEND_DST_ALPHA;
    blendFactorMap["one minus src color"] = GL::BLEND_ONE_MINUS_SRC_COLOR;
    blendFactorMap["one minus dst color"] = GL::BLEND_ONE_MINUS_DST_COLOR;
    blendFactorMap["one minus src alpha"] = GL::BLEND_ONE_MINUS_SRC_ALPHA;
    blendFactorMap["one minus dst alpha"] = GL::BLEND_ONE_MINUS_DST_ALPHA;
  }

  if (functionMap.isEmpty())
  {
    functionMap["never"] = GL::ALLOW_NEVER;
    functionMap["always"] = GL::ALLOW_ALWAYS;
    functionMap["equal"] = GL::ALLOW_EQUAL;
    functionMap["not equal"] = GL::ALLOW_NOT_EQUAL;
    functionMap["lesser"] = GL::ALLOW_LESSER;
    functionMap["lesser or equal"] = GL::ALLOW_LESSER_EQUAL;
    functionMap["greater"] = GL::ALLOW_GREATER;
    functionMap["greater or equal"] = GL::ALLOW_GREATER_EQUAL;
  }

  /*
  if (operationMap.isEmpty())
  {
    operationMap["keep"] = GL_KEEP;
    operationMap["reset"] = GL_ZERO;
    operationMap["replace"] = GL_REPLACE;
    operationMap["increment"] = GL_INCR;
    operationMap["decrement"] = GL_DECR;
    operationMap["invert"] = GL_INVERT;
  }

  if (filterMap.isEmpty())
  {
    filterMap["nearest"] = GL_NEAREST;
    filterMap["linear"] = GL_LINEAR;
    filterMap["nearest mipmap nearest"] = GL_NEAREST_MIPMAP_NEAREST;
    filterMap["nearest mipmap linear"] = GL_NEAREST_MIPMAP_LINEAR;
    filterMap["linear mipmap nearest"] = GL_LINEAR_MIPMAP_NEAREST;
    filterMap["linear mipmap linear"] = GL_LINEAR_MIPMAP_LINEAR;
  }

  if (addressModeMap.isEmpty())
  {
    addressModeMap["wrap"] = GL_REPEAT;
    addressModeMap["clamp"] = GL_CLAMP;
  }
  */

  currentTechnique = NULL;
  currentPass = NULL;

  styleName = name;

  if (!XML::Codec::read(stream))
  {
    style = NULL;
    return NULL;
  }

  if (!style->getTechniqueCount())
  {
    Log::writeError("No valid techniques found in render style %s",
                    style->getName().c_str());

    style = NULL;
    return NULL;
  }

  return style.detachObject();
}

bool StyleCodec::write(const Path& path, const Style& style)
{
  return ResourceCodec<Style>::write(path, style);
}

bool StyleCodec::write(Stream& stream, const Style& style)
{
  GL::RenderState defaults;

  try
  {
    setStream(&stream);

    beginElement("style");
    addAttribute("version", (int) RENDER_STYLE_XML_VERSION);

    for (unsigned int i = 0;  i < style.getTechniqueCount();  i++)
    {
      const Technique& technique = style.getTechnique(i);

      beginElement("technique");
      addAttribute("name", technique.getName());
      addAttribute("quality", technique.getQuality());

      for (unsigned int i = 0;  i < technique.getPassCount();  i++)
      {
	const Pass& pass = technique.getPass(i);

	beginElement("pass");

	if (!pass.getName().empty())
	  addAttribute("name", pass.getName());

	if (pass.getSrcFactor() != defaults.getSrcFactor() ||
	    pass.getDstFactor() != defaults.getDstFactor())
	{
	  beginElement("blending");
	  addAttribute("src", blendFactorMap[pass.getSrcFactor()]);
	  addAttribute("dst", blendFactorMap[pass.getDstFactor()]);
	  endElement();
	}

	if (pass.isColorWriting() != defaults.isColorWriting())
	{
	  beginElement("color");
	  addAttribute("writing", pass.isColorWriting());
	  endElement();
	}

	if (pass.isDepthTesting() != defaults.isDepthTesting() ||
	    pass.isDepthWriting() != defaults.isDepthWriting())
	{
	  beginElement("depth");
	  addAttribute("testing", pass.isDepthTesting());
	  addAttribute("writing", pass.isDepthWriting());
	  addAttribute("function", functionMap[pass.getDepthFunction()]);
	  endElement();
	}

	/*
	if (pass.isStencilTesting() != defaults.isStencilTesting() ||
	    pass.getStencilFunction() != defaults.getStencilFunction() ||
	    pass.getStencilReference() != defaults.getStencilReference() ||
	    pass.getStencilMask() != defaults.getStencilMask() ||
	    pass.getStencilFailOperation() != defaults.getStencilFailOperation() ||
	    pass.getDepthPassOperation() != defaults.getDepthPassOperation() ||
	    pass.getDepthFailOperation() != defaults.getDepthFailOperation())
	{
	  beginElement("stencil");
	  addAttribute("testing", pass.isStencilTesting());
	  addAttribute("function", functionMap[pass.getStencilFunction()]);
	  addAttribute("reference", pass.getStencilReference());
	  addAttribute("mask", pass.getStencilMask());
	  addAttribute("stencil-failed", operationMap[pass.getStencilFailOperation()]);
	  addAttribute("depth-failed", operationMap[pass.getDepthFailOperation()]);
	  addAttribute("depth-passed", operationMap[pass.getDepthPassOperation()]);
	  endElement();
	}
	*/

	if (pass.isWireframe() != defaults.isWireframe() ||
	    pass.getCullMode() != defaults.getCullMode())
	{
	  beginElement("polygon");
	  addAttribute("wireframe", pass.isWireframe());
	  addAttribute("cull", cullModeMap[pass.getCullMode()]);
	  endElement();
	}

	if (GL::Program* program = pass.getProgram())
	{
	  beginElement("program");
	  addAttribute("name", program->getName());

	  for (unsigned int i = 0;  i < pass.getSamplerCount();  i++)
	  {
	    const GL::SamplerState& state = pass.getSamplerState(i);

	    Ref<GL::Texture> texture;
	    state.getTexture(texture);
	    if (texture)
	      continue;

	    beginElement("sampler");
	    addAttribute("name", state.getSampler().getName());
	    addAttribute("texture", texture->getName());
	    endElement();
	  }

	  endElement();
	}

	endElement();
      }

      endElement();
    }

    endElement();

    setStream(NULL);
  }
  catch (Exception& exception)
  {
    Log::writeError("Failed to write render style %s: %s",
                    style.getName().c_str(),
		    exception.what());
    setStream(NULL);
    return false;
  }

  return true;
}

bool StyleCodec::onBeginElement(const String& name)
{
  if (name == "style")
  {
    if (style)
    {
      Log::writeError("Only one render style per file allowed");
      return false;
    }

    const unsigned int version = readInteger("version");
    if (version != RENDER_STYLE_XML_VERSION)
    {
      Log::writeError("Render style XML format version mismatch");
      return false;
    }

    style = new Style(styleName);
    return true;
  }

  if (style)
  {
    if (name == "technique")
    {
      Technique& technique = style->createTechnique(readString("name"));
      technique.setQuality(readFloat("quality"));
      currentTechnique = &technique;
      return true;
    }

    if (currentTechnique)
    {
      if (name == "pass")
      {
	Pass& pass = currentTechnique->createPass(readString("name"));
	currentPass = &pass;
	return true;
      }

      if (currentPass)
      {
	if (name == "blending")
	{
	  String srcFactorName = readString("src");
	  if (srcFactorName.length())
	  {
	    if (blendFactorMap.hasKey(srcFactorName))
	      currentPass->setBlendFactors(blendFactorMap[srcFactorName],
	                                   currentPass->getDstFactor());
	    else
	    {
	      Log::writeError("Invalid blend factor name %s", srcFactorName.c_str());
	      return false;
	    }
	  }

	  String dstFactorName = readString("dst");
	  if (dstFactorName.length())
	  {
	    if (blendFactorMap.hasKey(dstFactorName))
	      currentPass->setBlendFactors(currentPass->getSrcFactor(),
	                                   blendFactorMap[dstFactorName]);
	    else
	    {
	      Log::writeError("Invalid blend factor name %s",
	                      dstFactorName.c_str());
	      return false;
	    }
	  }

	  return true;
	}

	if (name == "color")
	{
	  currentPass->setColorWriting(readBoolean("writing", currentPass->isColorWriting()));
	  return true;
	}

	if (name == "depth")
	{
	  currentPass->setDepthTesting(readBoolean("testing", currentPass->isDepthTesting()));
	  currentPass->setDepthWriting(readBoolean("writing", currentPass->isDepthWriting()));

	  String functionName = readString("function");
	  if (functionName.length())
	  {
	    if (functionMap.hasKey(functionName))
	      currentPass->setDepthFunction(functionMap[functionName]);
	    else
	    {
	      Log::writeError("Invalid depth test function name %s",
	                      functionName.c_str());
	      return false;
	    }
	  }

	  return true;
	}
	
	/*
	if (name == "stencil")
	{
	  currentPass->setStencilTesting(readBoolean("testing", currentPass->isStencilTesting()));

	  String functionName = readString("function");
	  if (!functionMap.hasKey(functionName))
	    return false;

	  currentPass->setStencilFunction(functionMap[functionName]);

	  String operationName;

	  operationName = readString("stencil-failed");
	  if (operationMap.hasKey(operationName))
	    currentPass->setStencilOperations(operationMap[operationName],
					      currentPass->getDepthFailOperation(),
					      currentPass->getDepthPassOperation());
	  else
	    return false;

	  operationName = readString("depth-failed");
	  if (operationMap.hasKey(operationName))
	    currentPass->setStencilOperations(currentPass->getStencilFailOperation(),
					      operationMap[operationName],
					      currentPass->getDepthPassOperation());
	  else
	    return false;

	  operationName = readString("depth-passed");
	  if (operationMap.hasKey(operationName))
	    currentPass->setStencilOperations(currentPass->getStencilFailOperation(),
					      currentPass->getDepthFailOperation(),
					      operationMap[operationName]);
	  else
	    return false;

	  const unsigned int reference = readInteger("reference", currentPass->getStencilReference());
	  const unsigned int mask = readInteger("mask", currentPass->getStencilMask());

	  currentPass->setStencilValues(reference, mask);

	  return true;
	}
	*/

	if (name == "polygon")
	{
	  currentPass->setWireframe(readBoolean("wireframe"));

	  String cullModeName = readString("cull");
	  if (cullModeName.length())
	  {
	    if (cullModeMap.hasKey(cullModeName))
	      currentPass->setCullMode(cullModeMap[cullModeName]);
	    else
	    {
	      Log::writeError("Invalid cull mode %s", cullModeName.c_str());
	      return false;
	    }
	  }

	  return true;
	}

	if (name == "program")
	{
	  String programName = readString("name");
	  if (programName.empty())
	  {
	    Log::writeError("Shader program name missing");
	    return false;
	  }

	  GL::Program* program = GL::Program::readInstance(programName);
	  if (!program)
	  {
	    Log::writeWarning("Failed to load shader program %s; skipping technique %u in render style %s",
	                      programName.c_str(),
	                      style->getTechniqueCount(),
	                      style->getName().c_str());

	    style->destroyTechnique(*currentTechnique);
	    currentTechnique = NULL;
	    return true;
	  }
	  
	  currentPass->setProgram(program);
	  return true;
	}

	if (currentPass->getProgram())
	{
	  if (name == "sampler")
	  {
	    String samplerName = readString("name");
	    if (samplerName.empty())
	      return true;

	    if (!currentPass->getProgram()->findSampler(samplerName))
	    {
	      Log::writeError("Shader program %s does not have sampler uniform %s",
	                      currentPass->getProgram()->getName().c_str(),
			      samplerName.c_str());
	      return false;
	    }

	    String textureName = readString("texture");
	    if (textureName.empty())
	      return true;

	    Ref<GL::Texture> texture = GL::Texture::readInstance(textureName);
	    if (!texture)
	      return false;

	    currentPass->getSamplerState(samplerName).setTexture(texture);
	    return true;
	  }
	}

	/*
	if (name == "filter")
	{
	  String filterName;
	  
	  filterName = readString("min");
	  if (!filterName.empty())
	  {
	    if (filterMap.hasKey(filterName))
	      currentLayer->setFilters(filterMap[filterName],
	                               currentLayer->getMagFilter());
	    else
	      Log::writeError("Invalid texture layer min filter type %s",
	                      filterName.c_str());
	  }
	  
	  filterName = readString("mag");
	  if (!filterName.empty())
	  {
	    if (filterMap.hasKey(filterName))
	      currentLayer->setFilters(currentLayer->getMinFilter(),
	                               filterMap[filterName]);
	    else
	      Log::writeError("Invalid texture layer mag filter type %s",
	                      filterName.c_str());
	  }
	}

	if (name == "address")
	{
	  String addressModeName = readString("mode");
	  if (!addressModeName.empty())
	  {
	    if (addressModeMap.hasKey(addressModeName))
	      currentLayer->setAddressMode(addressModeMap[addressModeName]);
	    else
	      Log::writeError("Invalid texture layer address mode %s",
	                      addressModeName.c_str());
	  }
	}
	*/
      }
    }
  }

  return true;
}

bool StyleCodec::onEndElement(const String& name)
{
  if (style)
  {
    if (currentTechnique)
    {
      if (name == "technique")
      {
	currentTechnique = NULL;
	return true;
      }

      if (currentPass)
      {
	if (name == "pass")
	{
	  currentPass = NULL;
	  return true;
	}
      }
    }
  }

  return true;
}

///////////////////////////////////////////////////////////////////////

  } /*namespace render*/
} /*namespace wendy*/

///////////////////////////////////////////////////////////////////////
