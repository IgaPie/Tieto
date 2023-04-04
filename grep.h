#ifndef GREP_H
#define GREP_H

#include <fstream>
#include <filesystem>
#include <iostream>
#include <map>
#include <vector>
#include <atomic>
#include <memory>

#include "threadpool.h"

using namespace std;

namespace fs = std::filesystem;

struct ThreadLogStruct{
    std::thread::id id;
    std::vector<std::string> filenames;
};

class Grep{
public:
    Grep(int argc, char *argv[]);
    void Execute();
    ~Grep();
private:
    int threadCount;
    fs::path baseDir;
    fs::path programPath;
    std::string logName;
    std::string resultName;
    std::string searchWord;
    fstream resultFile;
    fstream logFile;
    atomic_int fileCount = 0;
    atomic_int filePatternCount = 0;
    atomic_int patternCount = 0;
    std::vector<ThreadLogStruct> threadLogVec;
    std::map<std::thread::id, int> threadLogMapToIndex;
    std::unique_ptr<ThreadPool> threadPool;
    std::mutex resultMutex;
};

#endif // GREP_H
