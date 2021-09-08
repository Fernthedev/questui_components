#include "shared/Logger.hpp"

// Returns a logger, useful for printing debug messages
Logger& QuestUI_Components::Loggerr::getLogger() {
    static auto* logger = new Logger(modInfo, LoggerOptions(false, true));
    return *logger;
}