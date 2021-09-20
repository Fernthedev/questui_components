#pragma once

#include "UnityEngine/Vector2.hpp"

#include "shared/context.hpp"
#include "questui/shared/BeatSaberUI.hpp"
#include "beatsaber-hook/shared/utils/utils.h"

#include <string>
#include <string_view>
#include <functional>

#include "TMPro/TextMeshProUGUI.hpp"
#include "UnityEngine/UI/Toggle.hpp"

namespace QUC {
    struct ToggleSetting {
        using OnCallback = std::function<void(ToggleSetting*, bool, UnityEngine::Transform*)>;
        std::string text;
        OnCallback callback;
        bool enabled;
        bool interactable;
        bool value;
        UnityEngine::Vector2 anchoredPosition;

        template<class F>
        ToggleSetting(std::string_view txt, F&& callable, bool enabled_ = true, bool interact = true, bool currentValue = false, UnityEngine::Vector2 anch = {})
            : text(txt), callback(callable), enabled(enabled_), interactable(interact), value(currentValue), anchoredPosition(anch) {}

        auto render(RenderContext& ctx) {
            // TODO: Cache this properly
            auto parent = &ctx.parentTransform;
            auto toggle = QuestUI::BeatSaberUI::CreateToggle(parent, text, value, anchoredPosition, [this, parent](bool val) {
                callback(this, val, parent);
            });
            auto nameText = CRASH_UNLESS(toggle->get_transform()->get_parent()->Find(il2cpp_utils::createcsstr("NameText")))->get_gameObject();
            CRASH_UNLESS(nameText);
            auto txt = nameText->GetComponent<TMPro::TextMeshProUGUI*>();
            CRASH_UNLESS(txt);
            txt->set_text(il2cpp_utils::newcsstr(text));
            toggle->set_enabled(enabled);
            toggle->set_interactable(interactable);
            toggle->set_isOn(value);
            return toggle->get_transform();
        }
    };
    static_assert(renderable<ToggleSetting>);
}
