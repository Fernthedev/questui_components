#pragma once

#include "UnityEngine/RectOffset.hpp"
#include "UnityEngine/RectTransform.hpp"
#include "UnityEngine/UI/ContentSizeFitter.hpp"


#include "shared/RootContainer.hpp"
#include "questui/shared/BeatSaberUI.hpp"

#include <utility>
#include <vector>

namespace QuestUI_Components {
    // TODO: Replace with QuestUI method
    static UnityEngine::UI::VerticalLayoutGroup *CreateModifierContainer(UnityEngine::Transform *parent) {
        using namespace QuestUI::BeatSaberUI;
        using namespace UnityEngine;
        using namespace UnityEngine::UI;

        UnityEngine::UI::VerticalLayoutGroup* group = CreateVerticalLayoutGroup(parent);

        group->set_padding(RectOffset::New_ctor(3, 3, 2, 2));
        group->set_childControlHeight(false);
        group->set_childForceExpandHeight(false);

        group->get_gameObject()->GetComponent<ContentSizeFitter*>()->set_verticalFit(ContentSizeFitter::FitMode::PreferredSize);

        RectTransform* rectTransform = group->get_rectTransform(); //group->get_transform()->get_parent()->get_gameObject()->GetComponent<RectTransform*>();
        rectTransform->set_anchoredPosition({0, 3});
        rectTransform->set_anchorMin(UnityEngine::Vector2(0.5f, 0.5f));
        rectTransform->set_anchorMax(UnityEngine::Vector2(0.5f, 0.5f));
        rectTransform->set_sizeDelta(UnityEngine::Vector2(54.0f, 0.0f));


        return group;
    }

    class ModifierContainer : public BaseContainer {
    public:
        explicit ModifierContainer(std::initializer_list<ComponentWrapper> children) : BaseContainer(children) {}
        explicit ModifierContainer(std::vector<ComponentWrapper> children) : BaseContainer(children) {}

    protected:
        Component* render(UnityEngine::Transform *parentTransform) override;

        UnityEngine::UI::VerticalLayoutGroup* verticalLayoutGroup = nullptr;
    };
}