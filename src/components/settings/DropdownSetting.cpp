#include "components/settings/DropdownSetting.hpp"

#include "main.hpp"

#include <utility>

#include "questui/shared/BeatSaberUI.hpp"

#include "TMPro/TextMeshPro.hpp"
#include "TMPro/TextMeshProUGUI.hpp"

using namespace QuestUI;
using namespace QuestUI_Components;

Component* QuestUI_Components::DropdownSetting::render(UnityEngine::Transform *parentTransform) {
    CallbackWrapper callbackWrapper = constructWrapperCallback(parentTransform);

    uiDropdown = BeatSaberUI::CreateDropdown(parentTransform, std::string(data.text), getValue(), data.values, callbackWrapper);


    transform = uiDropdown->get_transform();

    // From QuestUI
    static auto labelName = il2cpp_utils::newcsstr<il2cpp_utils::CreationType::Manual>("Label");
    auto labelTransform = transform->get_parent()->Find(labelName);
    if (labelTransform) {
        UnityEngine::GameObject *labelObject = labelTransform->get_gameObject();
        if (labelObject) {
            uiText = labelObject->GetComponent<TMPro::TextMeshProUGUI *>();
        }
    }
//    uiText = uiDropdown->text;

    markAsRendered();
    update();

    return this;
}

void QuestUI_Components::DropdownSetting::update() {
    if (!rendered)
        throw std::runtime_error("String setting component has not rendered!");

    getLogger().debug("Update %p", uiText);
    BaseSetting::update();

    uiDropdown->set_enabled(data.enabled);
    if (uiDropdown->button) {
        uiDropdown->button->set_interactable(data.interactable);
    }


    List<Il2CppString*>* list = List<Il2CppString*>::New_ctor();
    int selectedIndex = 0;
    for(int i = 0; i < data.values.size(); i++){
        std::string value = data.values[i];
        if(getValue() == value)
            selectedIndex = i;
        list->Add(il2cpp_utils::newcsstr(value));
    }
    uiDropdown->SetTexts(reinterpret_cast<System::Collections::Generic::IReadOnlyList_1<Il2CppString*>*>(list));

    if (uiDropdown->selectedIndex != selectedIndex)
        uiDropdown->SelectCellWithIdx(selectedIndex);

}