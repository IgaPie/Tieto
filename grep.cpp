#include "grep.h"

Grep::Grep(int argc, char *argv[])
{
    threadCount = 4;
    baseDir = fs::current_path();
    programPath = argv[0];
    logName = programPath.filename().string() + ".log";
    resultName = programPath.filename().string() + ".txt";

    for(int i=1; i<argc; i++){
        std::string arg(argv[i]);
        if( arg == "--dir" || arg == "-d"){
            baseDir = argv[++i];
        }
        else if( arg == "--log_file" || arg == "-l"){
            logName = argv[++i];
        }
        else if( arg == "--result_file" || arg == "-r"){
            resultName = argv[++i];
        }
        else if( arg == "--threads" || arg == "-t"){
            threadCount = atoi(argv[++i]);
        }
        else {
            searchWord = argv[i];
        }
    }

    resultFile.open(resultName, ios::out);
    logFile.open(logName, ios::out);
    threadLogMap.insert_or_assign(std::this_thread::get_id(), std::vector<std::string>());
}
Grep::~Grep(){
    resultFile.close();
    logFile.close();
}
void Grep::Execute(){
    for(const fs::directory_entry& dir_entry : fs::recursive_directory_iterator(baseDir))
    {
        if(dir_entry.is_directory()){
            continue;
        }
        fileCount++;
        threadLogMap[std::this_thread::get_id()].push_back(dir_entry.path().filename().string());
        auto path = dir_entry.path();
        fstream file;
        file.open(path);
        string line;
        int nrLine = 1;
        bool patternFound = false;
        while(getline(file, line)){
            if(line.find(searchWord) != string::npos){
                patternCount++;
                patternFound = true;
                resultFile << path << ":" << nrLine << ": " << line <<std::endl;
            }
            nrLine++;
        }
        if (patternFound) filePatternCount++;
    }
    for(auto &thread : threadLogMap){
        logFile << thread.first << ":";
        for(auto &str : thread.second){
            logFile << str << ",";
        }
        logFile << std::endl;
    }
    std::cout << "Searched files: " << fileCount <<std::endl;
    std::cout << "Files with pattern: " << filePatternCount <<std::endl;
    std::cout << "Patterns number: " << patternCount << std::endl;
    std::cout << "Result file: " << resultName << std::endl;
    std::cout << "Log file: " << logName << std::endl;
    std::cout << "Used threads: " << threadCount << std::endl;
    std::cout << "Elapsed time: " << 0 << std::endl;
}
