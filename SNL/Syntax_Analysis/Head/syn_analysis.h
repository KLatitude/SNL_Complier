#ifndef SYN_ANALYSIS_H_INCLUDED
#define SYN_ANALYSIS_H_INCLUDED

#include <vector>
#include <string>
#include <stack>
#include "../../Lex_Analysis/Head/lex_analysis.h"

typedef enum {
    Program = 42,
    ProgramHead, ProgramName,
    DeclarePart,
    TypeDecpart, TypeDec, TypeDecList, TypeDecMore, TypeId,
    TypeDef, BaseType, StructureType, ArrayType, Low, Top, RecType, FieldDecList, FieldDecMore, IdList, IdMore,
    VarDecpart, VarDec, VarDecList, VarDecMore, VarIdList, VarIdMore,
    ProcDecpart, ProcDec, ProcDecMore, ProcName,
    ParamList, ParamDecList, ParamMore, Param, FormList, FidMore, ProcDecPart,
    ProcBody, ProgramBody,
    StmList, StmMore, Stm, AssCall,
    AssignmentRest, ConditionalStm, LoopStm, InputStm, Invar, OutputStm, ReturnStm, CallStmRest, ActParamList, ActParamMore,
    RelExp, OtherRelE, Exp, OtherTerm, Term, OtherFactor, Factor, Variable, VariMore, FieldVar, FieldVarMore, CmpOp, AddOp, MultOp

}EleType;

typedef enum {
    ProK, PheadK, TypeK, VarK, ProcDecK, StmLK, DecK, StmtK, ExpK
}TNodeType;

typedef enum {
    ArrayK, CharK, IntegerK, RecordK, IdK,
    IfK, WhileK, AssignK, ReadK, WriteK, CallK, ReturnK,
    OpK, ConstK, VariK
}SpecType;


typedef struct {
    int low, up;
    SpecType child_type;
}ArrayAttr;

typedef enum {
    valparamtype, varparamtype, NoVal
}ParamType;

typedef struct {
    ParamType paramt = NoVal;
}ProcAttr;

typedef enum {
    IdV, ArrayMembV, FieldMembV
}VariType;

typedef enum {
    Void, Integer, Boolean
}ExpType;

typedef struct {
    LexType op;
    int val;
    VariType var_kind;
    ExpType type;
}ExpAttr;

typedef struct {
    ArrayAttr array_attr;
    ProcAttr proc_attr;
    ExpAttr exp_attr;
}TNodeAttr;

class TreeNode {
public:
    std::vector<TreeNode*> child_; //child node
    TreeNode* sibling_ = NULL; //brother node
    int lineno_ = 0; //line number
    TNodeType node_kind_; //Syntax Tree Node's type
    SpecType kind_ = IntegerK; //Syntax Tree Node's specific type
    int idnum_ = 0; //number of identifiers
    std::vector<std::string> name_; //name of identifiers
    //std::vector<> table_;
    std::string type_name_ = ""; //type alias name(IdK)
    TNodeAttr attr_;

    TreeNode() {
        for (int i = 0; i < 3; i++)
            child_.push_back(NULL);
    }
    ~TreeNode() {
        for (int i = 0; i < (int)child_.size(); i++) {
            if (child_[i] != NULL)
                delete child_[i];
        }
        if (sibling_ != NULL)
            delete sibling_;
    }
};


class SynAnalysis {
private:
    bool error_ = false;
    std::stack<TreeNode**> syn_tree_;
    std::stack<int> symbol_;
    std::stack<TreeNode*> operator_;
    std::stack<TreeNode*> oprand_;
    //std::stack<>
    std::vector<std::vector<int> > LL1_table_ = std::vector<std::vector<int>>(67 + 42, std::vector<int>(42, 0));
    TreeNode* root = NULL;
    TreeNode* currentP = NULL;
    TreeNode* saveP = NULL;
    SpecType* temp = NULL;
    bool getExpResult_ = true, getExpResult2_ = false;
    int expflag_ = 0;

    int now_ = 0;
    std::vector<Token*> token_list_;
public:
    SynAnalysis(std::vector<Token*>& token_list) {
        this->token_list_ = token_list;
    }
    ~SynAnalysis() {
        if (root != NULL)
            delete root;
        while (!operator_.empty()) {
            TreeNode* t = operator_.top();
            operator_.pop();
            delete t;
        }
        while (!oprand_.empty()) {
            TreeNode* t = oprand_.top();
            oprand_.pop();
            delete t;
        }
    }

    bool isError();
    int getFromStr(std::string str);
    TreeNode* parse();
    void createLL1Table();
    void predict(int num);
    int priosity(LexType op);
    void printTree();
    void dfs(TreeNode* root, int k);
    void printNode(TreeNode* root, int k);
    void process1();
    void process2();
    void process3();
    void process4();
    void process5();
    void process6();
    void process7();
    void process8();
    void process9();
    void process10();
    void process11();
    void process12();
    void process13();
    void process14();
    void process15();
    void process16();
    void process17();
    void process18();
    void process19();
    void process20();
    void process21();
    void process22();
    void process23();
    void process24();
    void process25();
    void process26();
    void process27();
    void process28();
    void process29();
    void process30();
    void process31();
    void process32();
    void process33();
    void process34();
    void process35();
    void process36();
    void process37();
    void process38();
    void process39();
    void process40();
    void process41();
    void process42();
    void process43();
    void process44();
    void process45();
    void process46();
    void process47();
    void process48();
    void process49();
    void process50();
    void process51();
    void process52();
    void process53();
    void process54();
    void process55();
    void process56();
    void process57();
    void process58();
    void process59();
    void process60();
    void process61();
    void process62();
    void process63();
    void process64();
    void process65();
    void process66();
    void process67();
    void process68();
    void process69();
    void process70();
    void process71();
    void process72();
    void process73();
    void process74();
    void process75();
    void process76();
    void process77();
    void process78();
    void process79();
    void process80();
    void process81();
    void process82();
    void process83();
    void process84();
    void process85();
    void process86();
    void process87();
    void process88();
    void process89();
    void process90();
    void process91();
    void process92();
    void process93();
    void process94();
    void process95();
    void process96();
    void process97();
    void process98();
    void process99();
    void process100();
    void process101();
    void process102();
    void process103();
    void process104();
};


#endif // SYN_ANALYSIS_H_INCLUDED
