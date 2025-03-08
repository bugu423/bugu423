#pragma once
#pragma once
#include <string>
#include <unordered_map>

class PlagiarismChecker {
public:
    std::string readFile(const std::string& filePath);
    double calculateSimilarity(const std::string& original, const std::string& plagiarized);
    void writeResult(const std::string& resultPath, double similarity);
};
