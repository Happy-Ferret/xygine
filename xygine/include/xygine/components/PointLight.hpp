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

#ifndef XY_POINT_LIGHT_HPP_
#define XY_POINT_LIGHT_HPP_

#include <xygine/components/Component.hpp>

#include <SFML/System/Vector3.hpp>

namespace xy
{
    /*!
    \brief Point Light Component

    Point lights represent a point in a scene which casts light of a specified
    colour for a specified distance. Only a limited amount of lights can be active
    at any one time and only affect drawables which use the normal map shader
    (or any shader written specifically to take advantage of point lights).
    Typically one would also attach a quad tree component to entities with point
    lights and query the scene's quad tree to find the nearest lights to activate
    and with which to update the shader's light parameters.
    */
    class XY_EXPORT_API PointLight final : public Component
    {
    public:
        /*!
        \brief constructor.
        \param MessageBus
        \param diffuseColour The main light colour. Defaults to white.
        \param specularCOlour The colour with which to affect specular highlights. Defualts to White
        */
        PointLight(MessageBus&, float range, const sf::Color& diffuseColour = sf::Color::White, const sf::Color& specularColour = sf::Color::White);

        Component::Type type() const { return Component::Type::Script; }
        void entityUpdate(Entity&, float) override;

        /*!
        \brief Set the virtual z-depth of the light.
        This should be a positive number, the larger the value the
        further the perceived light is from the scene
        */
        void setDepth(float);

        /*!
        \brief Set the light's intensity.
        The light's intensity or brightness is a multiplier of
        the diffuse colour. Usually in the range 0 - 1 (where 0
        effectively turns the light off) the value may be higher
        but will probably onl serve to cause saturation.
        */
        void setIntensity(float);

        /*!
        \brief Set the range of the light.
        A positive value which defines the range, in pixels, of
        the light's influence before it falls off to 0
        */
        void setRange(float);

        /*!
        \brief Set the light's diffuse colour
        */
        void setDiffuseColour(const sf::Color&);

        /*!
        \brief Set the light's specular colour
        */
        void setSpecularColour(const sf::Color&);

        /*!
        \brief Get the light's world position.
        The positions is thre dimensional, including the percieved
        depth value. This is so that shader position properies can
        be set directly.
        */
        const sf::Vector3f& getWorldPosition() const;

        /*!
        \brief Get the light's intensity.
        \see setIntensity
        */
        float getIntensity() const;

        /*!
        \brief Return the light's *inverse* range.
        The inverse range value is used by the shader to
        calculate falloff. The shader property should be set directly
        with this value
        */
        float getInverseRange() const;

        /*!
        \brief Returns the light's current diffuse colour
        */
        const sf::Color& getDiffuseColour() const;

        /*!
        \brief Returns the light's current specular colour
        */
        const sf::Color& getSpecularColour() const;

    private:

        float m_range;
        float m_inverseRange;
        sf::Vector3f m_position;
        float m_intensity;
        sf::Color m_diffuseColour;
        sf::Color m_specularColour;
    };
}

#endif //XY_POINT_LIGHT_HPP_