#pragma once

#include "../context.hpp"
#include "shared/state.hpp"
#include "shared/unity/WeakPtrGO.hpp"

#include <string>

#include "UnityEngine/Vector2.hpp"
#include "UnityEngine/Color.hpp"
#include "UnityEngine/RectTransform.hpp"

#include "TMPro/TextMeshProUGUI.hpp"
#include "HMUI/CurvedTextMeshPro.hpp"


#include "questui/shared/BeatSaberUI.hpp"

#include "sombrero/shared/ColorUtils.hpp"
#include "sombrero/shared/Vector2Utils.hpp"

#include "beatsaber-hook/shared/utils/il2cpp-utils.hpp"

namespace QUC {
    struct Text {
        Text(Text const &text) = default;

        HeldData<std::string> text;
        HeldData<bool> enabled;
        HeldData<std::optional<Sombrero::FastColor>> color;
        HeldData<float> fontSize;
        HeldData<bool> italic;
        Sombrero::FastVector2 anchoredPosition; // TODO: Const-ify
        Sombrero::FastVector2 sizeDelta; // TODO: Const-ify

        Text(std::string_view t = "", bool enabled_ = true, std::optional<Sombrero::FastColor> c = std::nullopt, float fontSize_ = 4, bool italic_ = true, UnityEngine::Vector2 anch = {0.0f, 0.0f}, UnityEngine::Vector2 sd = {60.0f, 10.0f})
            : text(t), enabled(enabled_), color(c), fontSize(fontSize_), italic(italic_), anchoredPosition(anch), sizeDelta(sd) {}

        UnityEngine::Transform* render(RenderContext& ctx) {
            auto& parent = ctx.parentTransform;
            // Recreating our own is not very bueno... ASSUMING we can avoid it, which we should be able to.
            if (textComp) {
                // Rewrite our existing text instance instead of making a new one
                assign();
            } else {
                textComp = QuestUI::BeatSaberUI::CreateText(&parent, text.getData(), *italic, anchoredPosition,
                                                            sizeDelta);

                textComp->set_fontSize(fontSize.getData());
                if (color) {
                    textComp->set_color(**color);
                }

                auto rectTransform = textComp->get_rectTransform();

                rectTransform->set_anchoredPosition(anchoredPosition);
                rectTransform->set_sizeDelta(sizeDelta);
                rectTransform->set_anchorMin(UnityEngine::Vector2(0.5f, 0.5f));
                rectTransform->set_anchorMax(UnityEngine::Vector2(0.5f, 0.5f));

                textComp->set_richText(true);

                assign<true>();
            }
            return textComp->get_transform();
        }

        inline void update() {
            CRASH_UNLESS(textComp);
            assign<false>();
        }

        [[nodiscard]] Text clone() const {
            Text clone(*this);
            clone.textComp = nullptr;
            return clone;
        }


#pragma region internal
        // Grab values from tmp
        explicit Text(TMPro::TextMeshProUGUI* textComp) :
                anchoredPosition(textComp->get_rectTransform()->get_anchoredPosition()),
                sizeDelta(textComp->get_rectTransform()->get_sizeDelta()) {
            CRASH_UNLESS(textComp);
            this->textComp = textComp;

            text = to_utf8(csstrtostr(textComp->get_text()));
            italic = text.getData().starts_with("<i>") && text.getData().ends_with("</i>");
            fontSize = textComp->get_fontSize();
            enabled = textComp->get_enabled();
            color = textComp->get_color();
        }

    protected:
        WeakPtrGO<TMPro::TextMeshProUGUI> textComp;

        template<bool created = false>
        void assign() {
            CRASH_UNLESS(textComp);
            if (enabled) {
                textComp->set_enabled(*enabled);
                enabled.clear();
            }
            if (!*enabled) {
                // Don't bother setting anything if we aren't enabled.
                return;
            }

            if constexpr (!created) {
                // Only set these properties if we did NOT JUST create the text.
                Il2CppString* text_cs = nullptr;

                if (italic) {
                    if (*italic) {
                        text_cs = il2cpp_utils::newcsstr("<i>" + std::string(text) + "</i>");
                    }
                    italic.clear();
                }
                else if (text) {
                    text_cs = il2cpp_utils::newcsstr(text.getData());
                    text.clear();
                }


                if (text_cs) { textComp->set_text(text_cs); }

                if (fontSize) {
                    textComp->set_fontSize(*fontSize);
                    fontSize.clear();
                }
                if (color) {
                    if (*color)
                        textComp->set_color(**color);

                    color.clear();
                }
            }
        }

#pragma endregion
    };
    static_assert(renderable<Text>);
    static_assert(renderable_return<Text, UnityEngine::Transform*>);
    static_assert(cloneable<Text>);
}
