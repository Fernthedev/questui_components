#include "shared/components/Image.hpp"

#include "questui/shared/BeatSaberUI.hpp"

using namespace QuestUI;
using namespace QuestUI_Components;

Component* QuestUI_Components::Image::render(UnityEngine::Transform *parentTransform) {
    uiImage = BeatSaberUI::CreateImage(parentTransform, data.sprite, initialImageData.anchoredPosition, initialImageData.sizeDelta);
    transform = uiImage->get_transform();

    markAsRendered();
    update();

    return this;
}

void QuestUI_Components::Image::update() {
    if (!rendered)
        throw std::runtime_error("Image component has not rendered!");

    if (!data.sprite)
        throw std::runtime_error("Sprite is null! Cannot happen");


    uiImage->set_sprite(data.sprite);
    uiImage->set_enabled(data.enabled);
}
