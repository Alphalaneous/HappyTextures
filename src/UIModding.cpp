#include <Geode/Geode.hpp>
#include <Geode/modify/CCScene.hpp>
#include <Geode/modify/GJDropDownLayer.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/utils/web.hpp>
#include "FileWatcher.h"
#include "Utils.h"
#include "CCLabelBMFont.h"
#include "UIModding.h"

using namespace geode::prelude;

class $modify(MenuLayer){
    bool init(){
        startFileListeners();
        return MenuLayer::init();
    }
};

class $modify(GJDropDownLayer){
	void showLayer(bool p0){
		GJDropDownLayer::showLayer(p0);
		doUICheck(this);
	}
};

class $modify(MyCCScene, CCScene){

	struct Fields {
		int currentCount = 0;
	};

	static CCScene* create(){
		auto ret = CCScene::create();
		bool doModify = Mod::get()->getSettingValue<bool>("ui-modifications");
		if(doModify){
			ret->schedule(schedule_selector(MyCCScene::checkForUpdates));
            for(CCNode* node : CCArrayExt<CCNode*>(ret->getChildren())){
				doUICheck(node);
			}
		}
		return ret;
	}

	void checkForUpdates(float dt){
		if(this->getChildrenCount() != m_fields->currentCount && this->getChildrenCount() != 1){
            log::info("update {}", this->getChildrenCount());
			int idx = 0;
			for(CCNode* node : CCArrayExt<CCNode*>(this->getChildren())){
				idx++;
				if(idx >= m_fields->currentCount){
					doUICheck(node);
				}
			}
			m_fields->currentCount = this->getChildrenCount();
		}
	}
};

void recursiveModify(CCNode* node, matjson::Object elements){

	for(CCNode* node : CCArrayExt<CCNode*>(node->getChildren())){
		if(elements.contains(node->getID())){
			matjson::Value nodeValue = elements[node->getID()];

			if(nodeValue.is_object()){
				matjson::Object nodeObject = nodeValue.as_object();
				handleModifications(node, nodeObject);
			}
		}
	}
}

void runAction(CCNode* node, matjson::Object attributes){

	if(attributes.contains("actions")){
		matjson::Value actionsValue = attributes["actions"];
		if(actionsValue.is_array()){
			
			CCArray* actionArray = CCArray::create();
			matjson::Array actionValues = actionsValue.as_array();

			for(matjson::Value action : actionValues){

				CCActionInterval* actionInterval = createAction(action);

				if(actionInterval){
					actionArray->addObject(actionInterval);
				}
			}
			if(actionArray->count() > 0){
				node->runAction(CCSpawn::create(actionArray));
			}
		}
	}
}

