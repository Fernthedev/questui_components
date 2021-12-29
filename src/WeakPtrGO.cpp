#include "shared/unity/WeakPtrGO.hpp"

DEFINE_TYPE(QUC::inner, WeakPtrGOComponent)

QUC::WeakPtrHolder::ComponentMap QUC::WeakPtrHolder::goComponentMap = {};

namespace QUC::inner {
    using namespace QUC;

    void WeakPtrGOComponent::Init(void *innerPtr) {
        WeakPtrHolder::goComponentMap[this].emplace((void*) innerPtr);
    }

    void WeakPtrGOComponent::OnDestroy() {
        this->~WeakPtrGOComponent();
    }

    WeakPtrGOComponent::~WeakPtrGOComponent() {
        doDestroy();
    }

    void WeakPtrGOComponent::doDestroy() {
        WeakPtrHolder::goComponentMap.erase(this);
    }
}