#pragma once

#include "../context.hpp"
#include "shared/unity/WeakPtrGO.hpp"

#include <string>
#include "UnityEngine/Vector2.hpp"
#include "UnityEngine/Color.hpp"
#include "sombrero/shared/ColorUtils.hpp"
#include "TMPro/TextMeshProUGUI.hpp"
#include "HMUI/CurvedTextMeshPro.hpp"
#include "UnityEngine/RectTransform.hpp"
#include "beatsaber-hook/shared/utils/il2cpp-utils.hpp"
#include "questui/shared/BeatSaberUI.hpp"

namespace QUC {
    struct Text {
        Text(Text const &text) = default;

        std::string text;
        bool enabled;
        std::optional<Sombrero::FastColor> color;
        float fontSize;
        bool italic;
        UnityEngine::Vector2 anchoredPosition;
        UnityEngine::Vector2 sizeDelta;

        Text(std::string_view t = "", bool enabled_ = true, std::optional<Sombrero::FastColor> c = std::nullopt, float fontSize_ = 4, bool italic_ = true, UnityEngine::Vector2 anch = {0.0f, 0.0f}, UnityEngine::Vector2 sd = {60.0f, 10.0f})
            : text(t), enabled(enabled_), color(c), fontSize(fontSize_), italic(italic_), anchoredPosition(anch), sizeDelta(sd) {}

        UnityEngine::Transform* render(RenderContext& ctx) {
            auto& parent = ctx.parentTransform;
            // Recreating our own is not very bueno... ASSUMING we can avoid it, which we should be able to.
            if (textComp) {
                // Rewrite our existing text instance instead of making a new one
                assign();
                return textComp->get_transform();
            }
            textComp = QuestUI::BeatSaberUI::CreateText(&parent, text, italic, anchoredPosition, sizeDelta);
            assign<true>();
            return textComp->get_transform();
        }

    protected:
        // Copy with existing TMP
        Text(TMPro::TextMeshProUGUI* textComp, Text const& text)
                : Text(text) {
            CRASH_UNLESS(textComp);
            this->textComp = textComp;
        }

        // Grab values from tmp
        explicit Text(TMPro::TextMeshProUGUI* textComp) {
            CRASH_UNLESS(textComp);
            this->textComp = textComp;

            auto rectTransform = textComp->get_rectTransform();

            sizeDelta = rectTransform->get_sizeDelta();
            text = to_utf8(csstrtostr(textComp->get_text()));
            anchoredPosition = rectTransform->get_anchoredPosition();
            italic = text.starts_with("<i>") && text.ends_with("</i>");
            fontSize = textComp->get_fontSize();
            enabled = textComp->get_enabled();
            color = textComp->get_color();
        }

        WeakPtrGO<TMPro::TextMeshProUGUI> textComp;

        template<bool created = false>
        void assign() {
            CRASH_UNLESS(textComp);
            textComp->set_enabled(enabled);
            if (!enabled) {
                // Don't bother setting anything if we aren't enabled.
                return;
            }
            auto rectTransform = textComp->get_rectTransform();
            if constexpr (!created) {
                // Only set these properties if we did NOT JUST create the text.
                Il2CppString* text_cs = nullptr;
                if (italic) text_cs = il2cpp_utils::newcsstr("<i>" + std::string(text) + "</i>");
                else text_cs = il2cpp_utils::newcsstr(text);
                rectTransform->set_anchorMin(UnityEngine::Vector2(0.5f, 0.5f));
                rectTransform->set_anchorMax(UnityEngine::Vector2(0.5f, 0.5f));
                textComp->set_richText(true);
                textComp->set_text(text_cs);

                rectTransform->set_anchoredPosition(anchoredPosition);
                rectTransform->set_sizeDelta(sizeDelta);
            }
            textComp->set_fontSize(fontSize);
            if (color) {
                textComp->set_color(*color);
            }
        }
    };
    static_assert(renderable<Text>);
    static_assert(renderable_return<Text, UnityEngine::Transform*>);
}
