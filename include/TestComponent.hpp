#pragma once

#include "Component.hpp"
#include "RootContainer.hpp"

#include "components/Text.hpp"
#include "components/layouts/MultiComponentGroup.hpp"

#include <utility>
#include <vector>
#include <string>


namespace UnityEngine {
    class Transform;
}

namespace QuestUI_Components {
    class TestComponent : public Component {
    public:
        explicit TestComponent(std::string_view prefix);

    protected:
        Component* render(UnityEngine::Transform *parentTransform) override;

        // Store as a shared_ptr because parent wil NOT take ownership of this.
        // However, it will take ownership of us
        std::shared_ptr<MultiComponentGroup> group;
    };
}