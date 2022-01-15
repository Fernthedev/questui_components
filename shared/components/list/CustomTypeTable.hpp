#pragma once

#include "beatsaber-hook/shared/utils/typedefs-string.hpp"

#include "HMUI/TableCell.hpp"
#include "HMUI/TableView.hpp"
#include "HMUI/TableView_IDataSource.hpp"
#include "HMUI/HoverHint.hpp"
#include "HMUI/TableCell.hpp"

#include "UnityEngine/MonoBehaviour.hpp"
#include "UnityEngine/GameObject.hpp"

#include "TMPro/TextMeshProUGUI.hpp"

#include "custom-types/shared/macros.hpp"
#include "UnsafeAny.hpp"


#include <functional>

#define GET_FIND_METHOD(mPtr) \
    il2cpp_utils::il2cpp_type_check::MetadataGetter<mPtr>::get()

namespace QUC::CustomTypeList {
    struct QUCDescriptor {
        bool interactable;
    };

    struct QUCTableInitData {
        StringW reuseIdentifier;
        float cellSize;
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
            DECLARE_OVERRIDE_METHOD_MATCH(int, NumberOfCells, &HMUI::TableView::IDataSource::NumberOfCells); \
            \
            public: \
            using CustomQUCDescriptorT = CustomQUCDescriptor;         \
            using CustomQUCCustomCellT = CustomCell;                                            \
            static_assert(std::is_convertible_v<CustomQUCDescriptorT*, QUC::CustomTypeList::QUCDescriptor*>);  \
            void Init(QUCTableInitData const& initData); \
            \
            \
            std::function<HMUI::TableCell*(HMUI::TableView* tableView, int idx)> getCellForIdx = nullptr; \
            using CreateCellCallback = std::function<void(QUCObjectTableCell* cell, bool created, CustomQUCDescriptorT const& descriptor)>; \
            \
            CreateCellCallback buildCell; \
            \
            std::vector<CustomQUCDescriptorT> descriptors; \
            private:  \
            QUCTableInitData initData; \
)
// TODO: Should descriptors be allowed to be modified at runtime?

#define DECLARE_QUC_TABLE_CELL(namespaze, name, __VA_ARGS__) \
DECLARE_CLASS_CODEGEN(namespaze, name, HMUI::TableCell,                           \
      void Setup();                                          \
      bool isCreated(); \
private: \
bool created = false; \
)

// cell
#define DEFINE_QUC_CUSTOMLIST_CELL(namespaze, clazzName) \
DEFINE_TYPE(namespaze, clazzName); \
void namespaze::clazzName::Setup() \
{ \
    if (!created) { \
        get_gameObject()->AddComponent<HMUI::Touchable *>(); \
        created = true; \
    } \
} \
bool namespaze::clazzName::isCreated() { \
    return created; \
}

// table data
#define DEFINE_QUC_CUSTOMLIST_TABLEDATA(namespaze, clazzName) \
DEFINE_TYPE(namespaze, clazzName); \
void namespaze::clazzName::Init(QUCTableInitData const &initData) { \
    this->initData = initData; \
} \
 \
float namespaze::clazzName::CellSize() { \
    return initData.cellSize; \
} \
\
int namespaze::clazzName::NumberOfCells() { \
    return descriptors.size(); \
} \
\
HMUI::TableCell * namespaze::clazzName::CellForIdx(HMUI::TableView *tableView, int idx) { \
    if (getCellForIdx(tableView, idx)) return getCellForIdx(tableView, idx); \
    auto tableCell = reinterpret_cast<CustomQUCCustomCellT *>(tableView->DequeueReusableCellForIdentifier( \
            initData.reuseIdentifier)); \
    if (!tableCell) { \
        tableCell = CreateQUCCell<CustomQUCCustomCellT>(); \
    } \
    tableCell->set_reuseIdentifier(initData.reuseIdentifier); \
\
    auto const &data = descriptors[idx]; \
    bool newlyCreated = tableCell->isCreated(); \
\
    tableCell->Setup(); \
    tableCell->set_interactable(data.interactable); \
    buildCell(tableCell, newlyCreated, data); \
    return tableCell; \
}


namespace QUC::CustomTypeList {
    template<typename TableData, typename CreateCellCallback = typename TableData::CreateCellCallback>
    TableData *CreateCustomList(UnityEngine::Transform *parent, CreateCellCallback createCell) {
        UnityEngine::GameObject *go = UnityEngine::GameObject::New_ctor(StringW("QUC_CustomList"));
        auto transform = go->get_transform();
        transform->SetParent(parent, false);

        auto list = go->AddComponent<TableData *>();

        list->buildCell = createCell;

        return list;
    }
}