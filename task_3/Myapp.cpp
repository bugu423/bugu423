#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <set>
#include <algorithm>
#include <cmath>
using namespace std;

// --------------------- 辅助函数 ---------------------
int myGCD(int a, int b) {
    a = abs(a); b = abs(b);
    return b == 0 ? a : myGCD(b, a % b);
}

// --------------------- 分数类 ---------------------
struct Fraction {
    int num; // 分子
    int den; // 分母，始终大于0

    Fraction(int n = 0, int d = 1) : num(n), den(d) {
        if (d == 0) {
            throw "Denominator cannot be zero";
        }
        normalize();
    }

    void normalize() {
        if (den < 0) { num = -num; den = -den; }
        int g = myGCD(num, den);
        if (g == 0) g = 1;
        num /= g;
        den /= g;
    }

    Fraction operator+(const Fraction& other) const {
        return Fraction(num * other.den + other.num * den, den * other.den);
    }

    Fraction operator-(const Fraction& other) const {
        return Fraction(num * other.den - other.num * den, den * other.den);
    }

    Fraction operator*(const Fraction& other) const {
        return Fraction(num * other.num, den * other.den);
    }

    Fraction operator/(const Fraction& other) const {
        // 假设 other 不为 0
        return Fraction(num * other.den, den * other.num);
    }

    bool operator<(const Fraction& other) const {
        return num * other.den < other.num * den;
    }

    bool operator==(const Fraction& other) const {
        return num == other.num && den == other.den;
    }

    // 输出格式：当分子大于分母时转换为带整数部分的混合分数，否则 a/b 格式
    string toString() const {
        if (num < 0) { // 保证非负（本题生成均为非负结果）
            return "-" + Fraction(-num, den).toString();
        }
        if (num >= den) {
            int whole = num / den;
            int remainder = num % den;
            if (remainder == 0)
                return to_string(whole);
            else
                return to_string(whole) + "\'" + to_string(remainder) + "/" + to_string(den);
        }
        else {
            return to_string(num) + "/" + to_string(den);
        }
    }
};

// --------------------- 表达式树 ---------------------
enum ExprType { NUMBER, OPERATOR };

struct Expression {
    ExprType type;
    Fraction value;   // 当 type==NUMBER 时有效
    char op;          // 当 type==OPERATOR 时有效
    Expression* left;
    Expression* right;

    // 构造数字节点
    Expression(const Fraction& val) : type(NUMBER), value(val), left(nullptr), right(nullptr) { }

    // 构造运算节点
    Expression(char opr, Expression* l, Expression* r) : type(OPERATOR), op(opr), left(l), right(r) { }
};

// 递归释放表达式树
void deleteExpr(Expression* expr) {
    if (expr == nullptr) return;
    if (expr->type == OPERATOR) {
        deleteExpr(expr->left);
        deleteExpr(expr->right);
    }
    delete expr;
}

// 计算表达式值
Fraction evaluate(Expression* expr) {
    if (expr->type == NUMBER)
        return expr->value;
    Fraction l = evaluate(expr->left);
    Fraction r = evaluate(expr->right);
    switch (expr->op) {
    case '+': return l + r;
    case '-': return l - r;
    case '*': return l * r;
    case '/': return l / r;
    }
    return Fraction(0, 1);
}

// 输出表达式字符串（每个运算符和括号间用空格分隔）
string exprToString(Expression* expr) {
    if (expr->type == NUMBER)
        return expr->value.toString();
    // 始终加括号以明确运算顺序
    return "( " + exprToString(expr->left) + " " + string(1, expr->op) + " " + exprToString(expr->right) + " )";
}

// 计算表达式的标准化表示，用于唯一性判断（对 + 和 * 交换律排序）
string canonical(Expression* expr) {
    if (expr->type == NUMBER)
        return expr->value.toString();
    string leftStr = canonical(expr->left);
    string rightStr = canonical(expr->right);
    if (expr->op == '+' || expr->op == '*') {
        if (rightStr < leftStr)
            swap(leftStr, rightStr);
    }
    return "(" + leftStr + " " + expr->op + " " + rightStr + ")";
}

