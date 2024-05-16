# Happy Textures :3

A Geometry Dash mod that brings texture packs to the next level!

## Features

- Replacing bigFont (Pusab) will not replace the font within levels.
- Fixed overlapping texture issues with CCScale9Sprite (Transparent textures in the level search look good now)
- Extensive Data Driven UI support with JSON

## Data Driven UI

Geometry Dash UIs are unable to be modified without separate mods by default. This mod aims to resolve that issue, such that any texture pack can change UI elements easily. 

Within a texture pack, add a `ui` folder, this is where any UI modifications will go.

To help with the UI process, it is recommended to install the **DevTools** mod on Geode.

(Almost) every page on GD has an ID, you will want to create a JSON file with the format `ID.json` inside the ui folder to begin. Within DevTools, you can find this ID by going to the **Tree** tab, clicking on the CCScene dropdown to view the rest of the elements, and looking at IDs of the elements, in quotes, on that scene. For example, `MenuLayer.json` will be found on the main menu, which will be used as an example throughout the rest of this.

You can expand these elements in DevTools to find what you may want to modify. Example below

```
▼ [0] cocos2d::CCScene <1>
   ▼ [0] MenuLayer "MenuLayer" <13>
      🞂 [0] MenuGameLayer "main-menu-bg" <17>
      ▼ [1] cocos2d::CCMenu "main-menu" <3>
         🞂 [0] CCMenuItemSpriteExtra "icon-kit-button" <1>
         🞂 [1] CCMenuItemSpriteExtra "play-button" <1>
         🞂 [2] CCMenuItemSpriteExtra "editor-button" <1>
      🞂 [2] cocos2d::CCMenu "bottom-menu" <8>
      ...
```

Every node can have `"children"`, there are three ways of accessing them, either with `"node"` to get them by name, `"index"` an array of objects to get them by their index in the node tree and optionally by type, and `"all"` which selects all the children to modify. For example:

```json
{
    "children": {
        "node": {
            "example-node":{

            }
        },
        "index": [
            {
                "type": "CCSprite",
                "index": 0,
                "attributes": {

                }
            }
        ],
        "all": {

        }
    }
}
```

Within MenuLayer, we can see there is a menu, the main menu, which has three buttons. Any or all of these buttons can be modified within that MenuLayer.json file. We would first want to get the main menu by grabbing the children of the MenuLayer, and finding the `"main-menu"` ID:

```json
{
    "children": {
        "node": {
            "main-menu":{

            }
        }
        
    }
}
```

Let's say we want to edit the play button, and scale it to twice it's original size, we would get the `"play-button"` child of the main-menu and then edit it's `"attributes"`:

```json
{
    "children": {
        "node": {
            "main-menu": {
                "children": {
                    "node":{
                        "play-button": {
                            "attributes": {
                                "scale": 2
                            }
                        }
                    }
                }
            }
        }
    }
}
```

There are a lot of attributes you can use, and they will be listed at the end of this page. 

Some attributes only work if you are selecting a **CCSprite** or a **CCMenuItemSpriteExtra**. These attributes are `"sprite"` and `"sprite-frame"` which take in any image or a frame of an existing spritesheet. 

When it comes to using the position attribute, you can change the x and y coordinates, which will place them at those exact coordinates on the screen no matter what, or you can choose a `"relative"` position, which makes x and y modifiy that. A relative position can be something such as `"center"` to place something directly in the center of the screen, or `"top-right"` to place something at the top right of the screen.

## Full example of a MenuLayer UI modification

```json
{
    "children": {
        "node": {
            "social-media-menu": {
                "attributes": {
                    "visible": false
                }
            },
            "profile-menu": {
                "attributes": {
                    "position": {
                        "relative": "bottom-left",
                        "x": 140,
                        "y": 50
                    }
                }
            },
            "player-username": {
                "attributes": {
                    "position": {
                        "relative": "bottom-left",
                        "x": 94,
                        "y": 84
                    }
                }
            },
            "main-menu": {
                "children": {
                    "all":{
                        "attributes": {
                            "actions": [
                                {
                                    "type": "RotateBy",
                                    "repeat": true,
                                    "value": 180,
                                    "duration": 1
                                }
                            ]
                        }
                    }
                }
            },
            "bottom-menu": {
                "attributes": {
                    "layout": {
                        "flip-axis": false,
                        "gap": 0,
                        "axis": "column"
                    },
                    "content-size": {
                        "width": 30,
                        "height": 250
                    },
                    "position": {
                        "relative": "center-left",
                        "x": 25,
                        "y": -15
                    }
                }
            }
        }
    }
}
```

