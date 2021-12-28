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

    protected:
       std::optional<Sombrero::FastColor> color;

        custom_types::Helpers::Coroutine rainbowCoroutine(RenderContext ctx) {
            co_yield nullptr;

#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"
            while (true) {
                if (!color) color = UnityEngine::Color(1.0f, 0.0f, 0.0f, 1.0f);

                // https://forum.unity.com/threads/solved-rainbow-hue-shift-over-time-c-script.351751/#post-6651745
                // Assign HSV values to float h, s & v. (Since data.color is stored in RGB)
                float h, s, v;

                Sombrero::ColorRGBToHSV(color.value(), h, s, v);

                // Use HSV values to increase H in HSVToRGB. It looks like putting a value greater than 1 will round % 1 it
                this->color = Sombrero::ColorHSVToRGB(h + UnityEngine::Time::get_deltaTime() * 0.25f * speed, s, v);
                Text::render(ctx);

                co_yield nullptr;
            }
#pragma clang diagnostic pop
        }

        UnityEngine::Transform* render(RenderContext& ctx) {
            auto ret = Text::render(ctx);

            auto text = ret->GetComponent<TMPro::TextMeshProUGUI*>(); // TODO: Avoid this
            text->StartCoroutine(reinterpret_cast<System::Collections::IEnumerator *>(custom_types::Helpers::CoroutineHelper::New(rainbowCoroutine(ctx))));

            return ret;
        }

    private:
        int progress = 0;

    };
}