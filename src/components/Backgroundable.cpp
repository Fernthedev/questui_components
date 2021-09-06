#include "shared/components/Backgroundable.hpp"

#include "questui/shared/BeatSaberUI.hpp"
#include "questui/shared/CustomTypes/Components/Backgroundable.hpp"

#include "UnityEngine/UI/ContentSizeFitter.hpp"

using namespace QuestUI;
using namespace QuestUI_Components;

Component* QuestUI_Components::Backgroundable::render(UnityEngine::Transform *parentTransform) {
    transform = renderComponent(child, parentTransform)->getTransform();

    if (replaceExistingBackground) {
        background = getTransform()->get_gameObject()->AddComponent<QuestUI::Backgroundable *>();
    } else {
        background = getTransform()->get_gameObject()->GetComponent<QuestUI::Backgroundable *>();
        if (!background)
            background = getTransform()->get_gameObject()->AddComponent<QuestUI::Backgroundable *>();
    }
    background->ApplyBackground(il2cpp_utils::newcsstr(backgroundType));

    return this;
}

Component *BackgroundableContainer::render(UnityEngine::Transform *parentTransform) {
    auto gameObject = UnityEngine::GameObject::New_ctor();
    gameObject->set_name(il2cpp_utils::newcsstr("BSMLBackground"));
    gameObject->get_transform()->SetParent(parentTransform, false);
    gameObject->AddComponent<UnityEngine::UI::ContentSizeFitter*>();
    background = gameObject->AddComponent<QuestUI::Backgroundable*>();

    auto rectTransform = gameObject->GetComponent<UnityEngine::RectTransform*>();
    rectTransform->set_anchorMin({0,0});
    rectTransform->set_anchorMax({1,1});
    rectTransform->set_sizeDelta({0,0});

    background->ApplyBackground(il2cpp_utils::newcsstr(backgroundType));

    transform = gameObject->get_transform();
    rendered = true;

    update();

    return this;
}