CCActionInterval* createAction(matjson::Value action){
	CCActionInterval* retAction = nullptr;

	if(action.is_object()){

		if(action.contains("type")){

			bool repeat = false;

			float value = 0;
			float duration = 0;
			float x = 0;
			float y = 0;
			float easingRate = 0;
			int repeatCount = INT32_MAX;

			std::string easingType = "none";
			std::string link = "";

			CCActionInterval* actionToDo = nullptr;

			if(action.contains("easing")){
				matjson::Value easingValue = action["easing"];
				if(easingValue.is_string()){
					easingType = easingValue.as_string();
				}
			}
			if(action.contains("easing-rate")){
				matjson::Value easingRateVal = action["easing-rate"];
				if(easingRateVal.is_number()){
					easingRate = easingRateVal.as_double();
				}
			}
			if(action.contains("repeat")){
				matjson::Value repeatVal = action["repeat"];
				if(repeatVal.is_bool()){
					repeat = repeatVal.as_bool();
				}
				if(repeatVal.is_number()){
					repeatCount = repeatVal.as_int();
				}
			}
			if(action.contains("duration")){
				matjson::Value durationValue = action["duration"];
				if(durationValue.is_number()){
					duration = durationValue.as_double();
				}
			}

			bool isNumber = false;

			if(action.contains("value")){
				matjson::Value valueValue = action["value"];
				if(valueValue.is_number()){
					isNumber = true;
					value = valueValue.as_double();
				}
				else if(valueValue.is_object()){
					if(valueValue.contains("x") && valueValue.contains("y")){
						matjson::Value xValue = valueValue["x"];
						matjson::Value yValue = valueValue["y"];

						if(xValue.is_number() && yValue.is_number()){
							x = xValue.as_double();
							y = yValue.as_double();
						}
					}
				}
			}


			matjson::Value typeValue = action["type"];
			if(typeValue.is_string()){
				std::string type = typeValue.as_string();

				if(type == "Sequence"){

					CCArray* sequentialActions = CCArray::create();

					if(action.contains("actions")){

						matjson::Value seqActions = action["actions"];
						if(seqActions.is_array()){
							matjson::Array seqActionsArr = seqActions.as_array();
							for(matjson::Value seqAction : seqActionsArr){

								CCActionInterval* seqActionToDo = createAction(seqAction);
								if(seqActionToDo){
									sequentialActions->addObject(seqActionToDo);
								}
							}
						}
					}

					if(sequentialActions->count() > 0){
						actionToDo = CCSequence::create(sequentialActions);
					}
				}
				if(type == "ScaleTo"){
					if(!isNumber){
						actionToDo = CCScaleTo::create(duration, x, y);
					}
					else {
						actionToDo = CCScaleTo::create(duration, value);
					}
				}
				if(type == "ScaleBy"){
					if(!isNumber){
						actionToDo = CCScaleBy::create(duration, x, y);
					}
					else {
						actionToDo = CCScaleBy::create(duration, value);
					}
				}
				if(type == "RotateBy"){
					if(!isNumber){
						actionToDo = CCRotateBy::create(duration, x, y);
					}
					else {
						actionToDo = CCRotateBy::create(duration, value);
					}
				}
				if(type == "RotateTo"){
					if(!isNumber){
						actionToDo = CCRotateTo::create(duration, x, y);
					}
					else {
						actionToDo = CCRotateTo::create(duration, value);
					}
				}
				if(type == "MoveBy"){
					if(!isNumber){
						actionToDo = CCMoveBy::create(duration, {x, y});
					}
				}
				if(type == "MoveTo"){
					if(!isNumber){
						actionToDo = CCMoveTo::create(duration, {x, y});
					}
				}
				if(type == "SkewBy"){
					if(!isNumber){
						actionToDo = CCSkewBy::create(duration, x, y);
					}
				}
				if(type == "SkewTo"){
					if(!isNumber){
						actionToDo = CCSkewTo::create(duration, x, y);
					}
				}
				if(type == "FadeIn"){
					if(!isNumber){
						actionToDo = CCFadeIn::create(duration);
					}
				}
				if(type == "FadeOut"){
					if(!isNumber){
						actionToDo = CCFadeOut::create(duration);
					}
				}
				if(type == "FadeTo"){
					if(!isNumber){
						actionToDo = CCFadeTo::create(duration, value);
					}
				}
			}
			if(actionToDo){

				CCActionInterval* finalAction = getEasingType(easingType, actionToDo, easingRate);
				if(finalAction){
					if(repeat){
						retAction = CCRepeat::create(finalAction, repeatCount);
					}
					else{
						retAction = finalAction;
					}
				}
				
			}
		}
	}
	return retAction;
}

CCActionInterval* getEasingType(std::string name, CCActionInterval* action, float rate){

	CCActionInterval* easingType = nullptr;
	
	if(name == "none"){
		easingType = action;
	}

    typeForEaseCC(EaseInOut);
    typeForEaseCC(EaseIn);
    typeForEaseCC(EaseOut);
    typeForEaseRate(ElasticInOut);
    typeForEaseRate(ElasticIn);
    typeForEaseRate(ElasticOut);
    typeForEase(BounceInOut);
    typeForEase(BounceIn);
    typeForEase(BounceOut);
    typeForEase(ExponentialInOut);
    typeForEase(ExponentialIn);
    typeForEase(ExponentialOut);
    typeForEase(SineInOut);
    typeForEase(SineIn);
    typeForEase(SineOut);
    typeForEase(BackInOut);
    typeForEase(BackIn);
    typeForEase(BackOut);

	return easingType;
}

