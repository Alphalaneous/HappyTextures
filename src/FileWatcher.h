#pragma once

#ifndef FILEWATCHER_H
#define FILEWATCHER_H

#include <filesystem>
#include <chrono>
#include <thread>
#include <unordered_map>
#include <string>
#include <functional>
#include <Geode/Geode.hpp>

using namespace geode::prelude;
/**
* https://github.com/sol-prog/cpp17-filewatcher/blob/master/FileWatcher.h
*/

enum class FileStatus {created, modified, erased};

class FileWatcher {
public:

    FileWatcher(std::string pathToWatch, std::chrono::duration<int, std::milli> delay) : m_pathToWatch(pathToWatch), m_delay{delay} {

        if(ghc::filesystem::is_directory(pathToWatch)){
            for(auto& file : ghc::filesystem::recursive_directory_iterator(pathToWatch)) {
                m_paths[file.path().string()] = ghc::filesystem::last_write_time(file);
            }
        }
    }

    void stop(){
        m_running = false;
    }

    void start(const std::function<void (std::string, FileStatus)> &action) {

        while(m_running) {

            std::this_thread::sleep_for(m_delay);
            if(ghc::filesystem::is_directory(m_pathToWatch)){

                auto it = m_paths.begin();
                while (it != m_paths.end()) {
                    if (!ghc::filesystem::exists(it->first)) {
                        action(it->first, FileStatus::erased);
                        it = m_paths.erase(it);
                    }
                    else {
                        it++;
                    }                    
                }

                for(auto& file : ghc::filesystem::recursive_directory_iterator(m_pathToWatch)) {
                    auto currentFileLastWriteTime = ghc::filesystem::last_write_time(file);

                    if(!contains(file.path().string())) {
                        m_paths[file.path().string()] = currentFileLastWriteTime;
                        action(file.path().string(), FileStatus::created);
                    } else {
                        if(m_paths[file.path().string()] != currentFileLastWriteTime) {
                            m_paths[file.path().string()] = currentFileLastWriteTime;
                            action(file.path().string(), FileStatus::modified);
                        }
                    } 
                }
            }
        }
        delete this;
    }
private:
    std::unordered_map<std::string, ghc::filesystem::file_time_type> m_paths;
    std::chrono::duration<int, std::milli> m_delay;
    std::string m_pathToWatch = "";
    bool m_running = true;

    bool contains(const std::string &key) {
        auto el = m_paths.find(key);
        return el != m_paths.end();
    }
};

#endif