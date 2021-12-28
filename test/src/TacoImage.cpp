#include "TacoImage.hpp"
#include "Sprites.hpp"

#include "questui/shared/BeatSaberUI.hpp"


using namespace QuestUI;
using namespace QuestUI_Components;

QUC::TacoImage::TacoImage(const UnityEngine::Vector2 &sd, UnityEngine::Vector2 anch, bool enabled) :
Image(BeatSaberUI::ArrayToSprite(il2cpp_utils::vectorToArray<uint8_t>(QuestUI_Components::Sprites::TacoSpriteBase64)), sd, anch, enabled) {

}
