/*********************************************************************
Matt Marchant 2014 - 2015
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

#include <xygine/physics/JointDistance.hpp>
#include <xygine/physics/RigidBody.hpp>
#include <xygine/Assert.hpp>

using namespace xy;
using namespace xy::Physics;

DistanceJoint::DistanceJoint(const RigidBody& rbA, const sf::Vector2f& worldPosA, const sf::Vector2f& worldPosB)
    : m_bodyA   (&rbA),
    m_bodyB     (nullptr)
{
    m_anchorA = World::sfToBoxVec(worldPosA);
    m_anchorB = World::sfToBoxVec(worldPosB);
}

//public


//private
const b2JointDef* DistanceJoint::getDefinition()
{
    XY_ASSERT(m_bodyA && m_bodyB, "Don't forget to set your Bodies!");
    
    //apply def settings first
    m_definition.Initialize(m_bodyA->m_body, m_bodyB->m_body, m_anchorA, m_anchorB);

    return static_cast<b2JointDef*>(&m_definition);
}