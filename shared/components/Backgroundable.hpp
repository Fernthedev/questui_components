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

#include "sombrero/shared/FastColor.hpp"

#include "../state.hpp"

namespace QUC {

    template<class T>
    requires (renderable_return<T, UnityEngine::Transform*>)
    struct Backgroundable {
        const std::string backgroundType;
        const bool replaceExisting;
        std::remove_reference_t<T> child;
        const Key key;
        float alpha = 0.5f;
        RenderHeldData<std::optional<Sombrero::FastColor>> color;

        constexpr Backgroundable(std::string_view bkgType, bool replace, T child_, float alpha = 0.5f, std::optional<Sombrero::FastColor> color = std::nullopt)
                : backgroundType(bkgType), replaceExisting(replace), child(child_), alpha(alpha), color(color) {}
        UnityEngine::Transform* render(RenderContext& ctx, RenderContextChildData& data) {
            auto res = detail::renderSingle(child, ctx);
            auto backgroundable = data.getData<QuestUI::Backgroundable*>();

            if (!backgroundable) {
                auto go = res->get_gameObject();
                backgroundable = go->template AddComponent<QuestUI::Backgroundable*>();
            }

            backgroundable->ApplyBackgroundWithAlpha(il2cpp_utils::newcsstr(backgroundType), alpha);

            if (color.readAndClear(ctx) && color.getData()) {
                backgroundable->background->set_color(*color.getData());
            }

            return res;
        }


    };


    namespace detail {
        template<class... TArgs>
        requires ((renderable<TArgs> && ...))
        struct BackgroundableContainer : Container<TArgs...> {
            const Key key;
            const std::string backgroundType;

            BackgroundableContainer(std::string_view type, TArgs... args) : backgroundType(type), Container<TArgs...>(args...) {}

            UnityEngine::Transform* render(RenderContext& ctx, RenderContextChildData& data) {
                auto& container = data.getData<UnityEngine::GameObject*>();

                if (!container) {
                    container = UnityEngine::GameObject::New_ctor();
                    static auto strName = il2cpp_utils::newcsstr<il2cpp_utils::CreationType::Manual>("BSMLBackground");
                    container->set_name(strName);

                    UnityEngine::Transform* transform = container->get_transform();

                    transform->SetParent(CRASH_UNLESS(&ctx.parentTransform), false);
                    container->AddComponent<UnityEngine::UI::ContentSizeFitter *>();
                    auto background = container->AddComponent<QuestUI::Backgroundable *>();

                    auto rectTransform = container->GetComponent<UnityEngine::RectTransform *>();
                    rectTransform->set_anchorMin({0, 0});
                    rectTransform->set_anchorMax({1, 1});
                    rectTransform->set_sizeDelta({0, 0});

                    background->ApplyBackground(il2cpp_utils::newcsstr(backgroundType));
                }

                // Then we render our children to ourselves.
                auto& ctx2 = data.getChildContext([container] {return container->get_transform(); });
                detail::Container<TArgs...>::render(ctx2, data);
                return &ctx2.parentTransform;
            }
        };
    }

    template<class... TArgs>
    requires ((renderable<TArgs> && ...))
    auto BackgroundableContainer(std::string_view type, TArgs&&... args) {
        return detail::BackgroundableContainer<TArgs...>(type, std::forward<TArgs>(args)...);
    }
}
