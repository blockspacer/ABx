#pragma once

#include <Urho3DAll.h>

class Spinner : public BorderImage
{
    URHO3D_OBJECT(Spinner, BorderImage)
private:
    int min_{ 0 };
    int max_{ 100 };
    int value_{ 0 };
    unsigned step_{ 1 };
    WeakPtr<LineEdit> edit_;
    void HandleMouseWheel(StringHash eventType, VariantMap& eventData);
    void HandleIncreaseClicked(StringHash eventType, VariantMap& eventData);
    void HandleDecreaseClicked(StringHash eventType, VariantMap& eventData);
    void Validate();
public:
    static void RegisterObject(Context* context);

    Spinner(Context* context);
    ~Spinner() override;

    int GetMin() const { return min_; }
    void SetMin(int value);
    int GetMax() const { return max_; }
    void SetMax(int value);
    int GetValue() const { return value_; }
    void SetValue(int value);
    unsigned GetStep() const { return step_; }
    void SetStep(unsigned value) { step_ = value; }
    void Increase();
    void Decrease();
    void SetEdit(SharedPtr<LineEdit> value);
    bool HaveFocus() const;
};