void setLayout(CCNode* node, matjson::Object attributes){

	if(attributes.contains("layout")){

		matjson::Value layoutValue = attributes["layout"];
		if(layoutValue.is_object()){
			AxisLayout* layout;

			if(node->getLayout()){
				layout = dynamic_cast<AxisLayout*>(node->getLayout());
			}
			else{
				layout = AxisLayout::create();
				node->setLayout(layout);
			}
			if(layout){

				if(layoutValue.contains("axis")){
					matjson::Value axisValue = layoutValue["axis"];
					if(axisValue.is_string()){
						std::string axis = axisValue.as_string();
						if(axis == "row"){
							layout->setAxis(Axis::Row);
						}
						else if(axis == "column"){
							layout->setAxis(Axis::Column);
						}
					}
				}
				if(layoutValue.contains("flip-axis")){

					matjson::Value flipAxisValue = layoutValue["flip-axis"];
					if(flipAxisValue.is_bool()){
						bool flipAxis = flipAxisValue.as_bool();
						layout->setAxisReverse(flipAxis);
					}
				}
				if(layoutValue.contains("flip-cross-axis")){
					matjson::Value flipCrossAxisValue = layoutValue["flip-cross-axis"];
					if(flipCrossAxisValue.is_bool()){
						bool flipCrossAxis = flipCrossAxisValue.as_bool();
						layout->setCrossAxisReverse(flipCrossAxis);
					}
				}
				if(layoutValue.contains("auto-scale")){
					matjson::Value autoScaleValue = layoutValue["auto-scale"];
					if(autoScaleValue.is_bool()){
						bool autoScale = autoScaleValue.as_bool();
						layout->setAutoScale(autoScale);
					}
				}
				if(layoutValue.contains("grow-cross-axis")){
					matjson::Value growCrossAxisValue = layoutValue["grow-cross-axis"];
					if(growCrossAxisValue.is_bool()){
						bool growCrossAxis = growCrossAxisValue.as_bool();
						layout->setGrowCrossAxis(growCrossAxis);
					}
				}
				if(layoutValue.contains("allow-cross-axis-overflow")){
					matjson::Value allowCrossAxisOverflowValue = layoutValue["allow-cross-axis-overflow"];
					if(allowCrossAxisOverflowValue.is_bool()){
						bool allowCrossAxisOverflow = allowCrossAxisOverflowValue.as_bool();
						layout->setCrossAxisOverflow(allowCrossAxisOverflow);
					}
				}
				if(layoutValue.contains("gap")){
					matjson::Value gapValue = layoutValue["gap"];
					if(gapValue.is_number()){
						float gap = gapValue.as_double();
						layout->setGap(gap);
					}
				}
				if(layoutValue.contains("axis-alignment")){
					matjson::Value axisAlignmentValue = layoutValue["axis-alignment"];
					if(axisAlignmentValue.is_string()){
						std::string axisAlignmentStr = axisAlignmentValue.as_string();
						AxisAlignment axisAlignment;
						if(axisAlignmentStr == "start"){
							axisAlignment = AxisAlignment::Start;
						}
						if(axisAlignmentStr == "center"){
							axisAlignment = AxisAlignment::Center;
						}
						if(axisAlignmentStr == "end"){
							axisAlignment = AxisAlignment::End;
						}
						if(axisAlignmentStr == "even"){
							axisAlignment = AxisAlignment::Even;
						}
						layout->setAxisAlignment(axisAlignment);

					}
				}
				if(layoutValue.contains("cross=axis-alignment")){
					matjson::Value crossAxisAlignmentValue = layoutValue["cross-axis-alignment"];
					if(crossAxisAlignmentValue.is_string()){
						std::string crossAxisAlignmentStr = crossAxisAlignmentValue.as_string();
						AxisAlignment axisAlignment;
						if(crossAxisAlignmentStr == "start"){
							axisAlignment = AxisAlignment::Start;
						}
						if(crossAxisAlignmentStr == "center"){
							axisAlignment = AxisAlignment::Center;
						}
						if(crossAxisAlignmentStr == "end"){
							axisAlignment = AxisAlignment::End;
						}
						if(crossAxisAlignmentStr == "even"){
							axisAlignment = AxisAlignment::Even;
						}
						layout->setCrossAxisAlignment(axisAlignment);

					}
				}
				if(layoutValue.contains("cross=axis-line-alignment")){
					matjson::Value crossAxisLineAlignmentValue = layoutValue["cross-axis-line-alignment"];
					if(crossAxisLineAlignmentValue.is_string()){
						std::string crossAxisLineAlignmentStr = crossAxisLineAlignmentValue.as_string();
						AxisAlignment axisAlignment;
						if(crossAxisLineAlignmentStr == "start"){
							axisAlignment = AxisAlignment::Start;
						}
						if(crossAxisLineAlignmentStr == "center"){
							axisAlignment = AxisAlignment::Center;
						}
						if(crossAxisLineAlignmentStr == "end"){
							axisAlignment = AxisAlignment::End;
						}
						if(crossAxisLineAlignmentStr == "even"){
							axisAlignment = AxisAlignment::Even;
						}
						layout->setCrossAxisLineAlignment(axisAlignment);

					}
				}
			}
			node->updateLayout();
		}
	}
}

std::string getSound(std::string sound){

    std::string soundRet = "";

    gd::vector<gd::string> paths = CCFileUtils::sharedFileUtils()->getSearchPaths();

    for(std::string path : paths){
        std::string soundPathStr = fmt::format("{}{}", path, sound);
        ghc::filesystem::path soundPath = ghc::filesystem::path(soundPathStr);
        if(ghc::filesystem::exists(soundPath)){
            soundRet = soundPath.string();
            break;
        }
    }

    return soundRet;
}


void playSound(CCNode* node, matjson::Object attributes){
    if(attributes.contains("sound")){
        matjson::Value soundVal = attributes["sound"];
        if(soundVal.is_string()){
            std::string sound = soundVal.as_string();

#ifndef GEODE_IS_ANDROID
            FMODAudioEngine::sharedEngine()->m_currentSoundChannel->setPaused(false);
            FMODAudioEngine::sharedEngine()->m_backgroundMusicChannel->setPaused(false);
            FMODAudioEngine::sharedEngine()->m_globalChannel->setPaused(false);
            FMODAudioEngine::sharedEngine()->m_system->update();
#endif
            std::string soundPath = getSound(sound);
            if(soundPath != ""){
		        FMODAudioEngine::sharedEngine()->playEffectAdvanced(soundPath, 1, 0, 1, 1, true, false, 0, 0, 0, 0, false, 0, false, true, 0, 0, 0, 0);

            }
        }
    }
}

