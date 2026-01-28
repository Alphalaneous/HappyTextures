#include "CCNode.hpp"

void MyCCNode::setBaseID(ZStringView ID) {
    m_fields->m_baseID = ID;
}

ZStringView MyCCNode::getBaseID() {
    return m_fields->m_baseID;
}

void MyCCNode::setAttributes(const matjson::Value& attributes) {
    m_fields->m_attributes.push_back(attributes);
}

void MyCCNode::clearAttributes() {
    m_fields->m_attributes.clear();
}

void MyCCNode::scheduleAttribute(const matjson::Value& attributes) {
    auto fields = m_fields.self();
    
    CCActionInstant* callFunc = CallFuncExt::create([this, attributes] {
        UIModding::get()->handleModifications(this, attributes);
    });
    CCDelayTime* delay = CCDelayTime::create(attributes["delay"].asDouble().unwrapOr(0));
    CCSequence* sequence = CCSequence::create(callFunc, delay, nullptr);
    CCRepeatForever* repeat = CCRepeatForever::create(sequence);

    fields->m_scheduledAttributes.push_back(repeat);

    runAction(repeat);
}

void MyCCNode::resetScheduledAttributes() {
    auto fields = m_fields.self();

    for (CCAction* action : fields->m_scheduledAttributes) {
        stopAction(action);
    }
}

std::vector<matjson::Value> MyCCNode::getAttributes() {
    return m_fields->m_attributes;
}

bool MyCCNode::isModified() {
    return m_fields->m_modified;
}

void MyCCNode::setModified() {
    auto fields = m_fields.self();
    fields->m_self = this;
    fields->m_modified = true;
}
