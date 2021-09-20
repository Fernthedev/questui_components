#pragma once

#include "shared/context.hpp"
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
                QuestUI::Backgroundable* background;
                if (replaceExisting) {
                    background = go->template AddComponent<QuestUI::Backgroundable*>();
                } else {
                    background = go->template GetComponent<QuestUI::Backgroundable*>();
                    if (!background) {
                        background = go->template AddComponent<QuestUI::Backgroundable*>();
                    }
                }
                if (background) {
                    background->ApplyBackground(il2cpp_utils::newcsstr(backgroundType));
                }
                return background->get_transform();
            }
            private:
            const std::string backgroundType;
            const bool replaceExisting;
            T child;
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
                auto gameObject = UnityEngine::GameObject::New_ctor();
                static auto strName = il2cpp_utils::newcsstr("BSMLBackground");
                gameObject->set_name(strName);
                auto transform = gameObject->get_transform();
                transform->SetParent(&ctx.parentTransform, false);
                gameObject->AddComponent<UnityEngine::UI::ContentSizeFitter*>();
                auto background = gameObject->AddComponent<QuestUI::Backgroundable*>();

                auto rectTransform = gameObject->GetComponent<UnityEngine::RectTransform*>();
                rectTransform->set_anchorMin({0,0});
                rectTransform->set_anchorMax({1,1});
                rectTransform->set_sizeDelta({0,0});

                background->ApplyBackground(il2cpp_utils::newcsstr(backgroundType));
                // Then we render our children to ourselves.
                RenderContext ctx2(transform);
                renderTuple(children, ctx2);
                return transform;
            }
            private:
            const std::string backgroundType;
            std::tuple<TArgs...> children;
        };
    }

    template<class... TArgs>
    requires ((renderable<TArgs> && ...))
    auto BackgroundableContainer(std::string_view type, TArgs&&... args) {
        return detail::BackgroundableContainer<TArgs...>(type, std::forward<TArgs>(args)...);
    }
}
