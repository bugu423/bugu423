#include <iostream>
#include "PlagiarismChecker.h"

int main(int argc, char* argv[]) {
    if (argc != 4) {
        std::cerr << "Usage: main.exe [原文文件] [抄袭版论文的文件] [答案文件]" << std::endl;
        return 1;
    }

    std::string originalPath = argv[1];
    std::string plagiarizedPath = argv[2];
    std::string resultPath = argv[3];

    PlagiarismChecker checker;
    try {
        std::string originalText = checker.readFile(originalPath);
        std::string plagiarizedText = checker.readFile(plagiarizedPath);
        double similarity = checker.calculateSimilarity(originalText, plagiarizedText);
        checker.writeResult(resultPath, similarity);
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}