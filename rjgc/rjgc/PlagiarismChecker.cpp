#include "PlagiarismChecker.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <cmath>
#include <iomanip>
#include <stdexcept>

std::string PlagiarismChecker::readFile(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + filePath);
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

double PlagiarismChecker::calculateSimilarity(const std::string& original, const std::string& plagiarized) {
    std::unordered_map<std::string, int> originalWordCount;
    std::unordered_map<std::string, int> plagiarizedWordCount;

    // 简单的分词处理
    std::istringstream originalStream(original);
    std::istringstream plagiarizedStream(plagiarized);
    std::string word;

    while (originalStream >> word) {
        originalWordCount[word]++;
    }

    while (plagiarizedStream >> word) {
        plagiarizedWordCount[word]++;
    }

    double dotProduct = 0.0;
    double originalNorm = 0.0;
    double plagiarizedNorm = 0.0;

    for (const auto& pair : originalWordCount) {
        const std::string& word = pair.first;
        int count = pair.second;
        originalNorm += count * count;
        if (plagiarizedWordCount.find(word) != plagiarizedWordCount.end()) {
            dotProduct += count * plagiarizedWordCount[word];
        }
    }

    for (const auto& pair : plagiarizedWordCount) {
        int count = pair.second;
        plagiarizedNorm += count * count;
    }

    originalNorm = std::sqrt(originalNorm);
    plagiarizedNorm = std::sqrt(plagiarizedNorm);

    if (originalNorm == 0 || plagiarizedNorm == 0) {
        return 0.0;
    }

    return dotProduct / (originalNorm * plagiarizedNorm);
}

void PlagiarismChecker::writeResult(const std::string& resultPath, double similarity) {
    std::ofstream file(resultPath);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + resultPath);
    }
    file << std::fixed << std::setprecision(2) << similarity;
}
