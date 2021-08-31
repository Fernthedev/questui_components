#include "TestComponent.hpp"
#include "components/Text.hpp"
#include "questui/shared/BeatSaberUI.hpp"

#include "main.hpp"

using namespace QuestUI;
using namespace QuestUI_Components;

TestComponent::TestComponent(std::string_view prefix) : group(new MultiComponentGroup{
    new Text(std::string(prefix) + "Group 1"),
    new Text(std::string(prefix) + "Group 2")
}) {

}

Component *TestComponent::render(UnityEngine::Transform *parentTransform) {
    return group.get();
}
