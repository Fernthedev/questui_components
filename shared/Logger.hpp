#pragma once

#include "beatsaber-hook/shared/utils/logging.hpp"

// intentional typo, yay
namespace QuestUI_Components::Loggerr {
    static ModInfo modInfo{ID,VERSION}; // Stores the ID and version of our mod, and is sent to the modloader upon startup

    Logger& getLogger();
}