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

// --------------------- �������� ---------------------
int myGCD(int a, int b) {
    a = abs(a); b = abs(b);
    return b == 0 ? a : myGCD(b, a % b);
}

// --------------------- ������ ---------------------
struct Fraction {
    int num; // ����
    int den; // ��ĸ��ʼ�մ���0

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
        // ���� other ��Ϊ 0
        return Fraction(num * other.den, den * other.num);
    }

    bool operator<(const Fraction& other) const {
        return num * other.den < other.num * den;
    }

    bool operator==(const Fraction& other) const {
        return num == other.num && den == other.den;
    }

    // �����ʽ�������Ӵ��ڷ�ĸʱת��Ϊ���������ֵĻ�Ϸ��������� a/b ��ʽ
    string toString() const {
        if (num < 0) { // ��֤�Ǹ����������ɾ�Ϊ�Ǹ������
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

// --------------------- ���ʽ�� ---------------------
enum ExprType { NUMBER, OPERATOR };

struct Expression {
    ExprType type;
    Fraction value;   // �� type==NUMBER ʱ��Ч
    char op;          // �� type==OPERATOR ʱ��Ч
    Expression* left;
    Expression* right;

    // �������ֽڵ�
    Expression(const Fraction& val) : type(NUMBER), value(val), left(nullptr), right(nullptr) { }

    // ��������ڵ�
    Expression(char opr, Expression* l, Expression* r) : type(OPERATOR), op(opr), left(l), right(r) { }
};

// �ݹ��ͷű��ʽ��
void deleteExpr(Expression* expr) {
    if (expr == nullptr) return;
    if (expr->type == OPERATOR) {
        deleteExpr(expr->left);
        deleteExpr(expr->right);
    }
    delete expr;
}

// ������ʽֵ
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

// ������ʽ�ַ�����ÿ������������ż��ÿո�ָ���
string exprToString(Expression* expr) {
    if (expr->type == NUMBER)
        return expr->value.toString();
    // ʼ�ռ���������ȷ����˳��
    return "( " + exprToString(expr->left) + " " + string(1, expr->op) + " " + exprToString(expr->right) + " )";
}

// ������ʽ�ı�׼����ʾ������Ψһ���жϣ��� + �� * ����������
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

// --------------------- �����Ŀ���� ---------------------
// �������һ����Ȼ��������������ɵ���ֵ���ڸ�����Χ�ڣ�
// ���� range ��ʾȡֵ��Χ������ 10 ��ʾ���� 10 ���ڵ���Ȼ�����Լ������еķ��ӡ��������־��ڴ˷�Χ�ڣ�
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
            int whole = rand() % range; // ��Ϸ�����������
            int denominator = rand() % (range - 1) + 2;
            int numerator = rand() % (denominator - 1) + 1;
            return Fraction(whole * denominator + numerator, denominator);
        }
    }
}

// �ݹ����ɱ��ʽ����opsLeft ��ʾ��Ҫ���ɵ��������������� 3 ������range Ϊ��ֵ��Χ
Expression* generateExpression(int opsLeft, int range) {
    if (opsLeft == 0) {
        Fraction num = generateNumberValue(range);
        return new Expression(num);
    }
    // ���ѡ�������
    char ops[4] = { '+', '-', '*', '/' };
    char op = ops[rand() % 4];
    // ����������������е������������ע�⣺���� = opsLeft - 1
    int leftOps = rand() % (opsLeft);
    int rightOps = opsLeft - 1 - leftOps;
    Expression* leftExpr = generateExpression(leftOps, range);
    Expression* rightExpr = generateExpression(rightOps, range);

    // �Լ�����ȷ�����ӱ��ʽֵ��С�����ӱ��ʽֵ����֤������������
    if (op == '-') {
        Fraction lVal = evaluate(leftExpr);
        Fraction rVal = evaluate(rightExpr);
        if (lVal < rVal) {
            deleteExpr(leftExpr);
            deleteExpr(rightExpr);
            return generateExpression(opsLeft, range);
        }
    }
    // �Գ�����ȷ��������Ϊ 0 �ҽ��Ϊ���������������С�ڳ�����
    if (op == '/') {
        Fraction rVal = evaluate(rightExpr);
        if (rVal.num == 0) {
            deleteExpr(leftExpr);
            deleteExpr(rightExpr);
            return generateExpression(opsLeft, range);
        }
        Fraction lVal = evaluate(leftExpr);
        // Ҫ����С��1��lVal < rVal
        if (!(lVal < rVal)) {
            deleteExpr(leftExpr);
            deleteExpr(rightExpr);
            return generateExpression(opsLeft, range);
        }
    }
    return new Expression(op, leftExpr, rightExpr);
}