// --------------------- 随机题目生成 ---------------------
// 随机生成一个自然数或真分数（生成的数值均在给定范围内）
// 参数 range 表示取值范围（例如 10 表示生成 10 以内的自然数，以及分数中的分子、整数部分均在此范围内）
Fraction generateNumberValue(int range) {
    bool useFraction = (rand() % 2 == 1);
    if (!useFraction) {
        int num = rand() % range;
        return Fraction(num, 1);
    }
    else {
        bool isMixed = (rand() % 2 == 1);
        if (!isMixed) {
            int denominator = rand() % (range - 1) + 2; // [2, range]
            int numerator = rand() % (denominator - 1) + 1; // 1 ~ denominator-1
            return Fraction(numerator, denominator);
        }
        else {
            int whole = rand() % range; // 混合分数整数部分
            int denominator = rand() % (range - 1) + 2;
            int numerator = rand() % (denominator - 1) + 1;
            return Fraction(whole * denominator + numerator, denominator);
        }
    }
}

// 递归生成表达式树，opsLeft 表示还要生成的运算符个数（最多 3 个），range 为数值范围
Expression* generateExpression(int opsLeft, int range) {
    if (opsLeft == 0) {
        Fraction num = generateNumberValue(range);
        return new Expression(num);
    }
    // 随机选择运算符
    char ops[4] = { '+', '-', '*', '/' };
    char op = ops[rand() % 4];
    // 随机分配左右子树中的运算符个数，注意：总数 = opsLeft - 1
    int leftOps = rand() % (opsLeft);
    int rightOps = opsLeft - 1 - leftOps;
    Expression* leftExpr = generateExpression(leftOps, range);
    Expression* rightExpr = generateExpression(rightOps, range);

    // 对减法，确保左子表达式值不小于右子表达式值（保证不产生负数）
    if (op == '-') {
        Fraction lVal = evaluate(leftExpr);
        Fraction rVal = evaluate(rightExpr);
        if (lVal < rVal) {
            deleteExpr(leftExpr);
            deleteExpr(rightExpr);
            return generateExpression(opsLeft, range);
        }
    }
    // 对除法，确保除数不为 0 且结果为真分数（即被除数小于除数）
    if (op == '/') {
        Fraction rVal = evaluate(rightExpr);
        if (rVal.num == 0) {
            deleteExpr(leftExpr);
            deleteExpr(rightExpr);
            return generateExpression(opsLeft, range);
        }
        Fraction lVal = evaluate(leftExpr);
        // 要求商小于1：lVal < rVal
        if (!(lVal < rVal)) {
            deleteExpr(leftExpr);
            deleteExpr(rightExpr);
            return generateExpression(opsLeft, range);
        }
    }
    return new Expression(op, leftExpr, rightExpr);
}

// --------------------- 题目生成与答案输出 ---------------------
void generateExercises(int num, int range) {
    ofstream exFile("Exercises.txt");
    ofstream ansFile("Answers.txt");
    if (!exFile || !ansFile) {
        cerr << "文件打开失败" << endl;
        return;
    }

    set<string> exprSet; // 存储标准化表达式，避免重复
    int count = 0;
    int trial = 0;
    while (count < num && trial < num * 10) { // 限制重试次数
        trial++;
        int opsCount = rand() % 4; // 运算符个数 0~3
        Expression* expr = generateExpression(opsCount, range);
        string cano = canonical(expr);
        if (exprSet.find(cano) != exprSet.end()) {
            deleteExpr(expr);
            continue;
        }
        exprSet.insert(cano);
        count++;
        // 写题目：格式 "1. 题目内容 ="
        exFile << count << ". " << exprToString(expr) << " =" << endl;
        // 计算答案
        Fraction ans = evaluate(expr);
        ansFile << "(" << count << ") " << ans.toString() << endl;
        deleteExpr(expr);
    }
    exFile.close();
    ansFile.close();
    cout << "生成 " << count << " 道题目到 Exercises.txt 和 Answers.txt" << endl;
}

