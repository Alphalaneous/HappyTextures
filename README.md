# Happy Textures :3

A Geometry Dash mod that brings texture packs to the next level!

## Features

- Replacing bigFont (Pusab) will not replace the font within levels.
- Fixed overlapping texture issues with CCScale9Sprite (Transparent textures in the level search look good now)
- Fixed comment borders being ugly with certain texture packs
- Extensive Data Driven UI support with JSON

## Data Driven UI

(For extra help, download the example pack)

Geometry Dash UIs are unable to be modified without separate mods by default. This mod aims to resolve that issue, such that any texture pack can change UI elements easily. 

Within a texture pack, add a `ui` folder, this is where any UI modifications will go.

To help with the UI process, it is recommended to install the **DevTools** mod on Geode.

(Almost) every page on GD has an ID, you will want to create a JSON file with the format `ID.json` inside the ui folder to begin (if an ID contains a forward slash (`/`), replace it with a dollar sign (`$`) in the filename). Within DevTools, you can find this ID by going to the **Tree** tab, clicking on the CCScene dropdown to view the rest of the elements, and looking at IDs of the elements, in quotes, on that scene. For example, `MenuLayer.json` will be found on the main menu, which will be used as an example throughout the rest of this.

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
                    "..."
                }
            }
        ],
        "all": {

        }
    }
}
```

You can also get the parent from a child:

```json
{
    "children": {
        "node": {
            "example-node":{
                "parent": {
                    "attributes":{
                        "..."
                    }
                }
            }
        }
    }
}
```

If you wish to add new nodes, you can add "new" to the children object, this contains an array of new nodes you want to add. You must specify a "type" and "id" value to each object. The type is the type of element you want to add, and the ID is the internal name you want to give the node (in case any other packs need to grab your node or you need to reference it in the future). 

```json
{
    "children": {
        "new": [
            {
                "type": "CCSprite",
                "id": "my-new-sprite",
                "attributes": {
                    "sprite": "happy-sprite.png",
                    "scale": 2
                }
            }
        ]
    }
}
```

The available types you can add are "CCSprite", "CCLabelBMFont", "CCMenu", "CCLayerColor", "CCMenuItemSpriteExtra", and "CCScale9Sprite".

Note that "CCMenuItemSpriteExtra" must be a child of a "CCMenu" to be clickable.

You can also create alerts and popups using this. Alerts require a "title" and "description" attribute. Popups are a blank slate and open like an alert, and you are required to add elements to it yourself. You can set a popup sprite, size, and title optionally. The sprite should be a scale 9 sprite such as "GJ_square01.png". When an alert or popup is created, they will only show on screen if you set "show" to true. It is best to throw these under a button's event to prevent annoying popups from opening whenever you switch scenes or when another popup appears.

Alerts optionally have the ability for you to change the "button-text" attribute, which changes it from the word "Okay" to whatever you specify. 

```json
{
    "children": {
        "new": [
            {
                "type": "Alert",
                "id": "my-new-alert",
                "attributes": {
                    "title": "Very Cool Alert",
                    "description": "This is a very cool alert!",
                    "button-text": "YAY!",
                    "show": true
                }
            }
        ]
    }
}
```


```json
{
    "children": {
        "new": [
            {
                "type": "Popup",
                "id": "my-new-popup",
                "attributes": {
                    "sprite": "GJ_square02.png",
                    "popup-size": {
                        "width": 200,
                        "height": 200
                    },
                    "title": "Very Cool Popup"
                }
            }
        ]
    }
}
```

Within MenuLayer, we can see there is a menu, the main menu, which has three buttons. Any or all of these buttons can be modified within that MenuLayer.json file. We would first want to get the main menu by grabbing the children of the MenuLayer, and finding the `"main-menu"` ID:

```json
{
    "children": {
        "node": {
            "main-menu":{
                "..."
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
                        "anchor": "bottom-left",
                        "x": 140,
                        "y": 50
                    }
                }
            },
            "player-username": {
                "attributes": {
                    "position": {
                        "anchor": "bottom-left",
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
                        "anchor": "center-left",
                        "x": 25,
                        "y": -15
                    }
                }
            }
        }
    }
}
```

You can also reference the parent of a node. In the following example, the child accesses the parent node and sets it's scale. It is useful for when you need to modify a parent on a button event.

```json
{
    "children": {
        "node": {
            "main-menu": {
                "parent": {
                    "attributes": {
                        "scale": 1.2
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
<td> Set the color of a node (Only works for some nodes), uses r, g, b values between 0 and 255. Use the text "reset" to reset it back to its initial color (CCMenuItemSpriteExtra).</td>
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

```json
"color": "reset"
```


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
<td> base-scale </td>
<td> Set the base scale of a button (scaling a button returns to this rather than 1 when clicked). </td>
<td>
    
```json
"base-scale": 2
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
<td> Set a sound to play on node attribute change. Useful for on-hover and on-click events. </td>
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
<td> Set a link to open on node attribute change. Useful for on-click events. Can be a URL or link to a "profile" or "level" with the type attribute. </td>
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
<td> Set the position of a node, uses x and y coordinates from the bottom left by default, set the anchor position with "anchor". You can choose the relative node using `"relative"` in the anchor object and then using `"to"` to chose the position, the relative nodes are `"parent"` and `"screen"`. Available anchor positions are mentioned later. </td>
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
     "anchor": {
        "to": "top-right",
        "relative": "screen"
    }
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
    "cross=axis-line-alignment": "end",
    "ignore-invisible": true
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
<td> disable-pages </td>
<td> Disable pages on menus if the Pages API mod is enabled. </td>
<td>
    
```json
"disable-pages": true
```

</td>

<td>

</td>

</tr>

<tr>
<td> remove </td>
<td> Marks the element for removal. (Try not to use on vanilla nodes, only custom ones you have made) </td>
<td>
    
```json
"remove": true
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
        "actions": [
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

## Button Events

Buttons (CCMenuItemSpriteExtra) can have events that occur when they are clicked, released, activated, hovered, and exited.

These events work just as everything else, within them, you can grab children, add children, and edit attributes. 

```json
{
    "children": {
        "new": [
            {
                "type": "CCMenu",
                "id": "my-very-cool-menu",
                "attributes": {
                    "ignore-anchor-pos": false
                },
                "children": {
                    "new": [
                        {
                            "type": "CCMenuItemSpriteExtra",
                            "id": "my-very-cool-button",
                            "attributes": {
                                "sprite": "happy_textures.png",
                                "position": {
                                    "anchor": "center",
                                    "x": 45,
                                    "y": 0
                                }
                            },
                            "event": {
                                "on-activate": {
                                    "children": {
                                        "new": [
                                            {
                                                "type": "Alert",
                                                "id": "alert-on-activate",
                                                "attributes": {
                                                    "title": "Example Alert",
                                                    "description": "This happened when I activated a button!",
                                                    "button-text": "Yay!",
                                                    "show": true
                                                }
                                            }
                                        ]
                                    }
                                }
                            }
                        }
                    ]
                }
            }
        ]
    }
}

```

```json
{
    "children": {
        "new": [
            {
                "type": "CCMenu",
                "id": "my-very-cool-menu",
                "attributes": {
                    "ignore-anchor-pos": false
                },
                "children": {
                    "new": [
                        {
                            "type": "CCMenuItemSpriteExtra",
                            "id": "my-very-cool-button",
                            "attributes": {
                                "sprite": "happy_textures.png",
                                "position": {
                                    "anchor": "center",
                                    "x": 45,
                                    "y": 0
                                }
                            },
                            "event": {
                                "on-hover": {
                                    "attributes" : {
                                        "actions": [
                                            {
                                                "type": "RotateBy",
                                                "easing": "ElasicIn",
                                                "value": 180,
                                                "duration": 1
                                            }
                                        ],
                                    }
                                },
                                "on-exit": {
                                    "attributes" : {
                                        "actions": [
                                            {
                                                "type": "RotateBy",
                                                "easing": "ElasicIn",
                                                "value": -180,
                                                "duration": 1
                                            }
                                        ],
                                    }
                                }
                            }
                        }
                    ]
                }
            }
        ]
    }
}

```

# Event Types

<table>

<tr>
<td> Event </td> <td> What affects it </td>
</tr>

<tr>
<td>on-click</td>
<td>Clicking on the button</td>
</tr>

<tr>
<td>on-release</td>
<td>Releasing the button</td>
</tr>

<tr>
<td>on-activate</td>
<td>Clicking then releasing the button (how buttons in GD work)</td>
</tr>

<tr>
<td>on-hover</td>
<td>Hovering over a button</td>
</tr>

<tr>
<td>on-exit</td>
<td>No longer hovering over a button</td>
</tr>

</table>

## Anchor Positions

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
- between

</td>
</tr>

</table>

## Action Attributes

All Actions have attributes that go along with them, some more than others. The available attributes are `easing`, `easing-rate`, `repeat`, `duration`, `value`.

<table>

<tr>
<td>Attribute</td> <td>Type</td> <td>Example</td> 
</tr>

<tr> 
<td>easing</td> <td> string (See "Action Easing" for types)</td> <td>

```json
"easing": "EaseInOut"
```

</td> 
</tr>

<tr>
<td>easing-rate</td> <td>float</td> <td>

```json
"easing-rate": 2
```

</td> 
</tr>

<tr>
<td>repeat</td> <td>bool or int (true for forever, int for amount of repeats)</td> <td>

```json
"repeat": true
```
or
```json
"repeat": 5
```

</td> 
</tr>

<tr>
<td>duration</td> <td>float</td> <td>

```json
"duration": 5.2
```

</td> 
</tr>

<tr>
<td>value</td> <td>float, or object of "x" and "y", see types below for which is used</td> <td>

```json
"value": {
    "x": 10,
    "y": 20
}
```
or
```json
"value": 5.5
```

</td> 
</tr>

</table>

## Action Types

<table>

<tr>
<td> Attribute </td> <td> Description </td> <td> Has Position Object </td> <td> Has Float Value </td>
</tr>

<tr>
<td>MoveBy</td>
<td>Move the node by a set amount of units</td>
<td>✅</td>
<td>⬜️</td>
</tr>

<tr>
<td>MoveTo</td>
<td>Move the node to a specific position</td>
<td>✅</td>
<td>⬜️</td>
</tr>

<tr>
<td>SkewBy</td>
<td>Skew the node by a set amount of units</td>
<td>✅</td>
<td>⬜️</td>
</tr>

<tr>
<td>SkewTo</td>
<td>Skew the node to a specific point</td>
<td>✅</td>
<td>⬜️</td>
</tr>

<tr>
<td>FadeIn</td>
<td>Fade the node to full opacity</td>
<td>⬜️</td>
<td>⬜️</td>
</tr>

<tr>
<td>FadeOut</td>
<td>Fade the node to zero opacity</td>
<td>⬜️</td>
<td>⬜️</td>
</tr>

<tr>
<td>FadeTo</td>
<td>Fade the node to a specific opacity</td>
<td>⬜️</td>
<td>⬜️</td>
</tr>

<tr>
<td>ScaleBy</td>
<td>Scale the node by a specific scale</td>
<td>⬜️</td>
<td>⬜️</td>
</tr>

<tr>
<td>ScaleTo</td>
<td>Scale the node to a specific scale</td>
<td>✅</td>
<td>✅</td>
</tr>

<tr>
<td>RotateBy</td>
<td>Rotate the node by a specific rotation</td>
<td>✅</td>
<td>✅</td>
</tr>

<tr>
<td>RotateTo</td>
<td>Rotate the node to a specific rotation</td>
<td>✅</td>
<td>✅</td>
</tr>


</table>

Another type of action can contain actions in itself, which are activated in order, this is an example of a sequence action that rotates a node by 180, then back on repeat:

```json
"actions": [
    {
        "type": "Sequence",
        "actions": [
            {
                "type": "RotateBy",
                "easing": "ElasicIn",
                "value": 180,
                "duration": 1
            },
            {
                "type": "RotateBy",
                "easing": "ElasicIn",
                "value": -180,
                "duration": 1
            }
        ],
        "repeat": true
    }
]
```

If you ever want to stop an exisitng action, just create a "Stop" action:

```json

"actions": [
    {
        "type": "Stop",
    }
]
```


## Action Easing

A list of easings provided are as follows:

- EaseInOut
- EaseIn
- EaseOut
- ElasticInOut
- ElasticIn
- ElasticOut
- BounceInOut
- BounceIn
- BounceOut
- ExponentialInOut
- ExponentialIn
- ExponentialOut
- SineInOut
- SineIn
- SineOut
- BackInOut
- BackIn
- BackOut

EaseInOut, EaseIn, and EaseOut require you to set the "easing-rate" parameter. 

For more info on easings, check https://easings.net/

An example of this is:

```json

{
    "type": "MoveBy",
    "value": {
        "x": 20,
        "y": 30
    },
    "easing": "EaseInOut",
    "easing-rate": 2
}
```

## Replaceable Textures

There are a few textures that aren't easily replaceable without affecting the base game, and are thus added for manual sprite overides in Happy Textures.

For the loading circle in LevelInfoLayer, you can define an outer, center, inner, and progress sprite (normally 200x200 for uhd). Add these within the normal resources of the pack like any other texture, suffixing them with -uhd, -hd, respectively. The names are:

- play_loading_outer
- play_loading_center
- play_loading_inner
- play_loading_progress

## Hardcoded Colors

It is possible to change GDs hardcoded colors via a `colors.json` file in the `ui` folder. The json is formatted as an object that contains the colors you want to change. These colors require an `r`, `g`, and `b` value, and the `a` (alpha) value is optional. Example (replace "color-id" with an actual ID provided below):

```json

{
    "color-id": {
        "r": 128,
        "g": 128,
        "b": 128,
        "a": 128
    }
}
```

A list of modifyable colors provided are as follows:

### Universal

- background

### LevelSearchLayer

- level-search-bg
- level-search-bar-bg
- quick-search-bg
- difficulty-filters-bg
- length-filters-bg

### GJListLayer

- list-layer-bg
- list-cell-odd
- list-cell-even
- comment-list-outline-brown
- comment-list-outline-blue

### GJCommentListLayer

- comment-cell-odd
- comment-cell-even
- comment-cell-small-odd
- comment-cell-small-even
- comment-cell-bg-odd
- comment-cell-bg-even
- comment-list-layer-bg

### InfoLayer

- info-description-bg

### EditLevelLayer

- edit-description-bg
- edit-name-bg

### MenuLayer

- main-menu-bg
- main-menu-ground

### GJChestSprite

- chest-opened-overlay
- 25-chest-opened-overlay
- 50-chest-opened-overlay
- 100-chest-opened-overlay

### LevelInfoLayer Loading Circle

- play-loading-outer
- play-loading-center
- play-loading-inner
- play-loading-progress