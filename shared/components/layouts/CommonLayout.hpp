#pragma once

#include "shared/context.hpp"
#include "shared/RootContainer.hpp"
#include "shared/state.hpp"


#include "UnityEngine/RectOffset.hpp"
#include "UnityEngine/TextAnchor.hpp"
#include "UnityEngine/UI/HorizontalOrVerticalLayoutGroup.hpp"

namespace QUC {
    struct ModifyLayout {
        QUC::HeldData<UnityEngine::TextAnchor> childAlignment;
        QUC::RenderHeldData<bool> childControlWidth;
        QUC::RenderHeldData<bool> childControlHeight;
        QUC::RenderHeldData<bool> childForceExpandHeight;
        QUC::RenderHeldData<bool> childForceExpandWidth;
        QUC::RenderHeldData<bool> childScaleWidth;
        QUC::RenderHeldData<float> spacing;

        QUC::HeldData<std::array<float, 4>> padding; // TODO: How to make this not stupid


        auto assignLayout(RenderContext& ctx, UnityEngine::UI::HorizontalOrVerticalLayoutGroup* viewLayout) {
            if (childAlignment.isModified()) {
                viewLayout->set_childAlignment(childAlignment.getData());
                childAlignment.clear();
            }

            if (childControlHeight.readAndClear<DiffModifyCheck::TRUE_WHEN_FOUND_OR_ASSIGNED>(ctx)) {
                viewLayout->set_childControlHeight(childControlHeight.getData());
            }

            if (childControlWidth.readAndClear<DiffModifyCheck::TRUE_WHEN_FOUND_OR_ASSIGNED>(ctx)) {
                viewLayout->set_childControlWidth(childControlWidth.getData());
            }

            if (childForceExpandHeight.readAndClear<DiffModifyCheck::TRUE_WHEN_FOUND_OR_ASSIGNED>(ctx)) {
                viewLayout->set_childForceExpandHeight(childForceExpandHeight.getData());
            }

            if (childForceExpandWidth.readAndClear<DiffModifyCheck::TRUE_WHEN_FOUND_OR_ASSIGNED>(ctx)) {
                viewLayout->set_childForceExpandWidth(childForceExpandWidth.getData());
            }

            if (childScaleWidth.readAndClear<DiffModifyCheck::TRUE_WHEN_FOUND_OR_ASSIGNED>(ctx)) {
                viewLayout->set_childScaleWidth(childScaleWidth.getData());
            }

            if (spacing.readAndClear<DiffModifyCheck::TRUE_WHEN_FOUND_OR_ASSIGNED>(ctx)) {
                viewLayout->set_spacing(spacing.getData());
            }

            if (padding.isModified()) {
                auto const& arr = *padding;
                viewLayout->set_padding(UnityEngine::RectOffset::New_ctor(arr[0], arr[1], arr[2], arr[3]));
                padding.clear();
            }
        }
    };

    template<typename Layout>
    requires (QUC::renderable_return<Layout, UnityEngine::Transform*>)
    struct ModifyLayoutElement {
        const QUC::Key key;
        std::remove_reference_t<Layout> layout;

        constexpr ModifyLayoutElement(Layout layout) : layout(layout) {}

        QUC::RenderHeldData<float> preferredWidth;
        QUC::RenderHeldData<float> preferredHeight;

        QUC::RenderHeldData<float> minWidth;
        QUC::RenderHeldData<float> minHeight;

        QUC::RenderHeldData<bool> ignoreLayout;

        constexpr auto render(QUC::RenderContext& ctx, QUC::RenderContextChildData& data) {
            UnityEngine::Transform* ret = QUC::detail::renderSingle(layout, ctx);
            auto& layoutElement = data.getData<UnityEngine::UI::LayoutElement*>();

            if (!layoutElement) {
                auto go = ret->get_gameObject();
                layoutElement = go->GetComponent<UnityEngine::UI::LayoutElement*>();
                if (!layoutElement) {
                    layoutElement = go->AddComponent<UnityEngine::UI::LayoutElement*>();
                }
            }

            if (preferredWidth.readAndClear<DiffModifyCheck::TRUE_WHEN_FOUND_OR_ASSIGNED>(ctx)) {
                layoutElement->set_preferredWidth(preferredWidth.getData());
            }

            if (preferredHeight.readAndClear<DiffModifyCheck::TRUE_WHEN_FOUND_OR_ASSIGNED>(ctx)) {
                layoutElement->set_preferredHeight(preferredHeight.getData());
            }

            if (minHeight.readAndClear<DiffModifyCheck::TRUE_WHEN_FOUND_OR_ASSIGNED>(ctx)) {
                layoutElement->set_minHeight(minHeight.getData());
            }

            if (minWidth.readAndClear<DiffModifyCheck::TRUE_WHEN_FOUND_OR_ASSIGNED>(ctx)) {
                layoutElement->set_minWidth(minWidth.getData());
            }

            if (ignoreLayout.readAndClear<DiffModifyCheck::TRUE_WHEN_FOUND_OR_ASSIGNED>(ctx)) {
                layoutElement->set_ignoreLayout(ignoreLayout.getData());
            }

            return ret;
        }

    };

    template<typename Layout>
    requires (QUC::renderable_return<Layout, UnityEngine::Transform*>)
    struct ModifyContentSizeFitter {
        const QUC::Key key;
        std::remove_reference_t<Layout> layout;

        constexpr ModifyContentSizeFitter(Layout layout) : layout(layout) {}

        QUC::HeldData<UnityEngine::UI::ContentSizeFitter::FitMode> horizontalFit;
        QUC::HeldData<UnityEngine::UI::ContentSizeFitter::FitMode> verticalFit;

        constexpr auto render(QUC::RenderContext& ctx, QUC::RenderContextChildData& data) {
            UnityEngine::Transform* ret = QUC::detail::renderSingle(layout, ctx);
            auto& layoutSizeFitter = data.getData<UnityEngine::UI::ContentSizeFitter*>();

            if (!layoutSizeFitter) {
                auto go = ret->get_gameObject();
                layoutSizeFitter = go->GetComponent<UnityEngine::UI::ContentSizeFitter*>();
                if (!layoutSizeFitter) {
                    layoutSizeFitter = go->AddComponent<UnityEngine::UI::ContentSizeFitter*>();
                }
            }

            if (horizontalFit.isModified()) {
                layoutSizeFitter->set_horizontalFit(*horizontalFit);
                horizontalFit.clear();
            }

            if (verticalFit.isModified()) {
                layoutSizeFitter->set_verticalFit(*verticalFit);
                verticalFit.clear();
            }

            return ret;
        }

    };
}