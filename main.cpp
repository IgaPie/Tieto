#include <iostream>
#include <filesystem>
using namespace std;

namespace fs = std::filesystem;

int main(int argc, char *argv[])
{
    int threadCount = 4;
    auto baseDir = fs::current_path();
    fs::path programPath = argv[0];
    std::string logName = programPath.filename().string() + ".log";
    std::string resultName = programPath.filename().string() + ".txt";

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
    }
    return 0;
}
