#include <iostream>
#include "./Lex_Analysis/Head/lex_analysis.h"
#include "./Syntax_Analysis/Head/syn_analysis.h"
#include "./Semantic_Analysis/Head/sem_analysis.h"
using namespace std;

int main() {
    //词法分析
    cout<<"-----Lex Analysis-----"<<endl;
    LexAnalysis lex_analysis;
    vector<Token*> rec;
    try {
        rec = lex_analysis.getTokenList("in.txt");
    } catch (exception) {
        lex_analysis.printTokenList("out.txt");
        return 0;
    }
    lex_analysis.printTokenList("out.txt");
    cout<<endl;


    //语法分析
    cout<<"-----Syn Analysis-----"<<endl;
    SynAnalysis syn_analysis(rec);
    TreeNode* root = NULL;
    try {
        root = syn_analysis.parse();
    } catch (exception) {
        cout<<"Error: There are unnecessary chars in the end!"<<endl;
        return 0;
    }
    if (!syn_analysis.isError())
        syn_analysis.printTree();
    else
        return 0;
    cout<<endl;

    //语义分析
    cout<<"-----Sem Analysis-----"<<endl;
    SemAnalysis sem_analysis;
    sem_analysis.analyze(root);
    if (!sem_analysis.isError())
        sem_analysis.printAllSymTable();
    return 0;
}

