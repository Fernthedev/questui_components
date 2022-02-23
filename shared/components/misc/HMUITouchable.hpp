#pragma once

#include "shared/context.hpp"

#include "UnityEngine/Transform.hpp"
#include "UnityEngine/GameObject.hpp"

#include "HMUI/Touchable.hpp"

namespace QUC {
    template<class T>
    requires (QUC::renderable_return<T, UnityEngine::Transform*>)
    struct HMUITouchable {
        std::remove_reference_t<T> child;
        const QUC::Key key;

        HMUITouchable(T child_)
                : child(std::move(child_)) {}

        auto render(QUC::RenderContext& ctx, QUC::RenderContextChildData& data) {
            auto res = QUC::detail::renderSingle(child, ctx);
            auto& touchable = data.getData<HMUI::Touchable*>();

            if (!touchable) {
                auto go = res->get_gameObject();
                touchable = go->template AddComponent<HMUI::Touchable*>();
            }

            return res;
        }


    };
}
