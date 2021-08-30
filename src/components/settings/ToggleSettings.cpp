#include "components/settings/ToggleSetting.hpp"

#include "questui/shared/BeatSaberUI.hpp"

#include "TMPro/TextMeshPro.hpp"
#include "TMPro/TextMeshProUGUI.hpp"

using namespace QuestUI;
using namespace QuestUI_Components;

Component* QuestUI_Components::ToggleSetting::render(UnityEngine::Transform *parentTransform) {
    std::function<void(bool)> callback;
    if (onToggle) {
        onToggleCallback toggle(onToggle); // copy
        callback = [=](bool val) {
            setToggleValue(val);
            toggle(this, val, parentTransform);
        };
    } else {
        callback = [=](bool val) {
            setToggleValue(val);
        };
    }

    if (toggleInitData) {
        uiToggle = BeatSaberUI::CreateToggle(parentTransform, std::string(data.toggleText), data.toggleValue, toggleInitData->anchoredPosition, callback);
    } else {
        uiToggle = BeatSaberUI::CreateToggle(parentTransform, std::string(data.toggleText), data.toggleValue, callback);
    }

    transform = uiToggle->get_transform();

    // From QuestUI
    UnityEngine::GameObject* nameText = transform->get_parent()->Find(il2cpp_utils::createcsstr("NameText"))->get_gameObject();
    uiToggleText = nameText->GetComponent<TMPro::TextMeshProUGUI*>();


    markAsRendered();
    update();

    return this;
}

void QuestUI_Components::ToggleSetting::update() {
    if (!rendered)
        throw std::runtime_error("Toggle setting component has not rendered!");

    uiToggle->set_enabled(data.enabled);
    uiToggle->set_interactable(data.interactable);

    if (uiToggleText) {
        uiToggleText->set_text(il2cpp_utils::newcsstr(data.toggleText));
    }
}

void ToggleSetting::internalSetToggleValue(bool val) {
    mutateData([&val](MutableToggleSettingsData data) {
        data.toggleValue = val;
        return data;
    });
}

