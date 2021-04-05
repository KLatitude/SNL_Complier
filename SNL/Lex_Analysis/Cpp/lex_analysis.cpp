#include "../Head/lex_analysis.h"
#include "../Head/scanner.h"
#include <iostream>

using namespace std;

vector<Token>& LexAnalysis::getTokenList(string source_file) {
    string context = "";
    FILE* file = fopen(const_cast<char*>(source_file.c_str()), "r");
    if (file) {
        cout<<"File open success"<<endl;
    } else {
        cout<<"File open fail"<<endl;
        return token_list_;
    }
    char ch = ' ';
    while (!feof(file)) {
        ch = fgetc(file);
        context.push_back(ch);
    }
    fclose(file);
    Scanner scanner(context);
    while (true) {
        Token t = scanner.scan();
        token_list_.push_back(t);
        cout<<t.line_<<"\t"<<getLexType(t.lex_)<<"\t"<<t.sem_<<endl;
        if (t.lex_ == ERROR)
            throw exception();
        if (t.lex_ == ENDFILE)
            break;
    }
    return token_list_;
}


void LexAnalysis::printTokenList(string target_file) {
    FILE* file = fopen(const_cast<char*>(target_file.c_str()), "w");
    if (file) {
        cout<<"File open success"<<endl;
    } else {
        cout<<"File open fail"<<endl;
        return;
    }
    for (int i = 0; i < (int)token_list_.size(); i++) {
        fprintf(file, "%d\t%s\t%s\n", token_list_[i].line_, getLexType(token_list_[i].lex_).c_str(), token_list_[i].sem_.c_str());
    }
    fclose(file);
}
