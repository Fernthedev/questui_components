#include "components/Modal.hpp"
#include "questui/shared/BeatSaberUI.hpp"

#include "main.hpp"

using namespace QuestUI;
using namespace QuestUI_Components;

Component* QuestUI_Components::Modal::render(UnityEngine::Transform *parentTransform) {

    std::function<void(HMUI::ModalView*)> callbackWrapper = nullptr;

    if (onBlockerClicked) {
        BlockerClickedCallback callback(onBlockerClicked); // copy

        callbackWrapper = [this, callback](HMUI::ModalView* modal) {
            callback(this, modal);
        };
    }


    if (initData) {
        modalView = BeatSaberUI::CreateModal(parentTransform, initData->sizeDelta, initData->anchoredPosition, callbackWrapper, initData->dismissOnBlockerClicked);
    } else {
        modalView = BeatSaberUI::CreateModal(parentTransform, callbackWrapper);
    }

    transform = modalView->get_transform();

    rendered = true;
    update();

    return this;
}

void QuestUI_Components::Modal::renderComponentInContainer(QuestUI_Components::ComponentWrapper &comp) {
    if (rendered) {
        renderComponent(comp, const_cast<UnityEngine::Transform*>(transform));
    }
}

void QuestUI_Components::Modal::dismiss() {
    if (!rendered)
        throw std::runtime_error("Not rendered yet");

    modalView->Hide(true, nullptr);
}

void QuestUI_Components::Modal::show() {
    if (!rendered)
        throw std::runtime_error("Not rendered yet");

    modalView->Show(true, true, nullptr);
}
