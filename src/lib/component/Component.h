#pragma once

class Object;

class Component {
public:
    Object* owner = nullptr;

    virtual ~Component() = default;

    virtual void OnAdded() {}
    virtual void OnRemoved() {}

    virtual void Update(float dt) {}

    template<typename T>
    bool IsType() const
    {
        return dynamic_cast<const T*>(this) != nullptr;
    }
};
