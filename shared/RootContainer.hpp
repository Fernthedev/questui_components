#pragma once

#include "Component.hpp"

#include "UnityEngine/Object.hpp"
#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/Transform.hpp"

#include <utility>
#include <vector>

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
        static Component* renderComponent(ComponentWrapper& comp, UnityEngine::Transform* transform) {
            if (comp->isRendered()) {
                if (auto updateableComponent = std::dynamic_pointer_cast<UpdateableComponentBase>(comp.getComponent())) {
                    updateableComponent->doUpdate();
                }

                return comp.getComponent().get();
            } else {
                return justRenderComponent(comp.getComponent().get(), transform);
            }
        }

        /**
         * @brief Used to cheat accessibility
         * @param comp
         */
        static inline Component* justRenderComponent(Component* comp, UnityEngine::Transform* transform) {
            auto compRendered = comp->render(transform);

            // TODO: Should we do this?
            // Recursively render.
            if (compRendered != comp) {
                return justRenderComponent(compRendered, transform);
            }

            return compRendered;
        }
    };

    class Container : public ComponentRenderer {
    public:
        virtual void addMultipleToHierarchy(std::vector<ComponentWrapper> components) {
            for (auto& component : components) {
                if (!renderChildrenSet.contains(component)) {
                    renderComponentInContainer(component);
                    renderChildren.emplace_back(component);
                    renderChildrenSet.emplace(component);
                }
            }
        }

        virtual void destroyAll() {
            for (auto& component : renderChildrenSet) {
                auto transform = component->getTransform();

                if (transform) {
                    UnityEngine::Object::Destroy(transform->get_gameObject());
                }
            }

            renderChildrenSet.clear();
            renderChildren.clear();
            renderChildren.shrink_to_fit();
        }

        virtual void removeMultipleFromHierarchy(std::vector<ComponentWrapper> const& components) {
            for (auto &component: components) {

                //man this hurts, so much waste
                auto itSet = renderChildrenSet.find(component);
                if (itSet != renderChildrenSet.end()) {
                    auto transform = component->getTransform();
                    std::erase(renderChildren, component);
                    renderChildrenSet.erase(itSet);
                    if (transform) {
                        UnityEngine::Object::Destroy(transform);
                    }
                    break;
                }
            }
        }

        inline void removeFromHierarchy(ComponentWrapper component) {
            removeMultipleFromHierarchy({std::move(component)});
        }

        inline void addToHierarchy(ComponentWrapper component) {
            addMultipleToHierarchy({std::move(component)});
        }

        Container() = default;

        virtual ~Container() = default;

    protected:
        Container(std::initializer_list<ComponentWrapper> children) : renderChildren(children), renderChildrenSet(children) {

        }

        virtual void renderComponentInContainer(ComponentWrapper& comp) = 0;

        void doRenderChildren(UnityEngine::Transform *transform) {
            for (auto& comp : renderChildren)
                renderComponent(comp, transform);
        }


    private:
        // Keep children alive
        std::vector<ComponentWrapper> renderChildren;

        // To ensure no duplicates.
        std::unordered_set<ComponentWrapper> renderChildrenSet;
    };

    template<typename UpdateComponent, typename CompParent = Component>
    class IBaseContainer : public Container, public CompParent, public UpdateComponent {
    public:
        IBaseContainer() : Container() {}


    protected:
        IBaseContainer(std::initializer_list<ComponentWrapper> children) : Container(children) {

        }

        void update() override {
            doRenderChildren(this->transform);
        }
    };

    using BaseContainer = IBaseContainer<UpdateableComponentBase, Component>;

}