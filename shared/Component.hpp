#pragma once

#include <optional>
#include <memory>
#include <utility>

namespace UnityEngine {
    class Transform;
}

namespace QuestUI_Components {
    class ComponentPtrWrapper;

#define CONSTRUCT_AFTER_COMPONENT(clazz) \
virtual clazz* craftLater(std::function<void(clazz *)> const& callback) { \
    callback(this);                         \
    return this;                            \
}


    class Component {
    protected:
        friend class ComponentRenderer;

        UnityEngine::Transform* transform;

        /**
         * @brief
         * @param parentTransform
         * @return
         */
        virtual UnityEngine::Transform* render(UnityEngine::Transform* parentTransform) = 0;
    public:
        virtual ~Component() = default;

        [[nodiscard]] UnityEngine::Transform* getTransform() const { return transform; }
    };

    // TODO: What to do with this?
    class IRenderOnce {
    protected:
        friend class ComponentRenderer;

        bool rendered = false;

        virtual void markAsRendered() {
            rendered = true;
        }
    public:
        [[nodiscard]] bool isRendered() const { return rendered; }
    };

    template<class T>
    class UpdateableComponent {
    protected:
        T data;

        // allow one initial update
        bool updated = true;

        virtual void update() = 0;
    public:
        T getData() {
            return data;
        }

        void mutateData(T newData) {
            data = std::move(newData);
            updated = true;
        }

        virtual void doUpdate() {
            if (!updated) return;

            update();
            updated = false;
        }
    };


    class ComponentPtrWrapper {
    public:
//        template<class F>
//        ComponentPtrWrapper(F&& comp) : component(std::forward(comp)) {}

        ComponentPtrWrapper(Component* comp) : component(comp) {}

        const std::shared_ptr<Component> component;

        explicit operator bool() const noexcept {
            return static_cast<bool>(component);
        }

        [[nodiscard]]  Component& operator*() const noexcept {
            return *component;
        }

        [[nodiscard]]  Component* operator->() const noexcept {
            return component.get();
        }
    };

//    using ComponentWrapper = std::shared_ptr<Component>;
        using ComponentWrapper = ComponentPtrWrapper;
}
