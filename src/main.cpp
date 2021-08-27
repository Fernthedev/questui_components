#include "main.hpp"

#include "questui/shared/QuestUI.hpp"
#include "questui/shared/BeatSaberUI.hpp"

#include "components/Text.hpp"
#include "components/ScrollableContainer.hpp"
#include "components/HoverHint.hpp"
#include "components/ViewComponent.hpp"
#include "components/Button.hpp"

using namespace QuestUI;
using namespace QuestUI_Components;

static ModInfo modInfo; // Stores the ID and version of our mod, and is sent to the modloader upon startup

// Loads the config from disk using our modInfo, then returns it for use
Configuration& getConfig() {
    static Configuration config(modInfo);
    config.Load();
    return config;
}

// Returns a logger, useful for printing debug messages
Logger& getLogger() {
    static auto* logger = new Logger(modInfo, LoggerOptions(false, true));
    return *logger;
}

// Called at the early stages of game loading
extern "C" void setup(ModInfo& info) {
    info.id = ID;
    info.version = VERSION;
    modInfo = info;
	
    getConfig().Load(); // Load the config file
    getLogger().info("Completed setup!");
}

template<typename T>
struct PtrHolder {
    T* ptr;

    PtrHolder(T *ptr) : ptr(ptr) {}
};

void DidActivate(HMUI::ViewController* self, bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling){
    getLogger().info("DidActivate: %p, %d, %d, %d", self, firstActivation, addedToHierarchy, screenSystemEnabling);

    if (firstActivation) {
        // keep these pointers alive so the lambdas can capture them. These would usually be instance fields in a ViewCoordinator
        auto* container = new PtrHolder<ScrollableContainer>(nullptr);
        auto* newText = new PtrHolder<Text>(nullptr);
        int* count = new int(0);

        // We can use new because ComponentWrapper takes ownership of the pointer
        // A component will then only be freed if the parent is freed
        // We then make sure the parent never dies by never freeing
        // A better way of doing this is tying the highest parent to a ViewCoordinator or anything else

        auto* view = new ViewComponent{self->get_transform(), {
            .children = {
                    container->ptr = new ScrollableContainer {{
                        new HoverHint("hint", new Text("hi!")),
                        (new Text("this is cool! Pink Cute!"))->craftLater([](Text* text){
                            auto data = text->getData();
                            data.color = UnityEngine::Color(255.0f / 255.0f, 61.0f / 255.0f, 171.0f / 255.0f, 1.0f);
                            text->mutateData(data);
                            // we don't update here because it hasn't rendered, this is called before rendering
                        }),
                        new HoverHint("hintee", new Text("hello from other world!")),
                        new HoverHint("another hintee", new Text("this is cooler!!")),
                        new Button("Click me!", [newText, container, count](Button* button, UnityEngine::Transform* parentTransform) {
                            if (!newText->ptr) {
                                newText->ptr = new Text("New text!");
                                container->ptr->addToHierarchy(newText->ptr);
                            } else {
                                (*count)++;
                                auto data = (newText->ptr)->getData();
                                data.text = "someOtherText" + std::to_string(*count);
                                (newText->ptr)->mutateData(data);
                                (newText->ptr)->doUpdate();
                            }

                            // Update button text
                            auto buttonData = button->getData();
                            buttonData.text = "Clicked: " + std::to_string(*count);
                            button->mutateData(buttonData);
                            button->doUpdate();
                        })
                    }}
            }
        }};

        view->render();

        // Multiple renders should simply just update, not crash or duplicate.
        view->render();
        view->render();
    }
}

// Called later on in the game loading - a good time to install function hooks
extern "C" void load() {
    il2cpp_functions::Init();

    getLogger().info("Installing hooks...");
    // Install our hooks (none defined yet)
    getLogger().info("Installed all hooks!");

    QuestUI::Init();
    QuestUI::Register::RegisterModSettingsViewController(modInfo, DidActivate);
    getLogger().info("Successfully installed Settings UI!");
}