// --------------------- 解析表达式（用于批改答案） ---------------------
// 分词（按照空格分隔，去掉空串）
vector<string> tokenize(const string& line) {
    vector<string> tokens;
    istringstream iss(line);
    string token;
    while (iss >> token) {
        tokens.push_back(token);
    }
    return tokens;
}

// 解析数字（支持形如 5, 3/4, 2'3/8）
Fraction parseFraction(const string& token) {
    if (token.find("'") != string::npos) {
        int pos = token.find("'");
        int whole = stoi(token.substr(0, pos));
        string fracPart = token.substr(pos + 1);
        int slashPos = fracPart.find("/");
        int numerator = stoi(fracPart.substr(0, slashPos));
        int denominator = stoi(fracPart.substr(slashPos + 1));
        return Fraction(whole * denominator + numerator, denominator);
    }
    else if (token.find("/") != string::npos) {
        int slashPos = token.find("/");
        int numerator = stoi(token.substr(0, slashPos));
        int denominator = stoi(token.substr(slashPos + 1));
        return Fraction(numerator, denominator);
    }
    else {
        int num = stoi(token);
        return Fraction(num, 1);
    }
}

// 前向声明
Expression* parseExpression(vector<string>& tokens, int& pos);

Expression* parseFactor(vector<string>& tokens, int& pos) {
    if (tokens[pos] == "(") {
        pos++; // 跳过 "("
        Expression* expr = parseExpression(tokens, pos);
        if (pos < tokens.size() && tokens[pos] == ")")
            pos++; // 跳过 ")"
        return expr;
    }
    else {
        // 数字可能包含 ' 或 /
        Expression* numExpr = new Expression(parseFraction(tokens[pos]));
        pos++;
        return numExpr;
    }
}

Expression* parseTerm(vector<string>& tokens, int& pos) {
    Expression* left = parseFactor(tokens, pos);
    while (pos < tokens.size() && (tokens[pos] == "*" || tokens[pos] == "/")) {
        char op = tokens[pos][0];
        pos++;
        Expression* right = parseFactor(tokens, pos);
        left = new Expression(op, left, right);
    }
    return left;
}

Expression* parseExpression(vector<string>& tokens, int& pos) {
    Expression* left = parseTerm(tokens, pos);
    while (pos < tokens.size() && (tokens[pos] == "+" || tokens[pos] == "-")) {
        char op = tokens[pos][0];
        pos++;
        Expression* right = parseTerm(tokens, pos);
        left = new Expression(op, left, right);
    }
    return left;
}

// 解析一道题目（题目行格式： "1. ( ... ) =" ），返回表达式字符串部分
string extractExprStr(const string& line) {
    // 找到第一个"."后和"="之前的部分
    size_t dotPos = line.find('.');
    size_t eqPos = line.rfind('=');
    if (dotPos == string::npos || eqPos == string::npos)
        return "";
    // 去掉前后空格
    string expr = line.substr(dotPos + 1, eqPos - dotPos - 1);
    // trim
    while (!expr.empty() && expr.front() == ' ')
        expr.erase(expr.begin());
    while (!expr.empty() && expr.back() == ' ')
        expr.pop_back();
    return expr;
}

