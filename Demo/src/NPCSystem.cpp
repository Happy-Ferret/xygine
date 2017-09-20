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

#include "NPCSystem.hpp"
#include "ActorSystem.hpp"
#include "BubbleSystem.hpp"
#include "Hitbox.hpp"
#include "ClientServerShared.hpp"

#include <xyginext/ecs/components/Transform.hpp>
#include <xyginext/util/Vector.hpp>
#include <xyginext/util/Random.hpp>

namespace
{
    const float WhirlybobSpeed = 100.f;
    const float ClocksySpeed = 86.f;

    const float maxVelocity = 800.f;
    const float gravity = 2200.f;
    const float initialJumpVelocity = 940.f;

    const std::array<float, 10> thinkTimes = { 20.f, 16.f, 12.f, 31.f, 15.4f, 14.9f, 25.f, 12.7f, 13.3f, 18.f };
    const float BubbleTime = 6.f;
}

NPCSystem::NPCSystem(xy::MessageBus& mb)
    : xy::System(mb, typeid(NPCSystem)),
    m_currentThinkTime(0)
{
    requireComponent<NPC>();
    requireComponent<Actor>();
    requireComponent<CollisionComponent>();
    requireComponent<xy::Transform>();
}

//public
void NPCSystem::handleMessage(const xy::Message&)
{

}

void NPCSystem::process(float dt)
{
    auto entities = getEntities();
    for (auto& entity : entities)
    {
        if (entity.getComponent<NPC>().state == NPC::State::Bubble)
        {
            updateBubbleState(entity, dt);
        }
        else
        {
            switch (entity.getComponent<Actor>().type)
            {
            default: break;
            case ActorID::Clocksy:
                updateClocksy(entity, dt);
                break;
            case ActorID::Whirlybob:
                updateWhirlybob(entity, dt);
                break;
            }
        }
    }
}

//private
void NPCSystem::updateWhirlybob(xy::Entity entity, float dt)
{
    auto& tx = entity.getComponent<xy::Transform>();
    auto& npc = entity.getComponent<NPC>();

    const auto& collision = entity.getComponent<CollisionComponent>();
    const auto& hitboxes = collision.getHitboxes();

    for (auto i = 0u; i < collision.getHitboxCount(); ++i)
    {
        auto& manifolds = hitboxes[i].getManifolds();
        for (auto j = 0u; j < hitboxes[i].getCollisionCount(); ++j)
        {
            auto& manifold = manifolds[j];
            switch (manifold.otherType)
            {
            default: break;
            case CollisionType::Solid:
            case CollisionType::Platform:
                tx.move(manifold.normal * manifold.penetration);
                npc.velocity = xy::Util::Vector::reflect(npc.velocity, manifold.normal);

                break;
            case CollisionType::Bubble:
                //switch to bubble state if bubble in spawn state
            {
                const auto& bubble = manifold.otherEntity.getComponent<Bubble>();
                if (bubble.state == Bubble::Spawning)
                {
                    npc.state = NPC::State::Bubble;
                    npc.velocity.y = BubbleVerticalVelocity;
                    npc.thinkTimer = BubbleTime;
                    return;
                }
            }
                break;
            case CollisionType::Teleport:
                tx.move(0.f, -(TeleportDistance - NPCSize));
                break;
            }
        }
    }

    if (npc.state == NPC::State::Normal)
    {
        tx.move(npc.velocity * WhirlybobSpeed * dt);
    }
    else
    {
        //we're in bubble state
    }
}

