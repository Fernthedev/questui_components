#include "shared/components/settings/ToggleSetting.hpp"

#include "questui/shared/BeatSaberUI.hpp"

#include "TMPro/TextMeshPro.hpp"
#include "TMPro/TextMeshProUGUI.hpp"

using namespace QuestUI;
using namespace QuestUI_Components;

Component* QuestUI_Components::ToggleSetting::render(UnityEngine::Transform *parentTransform) {
    CallbackWrapper callback = constructWrapperCallback(parentTransform);

    if (toggleInitData) {
        uiToggle = BeatSaberUI::CreateToggle(parentTransform, data.text, getValue(), toggleInitData->anchoredPosition, callback);
    } else {
        uiToggle = BeatSaberUI::CreateToggle(parentTransform, data.text, getValue(), callback);
    }

    transform = uiToggle->get_transform();

    // From QuestUI
    UnityEngine::GameObject* nameText = transform->get_parent()->Find(il2cpp_utils::newcsstr("NameText"))->get_gameObject();
    uiText = nameText->GetComponent<TMPro::TextMeshProUGUI*>();


    markAsRendered();
    update();

    return this;
}

void QuestUI_Components::ToggleSetting::update() {
    if (!rendered)
        throw std::runtime_error("Toggle setting component has not rendered!");

    BaseSetting::update();
    uiToggle->set_enabled(data.enabled);
    uiToggle->set_interactable(data.interactable);

    uiToggle->set_isOn(getValue());
}

