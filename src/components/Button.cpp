#include "components/Button.hpp"

#include "questui/shared/BeatSaberUI.hpp"

#include "UnityEngine/UI/Image.hpp"
#include "UnityEngine/UI/Button.hpp"

#include "TMPro/TextMeshPro.hpp"
#include "TMPro/TextMeshProUGUI.hpp"

using namespace QuestUI;

UnityEngine::Transform* QuestUI_Components::Button::render(UnityEngine::Transform *parentTransform) {
    onClickCallback click = onClick; // copy
    std::function<void()> callback = [=]{
        click(parentTransform);
    };

    if (buttonData) {
        uiButton = BeatSaberUI::CreateUIButton(parentTransform, std::string(data.text), std::string(buttonData->buttonTemplate), buttonData->anchoredPosition, buttonData->sizeDelta, callback);
    } else {
        uiButton = BeatSaberUI::CreateUIButton(parentTransform, std::string(data.text), callback);
    }

    uiButtonText = uiButton->GetComponentInChildren<TMPro::TextMeshProUGUI*>();
    transform = uiButton->get_transform();

    markAsRendered();
    update();

    return transform;
}

void QuestUI_Components::Button::update() {
    if (!rendered)
        throw std::runtime_error("Button component has not rendered!");

    uiButton->set_enabled(data.enabled);
    uiButton->set_interactable(data.interactable);

    if (data.image) {
        uiButton->set_image(*data.image);
    }

    if (uiButtonText) {
        uiButtonText->set_text(il2cpp_utils::newcsstr(data.text));
    }
}
