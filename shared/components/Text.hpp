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

        const Key key;

        Text(std::string_view t = "", bool enabled_ = true, std::optional<Sombrero::FastColor> c = std::nullopt, float fontSize_ = 4, bool italic_ = true, UnityEngine::Vector2 anch = {0.0f, 0.0f}, UnityEngine::Vector2 sd = {60.0f, 10.0f})
            : text(t), enabled(enabled_), color(c), fontSize(fontSize_), italic(italic_), anchoredPosition(anch), sizeDelta(sd) {}

        UnityEngine::Transform* render(RenderContext& ctx, RenderContextChildData& data) {
            auto& textComp = data.getData<TMPro::TextMeshProUGUI*>();
            auto& parent = ctx.parentTransform;
            // Recreating our own is not very bueno... ASSUMING we can avoid it, which we should be able to.
            if (textComp) {
                // Rewrite our existing text instance instead of making a new one
                assign(textComp);
            } else {
                textComp = QuestUI::BeatSaberUI::CreateText(&parent, text.getData(), *italic, anchoredPosition,
                                                            sizeDelta);



                assign<true>(textComp);
            }
            return textComp->get_transform();
        }



#pragma region internal
        // Grab values from tmp
        explicit Text(TMPro::TextMeshProUGUI* textComp) :
                anchoredPosition(textComp->get_rectTransform()->get_anchoredPosition()),
                sizeDelta(textComp->get_rectTransform()->get_sizeDelta()) {
            CRASH_UNLESS(textComp);

            text = to_utf8(csstrtostr(textComp->get_text()));
            italic = text.getData().starts_with("<i>") && text.getData().ends_with("</i>");
            fontSize = textComp->get_fontSize();
            enabled = textComp->get_enabled();
            color = textComp->get_color();
        }

        void copyFrom(TMPro::TextMeshProUGUI* textComp, RenderContext& ctx) {
            CRASH_UNLESS(textComp);

            auto rectTransform = textComp->get_rectTransform();

            anchoredPosition = rectTransform->get_anchoredPosition();
            sizeDelta = rectTransform->get_sizeDelta();


            text = to_utf8(csstrtostr(textComp->get_text()));
            italic = text.getData().starts_with("<i>") && text.getData().ends_with("</i>");
            fontSize = textComp->get_fontSize();
            enabled = textComp->get_enabled();
            color = textComp->get_color();

            // avoid creating another text later
            ctx.getChildData(key).getData<TMPro::TextMeshProUGUI*>() = textComp;
        }

    protected:
        template<bool created = false>
        void assign(TMPro::TextMeshProUGUI* textComp) {
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

            if constexpr (created) {
                textComp->set_fontSize(fontSize.getData());
                fontSize.clear();
                if (*color) {
                    textComp->set_color(**color);
                }
                color.clear();

                auto rectTransform = textComp->get_rectTransform();

                rectTransform->set_anchoredPosition(anchoredPosition);
                rectTransform->set_sizeDelta(sizeDelta);
                rectTransform->set_anchorMin(UnityEngine::Vector2(0.5f, 0.5f));
                rectTransform->set_anchorMax(UnityEngine::Vector2(0.5f, 0.5f));

                textComp->set_richText(true);
            }
        }

#pragma endregion
    };
    static_assert(renderable<Text>);
    static_assert(renderable_return<Text, UnityEngine::Transform*>);
    static_assert(cloneable<Text>);
}
