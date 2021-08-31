#pragma once

#include "Component.hpp"
#include "RootContainer.hpp"

#include <optional>
#include <utility>
#include <vector>

#include "UnityEngine/Vector2.hpp"

namespace HMUI {
    class ModalView;
}


namespace QuestUI_Components {

    class Modal : public BaseContainer {
    public:
        using BlockerClickedCallback = std::function<void(Modal*, HMUI::ModalView*)>;

        struct ModalInitData {
            UnityEngine::Vector2 sizeDelta = {30.0f, 40.0f};
            UnityEngine::Vector2 anchoredPosition = {0.0f, 0.0f};
            bool dismissOnBlockerClicked = true;
        };

        explicit Modal(std::initializer_list<ComponentWrapper> children, BlockerClickedCallback onBlockerClicked,
                       ModalInitData initData) :
                       onBlockerClicked(std::move(onBlockerClicked)),
                       initData(std::make_optional(initData)), BaseContainer(children) {}

        explicit Modal(std::initializer_list<ComponentWrapper> children, BlockerClickedCallback onBlockerClicked,
                       std::optional<ModalInitData> initData = std::nullopt) :
                       onBlockerClicked(std::move(onBlockerClicked)),
                       initData(initData), BaseContainer(children) {}

        void dismiss();
        void show();

        [[nodiscard]] HMUI::ModalView *getModalView() const {
            return modalView;
        }

        CONSTRUCT_AFTER_COMPONENT(Modal)

    protected:
        Component* render(UnityEngine::Transform *parentTransform) override;

        HMUI::ModalView* modalView = nullptr;
        std::optional<ModalInitData> initData;
        BlockerClickedCallback onBlockerClicked;
    };
}