#pragma once

#include "shared/components/layouts/VerticalLayoutGroup.hpp"
#include "shared/components/Text.hpp"
#include "shared/RootContainer.hpp"

#include <utility>
#include <vector>
#include <string>

#include "UnityEngine/TextAnchor.hpp"
#include "components/misc/Utility.hpp"

namespace UnityEngine {
    class Transform;
}

namespace QUC {
    static auto TestComponent(std::string_view prefix) {
        return Container(
                Text(std::string(prefix) + "Group 1"),
                Text(std::string(prefix) + "Group 2")
        );
    }

    struct AlignText {
        Text text;
        const Key key;

        HeldData<TMPro::TextAlignmentOptions> alignmentOptions;

        AlignText(Text const &text, TMPro::TextAlignmentOptions opt) : text(text), alignmentOptions(opt) {}




        void render(RenderContext& ctx, RenderContextChildData& data) {
            auto& textComp = ctx.getChildData(text.key).getData<TMPro::TextMeshProUGUI*>();
            bool existed = static_cast<bool>(textComp);
            detail::renderSingle(text, ctx);

            if (!existed || alignmentOptions.isModified()) {
                textComp->set_alignment(*alignmentOptions);
                alignmentOptions.clear();
            }
        }
    };

    static auto functionComponentTest1(RenderContext& ctx, RenderContextChildData& data) {
        auto& increment = data.getData<QuestUI::IncrementSetting*>();
        if (!increment) {
            increment = QuestUI::BeatSaberUI::CreateIncrementSetting(&ctx.parentTransform, "text", 0, 2, 4, nullptr);
        }

        return increment;
    }

    static auto functionComponentTest2() {
        FunctionalComponent([](RenderContext& ctx, RenderContextChildData& data){ // NOLINT(bugprone-unused-raii)
            return functionComponentTest1(ctx, data);
        });

        return FunctionalComponent(functionComponentTest1);
    }

    // Renders a list of text progressively
    struct MoreComplexType {
        // Identifies this component in the tree
        const Key key;

        std::string prefix;
        std::array<Text, 4> texts = {Text(prefix + "text 1"), Text(prefix + "text 2"), Text(prefix + "text 3"), Text(prefix + "text 4")};

        MoreComplexType(std::string_view prefix) : prefix(prefix) {}

        void render(RenderContext& ctx, RenderContextChildData& data) {
            // get state in the tree
            auto& timesRendered = data.getData<int>();
            if (timesRendered >= 0 && timesRendered < 4) {
                detail::renderSingle(texts[timesRendered], ctx);
                timesRendered++;
            }
        }
    };
    static_assert(renderable<MoreComplexType>);
}