void openLink(CCNode* node, matjson::Object attributes){
    if(attributes.contains("link")){
        matjson::Value linkVal = attributes["link"];

        if(linkVal.is_string()){
            std::string link = linkVal.as_string();
            web::openLinkInBrowser(link);
        }
        if(linkVal.is_object()){
            matjson::Object linkObject = linkVal.as_object();
            if(linkObject.contains("type") && linkObject.contains("id")){
                matjson::Value typeVal = linkObject["type"];
                matjson::Value idVal = linkObject["id"];

                if(typeVal.is_string() && idVal.is_number()){
                    std::string type = typeVal.as_string();
                    int id = idVal.as_int();

                    if(type == "profile"){
                        ProfilePage::create(id, false)->show();
                    }
                    if(type == "level"){
                        auto searchObject = GJSearchObject::create(SearchType::Type19, fmt::format("{}&gameVersion=22", id));
                        auto scene = LevelBrowserLayer::scene(searchObject);
                        CCDirector::sharedDirector()->replaceScene(CCTransitionFade::create(0.5f, scene));
                    }
                }
            }
        }
    }
}


void setZOrder(CCNode* node, matjson::Object attributes){
    if(attributes.contains("z-order")){
        matjson::Value zOrderVal = attributes["z-order"];
        if(zOrderVal.is_number()){
            int zOrder = zOrderVal.as_int();
            node->setZOrder(zOrder);
        }
    }
}

void setBlending(CCNode* node, matjson::Object attributes){

    if(attributes.contains("blending")){
        
        matjson::Value blendingVal = attributes["blending"];
		if(blendingVal.is_object()){
            matjson::Object blendingObj = blendingVal.as_object();
            if(blendingObj.contains("source") && blendingObj.contains("destination")){
                matjson::Value sourceVal = blendingObj["source"];
                matjson::Value destinationVal = blendingObj["destination"];

                if(sourceVal.is_string() && destinationVal.is_string()){

                    std::string source = sourceVal.as_string();
                    std::string destination = destinationVal.as_string();

                    unsigned int src = stringToBlendingMode(source);
                    unsigned int dst = stringToBlendingMode(destination);

                    if(src != -1 && dst != -1){
                        if(CCBlendProtocol* blendable = dynamic_cast<CCBlendProtocol*>(node)) {
                            blendable->setBlendFunc({src, dst});
                        }
                    }
                }
            }
        }
    }
}

std::map<std::string, unsigned int> strBlend = {
    {"GL_ZERO", GL_ZERO},
    {"GL_ONE", GL_ONE},
    {"GL_SRC_COLOR", GL_SRC_COLOR},
    {"GL_ONE_MINUS_SRC_COLOR", GL_ONE_MINUS_SRC_COLOR},
    {"GL_DST_COLOR", GL_DST_COLOR},
    {"GL_ONE_MINUS_DST_COLOR", GL_ONE_MINUS_DST_COLOR},
    {"GL_SRC_ALPHA", GL_SRC_ALPHA},
    {"GL_ONE_MINUS_SRC_ALPHA", GL_ONE_MINUS_SRC_ALPHA},
    {"GL_DST_ALPHA", GL_DST_ALPHA},
    {"GL_ONE_MINUS_DST_ALPHA", GL_ONE_MINUS_DST_ALPHA},
    {"GL_SRC_ALPHA_SATURATE", GL_SRC_ALPHA_SATURATE},
    {"GL_CONSTANT_COLOR", GL_CONSTANT_COLOR},
    {"GL_ONE_MINUS_CONSTANT_COLOR", GL_ONE_MINUS_CONSTANT_COLOR},
    {"GL_CONSTANT_ALPHA", GL_CONSTANT_ALPHA},
    {"GL_ONE_MINUS_CONSTANT_ALPHA", GL_ONE_MINUS_CONSTANT_ALPHA}
};

unsigned int stringToBlendingMode(std::string value){

    unsigned int val = -1;

    if(strBlend.contains(value)){
        val = strBlend[value];
    }

    return val;
}

