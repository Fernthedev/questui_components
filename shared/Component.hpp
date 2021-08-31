#pragma once

#include <optional>
#include <memory>
#include <utility>
#include <string>

namespace UnityEngine {
    class Transform;
}

namespace QuestUI_Components {
    class ComponentPtrWrapper;

#define CONSTRUCT_AFTER_COMPONENT(clazz) \
virtual clazz* with(std::function<void(clazz *)> const& withCallback) { \
    withCallback(reinterpret_cast<clazz*>(this));                         \
    return reinterpret_cast<clazz*>(this);                            \
}


    class Component {
    protected:
        friend class ComponentRenderer;

        bool rendered = false;

        virtual void markAsRendered() {
            rendered = true;
        }

        /**
         * @brief Use this method to ensure your render isn't called twice
         * However, this may not be necessary as ComponentRenderer by default will not render
         * a rendered component again.
         */
        virtual void ensureRenderOnce() {
            if (rendered) {
                throw std::runtime_error("Component " + std::string(typeid(this).name()) + " has already been rendered");
            }
        }

        UnityEngine::Transform* transform;

        // TODO: Rename to create?
        /**
         * @brief This is called on the main thread when the component will be placed in game.
         *
         * Use the constructor to initialize components on a different thread.
         *
         * @param parentTransform The transform of the parent component that is in game
         * @return The component that was rendered. Return this or another component.
         * The parent component is not guaranteed to take ownership of what is returned however.
         *
         * If the component needs updating, you will also have to manually update it by inheriting UpdateableComponentBase or a child of it.
         */
        virtual Component* render(UnityEngine::Transform* parentTransform) = 0;
    public:
        virtual ~Component() = default;

        [[nodiscard]] UnityEngine::Transform* getTransform() const { return transform; }

        [[nodiscard]] bool isRendered() const { return rendered; }
    };

    /**
     * @brief Extend this class to allow state functionality.
     * Components by default are stateless and static.
     */
    class UpdateableComponentBase {
    protected:
        /**
         * @brief Called when a update is deemed require by  UpdateableComponentBase.doUpdate
         */
        virtual void update() = 0;
    public:
        /**
         * @brief Use this method to update the component
         * @remark A public proxy to UpdateableComponentBase.update which can be used to only update when it is required.
         */
        virtual void doUpdate() {
            update();
        };
    };

    template<class T>
    class UpdateableComponent : public UpdateableComponentBase {
    protected:
        T data;

        // allow one initial update
        bool updated = true;

        /**
         * @brief Called when a update is deemed require by  UpdateableComponentBase.doUpdate
         */
        void update() override = 0;
    public:
        T getData() {
            return data;
        }

        // TODO: Remove?
        [[deprecated("Use the functional mutateData instead, it's nicer <3")]]
        void mutateData(T newData) {
            data = std::move(newData);
            updated = true;
        }

        void mutateData(std::function<T(T)> updateLambda) {
            data = updateLambda(data);
            updated = true;
        }

        /**
         * @brief Use this method to update the component
         * @remark A public proxy to UpdateableComponentBase.update which can be used to only update when it is required.
         */
        void doUpdate() override {
            if (!updated) return;

            update();
            updated = false;
        }
    };


    /**
     * @brief A wrapper for shared_ptr<Component> to allow implicit conversion
     * Note this will take ownership of a Component if it's not a shared_ptr
     */
    class ComponentPtrWrapper {
    private:
        /**
         * @brief the underlying component held
         */
        std::shared_ptr<Component> component;

    public:
//        template<class F>
//        ComponentPtrWrapper(F&& comp) : component(std::forward(comp)) {}

        ComponentPtrWrapper(Component* comp) : component(comp) {}

        ComponentPtrWrapper(std::function<Component*()> functionalInit) : component(functionalInit()) {}

        [[nodiscard]] const std::shared_ptr<Component> &getComponent() const {
            return component;
        }

        explicit operator bool() const noexcept {
            return static_cast<bool>(component);
        }

        [[nodiscard]]  Component& operator*() const noexcept {
            return *component;
        }

        [[nodiscard]]  Component* operator->() const noexcept {
            return component.get();
        }

        bool operator==(const ComponentPtrWrapper& other) const {
            return component.get() == other.component.get();
        }
    };


//    using ComponentWrapper = std::shared_ptr<Component>;
        using ComponentWrapper = ComponentPtrWrapper;
}

template<>
struct std::hash<QuestUI_Components::ComponentPtrWrapper> {
    [[nodiscard]] size_t operator()(const QuestUI_Components::ComponentPtrWrapper& comp) const noexcept {
        return std::hash<QuestUI_Components::Component*>()(comp.getComponent().get());
    }
};