/*********************************************************************
(c) Matt Marchant 2017 - 2018
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

#include "xyginext/ecs/components/NetInterpolation.hpp"

using namespace xy;

NetInterpolate::NetInterpolate()
    : m_enabled         (true),
    m_targetTimestamp   (0.f),
    m_elapsedTime       (0.f),
    m_timeDifference    (0.f),
    m_previousTimestamp (0.f)
{

}

//public
void NetInterpolate::setTarget(sf::Vector2f targetPosition, sf::Int32 timestamp)
{
    m_previousTimestamp = m_targetTimestamp;
    m_previousPosition = m_targetPosition;
    m_elapsedTime = 0.f;
    m_targetTimestamp = static_cast<float>(timestamp) / 1000.f;
    m_timeDifference = m_targetTimestamp - m_previousTimestamp;
    m_targetPosition = targetPosition;
}

void NetInterpolate::setEnabled(bool enabled)
{
    m_enabled = enabled;
}

bool NetInterpolate::getEnabled() const
{
    return m_enabled;
}

void NetInterpolate::resetPosition(sf::Vector2f position)
{
    m_previousPosition = m_targetPosition = position;
}