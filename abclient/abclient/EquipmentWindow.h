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

class Player;
class PostProcessController;

class EquipmentWindow : public Window
{
    URHO3D_OBJECT(EquipmentWindow, Window)
private:
    bool modelLoaded_{ false };
    SharedPtr<Scene> modelScene_;
    SharedPtr<Node> characterNode_;
    SharedPtr<View3D> modelViewer_;
    SharedPtr<AnimationController> animController_;
    bool mouseDown_{ false };
    bool initialized_{ false };
    void SubscribeEvents();
    void HandleCloseClicked(StringHash eventType, VariantMap& eventData);
    void HandleLoadClicked(StringHash eventType, VariantMap& eventData);
    void HandleSaveClicked(StringHash eventType, VariantMap& eventData);
    void HandleCreateClicked(StringHash eventType, VariantMap& eventData);
    void HandleSceneViewerMouseMove(StringHash eventType, VariantMap& eventData);
    void HandleSceneViewerMouseDown(StringHash eventType, VariantMap& eventData);
    void HandleSceneViewerMouseUp(StringHash eventType, VariantMap& eventData);
    bool LoadObject(uint32_t itemIndex, Node* node);
public:
    EquipmentWindow(Context* context);
    ~EquipmentWindow() override;

    void UpdateEquipment(Player* player);
    void Initialize(PostProcessController& pp);
};
