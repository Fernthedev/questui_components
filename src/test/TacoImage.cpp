#include "TacoImage.hpp"
#include "Sprites.hpp"

#include "questui/shared/BeatSaberUI.hpp"


using namespace QuestUI;
using namespace QuestUI_Components;


TacoImage::TacoImage(const QuestUI_Components::Image::InitialImageData &imageData) : Image(
        BeatSaberUI::ArrayToSprite(il2cpp_utils::vectorToArray<uint8_t>(QuestUI_Components::Sprites::TacoSpriteBase64)),
        imageData
        ) {}
