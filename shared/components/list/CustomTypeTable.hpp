#pragma once

#include "beatsaber-hook/shared/utils/typedefs-string.hpp"

#include "HMUI/TableCell.hpp"
#include "HMUI/TableView.hpp"
#include "HMUI/TableView_IDataSource.hpp"
#include "HMUI/Touchable.hpp"
#include "HMUI/TableCell.hpp"

#include "UnityEngine/MonoBehaviour.hpp"
#include "UnityEngine/GameObject.hpp"

#include "TMPro/TextMeshProUGUI.hpp"

#include "questui/shared/CustomTypes/Components/List/QuestUITableView.hpp"
#include "questui/shared/BeatSaberUI.hpp"

#include "custom-types/shared/macros.hpp"
#include "shared/UnsafeAny.hpp"
#include "shared/key.hpp"

#include <functional>

#define GET_FIND_METHOD(mPtr) \
    il2cpp_utils::il2cpp_type_check::MetadataGetter<mPtr>::get()

namespace QUC::CustomTypeList {
    struct QUCDescriptor {
        bool interactable;
    };

    struct QUCTableInitData {
        StringW reuseIdentifier = "QUC_ReuseIdentifier";
        float cellSize = 12.0f;
        bool scrollable = true;
        UnityEngine::Vector2 anchorPosition;
        UnityEngine::Vector2 sizeDelta;

        constexpr QUCTableInitData() = default;

        constexpr QUCTableInitData(StringW const &reuseIdentifier, float cellSize) : reuseIdentifier(
                reuseIdentifier), cellSize(cellSize) {}
    };

    template<typename T>
    requires (std::is_convertible_v<T *, HMUI::TableCell *> && !std::is_pointer_v<T>)
    T *CreateQUCCell() {
        static auto playerTableCellStr = il2cpp_utils::newcsstr<il2cpp_utils::CreationType::Manual>("GlobalLeaderboardTableCell");
        auto cellGO = UnityEngine::GameObject::New_ctor();
        auto playerCell = cellGO->AddComponent<T *>();
        cellGO->set_name(playerTableCellStr);
        playerCell->Setup();
        return playerCell;
    }
}

#define DECLARE_OVERRIDE_METHOD_MATCH(retval, method, mptr, ...) \
    DECLARE_OVERRIDE_METHOD(retval, method, il2cpp_utils::il2cpp_type_check::MetadataGetter<mptr>::get(), __VA_ARGS__)


#define DECLARE_QUC_TABLE_DATA(namespaze, name, CustomQUCDescriptor, CustomCell, __VA_ARGS__) \
___DECLARE_TYPE_WRAPPER_INHERITANCE(namespaze, name, Il2CppTypeEnum::IL2CPP_TYPE_CLASS, UnityEngine::MonoBehaviour, #namespaze, {classof(HMUI::TableView::IDataSource*)}, 0, nullptr, \
            DECLARE_DEFAULT_CTOR();                                                             \
            \
            DECLARE_OVERRIDE_METHOD_MATCH(HMUI::TableCell*, CellForIdx, &HMUI::TableView::IDataSource::CellForIdx, HMUI::TableView* tableView, int idx); \
            DECLARE_OVERRIDE_METHOD_MATCH(float, CellSize, &HMUI::TableView::IDataSource::CellSize); \
            DECLARE_OVERRIDE_METHOD_MATCH(int, NumberOfCells, &HMUI::TableView::IDataSource::NumberOfCells);                                                                          \
            DECLARE_INSTANCE_FIELD(QuestUI::TableView*, tableView);   \
            \
            public: \
            using CustomQUCDescriptorT = CustomQUCDescriptor;         \
            using CustomQUCCustomCellT = CustomCell;                                            \
            static_assert(std::is_convertible_v<CustomQUCDescriptorT*, QUC::CustomTypeList::QUCDescriptor*>);  \
            void Init(QUC::CustomTypeList::QUCTableInitData const& initData); \
            \
            \
            std::function<HMUI::TableCell*(HMUI::TableView* tableView, int idx)> getCellForIdx = nullptr; \
            using CreateCellCallback = std::function<void(CustomQUCCustomCellT* cell, bool created, CustomQUCDescriptorT const& descriptor)>; \
            \
            CreateCellCallback buildCell; \
            \
            std::vector<CustomQUCDescriptorT> descriptors; \
            private:  \
            QUC::CustomTypeList::QUCTableInitData initData; \
)
// TODO: Should descriptors be allowed to be modified at runtime?

#define DECLARE_QUC_TABLE_CELL(namespaze, name, __VA_ARGS__) \
DECLARE_CLASS_CODEGEN(namespaze, name, HMUI::TableCell,      \
      void Setup();                                          \
      bool isCreated();                                      \
      const Key key;                                         \
      DECLARE_DEFAULT_CTOR();                                \
      DECLARE_SIMPLE_DTOR();                                 \
private: \
bool created = false; \
)

// cell
#define DEFINE_QUC_CUSTOMLIST_CELL(namespaze, clazzName) \
DEFINE_TYPE(namespaze, clazzName); \
void namespaze::clazzName::Setup() \
{ \
    if (!created) { \
        created = true; \
    } \
} \
bool namespaze::clazzName::isCreated() { \
    return created; \
}

// table data
#define DEFINE_QUC_CUSTOMLIST_TABLEDATA(namespaze, clazzName) \
DEFINE_TYPE(namespaze, clazzName); \
void namespaze::clazzName::Init(QUC::CustomTypeList::QUCTableInitData const &initData) { \
    this->initData = initData;                                \
    this->tableView->ReloadData();                                                          \
    } \
 \
float namespaze::clazzName::CellSize() { \
    return initData.cellSize; \
} \
\
int namespaze::clazzName::NumberOfCells() {                   \
    return descriptors.size(); \
} \
\
HMUI::TableCell * namespaze::clazzName::CellForIdx(HMUI::TableView *tableView, int idx) { \
    if (getCellForIdx) return getCellForIdx(tableView, idx); \
    auto tableCell = reinterpret_cast<CustomQUCCustomCellT *>(tableView->DequeueReusableCellForIdentifier(initData.reuseIdentifier)); \
    if (!tableCell) { \
        tableCell = QUC::CustomTypeList::CreateQUCCell<CustomQUCCustomCellT>(); \
    }                                                         \
    tableCell->set_reuseIdentifier(initData.reuseIdentifier); \
\
    auto const &data = descriptors[idx]; \
    bool newlyCreated = tableCell->isCreated(); \
\
    tableCell->Setup(); \
    tableCell->set_interactable(data.interactable);           \
    CRASH_UNLESS(buildCell);                                  \
    buildCell(tableCell, newlyCreated, data); \
    return tableCell; \
}


namespace QUC::CustomTypeList {
    template<typename TableData, typename CreateCellCallback = typename TableData::CreateCellCallback>
    TableData *CreateCustomList(UnityEngine::Transform *parent, CreateCellCallback createCell, QUCTableInitData const& initData) {
        TableData* list;
        if (initData.scrollable) {
            list = QuestUI::BeatSaberUI::CreateScrollableCustomSourceList<TableData *>(parent, initData.anchorPosition, initData.sizeDelta);
        } else {
            list = QuestUI::BeatSaberUI::CreateCustomSourceList<TableData *>(parent, initData.anchorPosition, initData.sizeDelta);
        }

        list->buildCell = createCell;

        return list;
    }
}