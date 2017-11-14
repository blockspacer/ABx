#pragma once

#pragma warning( push )
#pragma warning( disable : 4100)
#include <Urho3D/Urho3DAll.h>
#pragma warning( pop )

#include "BaseLevel.h"

class LoginLevel : public BaseLevel
{
    URHO3D_OBJECT(LoginLevel, BaseLevel);
public:
    /// Construct.
    LoginLevel(Context* context);
    virtual void CreatePlayer(const Vector3& position, const Quaternion& direction) override;

protected:
    virtual void SubscribeToEvents();
    virtual void CreateUI();

private:
    bool loggingIn_;
    LineEdit* nameEdit_;
    LineEdit* passEdit_;
    Button* button_;
    void CreateScene();
    void HandleLoginClicked(StringHash eventType, VariantMap& eventData);
    void HandleUpdate(StringHash eventType, VariantMap& eventData);
    void HandleKeyUp(StringHash eventType, VariantMap& eventData);
    void HandleKeyDown(StringHash eventType, VariantMap& eventData);
    void DoLogin();
};
