#pragma once

#ifndef HAPPYUTILS_H
#define HAPPYUTILS_H

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