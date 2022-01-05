#pragma once

#include "shared/RootContainer.hpp"
#include "shared/context.hpp"
#include "shared/unity/WeakPtrGO.hpp"

#include "questui/shared/BeatSaberUI.hpp"

#include "UnityEngine/Vector2.hpp"
#include <functional>
#include <utility>

namespace HMUI {
    class ModalView;
}

namespace QUC {

    struct ModalWrapper {
    public:
        using ModalCallback = std::function<void(ModalWrapper*, HMUI::ModalView*)>;

        const UnityEngine::Vector2 sizeDelta;
        const UnityEngine::Vector2 anchoredPosition;
        const bool dismissOnBlockerClicked;
        const ModalCallback callback;

        ModalWrapper(const UnityEngine::Vector2 &sizeDelta, const UnityEngine::Vector2 &anchoredPosition,
                     bool dismissOnBlockerClicked, ModalCallback callback = {}) : sizeDelta(sizeDelta),
                                                                                    anchoredPosition(anchoredPosition),
                                                                                    dismissOnBlockerClicked(
                                                                                            dismissOnBlockerClicked),
                                                                                    callback(std::move(callback)) {}

        void dismiss() {
            if (!innerModal)
                throw std::runtime_error("Not rendered yet");

            innerModal->Hide(true, nullptr);
        }

        void show() {
            if (!innerModal)
                throw std::runtime_error("Not rendered yet");

            innerModal->Show(true, true, nullptr);
        }

    protected:
        WeakPtrGO<HMUI::ModalView> innerModal;
    };

    template<class... TArgs>
    using ModalCreateFunc = std::function<std::tuple<TArgs...>(ModalWrapper& modal)>;

    template<class... TArgs>
    requires ((renderable<TArgs> && ...))
    struct Modal : detail::Container<TArgs...>, public ModalWrapper {

        const Key key;

        template<class ModalCreateFunc2 = ModalCreateFunc<TArgs...>>
        Modal(ModalCreateFunc2 childrenCallback, ModalCallback callable = nullptr,
              UnityEngine::Vector2 sz = {30.0f, 40.0f}, UnityEngine::Vector2 anch = {0.0f, 0.0f}, bool dismiss = true)
                : detail::Container<TArgs...>(childrenCallback(*this)),
                        ModalWrapper(sz, anch, dismiss, callable)
                        {}

        // TODO: How to make children a second parameter with the sizes default?
        template<class F>
        Modal(F callable, std::tuple<TArgs...> children, UnityEngine::Vector2 sz = {30.0f, 40.0f}, UnityEngine::Vector2 anch = {0.0f, 0.0f}, bool dismiss = true)
            : detail::Container<TArgs...>(children), ModalWrapper(sz, anch, dismiss, callable) {}

        UnityEngine::Transform* render(RenderContext& ctx, RenderContextChildData& data) {
            innerModal = data.getData<HMUI::ModalView*>();
            // if inner modal is already created, skip recreating and forward render calls
            if (!innerModal) {
                std::function<void(HMUI::ModalView*)> cbk([this](HMUI::ModalView* arg) {
                    callback(this, arg);
                });

                // TODO: Add proper tree recaching on parent context.
                innerModal = QuestUI::BeatSaberUI::CreateModal(&ctx.parentTransform, sizeDelta, anchoredPosition, cbk,
                                                               dismissOnBlockerClicked);
            }


            // TODO: If modal is hidden, should we rerender inner comps?
            RenderContext& childrenCtx = ctx.getChildContext<HMUI::ModalView>(key, [this]() {
                return innerModal->get_transform();
            });
            detail::Container<TArgs...>::render(childrenCtx, data);

            return &childrenCtx.parentTransform;
        }


        [[nodiscard]] Modal clone() const {
            Modal m(this);
            m.innerModal = nullptr;
            m.children = detail::Container<TArgs...>::clone();
            return m;
        }
    };

    template<class... TArgs>
    Modal(ModalCreateFunc<TArgs...> childrenCallback) -> Modal<typename std::result_of_t<ModalCreateFunc<TArgs...>>>;

    template<class... TArgs>
    Modal(std::tuple<TArgs...>(*)(QUC::ModalWrapper &)) -> Modal<TArgs...>;

//    template<class... TArgs>
//    requires ((renderable<TArgs> && ...))
//    auto modalTT(ModalCreateFunc<TArgs...> childrenCallback) -> ModalCreateFunc<TArgs...>
//    {
//        return childrenCallback;
//    }
//
//    template<class... TArgs>
//    requires ((renderable<TArgs> && ...))
//    auto modalT(ModalCreateFunc<TArgs...> childrenCallback) -> ModalCreateFunc<TArgs...>
//    {
//        return Modal<TArgs...>(childrenCallback);
//    }

    static_assert(renderable<Modal<Text>>);
    static_assert(cloneable<Modal<Text>>);
}
