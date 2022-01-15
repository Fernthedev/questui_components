#include "components/list/CustomTypeTable.hpp"

#include "questui/shared/BeatSaberUI.hpp"

#include "HMUI/Touchable.hpp"
#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/UI/LayoutElement.hpp"

using namespace UnityEngine;
using namespace UnityEngine::UI;
using namespace QuestUI::BeatSaberUI;

// Header
DECLARE_QUC_TABLE_CELL(QUC::CustomTypeList, QUCObjectTableCell,)
DECLARE_QUC_TABLE_DATA(QUC::CustomTypeList, QUCObjectTableData, QUC::CustomTypeList::QUCDescriptor, QUCObjectTableCell,);

//DEFINE_TYPE(QUC::CustomTypeList, QUCObjectTableCell);
//DEFINE_TYPE(QUC::CustomTypeList, QUCObjectTableData);
//namespace QUC::CustomTypeList {
//    void QUCObjectTableData::Init(QUCTableInitData const &initData) {
//        this->initData = initData;
//    }
//
//    float QUCObjectTableData::CellSize() {
//        return initData.cellSize;
//    }
//
//    int QUCObjectTableData::NumberOfCells() {
//        return descriptors.size();
//    }
//
//    HMUI::TableCell *QUCObjectTableData::CellForIdx(HMUI::TableView *tableView, int idx) {
//        if (getCellForIdx(tableView, idx)) return getCellForIdx(tableView, idx);
//        auto tableCell = reinterpret_cast<CustomQUCCustomCellT *>(tableView->DequeueReusableCellForIdentifier(
//                initData.reuseIdentifier));
//        if (!tableCell) {
//            tableCell = CreateQUCCell<CustomQUCCustomCellT>();
//        }
//        tableCell->set_reuseIdentifier(initData.reuseIdentifier);
//
//        auto const &data = descriptors[idx];
//        bool newlyCreated = tableCell->isCreated();
//
//        tableCell->Setup();
//        tableCell->set_interactable(data.interactable);
//        buildCell(tableCell, newlyCreated, data);
//        return tableCell;
//    }
//}

// Source
DEFINE_QUC_CUSTOMLIST_TABLEDATA(QUC::CustomTypeList, QUCObjectTableData);
DEFINE_QUC_CUSTOMLIST_CELL(QUC::CustomTypeList, QUCObjectTableCell);