// --------------------- 判题功能 ---------------------
void gradeExercises(const string& exFileName, const string& ansFileName) {
    ifstream exFile(exFileName);
    ifstream stuAnsFile(ansFileName);
    if (!exFile || !stuAnsFile) {
        cerr << "打开题目或答案文件失败" << endl;
        return;
    }

    vector<string> correctAns; // 存放正确答案的字符串（标准化格式）
    vector<int> correctIdx, wrongIdx;

    string line;
    // 读题目文件，每行格式："i. <expression> ="
    while (getline(exFile, line)) {
        if (line.empty()) continue;
        string exprStr = extractExprStr(line);
        // 分词：注意，生成时我们保证各符号之间有空格
        vector<string> tokens = tokenize(exprStr);
        int pos = 0;
        Expression* expr = parseExpression(tokens, pos);
        Fraction ans = evaluate(expr);
        correctAns.push_back(ans.toString());
        deleteExpr(expr);
    }
    exFile.close();

    // 读学生答案文件，每行格式："(i) <answer>"
    vector<string> stuAns;
    while (getline(stuAnsFile, line)) {
        if (line.empty()) continue;
        // 找到右括号后面的答案部分
        size_t parenPos = line.find(')');
        if (parenPos == string::npos) continue;
        string answer = line.substr(parenPos + 1);
        // trim
        while (!answer.empty() && answer.front() == ' ')
            answer.erase(answer.begin());
        stuAns.push_back(answer);
    }
    stuAnsFile.close();

    int total = correctAns.size();
    for (int i = 0; i < total; i++) {
        // 简单比较字符串（要求生成和学生答案格式一致）
        if (i < stuAns.size() && stuAns[i] == correctAns[i])
            correctIdx.push_back(i + 1);
        else
            wrongIdx.push_back(i + 1);
    }

    ofstream gradeFile("Grade.txt");
    gradeFile << "Correct: " << correctIdx.size() << " (";
    for (size_t i = 0; i < correctIdx.size(); i++) {
        gradeFile << correctIdx[i];
        if (i != correctIdx.size() - 1)
            gradeFile << ", ";
    }
    gradeFile << ")" << endl;

    gradeFile << "Wrong: " << wrongIdx.size() << " (";
    for (size_t i = 0; i < wrongIdx.size(); i++) {
        gradeFile << wrongIdx[i];
        if (i != wrongIdx.size() - 1)
            gradeFile << ", ";
    }
    gradeFile << ")" << endl;
    gradeFile.close();

    cout << "批改结果已保存至 Grade.txt" << endl;
}

// --------------------- 帮助信息 ---------------------
void printHelp() {
    cout << "使用方法:" << endl;
    cout << "  生成题目: Myapp.exe -n <题目数量> -r <数值范围>" << endl;
    cout << "  判题: Myapp.exe -e <exercises file> -a <answer file>" << endl;
}

// --------------------- 主函数 ---------------------
int main(int argc, char* argv[]) {
    srand((unsigned)time(0));

    if (argc < 3) {
        printHelp();
        return 1;
    }

    string mode = argv[1];
    if (mode == "-n") {
        // 生成题目模式
        if (argc < 4) {
            printHelp();
            return 1;
        }
        int num = atoi(argv[2]);
        int range = 0;
        for (int i = 1; i < argc; i++) {
            if (string(argv[i]) == "-r" && i + 1 < argc) {
                range = atoi(argv[i + 1]);
                break;
            }
        }
        if (range <= 0) {
            cout << "必须指定有效的数值范围参数 -r" << endl;
            printHelp();
            return 1;
        }
        generateExercises(num, range);
    }
    else if (mode == "-e") {
        // 判题模式
        if (argc < 5) {
            printHelp();
            return 1;
        }
        string exFile, ansFile;
        for (int i = 1; i < argc; i++) {
            if (string(argv[i]) == "-e" && i + 1 < argc) {
                exFile = argv[i + 1];
            }
            if (string(argv[i]) == "-a" && i + 1 < argc) {
                ansFile = argv[i + 1];
            }
        }
        if (exFile.empty() || ansFile.empty()) {
            printHelp();
            return 1;
        }
        gradeExercises(exFile, ansFile);
    }
    else {
        printHelp();
        return 1;
    }

    return 0;
}