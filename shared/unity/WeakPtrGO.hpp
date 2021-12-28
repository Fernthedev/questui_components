#pragma once

#include "../context.hpp"

#include <unordered_map>

#include "custom-types/shared/macros.hpp"

#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/MonoBehaviour.hpp"

#include "beatsaber-hook/shared/utils/typedefs-wrappers.hpp"


// what have I created
namespace QUC::inner {
    class WeakPtrGOComponent;

    struct WeakPtrGoMap {
        inline static std::unordered_map<void*, QUC::inner::WeakPtrGOComponent*> goComponentMap;
    };
}

#define DECLARE_SIMPLE_MONO_DTOR() \
void __Finalize() {                \
    auto l = this;                               \
    il2cpp_utils::RunMethodThrow(l, il2cpp_utils::il2cpp_type_check::MetadataGetter<&UnityEngine::Object::Finalize>::get());                               \
    this->~___TargetType();        \
} \
___CREATE_INSTANCE_METHOD(__Finalize, "__Finalize", (::il2cpp_utils::FindMethod("System", "Object", "Finalize")->flags & ~METHOD_ATTRIBUTE_ABSTRACT) | METHOD_ATTRIBUTE_PUBLIC | METHOD_ATTRIBUTE_HIDE_BY_SIG, ::il2cpp_utils::FindMethod("System", "Object", "Finalize"))


// A hacky version of WeakPtr
DECLARE_CLASS_CODEGEN(QUC::inner, WeakPtrGOComponent, UnityEngine::MonoBehaviour,
  DECLARE_INSTANCE_METHOD(void, Init, void* innerPtr);

  DECLARE_INSTANCE_METHOD(void, OnDestroy); // calls destructor
  DECLARE_SIMPLE_MONO_DTOR();

  public:
  ~WeakPtrGOComponent();

  private:
  DECLARE_INSTANCE_FIELD(void*, innerPtr);
)

namespace QUC {
    template<typename T>
    requires(pointer_type_match<T*, Il2CppObject*>)
    class WeakPtrGO {
    public:
        WeakPtrGO() {
            ptr = nullptr;
        }

        explicit WeakPtrGO(T* ptr) {
            CRASH_UNLESS(ptr);
            emplace(ptr);
        }

        void emplace(T* ptr) {
            this->ptr = ptr;

            if (!ptr)
                return;

            auto it = inner::WeakPtrGoMap::goComponentMap.find((void*) ptr);
            if (it != inner::WeakPtrGoMap::goComponentMap.end()) {
                return;
            }

            static auto OBJ_NAME = il2cpp_utils::newcsstr<il2cpp_utils::CreationType::Manual>("WeakPtrGO_KeepAlive");

            auto go = UnityEngine::GameObject::Find(OBJ_NAME);

            if (!go) {
                go = UnityEngine::GameObject::New_ctor(OBJ_NAME);
                UnityEngine::GameObject::DontDestroyOnLoad(go);
            }

            auto comp = go->AddComponent<QUC::inner::WeakPtrGOComponent *>();
            comp->Init(ptr);
            inner::WeakPtrGoMap::goComponentMap[(void*) ptr] = comp;
        }

        inline WeakPtrGO<T>& operator=(T* other) {
            emplace(other);
            return *this;
        }

        T* const operator ->() const {
            if (!ptr)
                return nullptr;

            return ptr;
        }

        operator T* () const {
            validateInner();

            if (!ptr)
                return nullptr;

            return ptr;
        }

        operator bool() {
            validateInner();
            return (bool) ptr;
        }

    private:
        void validateInner() {
            if (ptr) {
                auto it = inner::WeakPtrGoMap::goComponentMap.find((void*) ptr);
                if (it == inner::WeakPtrGoMap::goComponentMap.end()) {
                    ptr = nullptr;
                }
            }
        }

        T* ptr;
    };
}

DEFINE_TYPE(QUC::inner, WeakPtrGOComponent)

namespace QUC::inner {
    void WeakPtrGOComponent::Init(void *innerPtr) {
        if (innerPtr)
            throw std::runtime_error("Inner ptr is already set");

        this->innerPtr = innerPtr;
    }

    void WeakPtrGOComponent::OnDestroy() {
        this->~WeakPtrGOComponent();
    }

    WeakPtrGOComponent::~WeakPtrGOComponent() {
        WeakPtrGoMap::goComponentMap.erase(innerPtr);
    }
}