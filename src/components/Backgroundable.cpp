#include "shared/components/Backgroundable.hpp"

#include "questui/shared/BeatSaberUI.hpp"
#include "questui/shared/CustomTypes/Components/Backgroundable.hpp"

using namespace QuestUI;
using namespace QuestUI_Components;

Component* QuestUI_Components::Backgroundable::render(UnityEngine::Transform *parentTransform) {
    transform = renderComponent(child, parentTransform)->getTransform();

    background = getTransform()->get_gameObject()->AddComponent<QuestUI::Backgroundable*>();
    background->ApplyBackground(il2cpp_utils::newcsstr(backgroundType));

    return this;
}
