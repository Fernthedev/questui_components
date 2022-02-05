#pragma once

#include "Logger.hpp"

#include "shared/components/list/CustomCelledList.hpp"
#include "components/Text.hpp"
#include "components/Button.hpp"

namespace QUC {
    struct CellData : QUC::CustomTypeList::QUCDescriptor {
        std::string displayedText;

        CellData(std::string_view displayedText) : displayedText(displayedText) {}
    };
}

// Header
DECLARE_QUC_TABLE_CELL(QUC, QUCObjectTableCell,)
DECLARE_QUC_TABLE_DATA(QUC, QUCObjectTableData, QUC::CellData, QUCObjectTableCell,);

namespace QUC {
    // Both of these components are valid and usable

    // More traditional QuestUI component
    struct QuestUICellComponent {
        Button button = Button("", nullptr);

        static void render(UnityEngine::Transform *cellTransform, CellData const &cellData, RenderContext &tableCtx,
                           RenderContextChildData &cellCtx) {
//            auto& tmpText = data.getData<UnityEngine::UI::Button*>();
//
//            if (!tmpText) {
//                QuestUI_Components::Loggerr::getLogger().debug("build cell");
//                tmpText = QuestUI::BeatSaberUI::CreateUIButton(parent, cell.displayedText);
//            } else {
//                QuestUI_Components::Loggerr::getLogger().debug("reuse cell");
//
//                auto buttonText = tmpText->GetComponentInChildren<TMPro::TextMeshProUGUI *>();
//                buttonText->set_text(static_cast<Il2CppString *>(StringW(cell.displayedText).convert()));
//            }

            // The commented code above is identical to this
            auto &cellComp = cellCtx.getData<QuestUICellComponent>();
            cellComp.button.text.text = cellData.displayedText;
            QUC::detail::renderSingle(cellComp.button, cellCtx.getChildContext([cellTransform] { return cellTransform; }));
        }
    };

    struct CellComponent {
        Button button = Button("", nullptr);

        void render(CellData const& cellData, RenderContext& cellCtx) {
            button.text.text = cellData.displayedText;
            QUC::detail::renderSingle(button, cellCtx);
        }
    };
    static_assert(ComponentCellRenderable<CellComponent, CellData>);
}