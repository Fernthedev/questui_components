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
    struct CellComponent {
        Button button = Button("", nullptr);

        static void render(UnityEngine::Transform* parent, CellData const& cellData, RenderContext& ctx, RenderContextChildData& data) {
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
            // copy to lose const&
            CellComponent& cellComp = data.getData<CellComponent>();
            cellComp.button.text = cellData.displayedText;
            QUC::detail::renderSingle(cellComp.button, data.getChildContext([parent]{return parent; }));
        }
    };
    static_assert(ComponentCellRenderable<CellComponent, CellData>);
}