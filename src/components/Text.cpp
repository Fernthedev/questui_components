#include "components/Text.hpp"

#include "questui/shared/BeatSaberUI.hpp"

using namespace QuestUI;
using namespace QuestUI_Components;

Component* QuestUI_Components::Text::render(UnityEngine::Transform *parentTransform) {
    if (initialTextData) {
        textUI = BeatSaberUI::CreateText(parentTransform, std::string(data.text), initialTextData->italic, initialTextData->anchoredPosition, initialTextData->sizeDelta);
    } else {
        textUI = BeatSaberUI::CreateText(parentTransform, std::string(data.text));
    }

    if (!data.fontSize)
        data.fontSize = textUI->get_fontSize();

    transform = textUI->get_transform();

    markAsRendered();
    update();

    return this;
}

void QuestUI_Components::Text::update() {
    if (!rendered)
        throw std::runtime_error("Text component has not rendered!");

    textUI->set_text(il2cpp_utils::newcsstr(data.text));
    textUI->set_enabled(data.enabled);

    if (data.color) {
        textUI->set_color(*data.color);
    }

    if (data.fontSize) {
        textUI->set_fontSize(data.fontSize.value());
    }
}
