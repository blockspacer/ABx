/**
 * Copyright 2020 Stefan Ascher
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is furnished to do
 * so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include "stdafx.h"
#include "TradeComp.h"
#include "Player.h"
#include <abscommon/NetworkMessage.h>
#include <AB/Packets/ServerPackets.h>
#include <AB/Packets/Packet.h>
#include "InventoryComp.h"
#include "ItemFactory.h"
#include <abscommon/Subsystems.h>

namespace Game {
namespace Components {

TradeComp::TradeComp(Player& owner) :
    owner_(owner)
{
    owner_.SubscribeEvent<void(void)>(EVENT_ON_STUCK, std::bind(&TradeComp::OnStuck, this));
    owner_.SubscribeEvent<void(AB::GameProtocol::CreatureState, AB::GameProtocol::CreatureState)>(EVENT_ON_STATECHANGE,
        std::bind(&TradeComp::OnStateChange, this, std::placeholders::_1, std::placeholders::_2));
}

void TradeComp::Update(uint32_t)
{
    if (auto target = target_.lock())
    {
        if (state_ == TradeState::MoveToTarget)
        {
            // We need to move to the target
            if (!CheckRange())
            {
                MoveToTarget(target);
                return;
            }
            else
            {
                owner_.autorunComp_->Reset();
                owner_.stateComp_.Reset();
                StartTrading();
            }
        }
    }
    else
        Cancel();
}

void TradeComp::Reset()
{
    ourOffer_.clear();
    ourOfferedMoney_ = 0;
    target_.reset();
    accepted_ = false;
    state_ = TradeState::Idle;
}

void TradeComp::Cancel()
{
    if (state_ > TradeState::Idle)
    {
        state_ = TradeState::Idle;

        auto msg = Net::NetworkMessage::GetNew();
        msg->AddByte(AB::GameProtocol::ServerPacketType::TradeCancel);
        AB::Packets::Server::TradeCancel packet;
        AB::Packets::Add(packet, *msg);
        owner_.WriteToOutput(*msg);

        if (auto target = target_.lock())
            target->tradeComp_->Cancel();
        target_.reset();
    }
}

void TradeComp::WriteError(TradeError error, Net::NetworkMessage& message)
{
    if (error != TradeError::None)
    {
        message.AddByte(AB::GameProtocol::ServerPacketType::PlayerError);
        AB::Packets::Server::GameError packet;
        switch (error)
        {
        case Components::TradeComp::TradeError::None:
            break;
        case Components::TradeComp::TradeError::TargetInvalid:
            packet.code = static_cast<uint8_t>(AB::GameProtocol::PlayerErrorValue::TradingPartnerInvalid);
            break;
        case Components::TradeComp::TradeError::TargetQueing:
            packet.code = static_cast<uint8_t>(AB::GameProtocol::PlayerErrorValue::TradingPartnerQueueing);
            break;
        case Components::TradeComp::TradeError::TargetTrading:
            packet.code = static_cast<uint8_t>(AB::GameProtocol::PlayerErrorValue::TradingPartnerTrading);
            break;
        case Components::TradeComp::TradeError::InProgress:
            packet.code = static_cast<uint8_t>(AB::GameProtocol::PlayerErrorValue::AlreadyTradingWithThisTarget);
            break;
        }
        AB::Packets::Add(packet, message);
    }
}

uint32_t TradeComp::GetTradePartnerId() const
{
    if (auto p = target_.lock())
        return p->id_;
    return 0;
}

void TradeComp::Offer(uint32_t money, std::vector<std::pair<uint16_t, uint32_t>>&& items)
{
    auto target = target_.lock();
    if (!target)
    {
        LOG_ERROR << "Target gone while trading" << std::endl;
        return;
    }
    state_ = TradeState::Offered;
    ourOffer_ = std::move(items);
    ourOfferedMoney_ = money;
    auto msg = Net::NetworkMessage::GetNew();
    msg->AddByte(AB::GameProtocol::ServerPacketType::TradeGotOffer);
    AB::Packets::Server::TradeOffer packet;
    packet.money = money;
    auto& invComp = *owner_.inventoryComp_;
    static_assert(AB::Packets::Server::TradeOffer::MOD_COUNT == static_cast<size_t>(ItemUpgrade::__Count),
        "AB::Packets::Server::TradeOffer::MOD_COUNT does not match ItemUpgrade::__Count");
    for (auto itemPair : ourOffer_)
    {
        auto* item = invComp.GetInventoryItem(itemPair.first);
        if (!item)
        {
            LOG_ERROR << "No item at pos " << static_cast<int>(itemPair.first) << " in inventory" << std::endl;
            continue;
        }

        AB::Packets::Server::TradeOffer::OfferedItem offeredItem;
        offeredItem.index = item->data_.index;
        offeredItem.count = itemPair.second;
        offeredItem.stats = item->concreteItem_.itemStats;
        offeredItem.type = static_cast<uint8_t>(item->data_.type);
        offeredItem.value = item->concreteItem_.value;
        for (size_t i = 0; i < static_cast<size_t>(ItemUpgrade::__Count); ++i)
        {
            if (auto* upgradeItem = item->GetUpgrade(static_cast<ItemUpgrade>(i)))
            {
                auto& upgrade = offeredItem.mods[i];
                upgrade.index = upgradeItem->data_.index;
                // An upgrade doesn't really have count (it's always 1), so just copy it
                upgrade.count = upgradeItem->concreteItem_.count;
                upgrade.stats = upgradeItem->concreteItem_.itemStats;
                upgrade.type = static_cast<uint8_t>(upgradeItem->data_.type);
                upgrade.value = upgradeItem->concreteItem_.value;
            }
        }
        packet.items.push_back(std::move(offeredItem));
    }
    packet.itemCount = static_cast<uint8_t>(packet.items.size());
    AB::Packets::Add(packet, *msg);
    target->WriteToOutput(*msg);
}

void TradeComp::Accept()
{
    // Must submit an offer before accept is possible
    if (state_ != TradeState::Offered)
        return;
    auto target = target_.lock();
    if (!target)
    {
        Cancel();
        return;
    }

    accepted_ = true;
    if (!target->tradeComp_->IsAccepted())
        return;
    // Both have accepted now.

    // The last acceptor makes the transaction.
    if (!owner_.inventoryComp_->CheckInventoryCapacity(target->tradeComp_->GetOfferedMoney(), target->tradeComp_->GetOfferedItemCount()))
    {
        owner_.CallEvent<void()>(EVENT_ON_INVENTORYFULL);
        Cancel();
        return;
    }
    if (!target->inventoryComp_->CheckInventoryCapacity(GetOfferedMoney(), GetOfferedItemCount()))
    {

        target->CallEvent<void()>(EVENT_ON_INVENTORYFULL);
        Cancel();
        return;
    }

    auto* factory = GetSubsystem<ItemFactory>();
    auto exchangeItem = [&](Item& item, uint32_t count,
        Player& removeFrom, Player& addTo,
        Net::NetworkMessage& removeMessage,
        Net::NetworkMessage& addMessage)
    {
        InventoryComp& removeInv = *removeFrom.inventoryComp_;
        InventoryComp& addtoInv = *addTo.inventoryComp_;
        if (item.concreteItem_.count == count)
        {
            // Shortcut, just move the item
            item.concreteItem_.accountUuid = target->GetAccountUuid();
            item.concreteItem_.playerUuid = target->data_.uuid;
            uint32_t id = removeInv.RemoveInventoryItem(item.concreteItem_.storagePos);
            // Use next free slot
            item.concreteItem_.storagePos = 0;
            addtoInv.SetInventoryItem(id, &addMessage);
            removeMessage.AddByte(AB::GameProtocol::ServerPacketType::InventoryItemDelete);
            AB::Packets::Server::InventoryItemDelete packet = {
                item.concreteItem_.storagePos
            };

            AB::Packets::Add(packet, removeMessage);
            return;
        }

        // We must split the stack into 2 items
        uint32_t itemId = factory->CreatePlayerItem(addTo, item.data_.uuid, count);
        addtoInv.SetInventoryItem(itemId, &addMessage);

        item.concreteItem_.count -= count;
        InventoryComp::WriteItemUpdate(&item, &removeMessage);
    };

    auto ourMessage = Net::NetworkMessage::GetNew();
    auto theirMessage = Net::NetworkMessage::GetNew();
    VisitOfferedItems([&](Item& item, uint32_t count)
    {
        if (item.concreteItem_.count > count)
            return Iteration::Continue;

        // Our offered items become theirs
        exchangeItem(item, count, owner_, *target, *ourMessage, *theirMessage);
        return Iteration::Continue;
    });
    target->tradeComp_->VisitOfferedItems([&](Item& item, uint32_t count)
    {
        if (item.concreteItem_.count > count)
            return Iteration::Continue;

        exchangeItem(item, count, *target, owner_, *theirMessage, *ourMessage);
        return Iteration::Continue;
    });

    // Finally exchange money
    if (GetOfferedMoney() != 0)
    {
        target->inventoryComp_->AddInventoryMoney(GetOfferedMoney(), theirMessage.get());
        owner_.inventoryComp_->RemoveInventoryMoney(GetOfferedMoney(), ourMessage.get());
    }
    if (target->tradeComp_->GetOfferedMoney() != 0)
    {
        target->inventoryComp_->RemoveInventoryMoney(target->tradeComp_->GetOfferedMoney(), theirMessage.get());
        owner_.inventoryComp_->AddInventoryMoney(target->tradeComp_->GetOfferedMoney(), ourMessage.get());
    }

    AB::Packets::Server::TradeAccepted packet{};
    ourMessage->AddByte(AB::GameProtocol::ServerPacketType::TradeAccepted);
    AB::Packets::Add(packet, *ourMessage);
    owner_.WriteToOutput(*ourMessage);

    theirMessage->AddByte(AB::GameProtocol::ServerPacketType::TradeAccepted);
    AB::Packets::Add(packet, *theirMessage);
    target->WriteToOutput(*theirMessage);

    Reset();
    target->tradeComp_->Reset();
}

void TradeComp::VisitOfferedItems(const std::function<Iteration(Item&, uint32_t)>& callback)
{
    auto& invComp = *owner_.inventoryComp_;
    for (auto itemPair : ourOffer_)
    {
        auto* item = invComp.GetInventoryItem(itemPair.first);
        if (!item)
            continue;
        if (callback(*item, itemPair.second) == Iteration::Break)
            break;
    }
}

TradeComp::TradeError TradeComp::TradeWith(std::shared_ptr<Player> target)
{
    if (!target)
        return TradeError::TargetInvalid;

    TradeError error = TestTarget(*target);
    if (error != TradeError::None)
        return error;

    target_ = target;
    if (target)
    {
        if (!CheckRange())
        {
            state_ = TradeState::MoveToTarget;
            MoveToTarget(target);
        }
        else
            StartTrading();
    }
    else
        state_ = TradeState::Idle;
    return TradeError::None;
}

void TradeComp::TradeReqeust(Player& source)
{
    target_ = source.GetPtr<Player>();
    state_ = TradeState::Trading;
}

void TradeComp::MoveToTarget(std::shared_ptr<Player> target)
{
    if (!owner_.autorunComp_->IsAutoRun())
    {
        if (owner_.autorunComp_->Follow(target, false, RANGE_PICK_UP))
        {
            owner_.stateComp_.SetState(AB::GameProtocol::CreatureState::Moving);
            owner_.autorunComp_->SetAutoRun(true);
        }
        else
            // No way to get to the target
            Reset();
    }
}

bool TradeComp::CheckRange()
{
    auto target = target_.lock();
    if (!target)
        return false;

    return owner_.GetDistance(target.get()) < RANGE_PICK_UP;
}

void TradeComp::StartTrading()
{
    if (auto target = target_.lock())
    {
        TradeError error = TestTarget(*target);
        if (error != TradeError::None)
        {
            auto msg = Net::NetworkMessage::GetNew();
            WriteError(error, *msg);
            owner_.WriteToOutput(*msg);
            Reset();
            return;
        }

        state_ = TradeState::Trading;
        target->tradeComp_->TradeReqeust(owner_);
        owner_.TriggerTradeDialog(target->id_);
    }
    else
        Reset();
}

void TradeComp::OnStuck()
{
    if (state_ == TradeState::MoveToTarget)
    {
        owner_.autorunComp_->Reset();
        owner_.stateComp_.Reset();
        Reset();
    }
}

void TradeComp::OnStateChange(AB::GameProtocol::CreatureState, AB::GameProtocol::CreatureState newState)
{
    if (state_ == TradeState::MoveToTarget)
    {
        if (newState != AB::GameProtocol::CreatureState::Moving)
            Reset();
    }
}

TradeComp::TradeError TradeComp::TestTarget(const Player& target)
{
    // It's not possible to trade with a partner that is queueing for a game,
    // because it may just disappear.
    if (target.IsQueueing())
        return TradeError::TargetQueing;
    if (target.tradeComp_->IsTrading())
    {
        if (target.tradeComp_->GetTradePartnerId() == owner_.id_)
            return TradeError::InProgress;
        return TradeError::TargetTrading;
    }

    return TradeError::None;
}

}
}
