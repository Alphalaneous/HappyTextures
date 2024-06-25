#pragma once

#ifndef MYLEVELCELL_H
#define MYLEVELCELL_H

#include <Geode/Geode.hpp>
#include <Geode/modify/LevelCell.hpp>
#include <Geode/modify/LevelListCell.hpp>
#include <Geode/modify/GJScoreCell.hpp>
#include <Geode/modify/MapPackCell.hpp>
#include "../Macros.h"

using namespace geode::prelude;

setCellColors(LevelCell, loadFromLevel, GJGameLevel);
setCellColors(LevelListCell, loadFromList, GJLevelList);
setCellColors(GJScoreCell, loadFromScore, GJUserScore);

#ifndef GEODE_IS_MACOS

setCellColors(MapPackCell, loadFromMapPack, GJMapPack);

#endif

#endif