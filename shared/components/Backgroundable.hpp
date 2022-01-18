#pragma once

#include "shared/context.hpp"
#include "shared/RootContainer.hpp"
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
            const std::string backgroundType;
            const bool replaceExisting;
            T child;
            const Key key;

            Backgroundable(std::string_view bkgType, bool replace, T&& child_)
                    : backgroundType(bkgType), replaceExisting(replace), child(child_) {}
            UnityEngine::Transform* render(RenderContext& ctx, RenderContextChildData& data) {
                auto res = detail::renderSingle(child, ctx);
                auto backgroundable = data.getData<QuestUI::Backgroundable*>();

                if (replaceExisting || !backgroundable) {
                    auto go = res->get_gameObject();
                    backgroundable = go->template AddComponent<QuestUI::Backgroundable*>();
                }

                backgroundable->ApplyBackground(il2cpp_utils::newcsstr(backgroundType));

                return backgroundable->get_transform();
            }


        };
    }
    template<class T>
    auto Backgroundable(std::string_view bkgType, bool replace, T&& child) {
        return detail::Backgroundable<T>(bkgType, replace, std::forward<T>(child));
    }

    namespace detail {
        template<class... TArgs>
        requires ((renderable<TArgs> && ...))
        struct BackgroundableContainer : Container<TArgs...> {
            const Key key;
            const std::string backgroundType;

            BackgroundableContainer(std::string_view type, TArgs... args) : backgroundType(type), Container<TArgs...>(args...) {}

            UnityEngine::Transform* render(RenderContext& ctx, RenderContextChildData& data) {
                UnityEngine::Transform* transform;
                auto& container = data.getData<UnityEngine::GameObject*>();

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
                auto& ctx2 = data.getChildContext([transform] {return transform; });
                detail::Container<TArgs...>::render(ctx2, data);
                return transform;
            }
        };
    }

    template<class... TArgs>
    requires ((renderable<TArgs> && ...))
    auto BackgroundableContainer(std::string_view type, TArgs&&... args) {
        return detail::BackgroundableContainer<TArgs...>(type, std::forward<TArgs>(args)...);
    }
}