void setFont(CCNode* node, matjson::Object attributes){
    if(attributes.contains("font")){
		matjson::Value fontVal = attributes["font"];
		if(fontVal.is_string()){

            CCLabelBMFont* textObject;

			std::string font = fontVal.as_string();
			if(CCLabelBMFont* textNode = dynamic_cast<CCLabelBMFont*>(node)) {
                textObject = textNode;
			}
            else if(CCMenuItemSpriteExtra* buttonNode = dynamic_cast<CCMenuItemSpriteExtra*>(node)) {
                if(SearchButton* searchButton = getChildOfType<SearchButton>(node, 0)){
                    textObject = getChildOfType<CCLabelBMFont>(searchButton, 0);
                }
                else if(ButtonSprite* buttonSprite = getChildOfType<ButtonSprite>(node, 0)){
                    textObject = getChildOfType<CCLabelBMFont>(buttonSprite, 0);
                }
                else{
                    textObject = getChildOfType<CCLabelBMFont>(node, 0);
                }
            }
            if(textObject){

                if(Utils::endsWith(font, ".fnt")){
                    MyCCLabelBMFont* myTextObject = static_cast<MyCCLabelBMFont*>(textObject);
                    
                    CCLabelBMFont* dummyTextObject = CCLabelBMFont::create("", font.c_str());
                    if(dummyTextObject){
                        textObject->setFntFile(font.c_str());

                        if(myTextObject->m_fields->m_isLimited){
                            textObject->limitLabelWidth(myTextObject->m_fields->m_limitWidth, myTextObject->m_fields->m_limitDefaultScale, myTextObject->m_fields->m_limitMinScale);
                        }
                    }
                }
            }
		}
	}
}

void setPosition(CCNode* node, matjson::Object attributes){
	if(attributes.contains("position")){
		matjson::Value position = attributes["position"];
		if(position.is_object()){
			
			float x = 0;
			float y = 0;

			if(position.contains("x") && position.contains("y")){
				matjson::Value xVal = position["x"];
				matjson::Value yVal = position["y"];

				if(xVal.is_number() && yVal.is_number()){
					x = xVal.as_double();
					y = yVal.as_double();
				}
			}
			if(position.contains("relative")){
				matjson::Value relativeValue = position["relative"];
				if(relativeValue.is_string()){
					std::string relative = relativeValue.as_string();

					CCSize winSize = CCDirector::sharedDirector()->getWinSize();

					if(relative == "top-left"){
						y += winSize.height;
					}
					else if(relative == "top-center"){
						x += winSize.width/2;
						y += winSize.height;
					}
					else if(relative == "top-right"){
						x += winSize.width;
						y += winSize.height;
					}
					else if(relative == "center-left"){
						y += winSize.height/2;
					}
					else if(relative == "center"){
						x += winSize.width/2;
						y += winSize.height/2;
					}
					else if(relative == "center-right"){
						x += winSize.width;
						y += winSize.height/2;
					}
					else if(relative == "bottom-center"){
						x += winSize.width/2;
					}
					else if(relative == "bottom-right"){
						x += winSize.width;
					}
					else if(relative == "self"){
						x += node->getPosition().x;
						y += node->getPosition().y;
					}
					//bottom-left is 0,0
				}
			}
			node->setPosition({x, y});
		}
	}
}

void setColor(CCNode* node, matjson::Object attributes){
	if(attributes.contains("color")){
		matjson::Value color = attributes["color"];
		if(color.is_object()){
			if(color.contains("r") && color.contains("g") && color.contains("b")){
				matjson::Value colorR = color["r"];
				matjson::Value colorG = color["g"];
				matjson::Value colorB = color["b"];

				if(colorR.is_number() && colorG.is_number() && colorB.is_number()){

					unsigned char r = colorR.as_int();
					unsigned char g = colorG.as_int();
					unsigned char b = colorB.as_int();

					if(CCNodeRGBA* nodeRGBA = dynamic_cast<CCNodeRGBA*>(node)) {
						nodeRGBA->setColor(ccColor3B{r, g, b});
					}
				}
			}
		}
	}
}

void setScaleMult(CCNode* node, matjson::Object attributes){
	if(attributes.contains("scale-multiplier")){
		matjson::Value textVal = attributes["scale-multiplier"];
		if(textVal.is_number()){
			float multiplier = textVal.as_double();
			if(CCMenuItemSpriteExtra* button = dynamic_cast<CCMenuItemSpriteExtra*>(node)) {
                button->m_scaleMultiplier = multiplier;
			}
		}
	}
}

void setText(CCNode* node, matjson::Object attributes){
	if(attributes.contains("text")){
		matjson::Value textVal = attributes["text"];
		if(textVal.is_string()){

            CCLabelBMFont* textObject;

			std::string text = textVal.as_string();
			if(CCLabelBMFont* textNode = dynamic_cast<CCLabelBMFont*>(node)) {
                textObject = textNode;
			}
            else if(CCMenuItemSpriteExtra* buttonNode = dynamic_cast<CCMenuItemSpriteExtra*>(node)) {
                if(SearchButton* searchButton = getChildOfType<SearchButton>(node, 0)){
                    textObject = getChildOfType<CCLabelBMFont>(searchButton, 0);
                }
                else if(ButtonSprite* buttonSprite = getChildOfType<ButtonSprite>(node, 0)){
                    textObject = getChildOfType<CCLabelBMFont>(buttonSprite, 0);
                }
                else{
                    textObject = getChildOfType<CCLabelBMFont>(node, 0);
                }
            }
            if(textObject){

                MyCCLabelBMFont* myTextObject = static_cast<MyCCLabelBMFont*>(textObject);
                textObject->setString(text.c_str());

                if(myTextObject->m_fields->m_isLimited){
                    textObject->limitLabelWidth(myTextObject->m_fields->m_limitWidth, myTextObject->m_fields->m_limitDefaultScale, myTextObject->m_fields->m_limitMinScale);
                }
            }
		}
	}
}

