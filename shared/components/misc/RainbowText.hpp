#pragma once

#include "shared/components/Text.hpp"
#include "custom-types/shared/coroutine.hpp"

#include "System/Collections/IEnumerator.hpp"
#include "UnityEngine/WaitForSecondsRealtime.hpp"
#include "UnityEngine/Time.hpp"

#include "sombrero/shared/ColorUtils.hpp"

#include <utility>
#include <vector>
#include <string>


namespace UnityEngine {
    class Transform;
}

namespace QUC {
    class RainbowText : public Text {
    public:
        explicit RainbowText(std::string_view prefix) : Text(prefix) {}

    float speed = 1.0f;

    constexpr UnityEngine::Transform* render(RenderContext& ctx, RenderContextChildData& data) {
        auto& childData = ctx.getChildData(Text::key);
        auto const& text = childData.getData<TMPro::TextMeshProUGUI *>();

        bool textWasBuilt = static_cast<bool>(text);

        auto ret = Text::render(ctx, childData);

        if (!textWasBuilt) {
            text->StartCoroutine(custom_types::Helpers::CoroutineHelper::New(
                    rainbowCoroutine(ctx, data, *this, text)));
        }

        return ret;
    }

    protected:
        static custom_types::Helpers::Coroutine rainbowCoroutine(RenderContext& ctx, RenderContextChildData& data, RainbowText rainbowText, TMPro::TextMeshProUGUI* const& text) {
            co_yield nullptr;

            auto& childData = ctx.getChildData(rainbowText.key);

#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"
            while (true) {

                // stop, text is null?
                if (!text || !text->get_enabled()) co_return;

                // if color has no value or if color
                if (!rainbowText.color.getData().has_value()) rainbowText.color = UnityEngine::Color(1.0f, 0.0f, 0.0f, 1.0f);

                // https://forum.unity.com/threads/solved-rainbow-hue-shift-over-time-c-script.351751/#post-6651745
                // Assign HSV values to float h, s & v. (Since data.color is stored in RGB)
                float h, s, v;

                Sombrero::ColorRGBToHSV(*rainbowText.color.getData(), h, s, v);

                // Use HSV values to increase H in HSVToRGB. It looks like putting a value greater than 1 will round % 1 it
                rainbowText.color = Sombrero::ColorHSVToRGB(h + UnityEngine::Time::get_deltaTime() * 0.25f * rainbowText.speed * 0.25f, s, v);

                rainbowText.Text::render(ctx, childData);

                co_yield nullptr;
            }
#pragma clang diagnostic pop
        }
    };
}