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

    inline std::string toLower(std::string s) {        
        for(char &c : s){
            c = tolower(c);
        }
        return s;
    }

    inline bool endsWith(std::string value, std::string ending){
        value = toLower(value);
        ending = toLower(ending);

        return value.ends_with(ending);
    }

    inline bool startsWith(std::string value, std::string start){
        value = toLower(value);
        start = toLower(start);

        return value.starts_with(start);
    }
}
#endif