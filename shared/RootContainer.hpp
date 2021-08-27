#pragma once

#include "Component.hpp"
#include <utility>
#include <vector>

namespace UnityEngine {
    class Transform;
}

namespace QuestUI_Components {
    /**
     * @brief Marks this class as a friend to QuestUI_Components::Component
     */
    class ComponentRenderer {
    protected:
        /**
         * @brief Used to cheat accessibility
         * @param comp
         */
        static inline UnityEngine::Transform* renderComponent(Component& comp, UnityEngine::Transform* transform) {
            return comp.render(transform);
        }

        /**
         * @brief Used to cheat accessibility
         * @param comp
         */
        static inline UnityEngine::Transform* renderComponent(ComponentWrapper& comp, UnityEngine::Transform* transform) {
            return comp->render(transform);
        }

        /**
         * @brief Used to cheat accessibility
         * @param comp
         */
        static inline UnityEngine::Transform* justRenderComponent(Component& comp, UnityEngine::Transform* transform) {
            return comp.render(transform);
        }

        /**
         * @brief Used to cheat accessibility
         * @param comp
         */
        static inline UnityEngine::Transform* justRenderComponent(ComponentWrapper& comp, UnityEngine::Transform* transform) {
            return comp->render(transform);
        }
    };
}