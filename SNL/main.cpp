#include <iostream>
#include "./Lex_Analysis/Head/lex_analysis.h"
using namespace std;

int main() {
    //词法分析
    LexAnalysis lex_analysis;
    vector<Token> rec;
    try {
        rec = lex_analysis.getTokenList("in.txt");
    } catch (exception) {
        lex_analysis.printTokenList("out.txt");
        return 0;
    }
    lex_analysis.printTokenList("out.txt");


    //语法分析

    return 0;
}