## Available Attributes

<table>
<tr>
<td> Attribute </td> <td> Description </td> <td> Example 1 </td> <td> Example 2 </td>
</tr>
<tr>
<td> scale </td>
<td> Set the scale of a node, can either be a number or an object containing x and y scale separately. </td>
<td>
    
```json
"scale": 2
```

</td>

<td>
    
```json
"scale": {
    "x": 2,
    "y": 1.5
}
```

</td>
</tr>

<tr>
<td> rotation </td>
<td> Set the rotation of a node, can either be a number or an object containing x and y rotation separately. </td>
<td>
    
```json
"rotation": 90
```

</td>

<td>
    
```json
"rotation": {
    "x": 90,
    "y": 45
}
```

</td>
</tr>

<tr>
<td> skew </td>
<td> Set the skew of a node, an object containing x and y skew separately. </td>
<td>
    
```json
"skew": {
    "x": 20,
    "y": 10
}
```

</td>

<td>


</td>
</tr>

<tr>
<td> anchor-point </td>
<td> Set the anchor of a node, an object containing x and y, values must be between 0 and 1. </td>
<td>
    
```json
"anchor-point": {
    "x": 0.5,
    "y": 0.5
}
```

</td>

<td>


</td>
</tr>

<tr>
<td> content-size </td>
<td> Set the content size of a node, an object containing width and height. </td>
<td>
    
```json
"content-size": {
    "width": 50,
    "height": 60
}
```

</td>

<td>


</td>
</tr>

<tr>
<td> visible </td>
<td> Set if a node should be visible. </td>
<td>
    
```json
"visible": false
```

</td>

<td>


</td>
</tr>

<tr>
<td> ignore-anchor-pos </td>
<td> Set if a node should ignore the anchor point for it's position. </td>
<td>
    
```json
"ignore-anchor-pos": false
```

</td>

<td>


</td>
</tr>

<tr>
<td> color </td>
<td> Set the color of a node (Only works for some nodes), uses r, g, b values between 0 and 255. </td>
<td>
    
```json
"color": {
    "r": 128,
    "g": 59,
    "b": 199
}
```

</td>

<td>


</td>
</tr>

<tr>
<td> opacity </td>
<td> Set the opacity of a node (Only works for some nodes), uses values between 0 and 255. </td>
<td>
    
```json
"opacity": 128
```

</td>

<td>


</td>
</tr>

<tr>
<td> sprite </td>
<td> Set the sprite of a node (Only works for CCSprite and CCMenuItemSpriteExtra). </td>
<td>
    
```json
"sprite": "path/to/my/sprite.png"
```

</td>

<td>


</td>
</tr>

<tr>
<td> sprite-frame </td>
<td> Set the sprite frame of a node, has to be in an existing spritesheet (Only works for CCSprite and CCMenuItemSpriteExtra). </td>
<td>
    
```json
"sprite-frame": "frameName.png"
```

</td>

<td>


</td>
</tr>

<tr>
<td> text </td>
<td> Set the text of a label or button. </td>
<td>
    
```json
"text": "example"
```

</td>

<td>


</td>
</tr>

</tr>

<tr>
<td> scale-multiplier </td>
<td> Set the scale multiplier for when you click on a button. </td>
<td>
    
```json
"scale-multiplier": 1.4
```

</td>

<td>


</td>
</tr>

<tr>
<td> z-order </td>
<td> Set the z order of a node. </td>
<td>
    
```json
"z-order": 2
```

</td>

<td>


</td>
</tr>

<tr>
<td> font </td>
<td> Set the bitmap font of a label or button. </td>
<td>
    
```json
"font": "chatFont.fnt"
```

</td>

<td>


</td>
</tr>

<tr>
<td> blending </td>
<td> Set the blending of a node, see OpenGL documentation for how to use blending modes. </td>
<td>
    
```json
"blending": {
    "source": "GL_ONE_MINUS_SRC_COLOR",
    "destination": "GL_ONE_MINUS_SRC_ALPHA"
}
```

</td>

<td>


