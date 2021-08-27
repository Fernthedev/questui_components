#include "components/Text.hpp"

#include "questui/shared/BeatSaberUI.hpp"

using namespace QuestUI;

UnityEngine::Transform* QuestUI_Components::Text::render(UnityEngine::Transform *parentTransform) {
    if (initialTextData) {
        textUI = BeatSaberUI::CreateText(parentTransform, std::string(data.text), initialTextData->italic, initialTextData->anchoredPosition, initialTextData->sizeDelta);
    } else {
        textUI = BeatSaberUI::CreateText(parentTransform, std::string(data.text));
    }

    transform = textUI->get_transform();

    markAsRendered();
    update();

    return transform;
}

void QuestUI_Components::Text::update() {
    if (!rendered)
        throw std::runtime_error("Text component has not rendered!");

    textUI->set_text(il2cpp_utils::newcsstr(data.text));
    textUI->set_enabled(data.enabled);

    if (data.color) {
        textUI->set_color(*data.color);
    }
}
