# Differences from QUC v1
QUC v1 is for the most part entirely heap allocated and reference counted by smart pointers. This made the internal QUC framework easy to build and render while also allowing whoever used the library easy ways of adding components or modifying them.

This however is slower since it is based on virtual lookups. Reference counting was also a double-edged sword since it could cause memory leaks or even deadlocks as two components tried to free each other.
It also leaves room on the table for potential optimizations since very little can be optimized at compile time.

QUC v2 aims to fix these problems by focusing on compile-time `constexpr` render calls and constructors. While avoiding `new` allocations and smart pointers, memory leaks and deadlocks are avoided. 
The compiler makes good work of these changes and is able to optimize them in ways I can't explain since they're above my pay grade. 
Just magical compiler stuff that makes cooler assembly.

This also causes components to be copy constructed, which means modifications may only apply to their copies depending on how it's done. This is where the [`RenderCtx`](#render-context) and [`Key`](#Key) data types come in.

# Components
Components are the first part of making your UI. There is a wide variety of [core components](../shared/components) that you can use already. 
However, in the chance you need to customize the component behaviour or group multiple components, making your own custom components is trivial.

For working examples, you may take a look at the [test components](../test/include/TestComponent.hpp)

The gist of a custom component is that it should adhere to the `renderable` requirements and `renderable_return<Component, UnityEngine::Transform*>` if it will be wrapped by other components such as `HoverHint`

A very basic example: 
```cpp
struct CustomComponent {
    // Identifies the component in the tree
    const Key key;
    HeldData<int> number = 0;

    void render(RenderContext& ctx, RenderContextChildData& data) {
        // number.isModified() is equivalent
        // if a new value was assigned, this returns true until clear() is called
        if (number) {
            int actualNumber = *number;
            number.clear();
        }
        // do stuff
        // parent transform is in ctx.parentTransform. You can use &ctx.parentTransform to retrieve the pointer.
    }
}
static_assert(renderable<CustomComponent>);
```

# Render Context
While components can store data such as text, it cannot store anything stateful such as Unity Text pointers or increment counters. 
To solve this problem, we use the `RenderCtx` type. This type stores the pointer to the parent UnityEngine.Transform and the data of the children components. 

These children components can get the state data through the RenderContext passed in `render()` and the `Key` field. 
This is practically just a hashmap lookup of data through the `Key` which will always definitively be unique relative to other instances.

# Key
This field is very simple and defined in [key.hpp](../shared/key.hpp). This type basically hashes the instance pointer of itself which guarantees its uniqueness. It is required by all components that can be rendered

This allows for caching data of components and reusing that data instead of destroying a tree and constructing a new one. It also allows for a component tree to reorder components or render specific components without breaking the UI. 
Be aware that reordering components won't actually change their order in Unity UI unless you do it manually.

Thanks sc2ad for this!


TODO:
## Specific components:
# Modal
# Recyclable List
Recyclable list is a component that wraps around BeatSaber's RecyclableCelledList. 
To use it, create a struct that extends `QUCDescriptor` to store the data each cell will consume.
```cpp
#include "questui_components/shared/components/list/CustomTypeTable.hpp"

struct CellData : QUC::CustomTypeList::QUCDescriptor {
    std::string displayedText;
    
    CellData(std::string_view displayedText) : displayedText(displayedText) {}
};

// Header
// Creates a custom type for cell and table that will consume CellData and build the cells
DECLARE_QUC_TABLE_CELL(QUC, QUCObjectTableCell, )
DECLARE_QUC_TABLE_DATA(QUC, QUCObjectTableData, CellData, QUCObjectTableCell, );
```

After you have created your `QUCDescriptor` struct and declared the custom-types, you'll need to create a QUC component that will render the data. These components will be recycled with data being provided by QUC.
This is an example rendering a button with strings and reusing existing components.
```cpp
#include "questui_components/shared/components/list/CustomCelledList.hpp"
#include "questui_components/shared/components/Button.hpp"

struct CellComponent {
    QUC::Button button = QUC::Button("", nullptr);

    void render(CellData const& cellData, QUC::RenderContext& cellCtx) {
        button.text.text = cellData.displayedText;
        QUC::detail::renderSingle(button, cellCtx);
    }
};

// Ensures the component can be rendered by QUC
static_assert(QUC::ComponentCellRenderable<CellComponent, CellData>);
```

Now, the final step for a RecyclableCelledList is to define the custom types in a source file.
```cpp
#include "MyTable.hpp"

// Source
DEFINE_QUC_CUSTOMLIST_TABLEDATA(QUC, QUCObjectTableData);
DEFINE_QUC_CUSTOMLIST_CELL(QUC, QUCObjectTableCell);
```

To use the RecycledTable

```cpp
#include "MyTable.hpp"

QUC::CustomTypeList::QUCTableInitData tableInitData;
tableInitData.sizeDelta = {60, 70};

std::vector<CellData> cellDatas({CellData("data"), CellData("thing"), {"magic"}, {"school"}, {"bus"}, {"tacos"}, {"are"}, {"cool"}, {"why"}, {"do"}, {"I"}, {"do"}, {"this"}});

RecycledTable<QUCObjectTableData, CellComponent>(
    cellDatas,
    tableInitData
);
```