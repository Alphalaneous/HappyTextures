#pragma once

#include <Geode/Geode.hpp>
#include <Geode/modify/LevelCell.hpp>
#include <Geode/modify/LevelListCell.hpp>
#include <Geode/modify/GJScoreCell.hpp>
#include <Geode/modify/MapPackCell.hpp>
#include <Geode/modify/AchievementCell.hpp>
#include <Geode/modify/GJMessageCell.hpp>
#include <Geode/modify/GJUserCell.hpp>

#include "../Macros.h"

using namespace geode::prelude;

setCellColors(LevelCell, loadFromLevel, GJGameLevel);
setCellColors(LevelListCell, loadFromList, GJLevelList);
setCellColors(GJScoreCell, loadFromScore, GJUserScore);
setCellColors(GJUserCell, loadFromScore, GJUserScore);

#ifndef GEODE_IS_MACOS

setCellColors(AchievementCell, loadFromDict, CCDictionary);
setCellColors(GJMessageCell, loadFromMessage, GJUserMessage);
setCellColors(MapPackCell, loadFromMapPack, GJMapPack);

#endif