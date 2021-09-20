#pragma once

#include "UnityEngine/Vector2.hpp"
#include "UnityEngine/Vector3.hpp"

#include "shared/context.hpp"
#include "questui/shared/BeatSaberUI.hpp"
#include "HMUI/InputFieldView.hpp"

#include <string>

namespace QUC {
    struct StringSetting {
        using OnCallback = std::function<void(StringSetting*, std::string, UnityEngine::Transform*)>;
        std::string text;
        OnCallback callback;
        bool enabled;
        bool interactable;
        std::string value;
        UnityEngine::Vector2 anchoredPosition;
        UnityEngine::Vector3 keyboardPositionOffset;

        template<class F>
        StringSetting(std::string_view txt, F&& callable, bool enabled_ = true, bool interact = true, std::string currentValue = "", UnityEngine::Vector2 anch = {}, UnityEngine::Vector3 offt = {})
            : text(txt), callback(callable), enabled(enabled_), interactable(interact), value(currentValue), anchoredPosition(anch), keyboardPositionOffset(offt) {}

        auto render(RenderContext& ctx) {
            // TODO: Cache this properly
            auto parent = &ctx.parentTransform;
            auto setting = QuestUI::BeatSaberUI::CreateStringSetting(parent, text, value, anchoredPosition, keyboardPositionOffset, [this, parent](std::string_view val) {
                callback(this, std::string(val), parent);
            });
            auto txt = setting->placeholderText->GetComponent<TMPro::TextMeshProUGUI*>();
            CRASH_UNLESS(txt);
            txt->set_text(il2cpp_utils::newcsstr(text));
            setting->set_enabled(enabled);
            setting->set_interactable(interactable);
            setting->SetText(il2cpp_utils::newcsstr(value));
            return setting->get_transform();
        }
    };
    static_assert(renderable<StringSetting>);
}
