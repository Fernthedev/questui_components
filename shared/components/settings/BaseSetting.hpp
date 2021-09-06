#pragma once

#include "UnityEngine/Vector2.hpp"

#include "shared/Component.hpp"
#include "shared/RootContainer.hpp"
#include "shared/components/HoverHint.hpp"

#include <string>
#include <utility>
#include <vector>

#include "UnityEngine/Transform.hpp"
#include "config-utils/shared/config-utils.hpp"

namespace UnityEngine::UI {
    class Image;
}

namespace TMPro {
    class TextMeshProUGUI;
}



namespace QuestUI_Components {

    template<typename ValueType>
    class BaseSettingModifier;

    template<typename ValueType>
    struct MutableSettingsData {
        // Mutable variables
        std::string text;

        bool enabled = true;
        bool interactable = true;

        friend class BaseSettingModifier<ValueType>;
    private:
        // To edit this, use getValue() and setValue()
        ValueType value;
    };

    // For encapsulation magic, woohoo!
    template<typename ValueType>
    class BaseSettingModifier {
    protected:
        static inline ValueType getValueOfData(const MutableSettingsData<ValueType>& data) {
            return data.value;
        }

        static inline ValueType setValueOfData(MutableSettingsData<ValueType>& data, const ValueType& value) {
            data.value = value;

            return data.value;
        }
    };

    template<typename ValueType, typename SettingType, typename MutableData = MutableSettingsData<ValueType>>
    class BaseSetting : public Component, public UpdateableComponent<MutableData>, protected BaseSettingModifier<ValueType> {
    public:
        using OnCallback = std::function<void(SettingType*, ValueType, UnityEngine::Transform*)>;
        using CallbackWrapper = std::function<void(ValueType)>;
//        using Modifier = BaseSettingModifier<ValueType>;

        explicit BaseSetting(std::string_view text, ValueType currentValue, OnCallback callback = nullptr) : callback(std::move(callback)) {
            this->data.text = text;
            this->setValueOfData(this->data, currentValue);
        }

        // Allow constructing with lambda
        CONSTRUCT_AFTER_COMPONENT(SettingType)

        // Override if needed. For example: config-utils
        virtual ValueType getValue() {
            return this->getValueOfData(this->data);
        }

        // This is always called on set, whether your callback is nullptr or not.
        ValueType setValue(ValueType val) {
            bool different = getValue() != val;
            this->updated |= different;
            if (different) {
                internalSetValue(val);
            }
            return val;
        }
    protected:
        CallbackWrapper constructWrapperCallback(UnityEngine::Transform *parentTransform) {
            if (callback) {
                return [this, parentTransform](ValueType val) {
                    setValue(val);
                    // better way of doing this than reinterpret cast to child?
                    callback(reinterpret_cast<SettingType*>(this), val, parentTransform);
                };
            } else {
                return [this](ValueType val) {
                    setValue(val);
                };
            }
        }

        void update() override {
            if (!rendered)
                throw std::runtime_error("Toggle setting component has not rendered!");

            if (uiText) {
                uiText->set_text(il2cpp_utils::newcsstr(this->data.text));
            }
        }

        virtual void internalSetValue(const ValueType& val) {
            this->mutateData([&val, this](MutableData data) {
                this->setValueOfData(data, val);
                return data;
            });
        }
        // render time
        TMPro::TextMeshProUGUI* uiText = nullptr;

        // constructor time
        const OnCallback callback;
    };



#if defined(AddConfigValue) || __has_include("config-utils/shared/config-utils.hpp")
    template<typename ValueType, typename SettingType, typename ConfigValueType = ValueType>
    class ConfigUtilsSetting : public SettingType {
    public:
        template<typename... TArgs>
        explicit ConfigUtilsSetting(ConfigUtils::ConfigValue<ConfigValueType>& configValue, TArgs&&... args) : ConfigUtilsSetting(configValue.GetValue(), configValue, args...) {

        }

        void doUpdate() override {
            this->setValue(this->getValue());
            SettingType::doUpdate();
        }

    protected:
        template<typename... TArgs>
        explicit ConfigUtilsSetting(ValueType currentValue, ConfigUtils::ConfigValue<ConfigValueType>& configValue, TArgs&&... args) : configValue(configValue), SettingType(configValue.GetName(), currentValue, args...) {
            if (!configValue.GetHoverHint().empty()) {
                hoverHint = std::make_shared<HoverHint>(configValue.GetHoverHint(), this);
            }
        }


        // reference capture should be safe here
        std::shared_ptr<HoverHint> hoverHint;
        const ConfigUtils::ConfigValue<ConfigValueType>& configValue;


        ValueType getValue() override {
            return configValue.get().GetValue();
        }

        void internalSetValue(const ValueType& val) override {
            configValue.get().SetValue(val);
        }

        Component *render(UnityEngine::Transform *parentTransform) override {
            SettingType::render(parentTransform);

            if (hoverHint) {
                return hoverHint.get();
            } else {
                return this;
            }
        };

        void update() override {
            SettingType::update();
        };
    };
#endif
}