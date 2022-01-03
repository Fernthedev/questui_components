#include "main.hpp"

#include "questui/shared/QuestUI.hpp"
#include "questui/shared/BeatSaberUI.hpp"
#include "questui/shared/CustomTypes/Components/MainThreadScheduler.hpp"

#include "shared/components/Text.hpp"
#include "shared/components/ScrollableContainer.hpp"
#include "shared/components/HoverHint.hpp"
#include "shared/components/Button.hpp"
#include "shared/components/Modal.hpp"
#include "shared/components/layouts/VerticalLayoutGroup.hpp"
#include "shared/components/layouts/HorizontalLayoutGroup.hpp"
#include "shared/components/layouts/ModifierContainer.hpp"
#include "shared/components/settings/ToggleSetting.hpp"
#include "shared/components/settings/StringSetting.hpp"
#include "shared/components/settings/IncrementSetting.hpp"
#include "shared/components/settings/DropdownSetting.hpp"
#include "shared/components/misc/RainbowText.hpp"

// Custom components
#include "TestComponent.hpp"
#include "TacoImage.hpp"

#include "UnityEngine/UI/Image.hpp"

using namespace QuestUI;
using namespace QuestUI_Components;
using namespace QuestUI_Components::Loggerr;

// Loads the config from disk using our modInfo, then returns it for use
Configuration& getConfig() {
    static Configuration config(modInfo);
    config.Load();
    return config;
}


// Called at the early stages of game loading
extern "C" void setup(ModInfo& info) {
    info.id = ID;
    info.version = VERSION;
    modInfo = info;
	
    getConfig().Load(); // Load the config file
    getLogger().info("Completed setup!");
}

#pragma region Loading


auto LoadingView(std::string_view loadingText) {
    using namespace QUC;

    return ScrollableContainer(
            Text(loadingText)
    );
}

void HandleLoadingView(QUC::RenderContext& ctx, bool& loaded) {
    using namespace QUC;

    const std::string templateLoadingText = "Loading";

    LoadingView(templateLoadingText).render(ctx);

    // async UI!
    std::thread([templateLoadingText, ctx, &loaded]{
        int periodCount = 0;
        bool once = true;

        while (!loaded || once) {
            getLogger().debug("Loading check! %i", periodCount);
            once = false;
            std::this_thread::sleep_for(std::chrono::milliseconds(200));

            if (loaded) break;

            periodCount++;
            if (periodCount > 4) periodCount = 1;

            std::string textStr(templateLoadingText + std::string(periodCount, '.'));

            QuestUI::MainThreadScheduler::Schedule([textStr, ctx, &loaded]() mutable {
                if (!loaded) {
                    // Updates the entire hierarchy
                    LoadingView(textStr).render(ctx);
                }
            });
        }
    }).detach();
}
#pragma endregion

#pragma region defaultView
auto tuple(QUC::ModalWrapper& modal) {
    using namespace QUC;
    return std::tuple(
            HorizontalLayoutGroup(
                    Text("Look at me!"),
                    Button("Close!", [&modal](Button *button, UnityEngine::Transform *) {
                        modal.dismiss();
                    })
            )
    );
}

auto DefaultView() {
    using namespace QUC;

    Text pinkCuteText("this is cool! Pink Cute!", true, UnityEngine::Color(1.0f, 61.0f / 255.0f, 171.0f / 255.0f, 1.0f));

    // TODO: Allow lambda construction here
    // We need to figure out how to do type deduction using lambda
    // C++ can't deduct the type of Modal from the lambda since Modal<> can't accept children of std::tuple<blah, blah, blah>()
    // and C++ won't deduct the type based on constructor arguments from the lambda return type
    // C++ life is sad
    Modal modal(tuple);

    // Image has to be loaded on main thread
    // When passing this to a container, it will take ownership, so we don't need to call delete
    auto tacoImage = TacoImage({128, 128});

    return ScrollableContainer(
            HoverHint("hint", Text("hi!")),
            pinkCuteText,

            // TODO: we can create components using lambdas too

            Button("More info!", [&modal](Button* button, UnityEngine::Transform* transform){
                modal.show();
            }),
            // Custom component
            TestComponent("pink cute eris cute"),
            RainbowText("Rainbow!"),

            // Image is loaded on main thread
            HorizontalLayoutGroup(
                    VerticalLayoutGroup(
                            tacoImage
                            )
                    ),

            // Toggles
            ToggleSetting("Toggle false", nullptr, false),
            ToggleSetting("Toggle true", [](ToggleSetting* set, bool val, UnityEngine::Transform*) {
                set->text->text =  "Toggle " + std::string(val ? "true" : "false");
                set->toggleButton.value = val;
                set->update();
            }, true),
            StringSetting("Text setting", [](StringSetting*, const std::string& input, UnityEngine::Transform*){
                getLogger().debug("Input! %s", input.c_str());
            }, "The current val!"),
            QUC::IncrementSetting("Increment!", [](QUC::IncrementSetting* set, float input, UnityEngine::Transform*){
                getLogger().debug("Increment value! %f", input);
                set->text = "Increment value: " + std::to_string(input);
                set->update();
            }, 5.0f, 2, 0.05f),
            QUC::VariableDropdownSetting("Dropdowns are cool!", "some val", [](VariableDropdownSetting* set, const std::string& selected, UnityEngine::Transform*){
                getLogger().debug("Dropdowns are cool %s", selected.c_str());
                set->text = "Dropdowns are coeaweol!" + selected;
                auto list = set->values.getData();
                list.emplace_back(std::to_string(list.size()));
                set->values = list;
                set->update();
            }, {"value1", "value2", "some val", "value3"}),

            HoverHint("hintee", Text("hello from other world!")),
            HoverHint("another hintee", Text("this is cooler!!")),
            Button("Click me!", [](Button* button, UnityEngine::Transform* parentTransform) {
                static bool clicked = false;
                static Text newText("New text!");
                static int count = 0;

                if (!clicked) {
                    clicked = true;
                    RenderContext ctx(parentTransform);
                    newText.render(ctx);
                } else {
                    count++;
                    newText.text = "someOtherText" + std::to_string(count);
                    newText.update();
                }

                // Update button text
                button->text = "Clicked: " + std::to_string(count);
                button->update();
            })
    );
}
#pragma endregion

void DidActivate(HMUI::ViewController* self, bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling){
    using namespace QUC;

    getLogger().info("DidActivate: %p, %d, %d, %d", self, firstActivation, addedToHierarchy, screenSystemEnabling);

    RenderContext ctx(self->get_transform());
    if (firstActivation) {
        // keep these pointers alive so the lambdas can capture them. These would usually be instance fields in a ViewCoordinator

        static bool loaded = false;

        if (!loaded) {
            HandleLoadingView(ctx, loaded);
        }

        static Text newText;
        static int count = 0;



#pragma region FullyLoadedUI



        std::thread([ctx]{
            // Simulate slow UI
            std::this_thread::sleep_for(std::chrono::seconds(2));

            auto defaultView = DefaultView();

            QuestUI::MainThreadScheduler::Schedule([defaultView, ctx]() mutable {
                loaded = true;
                defaultView.render(ctx);

                // Multiple renders should simply just update, not crash or duplicate.
                defaultView.render(ctx);
                defaultView.render(ctx);
            });
        }).detach();
#pragma clang diagnostic pop

#pragma endregion
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