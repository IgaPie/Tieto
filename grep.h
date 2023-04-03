#ifndef GREP_H
#define GREP_H

#include <fstream>
#include <filesystem>
#include <iostream>
#include <map>
#include <vector>
#include <thread>
using namespace std;

namespace fs = std::filesystem;


class Grep
{
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
    int fileCount = 0;
    int filePatternCount = 0;
    int patternCount = 0;
    std::map<const std::thread::id, std::vector<std::string>> threadLogMap;
};

#endif // GREP_H