void NPCSystem::updateClocksy(xy::Entity entity, float dt)
{
    auto& tx = entity.getComponent<xy::Transform>();
    auto& npc = entity.getComponent<NPC>();

    const auto& collision = entity.getComponent<CollisionComponent>();
    const auto& hitboxes = collision.getHitboxes();

    for (auto i = 0u; i < collision.getHitboxCount(); ++i)
    {    
        if (hitboxes[i].getType() == CollisionType::NPC)
        {
            auto& manifolds = hitboxes[i].getManifolds();
            auto collisionCount = hitboxes[i].getCollisionCount();

            if(collisionCount == 0)
            {
                npc.canLand = true;
            }

            for (auto j = 0u; j < collisionCount; ++j)
            {
                auto& manifold = manifolds[j];
                switch (manifold.otherType)
                {
                default: break;
                case CollisionType::Platform:
                    //collide when falling downwards
                    if (manifold.normal.y < 0 && npc.canLand)
                    {
                        npc.velocity.y = 0.f;
                        npc.state = NPC::State::Normal;

                        tx.move(manifold.normal * manifold.penetration);
                        break;
                    }
                    npc.canLand = false;
                    break;
                case CollisionType::Solid:               
                    tx.move(manifold.normal * manifold.penetration);

                    if (npc.state == NPC::State::Jumping)
                    {
                        if (npc.velocity.y > 0)
                        {
                            //moving down
                            npc.state = NPC::State::Normal;
                            npc.velocity.y = 0.f;                            
                        }
                        else //bonk head
                        {
                            npc.velocity.y *= 0.25f;
                        }
                    }
                    npc.velocity = xy::Util::Vector::reflect(npc.velocity, manifold.normal);
                    break;
                case CollisionType::Bubble:
                    //switch to bubble state if bubble in spawn state
                {
                    const auto& bubble = manifold.otherEntity.getComponent<Bubble>();
                    if (bubble.state == Bubble::Spawning)
                    {
                        npc.state = NPC::State::Bubble;
                        npc.velocity.y = BubbleVerticalVelocity;
                        npc.thinkTimer = BubbleTime;
                        return;
                    }
                }
                    break;
                case CollisionType::Teleport:
                    tx.move(0.f, -TeleportDistance);
                    break;
                }
            }
        }
        else //footbox
        {
            if (hitboxes[i].getCollisionCount() == 0)
            {
                //foots in the air so we're falling
                npc.state = NPC::State::Jumping;
            }
        }  
    }

    npc.thinkTimer -= dt;

    switch (npc.state)
    {
    default: break;
    case NPC::State::Normal:
        tx.move(npc.velocity * ClocksySpeed * dt);

        if (npc.thinkTimer < 0)
        {
            npc.thinkTimer = thinkTimes[m_currentThinkTime] * 0.125f;
            m_currentThinkTime = (m_currentThinkTime + 1) % thinkTimes.size();
            npc.state = NPC::State::Thinking;
        }

        break;
    case NPC::State::Jumping:
        npc.velocity.y += gravity * dt;
        tx.move(npc.velocity * dt);
        break;

    case NPC::State::Thinking:
        if (npc.thinkTimer < 0)
        {
            npc.thinkTimer = thinkTimes[m_currentThinkTime];
            m_currentThinkTime = (m_currentThinkTime + 1) % thinkTimes.size();
            
            if (xy::Util::Random::value(0,1) % 2 == 0)
            {
                npc.state = NPC::State::Normal;
                npc.velocity.x = -npc.velocity.x;
            }
            else
            {
                //do a jump
                npc.velocity.y = -initialJumpVelocity;
                npc.state = NPC::State::Jumping;
                tx.move(npc.velocity * dt);
            }
        }
        break;
    }
}

void NPCSystem::updateBubbleState(xy::Entity entity, float dt)
{
    auto& tx = entity.getComponent<xy::Transform>();
    auto& npc = entity.getComponent<NPC>();

    const auto& collision = entity.getComponent<CollisionComponent>();
    const auto& hitboxes = collision.getHitboxes();

    for (auto i = 0u; i < collision.getHitboxCount(); ++i)
    {
        auto& manifolds = hitboxes[i].getManifolds();
        for (auto j = 0u; j < hitboxes[i].getCollisionCount(); ++j)
        {
            auto& manifold = manifolds[j];
            switch (manifold.otherType)
            {
            default: break;
            case CollisionType::Solid:
            //case CollisionType::Platform:
                tx.move(manifold.normal * manifold.penetration);
                break;
            case CollisionType::Player:
                //kill ent and give player points
                break;
            }
        }
    }

    tx.move(npc.velocity * dt);

    npc.thinkTimer -= dt;
    if (npc.thinkTimer < 0)
    {
        //bubble wasn't burst in time, release NPC (angry mode?)
    }
}

void NPCSystem::onEntityAdded(xy::Entity entity)
{
    entity.getComponent<NPC>().thinkTimer = thinkTimes[m_currentThinkTime];

    m_currentThinkTime = (m_currentThinkTime + 1) % thinkTimes.size();
}