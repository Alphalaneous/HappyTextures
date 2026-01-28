#include <Geode/Geode.hpp>
#include <Geode/modify/CCSpriteFrameCache.hpp>
#include "../Utils.hpp"

class $modify(MyCCSpriteFrameCache, CCSpriteFrameCache) {
    CCSpriteFrame* spriteFrameByName(const char *pszName) {

        if (Utils::spriteExistsInPacks(pszName)) {
            CCTexture2D* pTexture = CCTextureCache::get()->addImage(pszName, false);
            if (pTexture){
                CCRect rect = CCRectZero;
                rect.size = pTexture->getContentSize();
                return CCSpriteFrame::createWithTexture(pTexture, rect);
            }
        }
        return CCSpriteFrameCache::spriteFrameByName(pszName);
    }
};
