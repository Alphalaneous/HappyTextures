#include <Geode/Geode.hpp>
#include "../Macros.hpp"
#include <alphalaneous.alphas_geode_utils/include/ObjectModify.hpp>
#include "../UIModding.hpp"
#include "../Utils.hpp"

using namespace geode::prelude;

class $classModify(MyInfoLayer, InfoLayer) {

    void modify(){
        if (UIModding::get()->doModify) {
            CCNode* theNode;
            for (CCNode* node : CCArrayExt<CCNode*>(this->getChildren())) {
                if (node->getChildrenCount() > 1) {
                    theNode = node;
                    break;
                }
            }
            
            if (CCScale9Sprite* bg = typeinfo_cast<CCScale9Sprite*>(theNode->getChildByID("desc-background"))) {
                Utils::setColorIfExists(bg, "info-description-bg");
            }
        }
    }
};