// --------------------- ��Ŀ���������� ---------------------
void generateExercises(int num, int range) {
    ofstream exFile("Exercises.txt");
    ofstream ansFile("Answers.txt");
    if (!exFile || !ansFile) {
        cerr << "�ļ���ʧ��" << endl;
        return;
    }

    set<string> exprSet; // �洢��׼�����ʽ�������ظ�
    int count = 0;
    int trial = 0;
    while (count < num && trial < num * 10) { // �������Դ���
        trial++;
        int opsCount = rand() % 4; // ��������� 0~3
        Expression* expr = generateExpression(opsCount, range);
        string cano = canonical(expr);
        if (exprSet.find(cano) != exprSet.end()) {
            deleteExpr(expr);
            continue;
        }
        exprSet.insert(cano);
        count++;
        // д��Ŀ����ʽ "1. ��Ŀ���� ="
        exFile << count << ". " << exprToString(expr) << " =" << endl;
        // �����
        Fraction ans = evaluate(expr);
        ansFile << "(" << count << ") " << ans.toString() << endl;
        deleteExpr(expr);
    }
    exFile.close();
    ansFile.close();
    cout << "���� " << count << " ����Ŀ�� Exercises.txt �� Answers.txt" << endl;
}

// --------------------- �������ʽ���������Ĵ𰸣� ---------------------
// �ִʣ����տո�ָ���ȥ���մ���
vector<string> tokenize(const string& line) {
    vector<string> tokens;
    istringstream iss(line);
    string token;
    while (iss >> token) {
        tokens.push_back(token);
    }
    return tokens;
}

// �������֣�֧������ 5, 3/4, 2'3/8��
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

// ǰ������
Expression* parseExpression(vector<string>& tokens, int& pos);

Expression* parseFactor(vector<string>& tokens, int& pos) {
    if (tokens[pos] == "(") {
        pos++; // ���� "("
        Expression* expr = parseExpression(tokens, pos);
        if (pos < tokens.size() && tokens[pos] == ")")
            pos++; // ���� ")"
        return expr;
    }
    else {
        // ���ֿ��ܰ��� ' �� /
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

// ����һ����Ŀ����Ŀ�и�ʽ�� "1. ( ... ) =" �������ر��ʽ�ַ�������
string extractExprStr(const string& line) {
    // �ҵ���һ��"."���"="֮ǰ�Ĳ���
    size_t dotPos = line.find('.');
    size_t eqPos = line.rfind('=');
    if (dotPos == string::npos || eqPos == string::npos)
        return "";
    // ȥ��ǰ��ո�
    string expr = line.substr(dotPos + 1, eqPos - dotPos - 1);
    // trim
    while (!expr.empty() && expr.front() == ' ')
        expr.erase(expr.begin());
    while (!expr.empty() && expr.back() == ' ')
        expr.pop_back();
    return expr;
}

// --------------------- ���⹦�� ---------------------
void gradeExercises(const string& exFileName, const string& ansFileName) {
    ifstream exFile(exFileName);
    ifstream stuAnsFile(ansFileName);
    if (!exFile || !stuAnsFile) {
        cerr << "����Ŀ����ļ�ʧ��" << endl;
        return;
    }

    vector<string> correctAns; // �����ȷ�𰸵��ַ�������׼����ʽ��
    vector<int> correctIdx, wrongIdx;

    string line;
    // ����Ŀ�ļ���ÿ�и�ʽ��"i. <expression> ="
    while (getline(exFile, line)) {
        if (line.empty()) continue;
        string exprStr = extractExprStr(line);
        // �ִʣ�ע�⣬����ʱ���Ǳ�֤������֮���пո�
        vector<string> tokens = tokenize(exprStr);
        int pos = 0;
        Expression* expr = parseExpression(tokens, pos);
        Fraction ans = evaluate(expr);
        correctAns.push_back(ans.toString());
        deleteExpr(expr);
    }
    exFile.close();

    // ��ѧ�����ļ���ÿ�и�ʽ��"(i) <answer>"
    vector<string> stuAns;
    while (getline(stuAnsFile, line)) {
        if (line.empty()) continue;
        // �ҵ������ź���Ĵ𰸲���
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
        // �򵥱Ƚ��ַ�����Ҫ�����ɺ�ѧ���𰸸�ʽһ�£�
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

    cout << "���Ľ���ѱ����� Grade.txt" << endl;
}

// --------------------- ������Ϣ ---------------------
void printHelp() {
    cout << "ʹ�÷���:" << endl;
    cout << "  ������Ŀ: Myapp.exe -n <��Ŀ����> -r <��ֵ��Χ>" << endl;
    cout << "  ����: Myapp.exe -e <exercises file> -a <answer file>" << endl;
}

// --------------------- ������ ---------------------
int main(int argc, char* argv[]) {
    srand((unsigned)time(0));

    if (argc < 3) {
        printHelp();
        return 1;
    }

    string mode = argv[1];
    if (mode == "-n") {
        // ������Ŀģʽ
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
            cout << "����ָ����Ч����ֵ��Χ���� -r" << endl;
            printHelp();
            return 1;
        }
        generateExercises(num, range);
    }
    else if (mode == "-e") {
        // ����ģʽ
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