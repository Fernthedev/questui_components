#pragma once

#include "shared/context.hpp"
#include "shared/state.hpp"

#include "UnityEngine/Transform.hpp"
#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/Resources.hpp"
#include "UnityEngine/UI/LayoutElement.hpp"

namespace QUC {
    struct LoadingIndicator {
        const QUC::Key key;

        QUC::RenderHeldData<bool> enabled;

        LoadingIndicator(bool enabled = true) : enabled(enabled) {}

        static auto buildLoadingIndicator(UnityEngine::Transform* parent) {
            static SafePtrUnity<UnityEngine::GameObject> loadingTemplate;

            if (!loadingTemplate)
                loadingTemplate = UnityEngine::Resources::FindObjectsOfTypeAll<UnityEngine::GameObject*>().First([](auto& x) { return x->get_name() == "LoadingIndicator"; });
            UnityEngine::GameObject* loadingIndicator = UnityEngine::Object::Instantiate(loadingTemplate.ptr(), parent, false);
            loadingIndicator->set_name("BSMLLoadingIndicator");

            loadingIndicator->AddComponent<UnityEngine::UI::LayoutElement*>();

            return loadingIndicator;
        }

        constexpr auto render(QUC::RenderContext& ctx, QUC::RenderContextChildData& data) const {
            auto& loadingIndicator = data.getData<UnityEngine::GameObject*>();

            if (!loadingIndicator) {
                loadingIndicator = buildLoadingIndicator(&ctx.parentTransform);
            }

            if (enabled.readAndClear(ctx)) {
                loadingIndicator->set_active(enabled.getData());
            }

            // create context so it can be yeeted
            return &data.getChildContext([loadingIndicator]{ return loadingIndicator->get_transform();}).parentTransform;
        }
    };
}