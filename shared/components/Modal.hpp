#pragma once

#include "shared/context.hpp"
#include "questui/shared/BeatSaberUI.hpp"

#include "UnityEngine/Vector2.hpp"
#include <functional>

namespace HMUI {
    class ModalView;
}

namespace QUC {
    struct Modal {
        UnityEngine::Vector2 sizeDelta;
        UnityEngine::Vector2 anchoredPosition;
        bool dismissOnBlockerClicked;
        
        template<class F>
        Modal(F&& callable, UnityEngine::Vector2 sz = {30.0f, 40.0f}, UnityEngine::Vector2 anch = {0.0f, 0.0f}, bool dismiss = true)
            : sizeDelta(sz), anchoredPosition(anch), dismissOnBlockerClicked(dismiss), callback(callable) {}
        auto render(RenderContext& ctx) {
            std::function<void(HMUI::ModalView*)> cbk([this](HMUI::ModalView* arg) {
                callback(this, arg);
            });
            // TODO: Add proper tree recaching on parent context.
            return QuestUI::BeatSaberUI::CreateModal(&ctx.parentTransform, sizeDelta, anchoredPosition, cbk, dismissOnBlockerClicked)->get_transform();
        }
        private:
        std::function<void(Modal*, HMUI::ModalView*)> callback;
    };
    static_assert(renderable<Modal>);
}
