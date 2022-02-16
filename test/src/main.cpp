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
#include "shared/components/settings/ToggleSetting.hpp"
#include "shared/components/settings/StringSetting.hpp"
#include "shared/components/settings/IncrementSetting.hpp"
#include "shared/components/settings/DropdownSetting.hpp"
#include "shared/components/misc/RainbowText.hpp"

// Custom components
#include "TestComponent.hpp"
#include "TacoImage.hpp"
#include "TestTable.hpp"

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


auto HandleLoadingView(QUC::RenderContext& ctx, bool& loaded) {
    using namespace QUC;

    const std::string templateLoadingText = "Loading";

    static Text text(templateLoadingText);
    static auto view = ScrollableContainer<Text&>(
            text
    );

    auto transform = QUC::detail::renderSingle(view, ctx);

    getLogger().debug("Loading %p", transform);

//    static UnityEngine::Transform* scrollTransform;
//    scrollTransform = LoadingView(templateLoadingText).render(ctx);

    // async UI!
    std::thread([templateLoadingText, &ctx, &loaded]()mutable{
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
            text.text = textStr;

            QuestUI::MainThreadScheduler::Schedule([&ctx, &loaded]() mutable {
                if (!loaded) {
                    QUC::detail::renderSingle(view, ctx);
//                    // Destroy old hierarchy
//                    UnityEngine::Object::Destroy(scrollTransform->get_gameObject());
//                    // Updates the entire hierarchy
//                    scrollTransform = view.render(ctx);
                }
            });
        }
    }).detach();

    return transform;
}
#pragma endregion

#pragma region defaultView
auto tuple(QUC::ModalWrapper& modal) {
    using namespace QUC;
    return std::tuple(

    );
}

auto DefaultView(QUC::TacoImage& tacoImage) {
    using namespace QUC;

    Text pinkCuteText("this is cool! Pink Cute!", true, UnityEngine::Color(1.0f, 61.0f / 255.0f, 171.0f / 255.0f, 1.0f));

    // We create a ModalWrapper which holds the Modal data
    // This will store the Modal instance data even across copies of Modal
    // And is reference counted
    auto modalWrapper = std::make_shared<ModalWrapper>();
    Modal modal(modalWrapper,
        HorizontalLayoutGroup(
            VerticalLayoutGroup(
                Text("Look at me!"),
                Button("Close!",
                   [modalWrapper](Button &button, UnityEngine::Transform *, RenderContext &ctx)mutable {
                       modalWrapper->dismiss();
               })
            )
        )
    );

    QUC::CustomTypeList::QUCTableInitData tableInitData;
    tableInitData.sizeDelta = {60, 70};

    return ScrollableContainer(
            HoverHint("hint", Text("hi!")),
            Text(pinkCuteText),

            Modal(modal),
            Button("More info!", [modal](Button& button, UnityEngine::Transform* transform, RenderContext& ctx)mutable{
//                auto modal = BeatSaberUI::CreateModal(transform);
//                auto horizontal = BeatSaberUI::CreateHorizontalLayoutGroup(modal->get_transform());
//                auto vertical = BeatSaberUI::CreateVerticalLayoutGroup(horizontal->get_transform());
//                auto text = BeatSaberUI::CreateText(vertical->get_transform(), "Ha");
//                auto buttonT = BeatSaberUI::CreateUIButton(vertical->get_transform(), "t", []{});
//
//                modal->Show(true, true, nullptr);
                modal.show();
            }),
            // Custom component
            TestComponent("pink cute eris cute"),
//            MoreComplexType("taco"),
            RainbowText("Rainbow!"),

            // Image is loaded on main thread
            HorizontalLayoutGroup(
                VerticalLayoutGroup(
                    TacoImage(tacoImage)
                )
            ),

//             Toggles
            ToggleSetting("Toggle false", nullptr, false),
            ToggleSetting("Toggle true", [](ToggleSetting& set, bool val, UnityEngine::Transform*, RenderContext& ctx) {
                set.text.text =  "Toggle " + std::string(val ? "true" : "false");
                set.toggleButton.value = val;
                set.update(ctx);
            }, true),
            StringSetting("Text setting", [](StringSetting&, const std::string& input, UnityEngine::Transform*, RenderContext& ctx){
                getLogger().debug("Input! %s", input.c_str());
            }, "The current val!"),
            QUC::IncrementSetting("Increment!", [](QUC::IncrementSetting& set, float input, UnityEngine::Transform*, RenderContext& ctx){
                getLogger().debug("Increment value! %f", input);
                set.text = "Increment value: " + std::to_string(input);
                set.update(ctx);
            }, 5.0f, 2, 0.05f),
            QUC::VariableDropdownSetting("Dropdowns are cool!", "some val", [](VariableDropdownSetting& set, const std::string& selected, UnityEngine::Transform*, RenderContext& ctx){
                getLogger().debug("Dropdowns are cool %s", selected.c_str());
                set.text = "Dropdowns are coeaweol!" + selected;
                auto list = set.values.getData();
                list.emplace_back(std::to_string(list.size()));
                set.values = list;
                set.update(ctx);
            }, {"value1", "value2", "some val", "value3"}),

            HoverHint("hintee", Text("hello from other world!")),
            HoverHint("another hintee", Text("this is cooler!!")),
            Button("Click me!", [](Button& button, UnityEngine::Transform* parentTransform, RenderContext& ctx) {
                static bool clicked = false;
                static Text newText("New text!");
                static int count = 0;

                if (!clicked) {
                    clicked = true;
                } else {
                    count++;
                    newText.text = "someOtherText" + std::to_string(count);
                }
                QUC::detail::renderSingle(newText, ctx);

                // Update button text
                button.text = "Clicked: " + std::to_string(count);
                button.update(ctx);
            }),

            RecycledTable<QUCObjectTableData, CellComponent>(
                    {CellData("data"), CellData("thing"), {"magic"}, {"school"}, {"bus"}, {"tacos"}, {"are"}, {"cool"}, {"why"}, {"do"}, {"I"}, {"do"}, {"this"}},
                    tableInitData
            )

    );
}
#pragma endregion

void DidActivate(HMUI::ViewController* self, bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling){
    using namespace QUC;

    getLogger().info("DidActivate: %p, %d, %d, %d", self, firstActivation, addedToHierarchy, screenSystemEnabling);

    static RenderContext loadingCtx(self->get_transform());
    static RenderContext ctx(self->get_transform());

    // Image has to be loaded on main thread
    // When passing this to a container, it will take ownership, so we don't need to call delete
    static auto tacoImage = TacoImage({128, 128});

    static UnityEngine::Transform* loadingViewTransform;
    if (firstActivation) {
        // keep these pointers alive so the lambdas can capture them. These would usually be instance fields in a ViewCoordinator

        static bool loaded = false;

        if (!loaded) {
            loadingViewTransform = HandleLoadingView(loadingCtx, loaded);
        }



#pragma region FullyLoadedUI
        std::thread([]{
            // Simulate slow UI
            std::this_thread::sleep_for(std::chrono::seconds(2));

            static auto defaultView = DefaultView(tacoImage);

            QuestUI::MainThreadScheduler::Schedule([]() {
                loaded = true;
                loadingCtx.destroyTree();

                QUC::detail::renderSingle(defaultView, ctx);

                // Multiple renders should simply just update, not crash or duplicate.
                QUC::detail::renderSingle(defaultView, ctx);
                QUC::detail::renderSingle(defaultView, ctx);
            });
        }).detach();

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