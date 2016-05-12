/*********************************************************************
Matt Marchant 2014 - 2016
http://trederia.blogspot.com

xygine - Zlib license.

This software is provided 'as-is', without any express or
implied warranty. In no event will the authors be held
liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute
it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented;
you must not claim that you wrote the original software.
If you use this software in a product, an acknowledgment
in the product documentation would be appreciated but
is not required.

2. Altered source versions must be plainly marked as such,
and must not be misrepresented as being the original software.

3. This notice may not be removed or altered from any
source distribution.
*********************************************************************/

#include <xygine/ShaderResource.hpp>
#include <xygine/detail/GLCheck.hpp>
#include <xygine/detail/GLExtensions.hpp>

#include <SFML/Graphics/Shader.hpp>

#include <xygine/Assert.hpp>

using namespace xy;

ShaderResource::ShaderResource(){}

//public
sf::Shader& ShaderResource::get(ShaderResource::Id type)
{
    auto result = m_shaders.find(type);
    XY_ASSERT(result != m_shaders.end(), "shader not loaded - did you forget to preload this shader?");

    return *result->second;
}

VertexAttribID ShaderResource::getAttribute(ShaderResource::Id type, const std::string& attribute) const
{
    auto result = m_attributes.find(type);
    XY_ASSERT(result != m_attributes.end(), "shader not loaded - did you forget to preload this shader?");

    auto attrib = result->second.find(attribute);
    if (attrib != result->second.end())
    {
        return attrib->second;
    }
    XY_WARNING(attrib == result->second.end(), attribute + ": not found in shader");
    return -1;
}

void ShaderResource::preload(ShaderResource::Id type, const std::string& vertShader, const std::string& fragShader)
{
    auto shader = std::make_unique<sf::Shader>();
#ifndef _DEBUG_
    shader->loadFromMemory(vertShader, fragShader);
#else
    XY_ASSERT(shader->loadFromMemory(vertShader, fragShader), "failed to create shader");
#endif //_DEBUG_

    //look up the shader attributes and store them
    m_attributes.insert(std::make_pair(type, std::map<std::string, VertexAttribID>()));

    sf::Shader::bind(shader.get());
    GLint activeAttribs;
    glCheck(glGetProgramiv(shader->getNativeHandle(), GL_ACTIVE_ATTRIBUTES, &activeAttribs));
    if (activeAttribs > 0)
    {
        GLint length;
        glCheck(glGetProgramiv(shader->getNativeHandle(), GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &length));
        if (length > 0)
        {
            std::vector<GLchar> attribName(length + 1);
            GLint attribSize;
            GLenum attribType;
            GLint attribLocation;

            for (auto i = 0; i < activeAttribs; ++i)
            {
                glCheck(glGetActiveAttrib(shader->getNativeHandle(), i, length, nullptr, &attribSize, &attribType, attribName.data()));
                attribName[length] = '\0';

                glCheck(attribLocation = glGetAttribLocation(shader->getNativeHandle(), attribName.data()));
                m_attributes[type].insert(std::make_pair(attribName.data(), attribLocation));
            }
        }
    }
    sf::Shader::bind(nullptr);

    m_shaders.insert(std::make_pair(type, std::move(shader)));
}