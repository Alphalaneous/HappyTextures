#pragma once

#include <Geode/Geode.hpp>
#include <Geode/modify/LevelCell.hpp>
#include <Geode/modify/LevelListCell.hpp>
#include <Geode/modify/GJScoreCell.hpp>
#include <Geode/modify/MapPackCell.hpp>
#include <Geode/modify/AchievementCell.hpp>
#include <Geode/modify/StatsCell.hpp>
#include <Geode/modify/GJMessageCell.hpp>
#include <Geode/modify/GJUserCell.hpp>
#include <Geode/modify/GJLevelScoreCell.hpp>
#include <Geode/modify/GJRequestCell.hpp>
#include "../UIModding.hpp"
#include "../Macros.hpp"

using namespace geode::prelude;

setCellColors(LevelCell, loadFromLevel, GJGameLevel);
setCellColors(LevelListCell, loadFromList, GJLevelList);
setCellColors(GJScoreCell, loadFromScore, GJUserScore);
setCellColors(GJUserCell, loadFromScore, GJUserScore);
setCellColors(AchievementCell, loadFromDict, CCDictionary);
setCellColors(StatsCell, loadFromObject, StatsObject);
setCellColors(GJMessageCell, loadFromMessage, GJUserMessage);
setCellColors(MapPackCell, loadFromMapPack, GJMapPack);
setCellColors(GJLevelScoreCell, loadFromScore, GJUserScore);
setCellColors(GJRequestCell, loadFromScore, GJUserScore);
