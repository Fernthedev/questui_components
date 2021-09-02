#pragma once

#include "Component.hpp"

#include "UnityEngine/Object.hpp"
#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/Transform.hpp"

#include "Logger.hpp"

#include <utility>
#include <vector>

namespace QuestUI_Components {
    /**
     * @brief Marks this class as a friend to QuestUI_Components::Component
     */
    class ComponentRenderer {
        // according to scad, blame him if this breaks
        static const int MAIN_THREAD_ID = 1;

    protected:
        /**
         * @brief Renders or updates the component
         * @param comp
         */
        static Component* renderComponent(ComponentWrapper& comp, UnityEngine::Transform* transform) {

             // TODO: THROW EXCEPTION?
//            if (std::this_thread::get_id() != MAIN_THREAD_ID) {
//                getLogger().warning("Rendering components should only be done from the main thread, you've been warned!");
//            }
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
            if (comp->isRendered()) return comp;

            auto compRendered = comp->render(transform);

            // TODO: Should we do this?
            // Recursively render.
            if (compRendered != comp) {
                return justRenderComponent(compRendered, transform);
            }

            return compRendered;
        }
    };

    /**
     * @brief A abstract class of ComponentRenderer who's main purpose is to handle ownership of child Component and rendering them.
     */
    class Container : public ComponentRenderer {
    public:
        /**
         * @brief Add to UI and ownership
         * @param components list of components to add
         */
        virtual void addMultipleToHierarchy(std::vector<ComponentWrapper> components) {
            for (auto& component : components) {
                if (!renderChildrenSet.contains(component)) {
                    renderComponentInContainer(component);
                    renderChildren.emplace_back(component);
                    renderChildrenSet.emplace(component);
                }
            }
        }

        /**
         * @brief Destroy all children in both UI and ownership
         */
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

        /**
         * @brief Removes the components from UI and ownership
         * @param components
         */
        virtual void removeMultipleFromHierarchy(std::vector<ComponentWrapper> const& components) {
            for (auto &component: components) {

                //man this hurts, so much waste
                auto itSet = renderChildrenSet.find(component);
                if (itSet != renderChildrenSet.end()) {
                    auto transform = component->getTransform();
                    std::erase(renderChildren, component);
                    renderChildrenSet.erase(itSet);
                    if (transform) {
                        UnityEngine::Object::Destroy(transform->get_gameObject());
                    }
                    break;
                }
            }
        }

        /**
         * @brief Removes a single component from ownership and UI
         * @param component
         */
        inline void removeFromHierarchy(ComponentWrapper component) {
            removeMultipleFromHierarchy({std::move(component)});
        }

        /**
         * @brief Adds a single component to UIand takes ownership
         * @param component
         */
        inline void addToHierarchy(ComponentWrapper component) {
            addMultipleToHierarchy({std::move(component)});
        }

        Container() = default;

        virtual ~Container() = default;

    protected:
        Container(std::initializer_list<ComponentWrapper> children) : renderChildren(children), renderChildrenSet(children) {

        }

        /**
         * @brief Method for rendering component in child
         * @param comp Component to render
         */
        virtual void renderComponentInContainer(ComponentWrapper& comp) = 0;

        /**
         * @brief Utility for rendering children
         * @param transform
         */
        void doRenderChildren(UnityEngine::Transform *transform) {
            for (auto& comp : renderChildren)
                renderComponent(comp, transform);
        }

    public:
        [[nodiscard]] const std::vector<ComponentWrapper> &getRenderChildren() const {
            return renderChildren;
        }

    private:
        // Keep children alive
        std::vector<ComponentWrapper> renderChildren;

        // To ensure no duplicates.
        std::unordered_set<ComponentWrapper> renderChildrenSet;
    };

    /**
     * @brief A base class implementation of Container with UpdateComponent and Component
     * @tparam UpdateComponent
     * @tparam CompParent
     */
    template<typename UpdateComponent, typename CompParent = Component>
    class IBaseContainer : public Container, public CompParent, public UpdateComponent {
    public:
        IBaseContainer() : Container() {}


    protected:
        IBaseContainer(std::initializer_list<ComponentWrapper> children) : Container(children) {

        }

        /**
         * @brief Updates the children of the container on update.
         */
        void update() override {
            doRenderChildren(this->transform);
        }

        /**
         * @brief Method for rendering component in child
         * @param comp Component to render
         */
        void renderComponentInContainer(ComponentWrapper &comp) override {
            if (this->rendered) {
                renderComponent(comp, const_cast<UnityEngine::Transform*>(this->transform));
            }
        }
    };

    using BaseContainer = IBaseContainer<UpdateableComponentBase, Component>;

}