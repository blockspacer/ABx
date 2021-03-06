/**
 * Copyright 2017-2020 Stefan Ascher
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

#pragma once

#include <Urho3DAll.h>

using namespace Urho3D;

namespace Events {

URHO3D_EVENT(E_ACCOUNTKEYADDED, AccountKeyAdded)
{
}

URHO3D_EVENT(E_SERVERJOINED, ServerJoined)
{
    URHO3D_PARAM(P_SERVERID, ServerId);        // String
}

URHO3D_EVENT(E_SERVERLEFT, ServerLeft)
{
    URHO3D_PARAM(P_SERVERID, ServerId);        // String
}

URHO3D_EVENT(E_GOTSERVICES, GotServices)
{
}

URHO3D_EVENT(E_CHARACTERDELETED, CharacterDeleted)
{
    URHO3D_PARAM(P_UUID, Uuid);        // String
}

URHO3D_EVENT(E_CHANGINGINSTANCE, ChangingInstance)
{
    URHO3D_PARAM(P_UPDATETICK, UpdateTick);
    URHO3D_PARAM(P_SERVERUUID, ServerUuid);                // String
    URHO3D_PARAM(P_MAPUUID, MapUuid);                      // String
    URHO3D_PARAM(P_INSTANCEUUID, InstanceUuid);            // String
}

URHO3D_EVENT(E_LEAVEINSTANCE, LeaveInstance)
{
    URHO3D_PARAM(P_UPDATETICK, UpdateTick);
}

URHO3D_EVENT(E_OBJECTSPAWN, ObjectSpawn)
{
    URHO3D_PARAM(P_UPDATETICK, UpdateTick);
    URHO3D_PARAM(P_EXISTING, Existing);      // bool
    URHO3D_PARAM(P_OBJECTID, ObjectId);
    URHO3D_PARAM(P_OBJECTTYPE, ObjectType);
    URHO3D_PARAM(P_VALIDFIELDS, ValidFields);
    URHO3D_PARAM(P_POSITION, Position);
    URHO3D_PARAM(P_ROTATION, Rotation);
    URHO3D_PARAM(P_UNDESTROYABLE, Undestroyable);  // bool
    URHO3D_PARAM(P_SELECTABLE, Selectable);  // bool
    URHO3D_PARAM(P_SCALE, Scale);
    URHO3D_PARAM(P_STATE, State);            // AB::GameProtocol::CreatureState
    URHO3D_PARAM(P_SPEEDFACTOR, SpeedFactor);  // float
    URHO3D_PARAM(P_GROUPID, GroupId);  // uint32_t
    URHO3D_PARAM(P_GROUPPOS, GroupPos);  // uint32_t
    URHO3D_PARAM(P_GROUPMASK, GroupMask);  // uint32_t
    URHO3D_PARAM(P_DATA, Data);
}

URHO3D_EVENT(E_OBJECTDESPAWN, ObjectDespawn)
{
    URHO3D_PARAM(P_UPDATETICK, UpdateTick);
    URHO3D_PARAM(P_OBJECTID, ObjectId);
}

URHO3D_EVENT(E_OBJECTPOSUPDATE, ObjectPosUpdate)
{
    URHO3D_PARAM(P_UPDATETICK, UpdateTick);
    URHO3D_PARAM(P_OBJECTID, ObjectId);
    URHO3D_PARAM(P_POSITION, Position);
}

URHO3D_EVENT(E_OBJECTROTUPDATE, ObjectRotUpdate)
{
    URHO3D_PARAM(P_UPDATETICK, UpdateTick);
    URHO3D_PARAM(P_OBJECTID, ObjectId);
    URHO3D_PARAM(P_ROTATION, Rotation);
    URHO3D_PARAM(P_MANUAL, Manual);
}

URHO3D_EVENT(E_OBJECTSTATEUPDATE, ObjectStateUpdate)
{
    URHO3D_PARAM(P_UPDATETICK, UpdateTick);
    URHO3D_PARAM(P_OBJECTID, ObjectId);
    URHO3D_PARAM(P_STATE, State);
}

URHO3D_EVENT(E_OBJECTSPEEDUPDATE, ObjectSpeedUpdate)
{
    URHO3D_PARAM(P_UPDATETICK, UpdateTick);
    URHO3D_PARAM(P_OBJECTID, ObjectId);
    URHO3D_PARAM(P_SPEEDFACTOR, SpeedFactor);      // float
}

URHO3D_EVENT(E_OBJECTSETPOSITION, ObjectSetPosition)
{
    URHO3D_PARAM(P_UPDATETICK, UpdateTick);
    URHO3D_PARAM(P_OBJECTID, ObjectId);
    URHO3D_PARAM(P_POSITION, Position);
}

URHO3D_EVENT(E_OBJECTGROUPMASKCHAGED, ObjectGroupMaskChanged)
{
    URHO3D_PARAM(P_UPDATETICK, UpdateTick);
    URHO3D_PARAM(P_OBJECTID, ObjectId);
    URHO3D_PARAM(P_GROUPMASK, GroupMask);
}

URHO3D_EVENT(E_OBJECTSETATTACKSPEED, ObjectSetAttackSpeed)
{
    URHO3D_PARAM(P_UPDATETICK, UpdateTick);
    URHO3D_PARAM(P_OBJECTID, ObjectId);
    URHO3D_PARAM(P_SPEED, Speed);            // float
}

URHO3D_EVENT(E_OBJECTSELECTED, ObjectSelected)
{
    URHO3D_PARAM(P_UPDATETICK, UpdateTick);
    URHO3D_PARAM(P_SOURCEID, SourceId);
    URHO3D_PARAM(P_TARGETID, TargetId);
}

URHO3D_EVENT(E_SKILLFAILURE, SkillFailure)
{
    URHO3D_PARAM(P_UPDATETICK, UpdateTick);
    URHO3D_PARAM(P_OBJECTID, ObjectId);
    URHO3D_PARAM(P_SKILLINDEX, SkillIndex);
    URHO3D_PARAM(P_ERROR, Error);                      // uint8_t
    URHO3D_PARAM(P_ERRORMSG, ErrorMsg);                // String
}

URHO3D_EVENT(E_ATTACKFAILURE, AttackFailure)
{
    URHO3D_PARAM(P_UPDATETICK, UpdateTick);
    URHO3D_PARAM(P_OBJECTID, ObjectId);
    URHO3D_PARAM(P_ERROR, Error);                      // uint8_t
    URHO3D_PARAM(P_ERRORMSG, ErrorMsg);                // String
}

URHO3D_EVENT(E_PLAYERERROR, PlayerError)
{
    URHO3D_PARAM(P_UPDATETICK, UpdateTick);
    URHO3D_PARAM(P_ERROR, Error);                      // uint8_t
    URHO3D_PARAM(P_ERRORMSG, ErrorMsg);                // String
}

URHO3D_EVENT(E_PLAYERAUTORUN, PlayerAutorun)
{
    URHO3D_PARAM(P_UPDATETICK, UpdateTick);
    URHO3D_PARAM(P_AUTORUN, autorun);                      // bool
}

URHO3D_EVENT(E_OBJECTUSESKILL, ObjectUseSkill)
{
    URHO3D_PARAM(P_UPDATETICK, UpdateTick);
    URHO3D_PARAM(P_OBJECTID, ObjectId);
    URHO3D_PARAM(P_SKILLINDEX, SkillIndex);
    URHO3D_PARAM(P_ENERGY, Energy);
    URHO3D_PARAM(P_ADRENALINE, Adrenaline);
    URHO3D_PARAM(P_ACTIVATION, Activation);
    URHO3D_PARAM(P_OVERCAST, Overcast);
    URHO3D_PARAM(P_HPCOST, HPCost);
}

URHO3D_EVENT(E_OBJECTENDUSESKILL, ObjectEndUseSkill)
{
    URHO3D_PARAM(P_UPDATETICK, UpdateTick);
    URHO3D_PARAM(P_OBJECTID, ObjectId);
    URHO3D_PARAM(P_SKILLINDEX, SkillIndex);
    URHO3D_PARAM(P_RECHARGE, Recharge);
}

URHO3D_EVENT(E_OBJECTPINGTARGET, ObjectPingTarget)
{
    URHO3D_PARAM(P_UPDATETICK, UpdateTick);
    URHO3D_PARAM(P_OBJECTID, ObjectId);
    URHO3D_PARAM(P_TARGETID, TargetId);
    URHO3D_PARAM(P_CALLTTYPE, CallType);
    URHO3D_PARAM(P_SKILLINDEX, SkillIndex);
}

URHO3D_EVENT(E_OBJECTEFFECTADDED, ObjectEffectAdded)
{
    URHO3D_PARAM(P_UPDATETICK, UpdateTick);
    URHO3D_PARAM(P_OBJECTID, ObjectId);
    URHO3D_PARAM(P_EFFECTINDEX, EffectIndex);
    URHO3D_PARAM(P_TICKS, Ticks);
}

URHO3D_EVENT(E_OBJECTEFFECTREMOVED, ObjectEffectRemoved)
{
    URHO3D_PARAM(P_UPDATETICK, UpdateTick);
    URHO3D_PARAM(P_OBJECTID, ObjectId);
    URHO3D_PARAM(P_EFFECTINDEX, EffectIndex);
}

URHO3D_EVENT(E_OBJECTDAMAGED, ObjectDamaged)
{
    URHO3D_PARAM(P_UPDATETICK, UpdateTick);
    URHO3D_PARAM(P_OBJECTID, ObjectId);
    URHO3D_PARAM(P_DAMAGERID, DamagerId);
    URHO3D_PARAM(P_DAMAGETYPE, DamageType);
    URHO3D_PARAM(P_DAMAGEVALUE, DamageValue);
    URHO3D_PARAM(P_INDEX, Index);
}

URHO3D_EVENT(E_OBJECTHEALED, ObjectHealed)
{
    URHO3D_PARAM(P_UPDATETICK, UpdateTick);
    URHO3D_PARAM(P_OBJECTID, ObjectId);
    URHO3D_PARAM(P_HEALERID, HealerId);
    URHO3D_PARAM(P_INDEX, Index);
    URHO3D_PARAM(P_HEALVALUE, HealValue);
}

URHO3D_EVENT(E_OBJECTPROGRESS, ObjectProgress)
{
    URHO3D_PARAM(P_UPDATETICK, UpdateTick);
    URHO3D_PARAM(P_OBJECTID, ObjectId);
    URHO3D_PARAM(P_TYPE, Type);
    URHO3D_PARAM(P_VALUE, Value);
}

URHO3D_EVENT(E_OBJECTITEMDROPPED, ObjectItemDropped)
{
    URHO3D_PARAM(P_UPDATETICK, UpdateTick);
    URHO3D_PARAM(P_OBJECTID, ObjectId);
    URHO3D_PARAM(P_TARGETID, TargetId);
    URHO3D_PARAM(P_ITEMID, ItemId);
    URHO3D_PARAM(P_ITEMINDEX, ItemIndex);
    URHO3D_PARAM(P_COUNT, Count);
    URHO3D_PARAM(P_VALUE, Value);
    URHO3D_PARAM(P_STATS, Stats);
}

URHO3D_EVENT(E_OBJECTRESOURCECHANGED, ObjectResourceChanged)
{
    URHO3D_PARAM(P_UPDATETICK, UpdateTick);
    URHO3D_PARAM(P_OBJECTID, ObjectId);
    URHO3D_PARAM(P_RESTYPE, ResType);              // uint32_t
    URHO3D_PARAM(P_VALUE, Value);                  // int32_t
}

URHO3D_EVENT(E_SERVERMESSAGE, ServerMessage)
{
    URHO3D_PARAM(P_UPDATETICK, UpdateTick);
    URHO3D_PARAM(P_MESSAGETYPE, MessageType);
    URHO3D_PARAM(P_SENDER, Sender);
    URHO3D_PARAM(P_DATA, Data);
}

URHO3D_EVENT(E_CHATMESSAGE, ChatMessage)
{
    URHO3D_PARAM(P_UPDATETICK, UpdateTick);
    URHO3D_PARAM(P_MESSAGETYPE, MessageType);
    URHO3D_PARAM(P_SENDERID, SenderId);
    URHO3D_PARAM(P_SENDER, Sender);
    URHO3D_PARAM(P_DATA, Data);
}

URHO3D_EVENT(E_MAILINBOX, MailInbox)
{
}

URHO3D_EVENT(E_MAILREAD, MailRead)
{
}

URHO3D_EVENT(E_INVENTORY, Inventory)
{
}

URHO3D_EVENT(E_INVENTORYITEMUPDATE, InventoryItemUpdate)
{
    URHO3D_PARAM(P_UPDATETICK, UpdateTick);
    URHO3D_PARAM(P_ITEMPOS, ItemPos);     // unit16_t
}

URHO3D_EVENT(E_INVENTORYITEMDELETE, InventoryItemDelete)
{
    URHO3D_PARAM(P_UPDATETICK, UpdateTick);
    URHO3D_PARAM(P_ITEMPOS, ItemPos);     // unit16_t
}

URHO3D_EVENT(E_CHEST, Chest)
{
}

URHO3D_EVENT(E_CHESTITEMUPDATE, ChestItemUpdate)
{
    URHO3D_PARAM(P_UPDATETICK, UpdateTick);
    URHO3D_PARAM(P_ITEMPOS, ItemPos);     // unit16_t
}

URHO3D_EVENT(E_CHESTITEMDELETE, ChestItemDelete)
{
    URHO3D_PARAM(P_UPDATETICK, UpdateTick);
    URHO3D_PARAM(P_ITEMPOS, ItemPos);     // unit16_t
}

URHO3D_EVENT(E_PARTYINVITED, PartyInvited)
{
    URHO3D_PARAM(P_UPDATETICK, UpdateTick);
    URHO3D_PARAM(P_SOURCEID, SourceId);     // unit32_t
    URHO3D_PARAM(P_TARGETID, TargetId);     // unit32_t
    URHO3D_PARAM(P_PARTYID, PartyId);       // unit32_t
}

URHO3D_EVENT(E_PARTYREMOVED, PartyRemoved)
{
    URHO3D_PARAM(P_UPDATETICK, UpdateTick);
    URHO3D_PARAM(P_SOURCEID, SourceId);     // unit32_t             Leader
    URHO3D_PARAM(P_TARGETID, TargetId);     // unit32_t             Actor
    URHO3D_PARAM(P_PARTYID, PartyId);       // unit32_t
}

URHO3D_EVENT(E_PARTYADDED, PartyAdded)
{
    URHO3D_PARAM(P_UPDATETICK, UpdateTick);
    URHO3D_PARAM(P_PLAYERID, PlayerId);     // unit32_t
    URHO3D_PARAM(P_LEADERID, LeaderId);     // unit32_t
    URHO3D_PARAM(P_PARTYID, PartyId);       // unit32_t
}

URHO3D_EVENT(E_PARTYINVITEREMOVED, PartyInviteRemoved)
{
    URHO3D_PARAM(P_UPDATETICK, UpdateTick);
    URHO3D_PARAM(P_SOURCEID, SourceId);     // unit32_t
    URHO3D_PARAM(P_TARGETID, TargetId);     // unit32_t
    URHO3D_PARAM(P_PARTYID, PartyId);       // unit32_t
}

URHO3D_EVENT(E_PARTYRESIGNED, PartyResigned)
{
    URHO3D_PARAM(P_UPDATETICK, UpdateTick);
    URHO3D_PARAM(P_PARTYID, PartyId);       // unit32_t
    URHO3D_PARAM(P_PARTYNAME, PartyName);   // String
}

URHO3D_EVENT(E_PARTYDEFEATED, PartyDefeated)
{
    URHO3D_PARAM(P_UPDATETICK, UpdateTick);
    URHO3D_PARAM(P_PARTYID, PartyId);       // unit32_t
    URHO3D_PARAM(P_PARTYNAME, PartyName);   // String
}

URHO3D_EVENT(E_PARTYINFOMEMBERS, PartyInfoMembers)
{
    URHO3D_PARAM(P_PARTYID, PartyId);       // unit32_t
    URHO3D_PARAM(P_MEMBERS, Members);       // VariantVector
}

URHO3D_EVENT(E_NEWMAIL, NewMail)
{
    URHO3D_PARAM(P_COUNT, Count);           // int
}

URHO3D_EVENT(E_DIALOGGTRIGGER, DialogTrigger)
{
    URHO3D_PARAM(P_UPDATETICK, UpdateTick);
    URHO3D_PARAM(P_TIGGERERID, TriggererId);     // unit32_t
    URHO3D_PARAM(P_DIALOGID, DialogId);     // unit32_t
}

URHO3D_EVENT(E_QUESTSELECTIONDIALOGGTRIGGER, QuestSelectionDialogTrigger)
{
    URHO3D_PARAM(P_UPDATETICK, UpdateTick);
    URHO3D_PARAM(P_TIGGERERID, TriggererId);     // unit32_t
    URHO3D_PARAM(P_QUESTS, Quests);     // VariantVector
}

URHO3D_EVENT(E_QUESTDIALOGGTRIGGER, QuestDialogTrigger)
{
    URHO3D_PARAM(P_UPDATETICK, UpdateTick);
    URHO3D_PARAM(P_TIGGERERID, TriggererId);     // unit32_t
    URHO3D_PARAM(P_QUESTINDEX, QuestIndex);     // unit32_t
}

URHO3D_EVENT(E_NPCHASQUEST, NpcHasQuest)
{
    URHO3D_PARAM(P_UPDATETICK, UpdateTick);
    URHO3D_PARAM(P_OBJECTID, ObjectId);     // unit32_t
    URHO3D_PARAM(P_HASQUEST, HasQuest);     // bool
}

URHO3D_EVENT(E_QUESTDELETED, QuestDeleted)
{
    URHO3D_PARAM(P_UPDATETICK, UpdateTick);
    URHO3D_PARAM(P_INDEX, Index);     // unit32_t
    URHO3D_PARAM(P_DELETED, Deleted);     // bool
}

URHO3D_EVENT(E_QUESTREWARDED, QuestRewarded)
{
    URHO3D_PARAM(P_UPDATETICK, UpdateTick);
    URHO3D_PARAM(P_INDEX, Index);     // unit32_t
    URHO3D_PARAM(P_REWARDED, Rewarded);     // bool
}

URHO3D_EVENT(E_GOT_FRIENDLIST, GotFriendList)
{
}

URHO3D_EVENT(E_FRIENDADDED, FriendAdded)
{
    URHO3D_PARAM(P_ACCOUNTUUID, AccountUuid);
    URHO3D_PARAM(P_RELATION, Relation);
}

URHO3D_EVENT(E_FRIENDREMOVED, FriendRemoved)
{
    URHO3D_PARAM(P_ACCOUNTUUID, AccountUuid);
    URHO3D_PARAM(P_RELATION, Relation);
}

URHO3D_EVENT(E_FRIENDRENAMED, FriendRenamed)
{
    URHO3D_PARAM(P_ACCOUNTUUID, AccountUuid);
    URHO3D_PARAM(P_RELATION, Relation);
    URHO3D_PARAM(P_NEWNAME, NewName);
}

URHO3D_EVENT(E_GOT_PLAYERINFO, GotPlayerInfo)
{
    URHO3D_PARAM(P_ACCOUNTUUID, AccountUuid);
}

URHO3D_EVENT(E_GOT_GUILDMEMBERS, GotGuildMembers)
{
}

URHO3D_EVENT(E_SET_ATTRIBUTEVALUE, SetAttributeValue)
{
    URHO3D_PARAM(P_OBJECTID, ObjectId);
    URHO3D_PARAM(P_ATTRIBINDEX, AttribIndex);  // uint32_t
    URHO3D_PARAM(P_VALUE, Value);              // int8_t
    URHO3D_PARAM(P_REMAINING, Remaining);      // uint8_t
}

URHO3D_EVENT(E_SET_SECPROFESSION, SetSecProfession)
{
    URHO3D_PARAM(P_OBJECTID, ObjectId);
    URHO3D_PARAM(P_PROFINDEX, ProfIndex);  // uint32_t
}

URHO3D_EVENT(E_SET_SKILL, SetSkill)
{
    URHO3D_PARAM(P_OBJECTID, ObjectId);
    URHO3D_PARAM(P_SKILLINDEX, SkillIndex);   // uint32_t
    URHO3D_PARAM(P_SKILLPOS, Pos);            // uint8_t
}

URHO3D_EVENT(E_LOAD_SKILLTEMPLATE, LoadSkillTemplate)
{
    URHO3D_PARAM(P_OBJECTID, ObjectId);
    URHO3D_PARAM(P_TEMPLATE, Template);   // String
}

URHO3D_EVENT(E_TRADEDIALOG_TRIGGER, TradeDialogTrigger)
{
    URHO3D_PARAM(P_SOURDEID, SourceId);
    URHO3D_PARAM(P_TARGETID, TargetId);
}

URHO3D_EVENT(E_TRADECANCEL, TradeCancel)
{
}

URHO3D_EVENT(E_TRADEOFFER, TradeOffer)
{
}

URHO3D_EVENT(E_TRADEACCEPTED, TradeAccepted)
{
}

URHO3D_EVENT(E_MERCHANT_ITEMS, MerchantItems)
{
}

URHO3D_EVENT(E_ITEM_PRICE, ItemPrice)
{
    URHO3D_PARAM(P_ITEMPOS, ItemPos);
    URHO3D_PARAM(P_PRICE, Price);
}

URHO3D_EVENT(E_CRAFTSMAN_ITEMS, CraftsmanItems)
{
}

URHO3D_EVENT(E_DROPTARGET_CHANGED, DropTargetChanged)
{
    URHO3D_PARAM(P_OBJECTID, ObjectId);
    URHO3D_PARAM(P_TARGETID, TargetId);
}

URHO3D_EVENT(E_POSITION_PINGED, PositionPinged)
{
    URHO3D_PARAM(P_OBJECTID, ObjectId);
    URHO3D_PARAM(P_POSITION, Position);
}

}
