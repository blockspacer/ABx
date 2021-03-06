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

#include "PartyItem.h"

class Actor;
class Player;
class PartyItem;

enum class PartyWindowMode
{
    ModeOutpost,
    ModeGame
};

class PartyWindow : public Window
{
    URHO3D_OBJECT(PartyWindow, Window)
private:
    PartyWindowMode mode_{ PartyWindowMode::ModeOutpost };
    SharedPtr<Window> characterWindow_;
    SharedPtr<LineEdit> addPlayerEdit_;
    uint8_t partySize_{ 0 };
    SharedPtr<UIElement> memberContainer_;
    SharedPtr<UIElement> partyContainer_;
    SharedPtr<UIElement> addContainer_;
    SharedPtr<UIElement> inviteContainer_;
    SharedPtr<UIElement> invitationContainer_;
    Vector<SharedPtr<UIElement>> memberContainers_;
    WeakPtr<Player> player_;
    HashMap<uint32_t, WeakPtr<Actor>> members_;
    HashMap<uint32_t, WeakPtr<Actor>> invitees_;
    HashMap<uint32_t, WeakPtr<Actor>> invitations_;
    WeakPtr<GameObject> target_;
    uint32_t leaderId_{ 0 };
    uint32_t groupId_{ 0 };
    void UpdateEnterButton();
    void HandleAddTargetClicked(StringHash eventType, VariantMap& eventData);
    void HandleEnterButtonClicked(StringHash eventType, VariantMap& eventData);
    void HandleCloseClicked(StringHash eventType, VariantMap& eventData);
    void HandleLeaveButtonClicked(StringHash eventType, VariantMap& eventData);
    void HandleObjectSelected(StringHash eventType, VariantMap& eventData);
    void HandlePartyInvited(StringHash eventType, VariantMap& eventData);
    void HandlePartyAdded(StringHash eventType, VariantMap& eventData);
    void HandlePartyInviteRemoved(StringHash eventType, VariantMap& eventData);
    void HandlePartyRemoved(StringHash eventType, VariantMap& eventData);
    void HandleActorClicked(StringHash eventType, VariantMap& eventData);
    void HandleActorDoubleClicked(StringHash eventType, VariantMap& eventData);
    void HandleAcceptInvitationClicked(StringHash eventType, VariantMap& eventData);
    void HandleRejectInvitationClicked(StringHash eventType, VariantMap& eventData);
    void HandleKickClicked(StringHash eventType, VariantMap& eventData);
    void HandleObjectDespawn(StringHash eventType, VariantMap& eventData);
    void HandlePartyInfoMembers(StringHash eventType, VariantMap& eventData);
    void HandleLeaveInstance(StringHash eventType, VariantMap& eventData);
    void HandleTargetPinged(StringHash eventType, VariantMap& eventData);
    void HandleSelectTarget(StringHash eventType, VariantMap& eventData);
    void SubscribeEvents();
    void UpdateCaption();
    void UpdateAll();
    void ClearMembers();
    void ClearInvitations();
    void ClearInvites();
    void ShowError(const String& msg);
    bool IsFull() const { return members_.Size() >= partySize_; }
    bool IsInvited(uint32_t actorId) { return invitees_.Contains(actorId); }
    PartyItem* GetItem(uint32_t actorId);
    void AddItem(UIElement* container, SharedPtr<Actor> actor, MemberType type);
public:
    static void RegisterObject(Context* context);

    PartyWindow(Context* context);
    ~PartyWindow() override;

    void SetPlayer(SharedPtr<Player> player);
    void SetPartySize(uint8_t value);
    void SetMode(PartyWindowMode mode);
    void AddMember(SharedPtr<Actor> actor, unsigned pos = 0);
    void RemoveMember(uint32_t actorId);
    void AddInvitee(SharedPtr<Actor> actor);
    void AddInvitation(SharedPtr<Actor> leader);
    void RemoveInvite(uint32_t actorId);
    void RemoveInvitation(uint32_t actorId);
    void RemoveActor(uint32_t actorId);
    void Clear();
    void UnselectAll();
    bool SelectItem(uint32_t actorId);
    bool UnselectItem(uint32_t actorId);
    void OnObjectSpawned(GameObject* object, uint32_t groupId, uint8_t groupPos);
    bool IsLeader();
};

