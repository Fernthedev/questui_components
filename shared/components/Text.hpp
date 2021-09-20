#pragma once

#include "../context.hpp"

#include <string>
#include "UnityEngine/Vector2.hpp"
#include "UnityEngine/Color.hpp"
#include "TMPro/TextMeshProUGUI.hpp"
#include "HMUI/CurvedTextMeshPro.hpp"
#include "UnityEngine/RectTransform.hpp"
#include "beatsaber-hook/shared/utils/il2cpp-utils.hpp"
#include "questui/shared/BeatSaberUI.hpp"

namespace QUC {
    struct Text {
        std::string text;
        bool enabled;
        UnityEngine::Color color;
        float fontSize;
        bool italic;
        UnityEngine::Vector2 anchoredPosition;
        UnityEngine::Vector2 sizeDelta;

        Text(std::string t = "", bool enabled_ = true, UnityEngine::Color c = {1.0f, 1.0f, 1.0f, 1.0f}, float fontSize_ = 4, bool italic_ = true, UnityEngine::Vector2 anch = {0.0f, 0.0f}, UnityEngine::Vector2 sd = {60.0f, 10.0f})
            : text(t), enabled(enabled_), color(c), fontSize(fontSize_), italic(italic_), anchoredPosition(anch), sizeDelta(sd) {}

        void render(RenderContext& ctx) {
            auto& parent = ctx.parentTransform;
            // TODO: If our parent transform already holds a text instance, we should change the existing one
            // Recreating our own is not very bueno... ASSUMING we can avoid it, which we should be able to.
            if (nameStr) {
                auto found = parent.FindChild(nameStr);
                if (found) {
                    auto comp = found->GetComponent<HMUI::CurvedTextMeshPro*>();
                    if (!comp) {
                        // Destroy dangling child
                        UnityEngine::Object::Destroy(found);
                    } else {
                        // Rewrite our existing text instance instead of making a new one
                        assign(comp);
                        return;
                    }
                }
            }
            auto textComp = QuestUI::BeatSaberUI::CreateText(&parent, text, italic, anchoredPosition, sizeDelta);
            assign<true>(textComp);
            if (!nameStr) {
                using namespace il2cpp_utils;
                nameStr = newcsstr<CreationType::Manual>(csstrtostr(textComp->get_gameObject()->get_name()));
            }
        }
        private:
        static inline Il2CppString* nameStr = nullptr;
        template<bool created = false>
        void assign(TMPro::TextMeshProUGUI* textMesh) {
            textMesh->set_enabled(enabled);
            if (!enabled) {
                // Don't bother setting anything if we aren't enabled.
                return;
            }
            auto rectTransform = textMesh->get_rectTransform();
            if constexpr (!created) {
                // Only set these properties if we did NOT JUST create the text.
                Il2CppString* text_cs = nullptr;
                if (italic) text_cs = il2cpp_utils::newcsstr("<i>" + std::string(text) + "</i>");
                else text_cs = il2cpp_utils::newcsstr(text);
                rectTransform->set_anchorMin(UnityEngine::Vector2(0.5f, 0.5f));
                rectTransform->set_anchorMax(UnityEngine::Vector2(0.5f, 0.5f));
                textMesh->set_richText(true);
                textMesh->set_text(text_cs);
            }
            textMesh->set_fontSize(fontSize);
            textMesh->set_color(color);
            rectTransform->set_anchoredPosition(anchoredPosition);
            rectTransform->set_sizeDelta(sizeDelta);
        }
    };
    static_assert(renderable<Text>);
}
