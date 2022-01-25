#pragma once

#include "shared/components/Image.hpp"

namespace QUC {
    class TacoImage : public Image {
    public:
        explicit TacoImage(const UnityEngine::Vector2 &sd, UnityEngine::Vector2 anch = {0.0f, 0.0f}, bool enabled_ = true);

    };
}
