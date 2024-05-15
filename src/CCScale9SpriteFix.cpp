#include <Geode/Geode.hpp>
#include <Geode/modify/CCScale9Sprite.hpp>
#include "Utils.h"

using namespace geode::prelude;

class $modify(CCScale9Sprite) {

	struct Fields {
		bool m_hasFixed = false;
	};

	void visit() {
		CCScale9Sprite::visit();
		if (!m_fields->m_hasFixed) {
			fixSprites();
			m_fields->m_hasFixed = true;
		}
	}

	void transformSprite(CCSprite* sprite, CCSize transform, int idx) {
		auto tRect = sprite->getTextureRect();

		tRect.size -= transform;

		if (idx == 0)
			sprite->setPosition(sprite->getPosition() + transform);
		else if (idx == 1 && (transform.width > 0 || transform.height > 0))
			sprite->setVisible(false);
		else if (idx == 2)
			tRect.origin += transform;

		sprite->setTextureRect(tRect);
	}

	void fixSprites() {
		CCSprite* sprites[3][3] = {
			{_topLeft, _top, _topRight},
			{_left, _centre, _right},
			{_bottomLeft, _bottom, _bottomRight}
		};

		auto halfSize = getContentSize() / 2;

		for (int i = 0; i < 3; ++i) {
			for (int j = 0; j < 3; ++j) {
				auto sprite = sprites[i][j];

				auto tRect = sprite->getTextureRect();
				auto diff = tRect.size - halfSize;
				diff = CCSize(fmax(0, diff.width), fmax(0, diff.height));

				transformSprite(sprite, CCSize(diff.width, 0), j);
				transformSprite(sprite, CCSize(0, diff.height), i);
			}
		}
	}
};