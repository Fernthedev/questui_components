#pragma once

#include "shared/RootContainer.hpp"

#include "UnityEngine/Vector2.hpp"

#include "questui/shared/CustomTypes/Components/List/CustomCellListWrapper.hpp"
#include "questui/shared/BeatSaberUI.hpp"

#include <utility>
#include <vector>


namespace QuestUI_Components {

    // This is an extremely lazy attempt for a list, I hate it.
    template<typename T = HMUI::TableView::IDataSource>
    class CustomCelledList : public BaseContainer {
    public:
        using CallbackWrapper = std::function<void(int)>;
        using Callback = std::function<void(CustomCelledList<T>* self, ComponentWrapper selectedComp, int selectedIndex)>;
        struct CustomCelledListInitData {
            UnityEngine::Vector2 anchoredPosition = {0,0};
            UnityEngine::Vector2 sizeDelta;
        };

        explicit CustomCelledList(std::initializer_list<ComponentWrapper> children, std::optional<CustomCelledListInitData> initData = std::nullopt, Callback callback = nullptr) : onCellWithIdxClicked(callback), initData(initData), BaseContainer(children) {}
        explicit CustomCelledList(std::vector<ComponentWrapper> const& children, std::optional<CustomCelledListInitData> initData = std::nullopt, Callback callback = nullptr) : onCellWithIdxClicked(callback), initData(initData), BaseContainer(children) {}

    protected:
        CallbackWrapper constructWrapperCallback() {
            if (onCellWithIdxClicked) {
                return [this](int index) {
                    ComponentWrapper& wrapper = getComponentAt(index);
                    // better way of doing this than reinterpret cast to child?
                    callback(this, wrapper, index);

                    updateComponent(wrapper);
                };
            } else {
                return [this](int index) {
                    setValue(getComponentAt(index));
                };
            }
        }

        ComponentWrapper& getComponentAt(int i) {
            return getRenderChildren()[i];
        }

        void updateComponent(ComponentWrapper& wrapper) {
            renderComponentInContainer(wrapper);
        }

        Component* render(UnityEngine::Transform *parentTransform) override {
            using namespace QuestUI;
            Callback callback = constructWrapperCallback();
            if (initData) {
                hmuiTable = BeatSaberUI::CreateCustomSourceList<T *>(parentTransform, initData->anchoredPosition, initData->sizeDelta, callback);
            } else {
                hmuiTable = BeatSaberUI::CreateCustomSourceList<T *>(parentTransform, callback);
            }
            monoBehaviour = il2cpp_utils::cast<UnityEngine::MonoBehaviour>(hmuiTable);
            dataSource = il2cpp_utils::cast<HMUI::TableView::IDataSource>(hmuiTable);
            transform = monoBehaviour->get_transform();

            rendered = true;
            update();

            return this;
        }

        // constructor time
        const Callback onCellWithIdxClicked = nullptr;
        const std::optional<CustomCelledListInitData> initData;

        // render time
        T* hmuiTable = nullptr;
        UnityEngine::MonoBehaviour* monoBehaviour = nullptr;
        HMUI::TableView::IDataSource* dataSource = nullptr;
    };
}