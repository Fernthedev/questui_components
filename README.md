# QuestUI Components
## What's this?
This is a declarative UI framework (or wrapper?) over QuestUI methods that aims to improve the developer experience and allow reusable components.

This is **highly** inspired by other UI declarative frameworks, specifically Flutter.

## How does this compare to BSML?
BSML can be considered an imperative UI framework, which would be separating the UI layout from the code. 

This is different because UI would be written in a `.bsml` file while in C# you would make a companion class that will handle the logic. 
This can be problematic in some ways:
- You have to assume your UI exists in C#, there is no guarantee or safety of what you're trying to use. Usually typos or mistakes
- The UI logic and layout are not directly related.

BSML still does have some advantages though: 
- Code can be split apart for organization, which is subjective. Angular does this by keeping the `.css`, `.html` and `.js` code all separated.
- XML UI may be easier to understand 
- Updating UI in C# can be more straightforward than state management in QuestUI Components.

## Philosophies and origin
Lots of people saw BSML as the only way forward in making QuestUI a better experience. As cool as it is, I highly disliked the idea of using an XML parser and packaging XML code in Quest mods. 
This and the experience I had with React & Flutter made me realize a different approach, one where UI can be just lots of components.

-----

### Component
Everything rendered in QuestUI Components is a component. By default, a `Component` is static and does not ever change.
- `render()` will be called as long as `rendered == false`. Run `markAsRendered()` in your render function to mark it as rendered.
- `transform` must be assigned on `render()`
- `render()` can return `this` or another component. If another component is returned, it will also be rendered.
- When a `Component` dies, its UI object and transform will **NOT** be destroyed. You should instead destroy it manually or call the container's `removeFromHierarchy` function.

#### UpdateableComponentBase
A `UpdateableComponentBase` (or more commonly used, `UpdateableComponent<MutableData>`) is a type of component that can be updated. Use this in conjunction with `Component`
- `mutateData([](MutableData data) { return data })` Method to mark a change in state, with the new data. 
- `doUpdate()` Applies the change, do not call if `render()` has not been called.
-----

### Container
A `Container` such as `ViewComponent` or `ScrollableContainer` is a type used to render and own multiple `Component`s.

#### ViewComponent
`ViewComponent` is not actually a `Component`, however it is a `Container`. It is usually the highest root parent, as it renders everything. You would usually have this as a field in your `ViewController` (and ensure it is cleaned on `ViewController` destroy)
- You can construct a `ViewComponent` at any time and thread you want
- You should only call `render()` in the main thread, you can use `QuestUI::MainThreadScheduler::Schedule` if on another thread.
-----

The best example code, while also confusing and extensive can be found in `DidActivate` [here](src/test/main.cpp)

## Custom components
An example of a custom component can be found here:
- [Header](include/TestComponent.hpp)
- [src](src/test/TestComponent.cpp)

For more extensive examples, look at existing components.

## Credits

* [zoller27osu](https://github.com/zoller27osu), [Sc2ad](https://github.com/Sc2ad) and [jakibaki](https://github.com/jakibaki) - [beatsaber-hook](https://github.com/sc2ad/beatsaber-hook)
* [raftario](https://github.com/raftario) 
* [Lauriethefish](https://github.com/Lauriethefish) and [danrouse](https://github.com/danrouse) for [this template](https://github.com/Lauriethefish/quest-mod-template)
