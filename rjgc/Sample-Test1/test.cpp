
#include "pch.h" 
#include "PlagiarismChecker.h"
#include <fstream>
#include <gtest/gtest.h>

// 后续测试代码
TEST(TestCaseName, TestName) {
    EXPECT_EQ(1, 1);
    EXPECT_TRUE(true);
}

TEST(PlagiarismCheckerTest, ReadFile) {
    PlagiarismChecker checker;
    std::string filePath = "test.txt";
    std::ofstream file(filePath);
    file << "Test text";
    file.close();

    std::string content = checker.readFile(filePath);
    EXPECT_EQ(content, "Test text");
}

TEST(PlagiarismCheckerTest, CalculateSimilarity) {
    PlagiarismChecker checker;
    std::string original = "Test text";
    std::string plagiarized = "Test text";
    double similarity = checker.calculateSimilarity(original, plagiarized);
    EXPECT_EQ(similarity, 1.0);
}

TEST(PlagiarismCheckerTest, WriteResult) {
    PlagiarismChecker checker;
    std::string resultPath = "result.txt";
    double similarity = 0.8;
    checker.writeResult(resultPath, similarity);

    std::ifstream file(resultPath);
    std::string content;
    file >> content;
    EXPECT_EQ(content, "0.80");
}

// 其他测试用例...

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