void setSprite(CCNode* node, matjson::Object attributes){
	if(attributes.contains("sprite")){
		matjson::Value sprite = attributes["sprite"];
		if(sprite.is_string()){
			std::string spriteName = sprite.as_string();
			if(CCSprite* spriteNode = dynamic_cast<CCSprite*>(node)) {
				CCSprite* spr = CCSprite::create(spriteName.c_str());
				if(spr){
					spriteNode->setTexture(spr->getTexture());
					spriteNode->setTextureRect(spr->getTextureRect());
				}
			}
			if(CCMenuItemSpriteExtra* buttonNode = dynamic_cast<CCMenuItemSpriteExtra*>(node)) {
				CCSprite* spr = CCSprite::create(spriteName.c_str());
				if(spr){
					buttonNode->setSprite(spr);
				}
			}
		}
	}
}

void setSpriteFrame(CCNode* node, matjson::Object attributes){
	if(attributes.contains("sprite-frame")){
		matjson::Value sprite = attributes["sprite-frame"];
		if(sprite.is_string()){
			std::string spriteName = sprite.as_string();
			if(CCSprite* spriteNode = dynamic_cast<CCSprite*>(node)) {
				CCSprite* spr = CCSprite::createWithSpriteFrameName(spriteName.c_str());
				if(spr){
					spriteNode->setTextureAtlas(spr->getTextureAtlas());
					spriteNode->setTexture(spr->getTexture());
					spriteNode->setTextureRect(spr->getTextureRect());
				}
			}
			if(CCMenuItemSpriteExtra* buttonNode = dynamic_cast<CCMenuItemSpriteExtra*>(node)) {
				CCSprite* spr = CCSprite::createWithSpriteFrameName(spriteName.c_str());
				if(spr){
					buttonNode->setSprite(spr);
				}
			}
		}
	}
}

void setOpacity(CCNode* node, matjson::Object attributes){
	if(attributes.contains("opacity")){
		matjson::Value opacity = attributes["opacity"];
		if(opacity.is_number()){
			if(CCNodeRGBA* nodeRGBA = dynamic_cast<CCNodeRGBA*>(node)) {
				unsigned char opacityNum = opacity.as_int();
				nodeRGBA->setOpacity(opacityNum);
			}
		}
	}
}

void setVisible(CCNode* node, matjson::Object attributes){

	if(attributes.contains("visible")){
		matjson::Value visible = attributes["visible"];
		if(visible.is_bool()){
			bool isVisible = visible.as_bool();
			node->setVisible(isVisible);
			if(!isVisible){
				node->setContentSize({0,0});
			}
		}
	}
}

void updateLayout(CCNode* node, matjson::Object attributes){
	if(attributes.contains("update-layout")){
		matjson::Value update = attributes["update-layout"];
		if(update.is_bool()){
			bool shouldUpdate = update.as_bool();
			if(shouldUpdate){
				CCNode* parent = node->getParent();
				if(parent){
					parent->updateLayout();
				}
			}
		}
	}
}

void setIgnoreAnchorPos(CCNode* node, matjson::Object attributes){
	if(attributes.contains("ignore-anchor-pos")){
		matjson::Value ignore = attributes["ignore-anchor-pos"];
		if(ignore.is_bool()){
			bool isIgnoring = ignore.as_bool();
			node->ignoreAnchorPointForPosition(isIgnoring);
		}
	}
}

void setScale(CCNode* node, matjson::Object attributes){
	if(attributes.contains("scale")){
		matjson::Value scale = attributes["scale"];
		if(scale.is_object()){
			matjson::Object scaleAttributes = scale.as_object();

			if(scaleAttributes.contains("x")){
				matjson::Value scaleX = scaleAttributes["x"];
				if(!scaleX.is_null() && scaleX.is_number()){
					float scaleValue = scaleX.as_double();
					node->setScaleX(scaleValue);
				}
			}
			if(scaleAttributes.contains("y")){
				matjson::Value scaleY = scaleAttributes["y"];
				if(!scaleY.is_null() && scaleY.is_number()){
					float scaleValue = scaleY.as_double();
					node->setScaleY(scaleValue);
				}
			}
		}

		if(scale.is_number()){
			float scaleValue = scale.as_double();
			node->setScale(scaleValue);
		}
	}
}

