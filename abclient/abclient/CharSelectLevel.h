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

#include "BaseLevel.h"
#include "AddAccountKeyDialog.h"

/// Character select
class CharSelectLevel final : public BaseLevel
{
    URHO3D_OBJECT(CharSelectLevel, BaseLevel)
public:
    CharSelectLevel(Context* context);
    void CreateCamera();
    SharedPtr<Window> characterWindow_;
protected:
    void SubscribeToEvents() override;
    void CreateUI() override;
    void SceneLoadingFinished() override;
private:
    void CreateScene() override;
    void HandleCharClicked(StringHash eventType, VariantMap& eventData);
    void HandleDeleteCharClicked(StringHash eventType, VariantMap& eventData);
    void HandleCreateCharClicked(StringHash eventType, VariantMap& eventData);
    void HandleBackClicked(StringHash eventType, VariantMap& eventData);
    void HandleUpdate(StringHash eventType, VariantMap& eventData);
    void HandleAddAccountKeyClicked(StringHash eventType, VariantMap& eventData);
    void HandleOptionsClicked(StringHash eventType, VariantMap& eventData);
    void HandleAccountKeyAdded(StringHash eventType, VariantMap& eventData);
    void HandleCharacterDeleted(StringHash eventType, VariantMap& eventData);
    void EnableButtons(bool enable);
};

