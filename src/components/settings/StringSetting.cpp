#include "components/settings/StringSetting.hpp"

#include <utility>

#include "questui/shared/BeatSaberUI.hpp"

#include "TMPro/TextMeshPro.hpp"
#include "TMPro/TextMeshProUGUI.hpp"

using namespace QuestUI;
using namespace QuestUI_Components;

Component* QuestUI_Components::StringSetting::render(UnityEngine::Transform *parentTransform) {
    std::function<void(std::string)> callback;
    if (onStringSet) {
        OnStringCallback onString(onStringSet); // copy
        callback = [=](const std::string& val) {
            setStringValue(val);
            onString(this, val, parentTransform);
        };
    } else {
        callback = [=](std::string val) {
            setStringValue(std::move(val));
        };
    }

    if (stringInitData) {
        uiString = BeatSaberUI::CreateStringSetting(parentTransform, std::string(data.stringText), data.stringValue, stringInitData->anchoredPosition, stringInitData->keyboardPositionOffset, callback);
    } else {
        uiString = BeatSaberUI::CreateStringSetting(parentTransform, std::string(data.stringText), data.stringValue, callback);
    }

    transform = uiString->get_transform();

    // From QuestUI
    uiStringText = uiString->placeholderText->GetComponent<TMPro::TextMeshProUGUI*>();


    markAsRendered();
    update();

    return this;
}

void QuestUI_Components::StringSetting::update() {
    if (!rendered)
        throw std::runtime_error("String setting component has not rendered!");

    uiString->set_enabled(data.enabled);
    uiString->set_interactable(data.interactable);

    if (uiStringText) {
        uiStringText->SetText(il2cpp_utils::newcsstr(data.stringText));
    }

    uiString->SetText(il2cpp_utils::newcsstr(data.stringValue));
    // set placeholder text too?

}

void StringSetting::internalSetStringValue(std::string val) {
    mutateData([&val](MutableStringSettingsData data) {
        data.stringValue = val;
        return data;
    });
}

