#include "grep.h"
#include <chrono>

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
    threadPool = make_unique<ThreadPool>(threadCount);

    for(int i = 0; i < threadCount; i++){
        ThreadLogStruct log = {threadPool->GetThreadID(i), std::vector<std::string>()};
        threadLogVec.push_back(log);
        threadLogMapToIndex.insert_or_assign(log.id, i);
    }
}

Grep::~Grep(){
    resultFile.close();
    logFile.close();
}

void Grep::Execute(){
    auto start = std::chrono::high_resolution_clock::now();
    for(const fs::directory_entry& dir_entry : fs::recursive_directory_iterator(baseDir))
    {
        if(dir_entry.is_directory()){
            continue;
        }
        fileCount++;
        threadPool->AddJob( [this, dir_entry](){
            threadLogVec[threadLogMapToIndex[std::this_thread::get_id()]].filenames.push_back(dir_entry.path().filename().string());
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
                    {
                        std::lock_guard lock(resultMutex);
                        resultFile << path << ":" << nrLine << ": " << line <<std::endl;
                    }
                }
                nrLine++;
            }
            if (patternFound) filePatternCount++;
        });
    }
    while(threadPool->IsBusy()){}
    threadPool->Terminate();

    std::sort(threadLogVec.begin(), threadLogVec.end(), [](auto &s1, auto &s2) -> bool{
        return s1.filenames.size() > s2.filenames.size();
    });

    for(auto &thread : threadLogVec){
        logFile << thread.id << ":";
        for(auto &str : thread.filenames){
            logFile << str << ",";
        }
        logFile << std::endl;
    }

    auto end = std::chrono::high_resolution_clock::now();

    std::cout << "Searched files: " << fileCount <<std::endl;
    std::cout << "Files with pattern: " << filePatternCount <<std::endl;
    std::cout << "Patterns number: " << patternCount << std::endl;
    std::cout << "Result file: " << resultName << std::endl;
    std::cout << "Log file: " << logName << std::endl;
    std::cout << "Used threads: " << threadCount << std::endl;
    std::cout << "Elapsed time: " << std::chrono::duration_cast<chrono::milliseconds>(end - start).count() << "ms" << std::endl;
}