</td>
</tr>

<tr>
<td> sound </td>
<td> Set a sound to play on node attribute change. Useful for onHover and onClick events. </td>
<td>
    
```json
"sound": "effect.ogg"
```

</td>

<td>


</td>
</tr>

<tr>
<td> link </td>
<td> Set a link to open on node attribute change. Useful for onClick events. Can be a URL or link to a "profile" or "level" with the type attribute. </td>
<td>
    
```json
"link": "https://google.com"
```

</td>

<td>

```json
"link": {
    "type": "profile",
    "id": 123456,
}
```

</td>
</tr>

<tr>
<td> position </td>
<td> Set the position of a node, uses x and y coordinates from the bottom left by default, set the relative position with "relative". Available relative positions are mentioned later. </td>
<td>
    
```json
"position": {
    "x": 100,
    "y": 150
}
```

</td>

<td>

```json
"position": {
    "x": 30,
    "y": -30,
    "relative": "top-left"
}
```

</td>
</tr>

<tr>
<td> layout </td>
<td> Set layout attributes to a node (creates a new layout if the node does not have one already) Available layout attributes are mentioned later.</td>
<td>
    
```json
"layout": {
    "axis": "column",
    "flip-axis": false,
    "flip-cross-axis": false,
    "auto-scale": true,
    "grow-cross-axis": true,
    "allow-cross-axis-overflow": false,
    "gap": 10,
    "axis-alignment": "center",
    "cross=axis-alignment": "start",
    "cross=axis-line-alignment": "end"
}
```

</td>

<td>

</td>
</tr>

<tr>
<td> update-layout </td>
<td> Set if a node should update it's parent's layout after being modified. </td>
<td>
    
```json
"update-layout": true
```

</td>

<td>

</td>
</tr>

<tr>
<td> actions </td>
<td> Set an array of animation actions that a node will follow when modified. Available actions and action attributes are mentioned later.</td>
<td>
    
```json
"actions": [
    {
        "type": "RotateBy",
        "repeat": true,
        "value": 180,
        "duration": 1
    }
]
```

</td>

<td>

```json
"actions": [
    {
        "type": "Sequence",
        "actions" [
            {
                "type": "RotateBy",
                "value": 360,
                "duration": 1
            },
            {
                "type": "FadeOut",
                "duration": 1
            }
        ]
    }
]
```

</td>
</tr>

</table>

## Relative Positions

<table>

<tr>
<td> Position </td> <td> Location </td>
</tr>

<tr>
<td> top-left </td>

<td>

```
⬛⬜⬜
⬜⬜⬜
⬜⬜⬜
```

</td>

</tr>

<tr>
<td> top-center </td>

<td>

```
⬜⬛⬜
⬜⬜⬜
⬜⬜⬜
```

</td>

</tr>

<tr>
<td> top-right </td>

<td>

```
⬜⬜⬛
⬜⬜⬜
⬜⬜⬜
```

</td>

</tr>

<tr>
<td> center-left </td>

<td>

```
⬜⬜⬜
⬛⬜⬜
⬜⬜⬜
```

</td>

</tr>

<tr>
<td> center </td>

<td>

```
⬜⬜⬜
⬜⬛⬜
⬜⬜⬜
```

</td>

</tr>

<tr>
<td> center-right </td>

<td>

```
⬜⬜⬜
⬜⬜⬛
⬜⬜⬜
```

</td>

</tr>

<tr>
<td> bottom-left </td>

<td>

```
⬜⬜⬜
⬜⬜⬜
⬛⬜⬜
```

</td>

</tr>

<tr>
<td> bottom-center </td>

<td>

```
⬜⬜⬜
⬜⬜⬜
⬜⬛⬜
```

</td>

</tr>

<tr>
<td> bottom-right </td>

<td>

```
⬜⬜⬜
⬜⬜⬜
⬜⬜⬛
```

</td>

</tr>

</tr>

<tr>
<td> self </td>

<td>
Original Location
</td>

</tr>

</table>

## Layout Attributes

<table>

<tr>
<td> Attribute </td> <td> Values </td>
</tr>

<tr>
<td>axis</td>
<td>

- column
- row

</td>
</tr>

<tr>
<td>alignment</td>
<td>

- start
- center
- end
- even

</td>
</tr>

</table>

## Action Attributes

## Action Types

## Action Easing