## 1.9.4
- Fix a bug with sprite frames not being able to be set

## 1.9.3
**NOTICE! Modifying nodes by ID will soon be depreciated. Once the next GD update comes out, I will be removing that feature in favor of modiifying nodes by their class name. This is the name BEFORE the one in quotes in DevTools. For example `ui/cvolton.betterinfo$ExtendedLevelInfo.json` will become `ui/ExtendedLevelInfo.json` and anything in the nodes folder will no longer work and should be placed in the UI folder.**
- Fixed crash when pressing space on main menu for mac

## 1.9.2
- Fix a memory leak when setting a font file on a CCLabelBMFont
- Remove batch node hack :P

## 1.9.1
- Fix a crash with labels
- Revamp Texture Loader pack nodes
- Add Friend Requests cells to the default cell color modifications
- Use geode::Popup for alerts
- General code cleanup

## 1.9.0
- Conditionals (If statements) (See Wiki)
- Root node shortcut (See Wiki)
- More variables (See Wiki)
- Updated Rift

## 1.8.8
- Implement a fix for another mod's mistake :P

## 1.8.7
- Fix a very teeny bug that may break MenuLayer changes

## 1.8.6
- Added "after-transition" object for layers in a scene, allowing for you to only make changes after the transition finishes (see wiki for more details)
- Fixed cache failing to be reset, resulting in missing textures when a pack is unapplied

## 1.8.5
- Cleanup filesystem code
- Fix a crash when there is an invalid path (hopefully)
- Fix an issue with CCSpriteWithHue

## 1.8.4
- Actually fix loading individual sprites from init

## 1.8.3
- Fix loading individual sprites from init

## 1.8.2
- Fix some layouting issues
- Fix duplicated node modifications for certain layers
- Fix individual sprites not working if only some qualities are present
- Fix applied packs not updating (uses a new Texture Loader API, make sure Texture Loader is up to date if you experience this issue!)

## 1.8.1
- Fix crash on non-windows
- Change individual sprite changing logic

## 1.8.0
- Support for retexturing individual sprites outside of spritesheets (see wiki for more details)
- Support for disabling batch nodes (for UI purposes, in levels they will not be disabled)

## 1.7.8
- list-cell-selected added to hardcoded color modifications for leaderboards
- Changed hook prio logic
- Mac support for everything

## 1.7.7 
- Zipped packs should work properly now
- Fixed issues with file listener
- Fixed issues with path separators

## 1.7.6
- Node by typeid bugfix

## 1.7.5
- Allow inputting hex colors
- Fix sprite frame texture rotate bug
- Fix an issue where onCreator and onGarage wouldn't work

## 1.7.4
- Fix a crash that can occur when pressing space on the main menu
- Fix a crash that can occur with invalid parents
- Added new scroll-to-top attribute for ScrollLayers
- Added new lock-attributes attribute to reset attributes every frame 

## 1.7.3
- Fix an issue with MenuLayer::onPlay callbacks not working because apparently it is a meanie head and wanted to be rude :(

## 1.7.2
- Add the ability to run certain button callbacks anywhere

## 1.7.1
- Fix a freeze that could happen on launch if GD was on a separate drive
- Fix node modifications not properly reloading on pack change
- Getting a node by type now allows for any class to be passed in

## 1.7.0
- Node Specific Changes Updates:
  - Now any node type can be modified
  - Node type modification code simplified (better performance)
  - Node type modification json are loaded on launch (lag fix)
  - See wiki on new Node type information
- Random Sprites
  - Pack makers can now assign a sprite to be randomized
  - See wiki for more information
- Fix changing button sprites not having proper content sizes
- Fix sounds not playing
- Allow stacking multiple events on a button instead of only 1
- General performance improvements

## 1.6.2
- Fix a crash with cursed FLAlertLayers

## 1.6.1
- Bug fix for variables not working in popups created by HT Packs

## 1.6.0
- <cg>Add extensive label variables with RIFT</c>
- Add color changing for music library cells
- Code cleanup

## <1.6.0
- Add everything
