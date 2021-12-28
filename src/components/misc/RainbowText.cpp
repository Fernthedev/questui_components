#include "shared/components/misc/RainbowText.hpp"
#include "questui/shared/BeatSaberUI.hpp"



using namespace QuestUI;
using namespace QuestUI_Components;

Component *RainbowText::render(UnityEngine::Transform *parentTransform) {
    auto comp = Text::render(parentTransform);

    textUI->StartCoroutine(reinterpret_cast<System::Collections::IEnumerator*>(custom_types::Helpers::CoroutineHelper::New(rainbowCoroutine())));

    return this;
}
