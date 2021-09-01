#include "shared/components/misc/RainbowText.hpp"
#include "questui/shared/BeatSaberUI.hpp"

#include "System/Collections/IEnumerator.hpp"
#include "UnityEngine/WaitForSecondsRealtime.hpp"
#include "UnityEngine/Time.hpp"

#include "sombrero/shared/ColorUtils.hpp"

#include "main.hpp"

using namespace QuestUI;
using namespace QuestUI_Components;

Component *RainbowText::render(UnityEngine::Transform *parentTransform) {
    auto comp = Text::render(parentTransform);

    textUI->StartCoroutine(reinterpret_cast<System::Collections::IEnumerator*>(custom_types::Helpers::CoroutineHelper::New(rainbowCoroutine())));

    return this;
}

custom_types::Helpers::Coroutine RainbowText::rainbowCoroutine() {
    while (true) {
        if (!data.color) data.color = UnityEngine::Color(1.0f, 0.0f, 0.0f, 1.0f);

        // https://forum.unity.com/threads/solved-rainbow-hue-shift-over-time-c-script.351751/#post-6651745
        // Assign HSV values to float h, s & v. (Since data.color is stored in RGB)
        float h, s, v;

        Sombrero::ColorRGBToHSV(data.color.value(), h, s, v);

        // Use HSV values to increase H in HSVToRGB. It looks like putting a value greater than 1 will round % 1 it
        UnityEngine::Color color = Sombrero::ColorHSVToRGB(h + UnityEngine::Time::get_deltaTime() * 0.25f * speed, s, v);
        data.color = color;
        textUI->set_color(color);

        co_yield nullptr;
    }
}
