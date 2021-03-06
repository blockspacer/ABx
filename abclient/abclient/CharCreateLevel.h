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

/// Character creation
class CharCreateLevel final : public BaseLevel
{
    URHO3D_OBJECT(CharCreateLevel, BaseLevel)
public:
    CharCreateLevel(Context* context);
    void CreateCamera();
protected:
    void SubscribeToEvents() override;
    void CreateUI() override;
    void SceneLoadingFinished() override;
private:
    SharedPtr<LineEdit> nameEdit_;
    SharedPtr<DropDownList> professionDropdown_;
    SharedPtr<DropDownList> sexDropdown_;
    SharedPtr<CheckBox> pvpCheckbox_;
    SharedPtr<Button> createButton_;
    SharedPtr<Button> cancelButton_;
    void DoCreateCharacter();
    void DoCancel();
    void CreateScene() override;
    Text* CreateDropdownItem(const String& text, uint32_t value);
    Text* CreateDropdownItem(const String& text, const String& value);
    void HandleUpdate(StringHash eventType, VariantMap& eventData);
    void HandleCreateClicked(StringHash eventType, VariantMap& eventData);
    void HandleCancelClicked(StringHash eventType, VariantMap& eventData);
};

