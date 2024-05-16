#pragma once

#ifndef HAPPYUTILS_H
#define HAPPYUTILS_H

#define public_cast(value, member) [](auto* v) { \
    class FriendClass__; \
    using T = std::remove_pointer<decltype(v)>::type; \
    class FriendeeClass__: public T { \
    protected: \
        friend FriendClass__; \
    }; \
    class FriendClass__ { \
    public: \
        auto& get(FriendeeClass__* v) { return v->member; } \
    } c; \
    return c.get(reinterpret_cast<FriendeeClass__*>(v)); \
}(value)

namespace Utils {

    inline void toLower(std::string value){
        for(auto& c : value){
            c = tolower(c);
        }
    }

    inline bool endsWith(std::string value, std::string ending){
        toLower(value);
        toLower(ending);

        return value.ends_with(ending);
    }

    inline bool startsWith(std::string value, std::string start){
        toLower(value);
        toLower(start);

        return value.starts_with(start);
    }
}
#endif