void setRotation(CCNode* node, matjson::Object attributes){
	if(attributes.contains("rotation")){
		matjson::Value rotation = attributes["rotation"];
		if(rotation.is_object()){
			matjson::Object rotationAttributes = rotation.as_object();

			if(rotationAttributes.contains("x")){
				matjson::Value rotationX = rotationAttributes["x"];
				if(!rotationX.is_null() && rotationX.is_number()){
					float rotationValue = rotationX.as_double();
					node->setRotationX(rotationValue);
				}
			}
			if(rotationAttributes.contains("y")){
				matjson::Value rotationY = rotationAttributes["y"];
				if(!rotationY.is_null() && rotationY.is_number()){
					float rotationValue = rotationY.as_double();
					node->setRotationY(rotationValue);
				}
			}
		}

		if(rotation.is_number()){
			float rotationValue = rotation.as_double();
			node->setRotation(rotationValue);
		}
	}
}

void setSkew(CCNode* node, matjson::Object attributes){
	if(attributes.contains("skew")){
		matjson::Value skew = attributes["skew"];
		if(skew.is_object()){
			matjson::Object skewAttributes = skew.as_object();

			if(skewAttributes.contains("x")){
				matjson::Value skewX = skewAttributes["x"];
				if(skewX.is_number()){
					float skewValue = skewX.as_double();
					node->setSkewX(skewValue);
				}
			}

			if(skewAttributes.contains("y")){
				matjson::Value skewY = skewAttributes["y"];
				if(skewY.is_number()){
					float skewValue = skewY.as_double();
					node->setSkewY(skewValue);
				}
			}
		}
	}
}

void setAnchorPoint(CCNode* node, matjson::Object attributes){
	if(attributes.contains("anchor-point")){
		matjson::Value anchorPoint = attributes["anchor-point"];
		if(anchorPoint.is_object()){
			matjson::Object anchorPointAttributes = anchorPoint.as_object();

			if(anchorPointAttributes.contains("x") && anchorPointAttributes.contains("y")){
				matjson::Value anchorPointX = anchorPointAttributes["x"];
				matjson::Value anchorPointY = anchorPointAttributes["y"];

				if(anchorPointX.is_number() && anchorPointY.is_number()){
					float anchorPointValueX = anchorPointX.as_double();
					float anchorPointValueY = anchorPointY.as_double();
					node->setAnchorPoint({anchorPointValueX, anchorPointValueY});
				}
			}
		}
	}
}

void setContentSize(CCNode* node, matjson::Object attributes){
	if(attributes.contains("content-size")){
		matjson::Value contentSize = attributes["content-size"];
		if(contentSize.is_object()){
			matjson::Object contentSizeAttributes = contentSize.as_object();

			if(contentSizeAttributes.contains("width") && contentSizeAttributes.contains("height")){
				matjson::Value contentSizeWidth = contentSizeAttributes["width"];
				matjson::Value contentSizeHeight = contentSizeAttributes["height"];

				if(contentSizeWidth.is_number() && contentSizeHeight.is_number()){
					float contentSizeValueWidth = contentSizeWidth.as_double();
					float contentSizeValueHeight = contentSizeHeight.as_double();

					node->setContentSize({contentSizeValueWidth, contentSizeValueHeight});
				}
			}
		}
	}
}

