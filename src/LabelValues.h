#pragma once

#include <Geode/Geode.hpp>
#include <rift.hpp>
#include "Macros.h"

using namespace geode::prelude;

namespace LabelValues {

    static std::pair<std::string, rift::Value> getUsername() {

        auto accountManager = GJAccountManager::get();
        std::string username = accountManager->m_accountID == 0 ? "Signed Out" : accountManager->m_username;

        return LABEL("username", std::string(username));
    }

    static std::pair<std::string, rift::Value> getGameVersion() {
        return LABEL("game_version", GEODE_GD_VERSION_STRING);
    }

    static std::pair<std::string, rift::Value> getGeodeVersion() {
        return LABEL("geode_version", Loader::get()->getVersion().toNonVString());
    }

    static std::pair<std::string, rift::Value> getOriginalString(std::string str) {
        return LABEL("previous_string", str);
    }

    static std::pair<std::string, rift::Value> getJumps() {
        return LABEL("jumps_stat", STAT(1));
    }

    static std::pair<std::string, rift::Value> getAttempts() {
        return LABEL("attempts_stat", STAT(2));
    }

    static std::pair<std::string, rift::Value> getCompletedLevels() {
        return LABEL("completed_levels_stat", STAT(3));
    }

    static std::pair<std::string, rift::Value> getCompletedOnlineLevels() {
        return LABEL("completed_online_levels_stat", STAT(4));
    }

    static std::pair<std::string, rift::Value> getDemons() {
        return LABEL("demons_stat", STAT(5));
    }

    static std::pair<std::string, rift::Value> getStars() {
        return LABEL("stars_stat", STAT(6));
    }

    static std::pair<std::string, rift::Value> getCompletedMapPacks() {
        return LABEL("completed_map_packs_stat", STAT(7));
    }

    static std::pair<std::string, rift::Value> getGoldCoins() {
        return LABEL("gold_coins_stat", STAT(8));
    }

    static std::pair<std::string, rift::Value> getPlayersDestroyed() {
        return LABEL("players_destroyed_stat", STAT(9));
    }

    static std::pair<std::string, rift::Value> getLikedLevels() {
        return LABEL("liked_levels_stat", STAT(10));
    }

    static std::pair<std::string, rift::Value> getRatedLevels() {
        return LABEL("rated_levels_stat", STAT(11));
    }

    static std::pair<std::string, rift::Value> getUserCoins() {
        return LABEL("user_coins_stat", STAT(12));
    }

    static std::pair<std::string, rift::Value> getDiamonds() {
        return LABEL("diamonds_stat", STAT(13));
    }

    static std::pair<std::string, rift::Value> getCurrentOrbs() {
        return LABEL("current_orbs_stat", STAT(14));
    }

    static std::pair<std::string, rift::Value> getCompletedDailies() {
        return LABEL("completed_dailies_stat", STAT(15));
    }

    static std::pair<std::string, rift::Value> getFireShards() {
        return LABEL("fire_shards_stat", STAT(16));
    }

    static std::pair<std::string, rift::Value> getIceShards() {
        return LABEL("ice_shards_stat", STAT(17));
    }

    static std::pair<std::string, rift::Value> getPoisonShards() {
        return LABEL("poison_shards_stat", STAT(18));
    }

    static std::pair<std::string, rift::Value> getShadowShards() {
        return LABEL("shadow_shards_stat", STAT(19));
    }

    static std::pair<std::string, rift::Value> getLavaShards() {
        return LABEL("lava_shards_stat", STAT(20));
    }

    static std::pair<std::string, rift::Value> getDemonKeys() {
        return LABEL("demon_keys_stat", STAT(21));
    }

    static std::pair<std::string, rift::Value> getTotalOrbs() {
        return LABEL("total_orbs_stat", STAT(22));
    }

    static std::pair<std::string, rift::Value> getEarthShards() {
        return LABEL("earth_shards_stat", STAT(23));
    }

    static std::pair<std::string, rift::Value> getBloodShards() {
        return LABEL("blood_shards_stat", STAT(24));
    }

    static std::pair<std::string, rift::Value> getMetalShards() {
        return LABEL("metal_shards_stat", STAT(25));
    }

    static std::pair<std::string, rift::Value> getLightShards() {
        return LABEL("light_shards_stat", STAT(26));
    }

    static std::pair<std::string, rift::Value> getSoulShards() {
        return LABEL("soul_shards_stat", STAT(27));
    }

    static std::pair<std::string, rift::Value> getMoons() {
        return LABEL("moons_stat", STAT(28));
    }

    static std::pair<std::string, rift::Value> getDiamondShards() {
        return LABEL("diamond_shards_stat", STAT(29));
    }

    static std::pair<std::string, rift::Value> getCompletedGauntlets() {
        return LABEL("completed_gauntlets_stat", STAT(40));
    }

