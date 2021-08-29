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
        static UnityEngine::Transform* renderComponent(ComponentWrapper& comp, UnityEngine::Transform* transform) {
            if (comp->isRendered()) {
                if (auto updateableComponent = std::dynamic_pointer_cast<UpdateableComponentBase>(comp.getComponent())) {
                    updateableComponent->doUpdate();
                }

                return comp->transform;
            } else {
                return justRenderComponent(comp, transform);
            }
        }

        /**
         * @brief Used to cheat accessibility
         * @param comp
         */
        static inline UnityEngine::Transform* justRenderComponent(ComponentWrapper& comp, UnityEngine::Transform* transform) {
            return comp->render(transform);
        }
    };

    class Container : public ComponentRenderer {
    public:
        virtual void addMultipleToHierarchy(std::vector<ComponentWrapper> components) {
            for (auto& component : components) {
                renderComponentInContainer(component);
                renderChildren.emplace_back(component);
            }
        }

        inline void addToHierarchy(ComponentWrapper component) {
            addMultipleToHierarchy({std::move(component)});
        }

        Container() = default;

    protected:
        Container(std::initializer_list<ComponentWrapper> children) : renderChildren(children) {}

        // Keep children alive
        std::vector<ComponentWrapper> renderChildren;

        virtual void renderComponentInContainer(ComponentWrapper& comp) = 0;
    };
}