void handleModifications(CCNode* node, matjson::Object nodeObject){

	if(nodeObject.contains("attributes")){
		matjson::Value nodeAttributes = nodeObject["attributes"];
		if(nodeAttributes.is_object()){
			matjson::Object nodeAttributesObject = nodeAttributes.as_object();

            nodesFor(Scale);
            nodesFor(Rotation);
            nodesFor(Skew);
            nodesFor(AnchorPoint);
            nodesFor(ContentSize);
            nodesFor(Visible);
            nodesFor(IgnoreAnchorPos);
            nodesFor(Color);
            nodesFor(Opacity);
            nodesFor(Sprite);
            nodesFor(SpriteFrame);
            nodesFor(Position);
            nodesFor(Text);
            nodesFor(ScaleMult);
            nodesFor(ZOrder);
            nodesFor(Font);
            nodesFor(Blending);
            nodesFor(Layout);
            nodesForMethod(updateLayout);
            nodesForMethod(runAction);
            nodesForMethod(playSound);
            nodesForMethod(openLink);
		}
	}

	if(nodeObject.contains("children")){
		matjson::Value childrenVal = nodeObject["children"];
		if(childrenVal.is_object()){
            matjson::Object childrenObject = childrenVal.as_object();

            if(childrenVal.contains("node")){
                matjson::Value nodeChildrenVal = childrenObject["node"];
                if(nodeChildrenVal.is_object()){
                    matjson::Object nodeChildrenObject = nodeChildrenVal.as_object();
			        recursiveModify(node, nodeChildrenObject);
                }
            }
            if(childrenVal.contains("index")){
                matjson::Value indexChildrenVal = childrenObject["index"];
                if(indexChildrenVal.is_array()){
                    matjson::Array nodeChildrenArray = indexChildrenVal.as_array();

                    for(matjson::Value value : nodeChildrenArray){
                        if(value.is_object()){
                            matjson::Object childObject = value.as_object();

                            int index = 0;
                            std::string type = "CCNode";
                            if(value.contains("index")){
                                matjson::Value indexVal = value["index"];
                                if(indexVal.is_number()){
                                    index = indexVal.as_int();
                                }
                            }
                            if(value.contains("type")){
                                matjson::Value typeVal = value["type"];
                                if(typeVal.is_string()){
                                    type = typeVal.as_string();
                                }
                            }

                            modifyForType(CCSprite);
                            modifyForType(CCMenu);
                            modifyForType(CCMenuItemSpriteExtra);
                            modifyForType(CCLabelBMFont);
                            modifyForType(CCLayer);
                            modifyForType(CCScale9Sprite);
                            modifyForType(CCSpriteBatchNode);
                            modifyForType(CCTextInputNode);
                            modifyForType(CCTextFieldTTF);
                            modifyForType(TextArea);
                            modifyForType(MultilineBitmapFont);
                            modifyForType(Slider);
                            modifyForType(ButtonSprite);
                            modifyForType(CircleButtonSprite);
                            modifyForType(SearchButton);
                            modifyForType(LoadingCircle);
                            modifyForType(BoomScrollLayer);
                            modifyForType(ListButtonBar);
                            modifyForType(ExtendedLayer);
                            modifyForType(SimplePlayer);
                            modifyForType(DailyLevelNode);
                            modifyForType(GJListLayer);
                            modifyForType(CustomListView);
                            modifyForType(TableView);
                            modifyForType(CCContentLayer);
                            modifyForType(CCLayerColor);
                            modifyForType(LevelCell);
                            modifyForType(CustomSongWidget);
                            modifyForType(GJDifficultySprite);
                            modifyForType(GJCommentListLayer);

                        }
                    }
                }
            }
            if(childrenVal.contains("all")){
                matjson::Value allChildrenVal = childrenObject["all"];
                if(allChildrenVal.is_object()){
                    matjson::Object allChildrenObject = allChildrenVal.as_object();
                    for(CCNode* node : CCArrayExt<CCNode*>(node->getChildren())){
                        handleModifications(node, allChildrenObject);
                    }
                }
            }
		}
	}
}

void doUICheck(CCNode* node){

	std::string path = "/ui/" + node->getID() + ".json";

	unsigned long fileSize = 0;
	unsigned char* buffer = CCFileUtils::sharedFileUtils()->getFileData(path.c_str(), "rb", &fileSize);    

	if (buffer && fileSize != 0) {
		
		std::string data = std::string(reinterpret_cast<char*>(buffer), fileSize);
		std::string error;
		std::optional<matjson::Value> value = matjson::parse(data, error);

		if(value.has_value()){

			matjson::Value expandedValue = value.value();
			if(!expandedValue.is_null()){
				matjson::Object object = expandedValue.as_object();
				handleModifications(node, object);
			}
		}
	}
	delete[] buffer;
}

std::vector<FileWatcher*> listeners;

void startFileListeners(){
    for(FileWatcher* fw : listeners){
        fw->stop();
    }
    listeners.clear();
    std::vector<std::string> packs = getActivePacks();
    for(std::string path : packs){
        std::string uiPath = fmt::format("{}{}", path, "ui\\");

        FileWatcher* fw = new FileWatcher(uiPath, std::chrono::milliseconds(500));
        listeners.push_back(fw);

        std::thread thread([=](FileWatcher* self){
            self->start([=] (std::string pathToWatch, FileStatus status) -> void {
                if(!std::filesystem::is_regular_file(std::filesystem::path(pathToWatch)) && status != FileStatus::erased) {
                    return;
                }
                Loader::get()->queueInMainThread([]{
                    CCScene* scene = CCDirector::sharedDirector()->getRunningScene();
                    for(CCNode* node : CCArrayExt<CCNode*>(scene->getChildren())){
                        doUICheck(node);
                    }
                });
            });
        }, fw);
        thread.detach();
    }
}

std::vector<std::string> getActivePacks(){

    gd::vector<gd::string> paths = CCFileUtils::sharedFileUtils()->getSearchPaths();
    std::vector<std::string> packPaths;
    Mod* textureLoader = Loader::get()->getLoadedMod("geode.texture-loader");
    ghc::filesystem::path textureLoaderPacks = textureLoader->getConfigDir();
    std::string packDirStr = fmt::format("{}{}", textureLoaderPacks, "\\packs");
    ghc::filesystem::path packDir = ghc::filesystem::path(packDirStr);

    for(std::string path : paths){

        ghc::filesystem::path fpath = ghc::filesystem::path(path);

        if(fpath.parent_path().parent_path() == packDir){
            if(std::find(packPaths.begin(), packPaths.end(), fpath.string()) == packPaths.end()) {
                packPaths.push_back(fpath.string());
            }
        }
    }

    std::string resourcesDir = fmt::format("{}{}", CCFileUtils::sharedFileUtils()->getWritablePath2(), "Resources\\");
    packPaths.push_back(resourcesDir);

    return packPaths;
}