    static std::pair<std::string, rift::Value> getCollectedListRewards() {
        return LABEL("collected_list_rewards_stat", STAT(41));
    }

    static std::pair<std::string, rift::Value> getLevelCreator() {
        GJGameLevel* level = Utils::getLevel();
        if (level) {
            return LABEL("level_creator", std::string(level->m_creatorName));
        }
        return LABEL("level_creator", "null");
    }

    static std::pair<std::string, rift::Value> getLevelName() {
        GJGameLevel* level = Utils::getLevel();
        if (level) {
            return LABEL("level_name", std::string(level->m_levelName));
        }
        return LABEL("level_name", "null");
    }

    static std::pair<std::string, rift::Value> getLevelDescription() {
        #ifndef GEODE_IS_MACOS
        GJGameLevel* level = Utils::getLevel();
        if (level) {
            return LABEL("level_description", std::string(level->getUnpackedLevelDescription()));
        }
        #endif
        return LABEL("level_description", "null");
    }

    static std::pair<std::string, rift::Value> getLevelID() {
        GJGameLevel* level = Utils::getLevel();
        if (level) {
            return LABEL("level_id", level->m_levelID.value());
        }
        return LABEL("level_id", "null");
    }

    static std::pair<std::string, rift::Value> getNormalPercent() {
        GJGameLevel* level = Utils::getLevel();
        if (level) {
            return LABEL("level_normal_percent", level->m_normalPercent.value());
        }
        return LABEL("level_normal_percent", "null");
    }

    static std::pair<std::string, rift::Value> getPracticePercent() {
        GJGameLevel* level = Utils::getLevel();
        if (level) {
            return LABEL("level_practice_percent", level->m_practicePercent);
        }
        return LABEL("level_practice_percent", "null");
    }

    static std::pair<std::string, rift::Value> getLevelAttempts() {
        GJGameLevel* level = Utils::getLevel();
        if (level) {
            return LABEL("level_attempts", level->m_attempts.value());
        }
        return LABEL("level_attempts", "null");
    }

    static std::pair<std::string, rift::Value> getLevelAttemptTime() {
        GJGameLevel* level = Utils::getLevel();
        if (level) {
            return LABEL("level_attempt_time", level->m_attemptTime.value());
        }
        return LABEL("level_attempt_time", "null");
    }

    static std::pair<std::string, rift::Value> getLevelBestTime() {
        GJGameLevel* level = Utils::getLevel();
        if (level) {
            return LABEL("level_best_time", level->m_bestTime);
        }
        return LABEL("level_best_time", "null");
    }

    static std::pair<std::string, rift::Value> getLevelJumps() {
        GJGameLevel* level = Utils::getLevel();
        if (level) {
            return LABEL("level_jumps", level->m_jumps.value());
        }
        return LABEL("level_jumps", "null");
    }

    static std::pair<std::string, rift::Value> getLevelPassword() {
        GJGameLevel* level = Utils::getLevel();
        if (level) {
            return LABEL("level_password", level->m_password.value());
        }
        return LABEL("level_password", "null");
    }

    static std::pair<std::string, rift::Value> getLevelStars() {
        GJGameLevel* level = Utils::getLevel();
        if (level) {
            return LABEL("level_stars", level->m_stars.value());
        }
        return LABEL("level_stars", "null");
    }

    static std::pair<std::string, rift::Value> getLevelLengthValue() {
        GJGameLevel* level = Utils::getLevel();
        if (level) {
            return LABEL("level_length_value", level->m_levelLength);
        }
        return LABEL("level_length_value", "null");
    }

    static std::unordered_map<std::string, rift::Value> getValueMap(std::string original) {
        return {
            getUsername(),
            getGameVersion(),
            getGeodeVersion(),
            getOriginalString(original),
            getJumps(),
            getAttempts(),
            getCompletedLevels(),
            getCompletedOnlineLevels(),
            getDemons(),
            getStars(),
            getCompletedMapPacks(),
            getGoldCoins(),
            getPlayersDestroyed(),
            getLikedLevels(),
            getRatedLevels(),
            getUserCoins(),
            getDiamonds(),
            getCurrentOrbs(),
            getCompletedDailies(),
            getFireShards(),
            getIceShards(),
            getPoisonShards(),
            getShadowShards(),
            getLavaShards(),
            getDemonKeys(),
            getTotalOrbs(),
            getEarthShards(),
            getBloodShards(),
            getMetalShards(),
            getLightShards(),
            getSoulShards(),
            getMoons(),
            getDiamondShards(),
            getCompletedGauntlets(),
            getCollectedListRewards(),
            getLevelCreator(),
            getLevelName(),
            getLevelDescription(),
            getLevelID(),
            getNormalPercent(),
            getPracticePercent(),
            getLevelAttempts(),
            getLevelAttemptTime(),
            getLevelBestTime(),
            getLevelJumps(),
            getLevelPassword(),
            getLevelStars(),
            getLevelLengthValue()
        };
    }
};