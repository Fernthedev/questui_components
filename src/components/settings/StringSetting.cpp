#include "shared/components/settings/StringSetting.hpp"

#include <utility>

#include "questui/shared/BeatSaberUI.hpp"

#include "TMPro/TextMeshPro.hpp"
#include "TMPro/TextMeshProUGUI.hpp"

using namespace QuestUI;
using namespace QuestUI_Components;

Component* QuestUI_Components::StringSetting::render(UnityEngine::Transform *parentTransform) {
    CallbackWrapper callbackWrapper = constructWrapperCallback(parentTransform);

    if (stringInitData) {
        uiString = BeatSaberUI::CreateStringSetting(parentTransform, std::string(data.text), getValue(), stringInitData->anchoredPosition, stringInitData->keyboardPositionOffset, callbackWrapper);
    } else {
        uiString = BeatSaberUI::CreateStringSetting(parentTransform, std::string(data.text), getValue(), callbackWrapper);
    }

    transform = uiString->get_transform();

    // From QuestUI
    uiText = uiString->placeholderText->GetComponent<TMPro::TextMeshProUGUI*>();


    markAsRendered();
    update();

    return this;
}

void QuestUI_Components::StringSetting::update() {
    if (!rendered)
        throw std::runtime_error("String setting component has not rendered!");

    BaseSetting::update();

    uiString->set_enabled(data.enabled);
    uiString->set_interactable(data.interactable);

    uiString->SetText(il2cpp_utils::newcsstr(getValue()));
}

