#pragma once

#include "shared/context.hpp"
#include "shared/unity/WeakPtrGO.hpp"
#include <string>
#include <string_view>

#include "questui/shared/CustomTypes/Components/Backgroundable.hpp"

#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/Transform.hpp"
#include "UnityEngine/UI/ContentSizeFitter.hpp"
#include "UnityEngine/RectTransform.hpp"

namespace QUC {
    namespace detail {
        template<class T>
        requires (renderable_return<T, UnityEngine::Transform*>)
        struct Backgroundable {
            static_assert(renderable<Backgroundable>);

            Backgroundable(std::string_view bkgType, bool replace, T&& child_)
                : backgroundType(bkgType), replaceExisting(replace), child(child_) {}
            auto render(RenderContext& ctx) {
                auto res = child.render(ctx);
                auto go = res->get_gameObject();

                if (replaceExisting || !backgroundable) {
                    backgroundable = go->template AddComponent<QuestUI::Backgroundable*>();
                }

                backgroundable->ApplyBackground(il2cpp_utils::newcsstr(backgroundType));

                return backgroundable->get_transform();
            }
            private:
            const std::string backgroundType;
            const bool replaceExisting;
            T child;
            WeakPtrGO<QuestUI::Backgroundable> backgroundable;
        };
    }
    template<class T>
    auto Backgroundable(std::string_view bkgType, bool replace, T&& child) {
        return detail::Backgroundable<T>(bkgType, replace, child);
    }

    namespace detail {
        template<class... TArgs>
        requires ((renderable<TArgs> && ...))
        struct BackgroundableContainer {
            static_assert(renderable<BackgroundableContainer>);
            BackgroundableContainer(std::string_view type, TArgs... args) : backgroundType(type), children(args...) {}

            auto render(RenderContext& ctx) {
                UnityEngine::Transform* transform;

                if (!container) {
                    container = UnityEngine::GameObject::New_ctor();
                    static auto strName = il2cpp_utils::newcsstr<il2cpp_utils::CreationType::Manual>("BSMLBackground");
                    container->set_name(strName);

                    transform = container->get_transform();

                    transform->SetParent(&ctx.parentTransform, false);
                    container->AddComponent<UnityEngine::UI::ContentSizeFitter *>();
                    auto background = container->AddComponent<QuestUI::Backgroundable *>();

                    auto rectTransform = container->GetComponent<UnityEngine::RectTransform *>();
                    rectTransform->set_anchorMin({0, 0});
                    rectTransform->set_anchorMax({1, 1});
                    rectTransform->set_sizeDelta({0, 0});

                    background->ApplyBackground(il2cpp_utils::newcsstr(backgroundType));
                }

                if (!transform)
                    transform = container->get_transform();

                // Then we render our children to ourselves.
                RenderContext ctx2(transform);
                renderTuple(children, ctx2);
                return transform;
            }
            private:
            const std::string backgroundType;
            std::tuple<TArgs...> children;
            WeakPtrGO<UnityEngine::GameObject> container;
        };
    }

    template<class... TArgs>
    requires ((renderable<TArgs> && ...))
    auto BackgroundableContainer(std::string_view type, TArgs&&... args) {
        return detail::BackgroundableContainer<TArgs...>(type, std::forward<TArgs>(args)...);
    }
}
