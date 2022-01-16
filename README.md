# QuestUI Components (QUC)
# Rewritten from the ground up to be cooler and faster
_Thanks sc2ad_
## What's this?
This is a declarative UI framework (or wrapper?) over QuestUI methods that aims to improve the developer experience and allow reusable components.

This is **highly** inspired by other UI declarative frameworks, specifically Flutter.

While this is inspired by Flutter and those alike, this is still different to other declarative UI frameworks as we're dealing with Unity and HMUI. 

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

You can read the docs at [docs/](docs/)

The best example code, while also confusing and extensive can be found in `DidActivate` [here](/test/src/main.cpp)

## Custom components
An example of a custom component can be found here:
- [Header](test/include/TestComponent.hpp)

For more extensive examples, look at existing components.

## Credits

Lots of this rewrite is credited to [Sc2ad](https://github.com/Sc2ad) who helped me with so much:
- [Initial writeup of QUC v2 specification](https://hackmd.io/@yUYoi67FSC2CrGMPULLK7w/S1jq1LNGY)
- Optimize component creation and rendering
- Greatly improve ugliness of QUC v1 (`new`, `virtual` etc.)
- Give ideas and solutions to problems through many _many_ godbolts

* [RedBrumbler](https://github.com/RedBrumbler) Help with recyclable celled tables.
* [zoller27osu](https://github.com/zoller27osu), [Sc2ad](https://github.com/Sc2ad) and [jakibaki](https://github.com/jakibaki) - [beatsaber-hook](https://github.com/sc2ad/beatsaber-hook)
* [raftario](https://github.com/raftario) 
* [Lauriethefish](https://github.com/Lauriethefish) and [danrouse](https://github.com/danrouse) for [this template](https://github.com/Lauriethefish/quest-mod-template)
