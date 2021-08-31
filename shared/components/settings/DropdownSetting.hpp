#pragma once

#include "UnityEngine/Vector2.hpp"

#include "BaseSetting.hpp"

#include <string>
#include <utility>
#include <vector>

namespace UnityEngine::UI {
    class Image;
}

namespace HMUI {
    class SimpleTextDropdown;
}

namespace TMPro {
    class TextMeshPro;
    class TextMeshProUGUI;
}

namespace QuestUI_Components {

    struct MutableDropdownSettingsData : public MutableSettingsData<std::string> {
        std::vector<std::string> values;
    };

class DropdownSetting : public BaseSetting<std::string, DropdownSetting, MutableDropdownSettingsData> {
    public:


        explicit DropdownSetting(std::string_view text, std::string_view currentValue,
                               std::initializer_list<std::string> values,
                               OnCallback callback = nullptr)
                               : BaseSetting(std::string(text), std::string(currentValue), std::move(callback))
                               {
            data.values = std::vector(values);
        }

    protected:
        void update() override;
        Component* render(UnityEngine::Transform *parentTransform) override;

        // render time
        HMUI::SimpleTextDropdown* uiDropdown = nullptr;
    };

// TODO: Test if it works
#pragma region ConfigEnum
#if defined(AddConfigValue) || defined(AddConfigValueString)
    template<typename EnumType>
    requires (std::is_enum<EnumType>::value)
    using EnumToStrType = std::unordered_map<EnumType, std::string>;

    template<typename EnumType>
    requires (std::is_enum<EnumType>::value)
    using StrToEnumType = std::unordered_map<std::string, EnumType>;

    template<typename EnumType>
    requires (std::is_enum<EnumType>::value)
    struct EnumToStr;

    template<typename EnumType>
    requires (std::is_enum<EnumType>::value)
    struct StrToEnum;


    template<typename EnumType>
    EnumToStrType<EnumType> createFromKeysAndValues(std::vector<std::string> keys, std::vector<EnumType> values) {
        EnumToStrType<EnumType> map(keys.size());
        for (int i =0; i < keys.size(); i++) {
            map.emplace(keys[i], values[i]);
        }

        return map;
    }

    template<typename EnumType>
    StrToEnumType<EnumType> createFromKeysAndValues(std::vector<EnumType> keys, std::vector<std::string> values) {
        StrToEnumType<EnumType> map(keys.size());
        for (int i =0; i < keys.size(); i++) {
            map.emplace(keys[i], values[i]);
        }

        return map;
    }

#define DROPDOWN_CREATE_ENUM_CLASS(EnumName, ...) \
enum struct EnumName {                            \
    __VA_ARGS__                                   \
}                                                 \
struct EnumToStr<EnumName> {                      \
    inline static const EnumToStrType<EnumName> map = createFromKeysAndValues({#__VA_ARGS__}, {__VA_ARGS__}) \
    EnumToStrType<EnumName> get(std::string const& key) {                                       \
        return map                                          \
    }                                              \
};                                                \
struct StrToEnum<EnumName> {                      \
    inline static const StrToEnumType<EnumName> map = createFromKeysAndValues({#__VA_ARGS__}, {__VA_ARGS__}) \
    StrToEnumType<EnumName> get(std::string const& key) {                                       \
        return map                                          \
    }                                              \
};


    template<typename EnumType, bool CrashOnBoundsExit = false>
    requires (std::is_enum<EnumType>::value)
    class ConfigUtilsEnumDropdownSetting : public ConfigUtilsSetting<std::string, DropdownSetting, EnumType> {
    public:
        static_assert(EnumToStr<EnumType>::map, "Please create a type specialization for EnumToStr");
        static_assert(StrToEnum<EnumType>::map, "Please create a type specialization for StrToEnum");

    protected:
        std::string getValue() override {
            const EnumToStrType<EnumType>& map = EnumToStr<EnumType>::map;
            if constexpr (CrashOnBoundsExit) {
                return map[this->configValue.get()];
            } else {
                auto it = map.find(this->configValue.get());

                if (it == map.end()) {
                    if (map.size() == 0) return "";
                    else return map.cbegin()->second;
                }

                return it->second;
            }
        };

        void internalSetValue(const std::string &val) override {
            const StrToEnumType<EnumType>& map = StrToEnum<EnumType>::map;
            if constexpr (CrashOnBoundsExit) {
                EnumType newValue = map[val];
                this->configValue.get().SetValue(newValue);
            } else {
                auto it = map.find(this->configValue.get());

                if (it == map.end()) {
                    if (map.size() == 0) this->configValue.get().SetValue((EnumType) 0);
                    else this->configValue.get().SetValue(map.cbegin()->second);
                }

                this->configValue.get().SetValue((EnumType) it->second);
            }
        }
    };
#endif
#pragma endregion
}