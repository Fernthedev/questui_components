#pragma once

#include "shared/context.hpp"
#include <string>
#include <string_view>
#include "questui/shared/BeatSaberUI.hpp"

namespace UnityEngine {
    class Transform;
}

namespace QUC {
    namespace detail {
        template<class T>
        requires (renderable_return<T, UnityEngine::Transform*>)
        struct HoverHint {
            static_assert(renderable<HoverHint>);
            std::string text;
            T child;

            HoverHint(std::string_view txt, T&& arg) : text(txt), child(arg) {}
            UnityEngine::Transform* render(RenderContext& ctx) {
                // First render our child to our parent.
                auto res = child.render(ctx);
                // Now, we know the result is convertible to Transform*, so we can pass that into make hover hint
                return QuestUI::BeatSaberUI::AddHoverHint(res->get_gameObject(), text)->get_transform();
            }
        };
    }
    template<class T>
    auto HoverHint(std::string_view txt, T&& arg) {
        return detail::HoverHint<T>(txt, arg);
    }
}
