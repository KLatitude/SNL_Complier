#ifndef SEM_ANALYSIS_H_INCLUDED
#define SEM_ANALYSIS_H_INCLUDED

#include "../../Syntax_Analysis/Head/syn_analysis.h"
#include <string>
#include <vector>

class TypeIR;
class FieldChain;
class Var;
class Proc;
class ParamTable;
class SymbolTable;
class Array;

class Array {
public:
    TypeIR* index_ty_ = NULL;
    TypeIR* elem_ty_ = NULL;
    int low = 0, up = 0;

    ~Array() {
        if (index_ty_ != NULL)
            delete index_ty_;
        if (elem_ty_ != NULL)
            delete elem_ty_;
    }
};
class TypeIR {
public:
    int size_ = 0;
    std::string kind_ = "";
    Array* array_attr_ = NULL;
    FieldChain* body_ = NULL;

    ~TypeIR() {
        if (array_attr_ != NULL)
            delete array_attr_;
        if (body_ != NULL)
            delete body_;
    }
};


class FieldChain {
public:
    std::string idname_ = "";
    TypeIR* unit_type_ = NULL;
    int offset_ = 0;
    FieldChain* next_ = NULL;

    ~FieldChain() {
        if (unit_type_ != NULL)
            delete unit_type_;
        if (next_ != NULL)
            delete next_;
    }
};

class Var {
public:
    std::string access_ = "";
    int level_ = -1;
    int off_ = 0;
};

class Proc {
public:
    int level_ = -1;
    ParamTable* param_ = NULL;
    int code_ = 0;
    int size_ = 0;

    ~Proc() {
        /*
        if (param_ != NULL)
            delete param_;
        */
    }
};

class AttributeIR {
public:
    TypeIR* id_type_ = NULL;
    std::string kind_ = "";
    Var* var_attr_ = NULL;
    Proc* proc_attr_ = NULL;

    ~AttributeIR() {
        if (id_type_ != NULL)
            delete id_type_;
        if (var_attr_ != NULL)
            delete var_attr_;
        if (proc_attr_ != NULL)
            delete proc_attr_;
    }
};

class SymbolTable {
public:
    std::string idname_ = "";
    AttributeIR* attrIR_ = new AttributeIR();
    SymbolTable* next_ = NULL;

    ~SymbolTable() {
        if (attrIR_ != NULL)
            delete attrIR_;
        if (next_ != NULL)
            delete next_;
    }
};

class ParamTable {
public:
    SymbolTable* entry_ = NULL;
    ParamTable* next_ = NULL;

    ~ParamTable() {
        if (entry_ != NULL)
            delete entry_;
        if (next_ != NULL)
            delete next_;
    }
};

class SemAnalysis {
private:
    bool error_sign = false;
    std::string error_message = "";
    TypeIR* intPtr = NULL;
    TypeIR* charPtr = NULL;
    TypeIR* boolPtr = NULL;
    int level_ = -1;
    int off_ = 0;
    int init_off_ = 7;
    std::vector<std::vector<SymbolTable*>> rec_sym_table_;
    std::vector<SymbolTable*> scope_;
public:
    void createTable();
    void destroyTable();
    bool enter(std::string id, AttributeIR* attribP, SymbolTable** entry);
    bool findEntry(std::string id, int flag, SymbolTable** entry);
    bool searchTable(std::string id, int current_level, SymbolTable** entry);
    bool findField(std::string id, FieldChain* head, FieldChain** entry);
    void printOneTypeIR(TypeIR* ptr);
    void printAllSymTable();
    void printOneSymTable(SymbolTable* head);

    void analyze(TreeNode* currentP);
    void initialize();
    TypeIR* typeProcess(TreeNode* t, SpecType kind);
    TypeIR* nameType(TreeNode* t);
    TypeIR* arrayType(TreeNode* t);

    TypeIR* recordType(TreeNode* t);
    void typeDecPart(TreeNode* t);
    void varDecList(TreeNode* t);
    void varDecPart(TreeNode* t);
    void procDecPart(TreeNode* t);
    SymbolTable* headProcess(TreeNode* t);
    ParamTable* paraDecList(TreeNode* t);
    void body(TreeNode* t);
    void statement(TreeNode* t);
    TypeIR* expr(TreeNode* t, std::string& kind);
    TypeIR* arrayVar(TreeNode* t);
    TypeIR* arrayVarForRecord(TreeNode* t, FieldChain* entry);
    TypeIR* recordVar(TreeNode* t);
    void assignStatement(TreeNode* t);
    void callStatement(TreeNode* t);
    void ifStatement(TreeNode* t);
    void whileStatement(TreeNode* t);
    void readStatement(TreeNode* t);
    void writeStatement(TreeNode* t);
    void returnStatement(TreeNode* t);
    bool isError();
    void analysisError(TreeNode* t, std::string message, std::string str);
    bool compat(TypeIR* lhs, TypeIR* rhs);
};

#endif // SEM_ANALYSIS_H_INCLUDED
