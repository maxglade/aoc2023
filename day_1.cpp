#include <iostream>
#include <fstream>
#include <regex>
#include <map>
#include <thread>
#include <vector>
#include <atomic>
#include <queue>
#include <mutex>

std::map<std::string, std::string> num_map = {
    {"one", "o1e"},
    {"two", "t2o"},
    {"three", "t3e"},
    {"four", "f4r"},
    {"five", "f5e"},
    {"six", "s6x"},
    {"seven", "s7n"},
    {"eight", "e8t"},
    {"nine", "n9e"}
};

std::atomic<int> agg{0};
std::mutex printMutex;

void processLine(std::queue<std::string>& lines) {
    while (true) {
        std::string line;
        {
            std::lock_guard<std::mutex> lock(printMutex);
            if (lines.empty()) {
                break;
            }
            line = std::move(lines.front());
            lines.pop();
        }
        
        std::string val = line;
        for (const auto& pair : num_map) {
            std::regex regex(pair.first);
            val = std::regex_replace(val, regex, pair.second);
        }
        val = std::regex_replace(val, std::regex("[^0-9]"), "");
        val = val.substr(0, 1) + val.substr(val.length() - 1);
        
        agg += std::stoi(val);
    }
}

int solutionD1P2() {
    std::ifstream file("./input.txt");
    if (file.is_open()) {
        std::string line;
        std::queue<std::string> lines;
        while (std::getline(file, line)) {
            lines.push(line);
        }
        file.close();
        
        const int numThreads = std::thread::hardware_concurrency();
        std::vector<std::thread> threads;
        for (int i = 0; i < numThreads; ++i) {
            threads.emplace_back(processLine, std::ref(lines));
        }
        
        for (auto& thread : threads) {
            thread.join();
        }
    }
    return agg;
}

int main() {
    std::cout << solutionD1P2() << std::endl;
    return 0;
}
