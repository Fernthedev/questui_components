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

    struct ModalWrapper;

    using ModalPtrWrapper = std::shared_ptr<ModalWrapper>;
    using ModalCallback = std::function<void(ModalWrapper *, HMUI::ModalView *)>;

    struct ModalWrapper {
    public:
        const std::optional<UnityEngine::Vector2> sizeDelta;
        const std::optional<UnityEngine::Vector2> anchoredPosition;
        const bool dismissOnBlockerClicked;
        const ModalCallback callback;

        HMUI::ModalView* modalViewPtr;

        ModalWrapper(const std::optional<UnityEngine::Vector2> &sizeDelta = std::nullopt,
                     const std::optional<UnityEngine::Vector2> &anchoredPosition = std::nullopt,
                     bool dismissOnBlockerClicked = true, ModalCallback callback = {}) : sizeDelta(sizeDelta),
                                                                                  anchoredPosition(anchoredPosition),
                                                                                  dismissOnBlockerClicked(
                                                                                          dismissOnBlockerClicked),
                                                                                  callback(std::move(callback))
                                                                                  {}

        void dismiss() const {
            auto innerModal = modalViewPtr;

            if (!innerModal)
                throw std::runtime_error("Not rendered yet");

            innerModal->Hide(true, nullptr);
        }

        void show() const {
            auto innerModal = modalViewPtr;

            if (!innerModal)
                throw std::runtime_error("Not rendered yet");

            innerModal->Show(true, true, nullptr);
        }
    };

    template<class... TArgs>
    using ModalCreateFunc = std::function<std::tuple<TArgs...>(ModalWrapper &modal)>;


    template<class... TArgs> requires ((renderable<TArgs>&& ...))
    struct Modal : detail::Container<TArgs...> {
        const Key key;

        Modal(TArgs... children)
                : detail::Container<TArgs...>(children...) {}

        Modal(ModalPtrWrapper ptr, TArgs... children)
                : modalViewPtr(std::move(ptr)), detail::Container<TArgs...>(children...) {}

        UnityEngine::Transform *render(RenderContext &ctx, RenderContextChildData &data) {
            auto &innerModal = data.getData<HMUI::ModalView *>();
            // if inner modal is already created, skip recreating and forward render calls
            if (!innerModal) {
                std::function<void(HMUI::ModalView *)> cbk([callback = modalViewPtr->callback, this](HMUI::ModalView *arg) {
                    if (callback)
                        callback(modalViewPtr.get(), arg);
                });


                if (modalViewPtr->sizeDelta) {
                    if (modalViewPtr->anchoredPosition) {
                        innerModal = QuestUI::BeatSaberUI::CreateModal(&ctx.parentTransform, *modalViewPtr->sizeDelta,
                                                                       *modalViewPtr->anchoredPosition, cbk,
                                                                       modalViewPtr->dismissOnBlockerClicked);
                    } else {
                        innerModal = QuestUI::BeatSaberUI::CreateModal(&ctx.parentTransform, *modalViewPtr->sizeDelta, cbk,
                                                                       modalViewPtr->dismissOnBlockerClicked);
                    }
                } else {
                    innerModal = QuestUI::BeatSaberUI::CreateModal(&ctx.parentTransform, cbk,
                                                                   modalViewPtr->dismissOnBlockerClicked);
                }

                modalViewPtr->modalViewPtr = innerModal;
            }


            // TODO: If modal is hidden, should we rerender inner comps?
            RenderContext &childrenCtx = ctx.getChildContext<HMUI::ModalView>(key, [this, innerModal]() {
                return innerModal->get_transform();
            });
            detail::Container<TArgs...>::render(childrenCtx, data);

            return &childrenCtx.parentTransform;
        }


        [[nodiscard]] Modal clone() const {
            Modal m(ModalWrapper(this));
            m.innerModal = nullptr;

            m.children = detail::Container<TArgs...>::clone();
            return m;
        }

        void dismiss() const {
            modalViewPtr->dismiss();
        }

        void show() const {
            modalViewPtr->show();
        }

    protected:
        const ModalPtrWrapper modalViewPtr = std::make_shared<ModalWrapper>();
    };

    static_assert(renderable<Modal<Text>>);
    static_assert(cloneable < Modal < Text >> );
}
