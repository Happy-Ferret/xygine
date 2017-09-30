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

#include "InventoryDirector.hpp"
#include "MessageIDs.hpp"
#include "PacketIDs.hpp"
#include "MapData.hpp"

#include <xyginext/network/NetHost.hpp>
#include <xyginext/ecs/Scene.hpp>

namespace
{
    const sf::Uint32 NpcScore = 500;
    const sf::Uint32 GooblyScore = 850;
    const sf::Uint32 SmallFruitScore = 350;
    const sf::Uint32 LargeFruitScore = 1000;
}

InventoryDirector::InventoryDirector(xy::NetHost& host)
    : m_host(host)
{

}

//public
void InventoryDirector::handleMessage(const xy::Message& msg)
{    
    switch (msg.id)
    {
    default: break;
    case MessageID::NpcMessage:
    {
        const auto& data = msg.getData<NpcEvent>();
        if (data.type == NpcEvent::Died)
        {
            auto actor = getScene().getEntity(data.entityID).getComponent<Actor>().type;
            
            if (actor == ActorID::Goobly)
            {
                m_playerValues[data.playerID].score += GooblyScore;
                sendUpdate(data.playerID, GooblyScore);
            }
            else
            {
                m_playerValues[data.playerID].score += NpcScore;
                sendUpdate(data.playerID, NpcScore);
            }            
        }
    }
        break;
    case MessageID::ItemMessage:
    {
        sf::Uint32 amount = 0;
        const auto& data = msg.getData<ItemEvent>();
        switch (data.actorID)
        {
        default: break;
        case ActorID::FruitLarge:

            break;
        case ActorID::FruitSmall:
            amount = SmallFruitScore;
            break;
        }

        m_playerValues[data.playerID].score += amount;
        sendUpdate(data.playerID, amount);
    }
        break;
    case MessageID::PlayerMessage:
    {
        const auto& data = msg.getData<PlayerEvent>();
        if (data.type == PlayerEvent::Spawned)
        {
            //reset the old scores
            const auto& player = data.entity.getComponent<Player>();
            m_playerValues[player.playerNumber].lives = player.lives;
            m_playerValues[player.playerNumber].score = 0;

            //send info on both, in case new player has joined
            sendUpdate(0, 0);
            sendUpdate(1, 0);
        }
        else if (data.type == PlayerEvent::Died)
        {
            const auto& player = data.entity.getComponent<Player>();
            m_playerValues[player.playerNumber].lives = player.lives;
            sendUpdate(player.playerNumber, 0);
        }
    }
        break;
    }
}

//private
void InventoryDirector::sendUpdate(sf::Uint8 player, sf::Uint32 amount)
{
        InventoryUpdate update;
        update.lives = m_playerValues[player].lives;
        update.score = m_playerValues[player].score;
        update.amount = amount;
        update.playerID = player;

        m_host.broadcastPacket(PacketID::InventoryUpdate, update, xy::NetFlag::Reliable, 1);
}