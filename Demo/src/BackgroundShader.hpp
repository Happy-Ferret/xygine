/*********************************************************************
(c) Matt Marchant 2017
http://trederia.blogspot.com

xygineXT - Zlib license.

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

#ifndef DEMO_BACKGROUND_SHADER_HPP_
#define DEMO_BACKGROUND_SHADER_HPP_

#include <xyginext/ecs/Entity.hpp>
#include <xyginext/ecs/components/Sprite.hpp>
#include <SFML/Graphics/Shader.hpp>

#include <string>

const static std::string BackgroundFragment = R"(
            #version 120
            uniform sampler2D u_diffuseMap;
            uniform float u_colourAngle;
                
            vec3 rgb2hsv(vec3 c)
            {
                vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
                vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));
                vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));
                float d = q.x - min(q.w, q.y);
                float e = 1.0e-10;
                return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
            }
            vec3 hsv2rgb(vec3 c)
            {
                vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
                vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
                return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
            }
            void main()
            {
                vec4 colour = texture2D(u_diffuseMap, gl_TexCoord[0].xy);
                vec3 hsv = rgb2hsv(colour.rgb);
                hsv.x += u_colourAngle / 360.0;
                hsv.x = mod(hsv.x, 1.0);
                gl_FragColor = vec4(hsv2rgb(hsv), colour.a);
            })";

class ColourRotator final
{
public:
    explicit ColourRotator(sf::Shader& shader)
        : m_shader(shader) {}

    void operator () (xy::Entity, float dt)
    {
        m_rotation += dt * 30.f;
        m_shader.setUniform("u_colourAngle", m_rotation);
        //m_shader.setUniform("u_diffuseMap", *entity.getComponent<xy::Sprite>().getTexture());
    }

private:
    float m_rotation = 0.f;
    sf::Shader& m_shader;
};

#endif //DEMO_BACKGROUND_SHADER_HPP_