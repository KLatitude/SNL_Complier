#include "../Head/syn_analysis.h"
#include<string>
#include<iostream>
using namespace std;

bool SynAnalysis::isError() {
    return error_;
}

void SynAnalysis::printNode(TreeNode* root, int k) {
    for (int i = 1; i <= k; i++)
        cout<<"\t";
    if (root->node_kind_ == ProK) {
        cout<<"ProK"<<endl;
        return;
    }
    if (root->node_kind_ == PheadK) {
        cout<<"PheadK "<<root->name_[0]<<endl;
        return;
    }
    if (root->node_kind_ == TypeK) {
        cout<<"TypeK"<<endl;
        return;
    }
    if (root->node_kind_ == VarK) {
        cout<<"VarK"<<endl;
        return;
    }
    if (root->node_kind_ == ProcDecK) {
        cout<<"ProcDecK"<<endl;
        return;
    }
    if (root->node_kind_ == StmLK) {
        cout<<"StmLK"<<endl;
        return;
    }
    if (root->node_kind_ == DecK) {
        cout<<"DecK ";
        if (root->attr_.proc_attr.paramt != NoVal) {
            switch ((int)root->attr_.proc_attr.paramt) {
                case valparamtype: cout<<"valparamtype: "; break;
                case varparamtype: cout<<"varparamtype: "; break;
            }
        }
        if (root->kind_ == IntegerK) {
            cout<<"IntegerK";
            for (int i = 0; i < root->idnum_; i++)
                cout<<" "<<root->name_[i];
            cout<<endl;
            return;
        }
        if (root->kind_ == CharK) {
            cout<<"CharK";
            for (int i = 0; i < root->idnum_; i++)
                cout<<" "<<root->name_[i];
            cout<<endl;
            return;
        }
        if (root->kind_ == ArrayK) {
            cout<<"ArrayK ";
            cout<<root->attr_.array_attr.low<<" "<<root->attr_.array_attr.up<<" ";
            switch((int)root->attr_.array_attr.child_type) {
                case IntegerK: cout<<"IntegerK"<<endl; break;
                case CharK: cout<<"CharK"<<endl; break;
            }
            return;
        }
        if (root->kind_ == RecordK) {
            cout<<"RecordK"<<endl;
            return;
        }
        if (root->kind_ == IdK) {
            cout<<"IdK "<<root->type_name_;
            for (int i = 0; i < root->idnum_; i++)
                cout<<" "<<root->name_[i];
            cout<<endl;
            return;
        }
    }
    if (root->node_kind_ == StmtK) {
        cout<<"StmtK ";
        switch ((int)(root->kind_)) {
            case IfK: cout<<"IfK"<<endl; break;
            case WhileK: cout<<"WhileK"<<endl; break;
            case AssignK: cout<<"AssignK"<<endl; break;
            case ReadK: cout<<"ReadK "<<root->name_[0]<<endl; break;
            case WriteK: cout<<"WriteK"<<endl; break;
            case CallK: cout<<"CallK"<<endl; break;
            case ReturnK: cout<<"ReturnK"<<endl; break;
        }
    }
    if (root->node_kind_ == ExpK) {
        cout<<"ExpK ";
        if (root->kind_ == OpK) {
            cout<<"OpK ";
            switch((int)root->attr_.exp_attr.op) {
                case LT: cout<<"<"<<endl; break;
                case EQ: cout<<"="<<endl; break;
                case ADD: cout<<"+"<<endl; break;
                case MINUS: cout<<"-"<<endl; break;
                case TIMES: cout<<"*"<<endl; break;
                case DIV: cout<<"/"<<endl; break;
            }
            return;
        }
        if (root->kind_ == ConstK) {
            cout<<"ConstK "<<root->attr_.exp_attr.val<<endl;
            return;
        }
        if (root->kind_ == VariK) {
            cout<<"VariK "<<root->name_[0]<<" ";
            switch ((int)root->attr_.exp_attr.var_kind) {
                case IdV: cout<<"IdV"<<endl; break;
                case ArrayMembV: cout<<"ArrayMembV"<<endl; break;
                case FieldMembV: cout<<"FieldMembV"<<endl; break;
            }
        }
    }
}

void SynAnalysis::dfs(TreeNode* root, int k) {
    if (root == NULL)
        return;
    printNode(root, k);
    for (int i = 0; i < (int)root->child_.size(); i++)
        dfs(root->child_[i], k + 1);
    dfs(root->sibling_, k);
    return;
}

void SynAnalysis::printTree() {
    dfs(root, 0);
}

int SynAnalysis::getFromStr(string str) {
    int sum = 0;
    for (int i = 0; i < (int)str.length(); i++)
        sum = sum * 10 + (int)(str[i] - '0');
    return sum;
}

TreeNode* SynAnalysis::parse() {
    createLL1Table();
    symbol_.push(Program);
    root = new TreeNode();
    root->node_kind_ = ProK;
    for (int i = 2; i >= 0; i--)
        syn_tree_.push(&(root->child_[i]));
    while (!symbol_.empty()) {
        int top = symbol_.top();
        symbol_.pop();
        if (top == token_list_[now_]->lex_) {
            now_++;
        } else {
            int pnum = LL1_table_[top][token_list_[now_]->lex_];
            if (pnum > 0)
                predict(pnum);
            else {
                //cout<<top<<endl;
                cout<<"Error: In: "<<token_list_[now_]->line_<<endl;
                if (token_list_[now_]->lex_ != ENDFILE)
                    cout<<"\tNo: "<<token_list_[now_]->sem_<<endl;
                else
                    cout<<"\tNo: ENDFILE"<<endl;
                cout<<"\tPossible Except:";
                if (top < 42)
                    cout<<" "<<getLexType((LexType)top);
                for (int i = 0; i < (int)LL1_table_[0].size(); i++) {
                    if (LL1_table_[top][i] != 0)
                        cout<<" "<<getLexType((LexType)i);
                }
                cout<<endl;
                error_ = true;
            }
        }

    }
    if (token_list_[now_]->lex_ != ENDFILE)
        throw exception();
    return root;
}

void SynAnalysis::createLL1Table() {
    LL1_table_[Program][PROGRAM] = 1;

    LL1_table_[ProgramHead][PROGRAM] = 2;

    LL1_table_[ProgramName][ID] = 3;

    LL1_table_[DeclarePart][TYPE] = 4;
    LL1_table_[DeclarePart][VAR] = 4;
    LL1_table_[DeclarePart][PROCEDURE] = 4;
    LL1_table_[DeclarePart][BEGIN] = 4;

    LL1_table_[TypeDecpart][VAR] = 5;
    LL1_table_[TypeDecpart][PROCEDURE] = 5;
    LL1_table_[TypeDecpart][BEGIN] = 5;
    LL1_table_[TypeDecpart][TYPE] = 6;

    LL1_table_[TypeDec][TYPE] = 7;

    LL1_table_[TypeDecList][ID] = 8;

    LL1_table_[TypeDecMore][VAR] = 9;
    LL1_table_[TypeDecMore][PROCEDURE] = 9;
    LL1_table_[TypeDecMore][BEGIN] = 9;
    LL1_table_[TypeDecMore][ID] = 10;

    LL1_table_[TypeId][ID] = 11;

    LL1_table_[TypeDef][INTEGER] = 12;
    LL1_table_[TypeDef][CHAR] = 12;
    LL1_table_[TypeDef][ARRAY] = 13;
    LL1_table_[TypeDef][RECORD] = 13;
    LL1_table_[TypeDef][ID] = 14;

    LL1_table_[BaseType][INTEGER] = 15;
    LL1_table_[BaseType][CHAR] = 16;

    LL1_table_[StructureType][ARRAY] = 17;
    LL1_table_[StructureType][RECORD] = 18;

    LL1_table_[ArrayType][ARRAY] = 19;

    LL1_table_[Low][INTC] = 20;

    LL1_table_[Top][INTC] = 21;

    LL1_table_[RecType][RECORD] = 22;

    LL1_table_[FieldDecList][INTEGER] = 23;
    LL1_table_[FieldDecList][CHAR] = 23;
    LL1_table_[FieldDecList][ARRAY] = 24;

    LL1_table_[FieldDecMore][END] = 25;
    LL1_table_[FieldDecMore][INTEGER] = 26;
    LL1_table_[FieldDecMore][CHAR] = 26;
    LL1_table_[FieldDecMore][ARRAY] = 26;

    LL1_table_[IdList][ID] = 27;

    LL1_table_[IdMore][SEMI] = 28;
    LL1_table_[IdMore][COMMA] = 29;

    LL1_table_[VarDecpart][PROCEDURE] = 30;
    LL1_table_[VarDecpart][BEGIN] = 30;
    LL1_table_[VarDecpart][VAR] = 31;

    LL1_table_[VarDec][VAR] = 32;

    LL1_table_[VarDecList][INTEGER] = 33;
    LL1_table_[VarDecList][CHAR] = 33;
    LL1_table_[VarDecList][ARRAY] = 33;
    LL1_table_[VarDecList][RECORD] = 33;
    LL1_table_[VarDecList][ID] = 33;

    LL1_table_[VarDecMore][PROCEDURE] = 34;
    LL1_table_[VarDecMore][BEGIN] = 34;
    LL1_table_[VarDecMore][INTEGER] = 35;
    LL1_table_[VarDecMore][CHAR] = 35;
    LL1_table_[VarDecMore][ARRAY] = 35;
    LL1_table_[VarDecMore][RECORD] = 35;
    LL1_table_[VarDecMore][ID] = 35;

    LL1_table_[VarIdList][ID] = 36;

    LL1_table_[VarIdMore][SEMI] = 37;
    LL1_table_[VarIdMore][COMMA] = 38;

    LL1_table_[ProcDecpart][BEGIN] = 39;
    LL1_table_[ProcDecpart][PROCEDURE] = 40;

    LL1_table_[ProcDec][PROCEDURE] = 41;

    LL1_table_[ProcDecMore][BEGIN] = 42;
    LL1_table_[ProcDecMore][PROCEDURE] = 43;

    LL1_table_[ProcName][ID] = 44;

    LL1_table_[ParamList][RPAREN] = 45;
    LL1_table_[ParamList][INTEGER] = 46;
    LL1_table_[ParamList][CHAR] = 46;
    LL1_table_[ParamList][ARRAY] = 46;
    LL1_table_[ParamList][RECORD] = 46;
    LL1_table_[ParamList][ID] = 46;
    LL1_table_[ParamList][VAR] = 46;

    LL1_table_[ParamDecList][INTEGER] = 47;
    LL1_table_[ParamDecList][CHAR] = 47;
    LL1_table_[ParamDecList][ARRAY] = 47;
    LL1_table_[ParamDecList][RECORD] = 47;
    LL1_table_[ParamDecList][ID] = 47;
    LL1_table_[ParamDecList][VAR] = 47;

    LL1_table_[ParamMore][RPAREN] = 48;
    LL1_table_[ParamMore][SEMI] = 49;


    LL1_table_[Param][INTEGER] = 50;
    LL1_table_[Param][CHAR] = 50;
    LL1_table_[Param][ARRAY] = 50;
    LL1_table_[Param][RECORD] = 50;
    LL1_table_[Param][ID] = 50;
    LL1_table_[Param][VAR] = 51;

    LL1_table_[FormList][ID] = 52;

    LL1_table_[FidMore][SEMI] = 53;
    LL1_table_[FidMore][RPAREN] = 53;
    LL1_table_[FidMore][COMMA] = 54;

    LL1_table_[ProcDecPart][TYPE] = 55;
    LL1_table_[ProcDecPart][VAR] = 55;
    LL1_table_[ProcDecPart][PROCEDURE] = 55;
    LL1_table_[ProcDecPart][BEGIN] = 55;

    LL1_table_[ProcBody][BEGIN] = 56;

    LL1_table_[ProgramBody][BEGIN] = 57;

    LL1_table_[StmList][ID] = 58;
    LL1_table_[StmList][IF] = 58;
    LL1_table_[StmList][WHILE] = 58;
    LL1_table_[StmList][RETURN] = 58;
    LL1_table_[StmList][READ] = 58;
    LL1_table_[StmList][WRITE] = 58;

    LL1_table_[StmMore][ELSE] = 59;
    LL1_table_[StmMore][FI] = 59;
    LL1_table_[StmMore][END] = 59;
    LL1_table_[StmMore][ENDWH] = 59;
    LL1_table_[StmMore][SEMI] = 60;

    LL1_table_[Stm][IF] = 61;
    LL1_table_[Stm][WHILE] = 62;
    LL1_table_[Stm][READ] = 63;
    LL1_table_[Stm][WRITE] = 64;
    LL1_table_[Stm][RETURN] = 65;
    LL1_table_[Stm][ID] = 66;

    LL1_table_[AssCall][LMIDPAREN] = 67;
    LL1_table_[AssCall][DOT] = 67;
    LL1_table_[AssCall][ASSIGN] = 67;
    LL1_table_[AssCall][LPAREN] = 68;

    LL1_table_[AssignmentRest][LMIDPAREN] = 69;
    LL1_table_[AssignmentRest][DOT] = 69;
    LL1_table_[AssignmentRest][ASSIGN] = 69;

    LL1_table_[ConditionalStm][IF] = 70;

    LL1_table_[LoopStm][WHILE] = 71;

    LL1_table_[InputStm][READ] = 72;

    LL1_table_[Invar][ID] = 73;

    LL1_table_[OutputStm][WRITE] = 74;

    LL1_table_[ReturnStm][RETURN] = 75;

    LL1_table_[CallStmRest][LPAREN] = 76;

    LL1_table_[ActParamList][RPAREN] = 77;
    LL1_table_[ActParamList][LPAREN] = 78;
    LL1_table_[ActParamList][INTC] = 78;
    LL1_table_[ActParamList][ID] = 78;

    LL1_table_[ActParamMore][RPAREN] = 79;
    LL1_table_[ActParamMore][COMMA] = 80;

    LL1_table_[RelExp][LPAREN] = 81;
    LL1_table_[RelExp][INTC] = 81;
    LL1_table_[RelExp][ID] = 81;

    LL1_table_[OtherRelE][LT] = 82;
    LL1_table_[OtherRelE][EQ] = 82;

    LL1_table_[Exp][LPAREN] = 83;
    LL1_table_[Exp][INTC] = 83;
    LL1_table_[Exp][ID] = 83;

    LL1_table_[OtherTerm][LT] = 84;
    LL1_table_[OtherTerm][EQ] = 84;
    LL1_table_[OtherTerm][RMIDPAREN] = 84;
    LL1_table_[OtherTerm][THEN] = 84;
    LL1_table_[OtherTerm][ELSE] = 84;
    LL1_table_[OtherTerm][FI] = 84;
    LL1_table_[OtherTerm][DO] = 84;
    LL1_table_[OtherTerm][ENDWH] = 84;
    LL1_table_[OtherTerm][RPAREN] = 84;
    LL1_table_[OtherTerm][END] = 84;
    LL1_table_[OtherTerm][SEMI] = 84;
    LL1_table_[OtherTerm][COMMA] = 84;
    LL1_table_[OtherTerm][ADD] = 85;
    LL1_table_[OtherTerm][MINUS] = 85;

    LL1_table_[Term][LPAREN] = 86;
    LL1_table_[Term][INTC] = 86;
    LL1_table_[Term][ID] = 86;

    LL1_table_[OtherFactor][ADD] = 87;
    LL1_table_[OtherFactor][MINUS] = 87;
    LL1_table_[OtherFactor][LT] = 87;
    LL1_table_[OtherFactor][EQ] = 87;
    LL1_table_[OtherFactor][RMIDPAREN] = 87;
    LL1_table_[OtherFactor][THEN] = 87;
    LL1_table_[OtherFactor][ELSE] = 87;
    LL1_table_[OtherFactor][FI] = 87;
    LL1_table_[OtherFactor][DO] = 87;
    LL1_table_[OtherFactor][ENDWH] = 87;
    LL1_table_[OtherFactor][RPAREN] = 87;
    LL1_table_[OtherFactor][END] = 87;
    LL1_table_[OtherFactor][SEMI] = 87;
    LL1_table_[OtherFactor][COMMA] = 87;
    LL1_table_[OtherFactor][TIMES] = 88;
    LL1_table_[OtherFactor][DIV] = 88;

    LL1_table_[Factor][LPAREN] = 89;
    LL1_table_[Factor][INTC] = 90;
    LL1_table_[Factor][ID] = 91;

    LL1_table_[Variable][ID] = 92;

    LL1_table_[VariMore][ASSIGN] = 93;
    LL1_table_[VariMore][TIMES] = 93;
    LL1_table_[VariMore][DIV] = 93;
    LL1_table_[VariMore][ADD] = 93;
    LL1_table_[VariMore][MINUS] = 93;
    LL1_table_[VariMore][LT] = 93;
    LL1_table_[VariMore][EQ] = 93;
    LL1_table_[VariMore][RMIDPAREN] = 93;
    LL1_table_[VariMore][THEN] = 93;
    LL1_table_[VariMore][ELSE] = 93;
    LL1_table_[VariMore][FI] = 93;
    LL1_table_[VariMore][DO] = 93;
    LL1_table_[VariMore][ENDWH] = 93;
    LL1_table_[VariMore][RPAREN] = 93;
    LL1_table_[VariMore][END] = 93;
    LL1_table_[VariMore][SEMI] = 93;
    LL1_table_[VariMore][COMMA] = 93;
    LL1_table_[VariMore][LMIDPAREN] = 94;
    LL1_table_[VariMore][DOT] = 95;

    LL1_table_[FieldVar][ID] = 96;

    LL1_table_[FieldVarMore][ASSIGN] = 97;
    LL1_table_[FieldVarMore][TIMES] = 97;
    LL1_table_[FieldVarMore][DIV] = 97;
    LL1_table_[FieldVarMore][ADD] = 97;
    LL1_table_[FieldVarMore][MINUS] = 97;
    LL1_table_[FieldVarMore][LT] = 97;
    LL1_table_[FieldVarMore][EQ] = 97;
    LL1_table_[FieldVarMore][RMIDPAREN] = 97;
    LL1_table_[FieldVarMore][THEN] = 97;
    LL1_table_[FieldVarMore][ELSE] = 97;
    LL1_table_[FieldVarMore][FI] = 97;
    LL1_table_[FieldVarMore][DO] = 97;
    LL1_table_[FieldVarMore][ENDWH] = 97;
    LL1_table_[FieldVarMore][RPAREN] = 97;
    LL1_table_[FieldVarMore][END] = 97;
    LL1_table_[FieldVarMore][SEMI] = 97;
    LL1_table_[FieldVarMore][COMMA] = 97;
    LL1_table_[FieldVarMore][LMIDPAREN] = 98;

    LL1_table_[CmpOp][LT] = 99;
    LL1_table_[CmpOp][EQ] = 100;

    LL1_table_[AddOp][ADD] = 101;
    LL1_table_[AddOp][MINUS] = 102;

    LL1_table_[MultOp][TIMES] = 103;
    LL1_table_[MultOp][DIV] = 104;
}

int SynAnalysis::priosity(LexType op) { //parameter is string? (unknown)
    if (op == TIMES || op == DIV)
        return 3;
    if (op == ADD || op == MINUS)
        return 2;
    if (op == LT || op == EQ)
        return 1;
    if (op == END || op == LPAREN) //add (op == LPAREN)
        return 0;
    return -1;
}

void SynAnalysis::predict(int num) {
    switch(num) {
        case 1: process1(); break;
        case 2: process2(); break;
        case 3: process3(); break;
        case 4: process4(); break;
        case 5: process5(); break;
        case 6: process6(); break;
        case 7: process7(); break;
        case 8: process8(); break;
        case 9: process9(); break;
        case 10: process10(); break;
        case 11: process11(); break;
        case 12: process12(); break;
        case 13: process13(); break;
        case 14: process14(); break;
        case 15: process15(); break;
        case 16: process16(); break;
        case 17: process17(); break;
        case 18: process18(); break;
        case 19: process19(); break;
        case 20: process20(); break;
        case 21: process21(); break;
        case 22: process22(); break;
        case 23: process23(); break;
        case 24: process24(); break;
        case 25: process25(); break;
        case 26: process26(); break;
        case 27: process27(); break;
        case 28: process28(); break;
        case 29: process29(); break;
        case 30: process30(); break;
        case 31: process31(); break;
        case 32: process32(); break;
        case 33: process33(); break;
        case 34: process34(); break;
        case 35: process35(); break;
        case 36: process36(); break;
        case 37: process37(); break;
        case 38: process38(); break;
        case 39: process39(); break;
        case 40: process40(); break;
        case 41: process41(); break;
        case 42: process42(); break;
        case 43: process43(); break;
        case 44: process44(); break;
        case 45: process45(); break;
        case 46: process46(); break;
        case 47: process47(); break;
        case 48: process48(); break;
        case 49: process49(); break;
        case 50: process50(); break;
        case 51: process51(); break;
        case 52: process52(); break;
        case 53: process53(); break;
        case 54: process54(); break;
        case 55: process55(); break;
        case 56: process56(); break;
        case 57: process57(); break;
        case 58: process58(); break;
        case 59: process59(); break;
        case 60: process60(); break;
        case 61: process61(); break;
        case 62: process62(); break;
        case 63: process63(); break;
        case 64: process64(); break;
        case 65: process65(); break;
        case 66: process66(); break;
        case 67: process67(); break;
        case 68: process68(); break;
        case 69: process69(); break;
        case 70: process70(); break;
        case 71: process71(); break;
        case 72: process72(); break;
        case 73: process73(); break;
        case 74: process74(); break;
        case 75: process75(); break;
        case 76: process76(); break;
        case 77: process77(); break;
        case 78: process78(); break;
        case 79: process79(); break;
        case 80: process80(); break;
        case 81: process81(); break;
        case 82: process82(); break;
        case 83: process83(); break;
        case 84: process84(); break;
        case 85: process85(); break;
        case 86: process86(); break;
        case 87: process87(); break;
        case 88: process88(); break;
        case 89: process89(); break;
        case 90: process90(); break;
        case 91: process91(); break;
        case 92: process92(); break;
        case 93: process93(); break;
        case 94: process94(); break;
        case 95: process95(); break;
        case 96: process96(); break;
        case 97: process97(); break;
        case 98: process98(); break;
        case 99: process99(); break;
        case 100: process100(); break;
        case 101: process101(); break;
        case 102: process102(); break;
        case 103: process103(); break;
        case 104: process104(); break;
        default: break;
    }
}


void SynAnalysis::process1() {
    symbol_.push(DOT);
    symbol_.push(ProgramBody);
    symbol_.push(DeclarePart);
    symbol_.push(ProgramHead);
    return;
}

void SynAnalysis::process2() {
    symbol_.push(ProgramName);
    symbol_.push(PROGRAM);
    currentP = new TreeNode();
    currentP->node_kind_ = PheadK;
    currentP->lineno_ = token_list_[now_]->line_;
    TreeNode** t = syn_tree_.top();
    syn_tree_.pop();
    *t = currentP;
    return;
}
void SynAnalysis::process3() {
    symbol_.push(ID);
    currentP->idnum_++;
    currentP->name_.push_back(token_list_[now_]->sem_);
    return;
}
void SynAnalysis::process4() {
    symbol_.push(ProcDecpart);
    symbol_.push(VarDecpart);
    symbol_.push(TypeDecpart);
    return;
}
void SynAnalysis::process5() {
    return;
}
void SynAnalysis::process6() {
    symbol_.push(TypeDec);
    return;
}
void SynAnalysis::process7() {
    symbol_.push(TypeDecList);
    symbol_.push(TYPE);
    currentP = new TreeNode();
    currentP->node_kind_ = TypeK;
    currentP->lineno_ = token_list_[now_]->line_;
    TreeNode** t = syn_tree_.top();
    syn_tree_.pop();
    *t = currentP;
    syn_tree_.push(&(currentP->sibling_));
    //currentP->child_.push_back(NULL);
    syn_tree_.push(&(currentP->child_[0]));
    return;
}
void SynAnalysis::process8() {
    symbol_.push(TypeDecMore);
    symbol_.push(SEMI);
    symbol_.push(TypeDef);
    symbol_.push(EQ);
    symbol_.push(TypeId);
    currentP = new TreeNode();
    currentP->node_kind_ = DecK;
    currentP->lineno_ = token_list_[now_]->line_;
    TreeNode** t = syn_tree_.top();
    syn_tree_.pop();
    *t = currentP;
    syn_tree_.push(&(currentP->sibling_));
    return;
}
void SynAnalysis::process9() {
    syn_tree_.pop();
    return;
}
void SynAnalysis::process10() {
    symbol_.push(TypeDecList);
    return;
}
void SynAnalysis::process11() {
    symbol_.push(ID);
    currentP->name_.push_back(token_list_[now_]->sem_);
    currentP->idnum_++;
    return;
}
void SynAnalysis::process12() {
    symbol_.push(BaseType);
    temp = &(currentP->kind_);
    return;
}
void SynAnalysis::process13() {
    symbol_.push(StructureType);
    return;
}
void SynAnalysis::process14() {
    symbol_.push(ID);
    currentP->kind_ = IdK;
    currentP->type_name_ = token_list_[now_]->sem_;
    return;
}
void SynAnalysis::process15() {
    symbol_.push(INTEGER);
    *temp = IntegerK;
    temp = NULL;
    return;
}
void SynAnalysis::process16() {
    symbol_.push(CHAR);
    *temp = CharK;
    temp = NULL;
    return;
}
void SynAnalysis::process17() {
    symbol_.push(ArrayType);
    return;
}
void SynAnalysis::process18() {
    symbol_.push(RecType);
    return;
}
void SynAnalysis::process19() {
    symbol_.push(BaseType);
    symbol_.push(OF);
    symbol_.push(RMIDPAREN);
    symbol_.push(Top);
    symbol_.push(UNDERANGE);
    symbol_.push(Low);
    symbol_.push(LMIDPAREN);
    symbol_.push(ARRAY);
    currentP->kind_ = ArrayK;
    temp = &(currentP->attr_.array_attr.child_type);
    return;
}
void SynAnalysis::process20() {
    symbol_.push(INTC);
    currentP->attr_.array_attr.low = getFromStr(token_list_[now_]->sem_);
    return;
}
void SynAnalysis::process21() {
    symbol_.push(INTC);
    currentP->attr_.array_attr.up = getFromStr(token_list_[now_]->sem_);
    return;
}
void SynAnalysis::process22() {
    symbol_.push(END);
    symbol_.push(FieldDecList);
    symbol_.push(RECORD);
    currentP->kind_ = RecordK;
    saveP = currentP;
    //currentP->child_.push_back(NULL);
    syn_tree_.push(&(currentP->child_[0]));
    return;
}
void SynAnalysis::process23() {
    symbol_.push(FieldDecMore);
    symbol_.push(SEMI);
    symbol_.push(IdList);
    symbol_.push(BaseType);
    currentP = new TreeNode();
    currentP->node_kind_ = DecK;
    currentP->lineno_ = token_list_[now_]->line_;
    TreeNode** t = syn_tree_.top();
    syn_tree_.pop();
    *t = currentP;
    temp = &(currentP->kind_);
    syn_tree_.push(&(currentP->sibling_));
    return;
}
void SynAnalysis::process24() {
    symbol_.push(FieldDecMore);
    symbol_.push(SEMI);
    symbol_.push(IdList);
    symbol_.push(ArrayType);
    currentP = new TreeNode();
    currentP->node_kind_ = DecK;
    currentP->lineno_ = token_list_[now_]->line_;
    TreeNode** t = syn_tree_.top();
    syn_tree_.pop();
    *t = currentP;
    syn_tree_.push(&(currentP->sibling_));
    return;
}
void SynAnalysis::process25() {
    syn_tree_.pop();
    currentP = saveP;
    return;
}
void SynAnalysis::process26() {
    symbol_.push(FieldDecList);
    return;
}
void SynAnalysis::process27() {
    symbol_.push(IdMore);
    symbol_.push(ID);
    currentP->idnum_++;
    currentP->name_.push_back(token_list_[now_]->sem_);
    return;
}
void SynAnalysis::process28() {
    return;
}
void SynAnalysis::process29() {
    symbol_.push(IdList);
    symbol_.push(COMMA);
    return;
}
void SynAnalysis::process30() {
    return;
}
void SynAnalysis::process31() {
    symbol_.push(VarDec);
    return;
}
void SynAnalysis::process32() {
    symbol_.push(VarDecList);
    symbol_.push(VAR);
    currentP = new TreeNode();
    currentP->node_kind_ = VarK;
    currentP->lineno_ = token_list_[now_]->line_;
    TreeNode** t = syn_tree_.top();
    syn_tree_.pop();
    *t = currentP;
    syn_tree_.push(&(currentP->sibling_));
    //currentP->child_.push_back(NULL);
    syn_tree_.push(&(currentP->child_[0]));
    return;
}
void SynAnalysis::process33() {
    symbol_.push(VarDecMore);
    symbol_.push(SEMI);
    symbol_.push(VarIdList);
    symbol_.push(TypeDef);
    currentP = new TreeNode();
    currentP->node_kind_ = DecK;
    currentP->lineno_ = token_list_[now_]->line_;
    TreeNode** t = syn_tree_.top();
    syn_tree_.pop();
    *t = currentP;
    syn_tree_.push(&(currentP->sibling_));
    return;
}
void SynAnalysis::process34() {
    syn_tree_.pop();
    return;
}
void SynAnalysis::process35() {
    symbol_.push(VarDecList);
    return;
}
void SynAnalysis::process36() {
    symbol_.push(VarIdMore);
    symbol_.push(ID);
    currentP->idnum_++;
    currentP->name_.push_back(token_list_[now_]->sem_);
    return;
}
void SynAnalysis::process37() {
    return;
}
void SynAnalysis::process38() {
    symbol_.push(VarIdList);
    symbol_.push(COMMA);
    return;
}
void SynAnalysis::process39() {
    //syn_tree_.pop(); //my add
    return;
}
void SynAnalysis::process40() {
    symbol_.push(ProcDec);
    return;
}
void SynAnalysis::process41() {
    symbol_.push(ProcDecMore);
    symbol_.push(ProcBody);
    symbol_.push(ProcDecPart);
    symbol_.push(SEMI);
    symbol_.push(RPAREN);
    symbol_.push(ParamList);
    symbol_.push(LPAREN);
    symbol_.push(ProcName);
    symbol_.push(PROCEDURE);
    currentP = new TreeNode();
    currentP->node_kind_ = ProcDecK;
    currentP->lineno_ = token_list_[now_]->line_;
    TreeNode** t = syn_tree_.top();
    syn_tree_.pop();
    *t = currentP;
    syn_tree_.push(&(currentP->sibling_));
//    for (int i = 0; i < 3; i++)
//        currentP->child_.push_back(NULL);
    for (int i = 2; i >= 0; i--)
        syn_tree_.push(&(currentP->child_[i]));
    return;
}
void SynAnalysis::process42() {
    //syn_tree_.pop(); //my add
    return;
}
void SynAnalysis::process43() {
    symbol_.push(ProcDec);
    return;
}
void SynAnalysis::process44() {
    symbol_.push(ID);
    currentP->idnum_++;
    currentP->name_.push_back(token_list_[now_]->sem_);
    return;
}
void SynAnalysis::process45() {
    syn_tree_.pop();
    return;
}
void SynAnalysis::process46() {
    symbol_.push(ParamDecList);
    return;
}
void SynAnalysis::process47() {
    symbol_.push(ParamMore);
    symbol_.push(Param);
    return;
}
void SynAnalysis::process48() {
    syn_tree_.pop(); //my modify
    return;
}
void SynAnalysis::process49() {
    symbol_.push(ParamDecList);
    symbol_.push(SEMI);
    return;
}
void SynAnalysis::process50() {
    symbol_.push(FormList);
    symbol_.push(TypeDef);
    currentP = new TreeNode();
    currentP->node_kind_ = DecK;
    currentP->lineno_ = token_list_[now_]->line_;
    currentP->attr_.proc_attr.paramt = valparamtype;
    TreeNode** t = syn_tree_.top();
    syn_tree_.pop();
    *t = currentP;
    syn_tree_.push(&(currentP->sibling_));
    return;
}
void SynAnalysis::process51() {
    symbol_.push(FormList);
    symbol_.push(TypeDef);
    symbol_.push(VAR);
    currentP = new TreeNode();
    currentP->node_kind_ = DecK;
    currentP->lineno_ = token_list_[now_]->line_;
    currentP->attr_.proc_attr.paramt = varparamtype;
    TreeNode** t = syn_tree_.top();
    syn_tree_.pop();
    *t = currentP;
    syn_tree_.push(&(currentP->sibling_));
    return;
}
void SynAnalysis::process52() {
    symbol_.push(FidMore);
    symbol_.push(ID);
    currentP->name_.push_back(token_list_[now_]->sem_);
    currentP->idnum_++;
    return;
}
void SynAnalysis::process53() {
    return;
}
void SynAnalysis::process54() {
    symbol_.push(FormList);
    symbol_.push(COMMA);
    return;
}
void SynAnalysis::process55() {
    symbol_.push(DeclarePart);
    return;
}
void SynAnalysis::process56() {
    symbol_.push(ProgramBody);
    return;
}
void SynAnalysis::process57() {
    symbol_.push(END);
    symbol_.push(StmList);
    symbol_.push(BEGIN);
    syn_tree_.pop();
    currentP = new TreeNode();
    currentP->node_kind_ = StmLK;
    currentP->lineno_ = token_list_[now_]->line_;
    TreeNode** t = syn_tree_.top();
    syn_tree_.pop();
    *t = currentP;
//    currentP->child_.push_back(NULL);
    syn_tree_.push(&(currentP->child_[0]));
    return;
}
void SynAnalysis::process58() {
    symbol_.push(StmMore);
    symbol_.push(Stm);
    return;
}
void SynAnalysis::process59() {
    syn_tree_.pop();
    return;
}
void SynAnalysis::process60() {
    symbol_.push(StmList);
    symbol_.push(SEMI);
    return;
}
void SynAnalysis::process61() {
    symbol_.push(ConditionalStm);
    currentP = new TreeNode();
    currentP->lineno_ = token_list_[now_]->line_;
    currentP->node_kind_ = StmtK;
    currentP->kind_ = IfK;
    TreeNode** t = syn_tree_.top();
    syn_tree_.pop();
    *t = currentP;
    syn_tree_.push(&(currentP->sibling_));
    return;
}
void SynAnalysis::process62() {
    symbol_.push(LoopStm);
    currentP = new TreeNode();
    currentP->lineno_ = token_list_[now_]->line_;
    currentP->node_kind_ = StmtK;
    currentP->kind_ = WhileK;
    TreeNode** t = syn_tree_.top();
    syn_tree_.pop();
    *t = currentP;
    syn_tree_.push(&(currentP->sibling_));
    return;
}
void SynAnalysis::process63() {
    symbol_.push(InputStm);
    currentP = new TreeNode();
    currentP->lineno_ = token_list_[now_]->line_;
    currentP->node_kind_ = StmtK;
    currentP->kind_ = ReadK;
    TreeNode** t = syn_tree_.top();
    syn_tree_.pop();
    *t = currentP;
    syn_tree_.push(&(currentP->sibling_));
    return;
}
void SynAnalysis::process64() {
    symbol_.push(OutputStm);
    currentP = new TreeNode();
    currentP->lineno_ = token_list_[now_]->line_;
    currentP->node_kind_ = StmtK;
    currentP->kind_ = WriteK;
    TreeNode** t = syn_tree_.top();
    syn_tree_.pop();
    *t = currentP;
    syn_tree_.push(&(currentP->sibling_));
    return;
}
void SynAnalysis::process65() {
    symbol_.push(ReturnStm);
    currentP = new TreeNode();
    currentP->lineno_ = token_list_[now_]->line_;
    currentP->node_kind_ = StmtK;
    currentP->kind_ = ReturnK;
    TreeNode** t = syn_tree_.top();
    syn_tree_.pop();
    *t = currentP;
    syn_tree_.push(&(currentP->sibling_));
    return;
}
void SynAnalysis::process66() {
    symbol_.push(AssCall);
    symbol_.push(ID);
    currentP = new TreeNode();
    currentP->node_kind_ = StmtK;
    currentP->lineno_ = token_list_[now_]->line_; //it probably has a mistake, example for `a \n := \n b` (not solved)
    TreeNode* t = new TreeNode();
    t->lineno_ = token_list_[now_]->line_;
    t->node_kind_ = ExpK;
    t->kind_ = VariK;
    t->name_.push_back(token_list_[now_]->sem_);
    t->idnum_++;
    currentP->child_[0] = t;
    TreeNode** t1 = syn_tree_.top();
    syn_tree_.pop();
    *t1 = currentP;
    syn_tree_.push(&(currentP->sibling_));
    return;
}
void SynAnalysis::process67() {
    symbol_.push(AssignmentRest);
    currentP->kind_ = AssignK;
    return;
}
void SynAnalysis::process68() {
    symbol_.push(CallStmRest);
    currentP->child_[0]->attr_.exp_attr.var_kind = IdV;
    currentP->kind_ = CallK;
    return;
}
void SynAnalysis::process69() {
    symbol_.push(Exp);
    symbol_.push(ASSIGN);
    symbol_.push(VariMore);
//    currentP->child_.push_back(NULL);
    syn_tree_.push(&(currentP->child_[1]));
    currentP = currentP->child_[0];
    //operator_.push("END"); //my modify
    //my modify
    TreeNode* t = new TreeNode();
    t->node_kind_ = ExpK;
    t->kind_ = OpK;
    t->attr_.exp_attr.op = END;
    operator_.push(t);
    return;
}
void SynAnalysis::process70() {
    symbol_.push(FI);
    symbol_.push(StmList);
    symbol_.push(ELSE);
    symbol_.push(StmList);
    symbol_.push(THEN);
    symbol_.push(RelExp);
    symbol_.push(IF);
//    for (int i = 0; i < 3; i++)
//        currentP->child_.push_back(NULL);
    for (int i = 2; i >= 0; i--)
        syn_tree_.push(&(currentP->child_[i]));
    return;
}
void SynAnalysis::process71() {
    symbol_.push(ENDWH);
    symbol_.push(StmList);
    symbol_.push(DO);
    symbol_.push(RelExp);
    symbol_.push(WHILE);
//    for (int i = 0; i < 2; i++)
//        currentP->child_.push_back(NULL);
    for (int i = 1; i >= 0; i--)
        syn_tree_.push(&(currentP->child_[i]));
    return;
}
void SynAnalysis::process72() {
    symbol_.push(RPAREN);
    symbol_.push(Invar);
    symbol_.push(LPAREN);
    symbol_.push(READ);
    return;
}
void SynAnalysis::process73() {
    symbol_.push(ID);
    currentP->idnum_++;
    currentP->name_.push_back(token_list_[now_]->sem_);
    return;
}
void SynAnalysis::process74() {
    symbol_.push(RPAREN);
    symbol_.push(Exp);
    symbol_.push(LPAREN);
    symbol_.push(WRITE);
//    currentP->child_.push_back(NULL);
    syn_tree_.push(&(currentP->child_[0]));
    TreeNode* t = new TreeNode();
    t->node_kind_ = ExpK;
    t->kind_ = OpK;
    t->attr_.exp_attr.op = END;
    operator_.push(t);
    return;
}
void SynAnalysis::process75() {
    symbol_.push(RETURN);
    return;
}
void SynAnalysis::process76() {
    symbol_.push(RPAREN);
    symbol_.push(ActParamList);
    symbol_.push(LPAREN);
//    currentP->child_.push_back(NULL);
    syn_tree_.push(&(currentP->child_[1]));
    return;
}
void SynAnalysis::process77() {
    syn_tree_.pop();
    return;
}
void SynAnalysis::process78() {
    symbol_.push(ActParamMore);
    symbol_.push(Exp);
    TreeNode* t = new TreeNode();
    t->node_kind_ = ExpK;
    t->kind_ = OpK;
    t->attr_.exp_attr.op = END;
    operator_.push(t);
    return;
}
void SynAnalysis::process79() {
    return;
}
void SynAnalysis::process80() {
    symbol_.push(ActParamList);
    symbol_.push(COMMA);
    syn_tree_.push(&(currentP->sibling_));
    return;
}
void SynAnalysis::process81() {
    symbol_.push(OtherRelE);
    symbol_.push(Exp);
    TreeNode* t = new TreeNode();
    t->node_kind_ = ExpK;
    t->kind_ = OpK;
    t->attr_.exp_attr.op = END;
    operator_.push(t);
    getExpResult_ = false;
    return;
}
void SynAnalysis::process82() {
    symbol_.push(Exp);
    symbol_.push(CmpOp);
    currentP = new TreeNode();
    currentP->lineno_ = token_list_[now_]->line_;
    currentP->node_kind_ = ExpK;
    currentP->kind_ = OpK;
    currentP->attr_.exp_attr.op = token_list_[now_]->lex_;
//    currentP->child_.push_back(NULL);
//    currentP->child_.push_back(NULL);
    //cout<<operator_.empty()<<endl;
    //cout<<operator_.top()->attr_.exp_attr.op<<endl;
    while (priosity(operator_.top()->attr_.exp_attr.op) >= priosity(currentP->attr_.exp_attr.op)) {
        TreeNode* t = operator_.top();
        operator_.pop();
        TreeNode* Rnum = oprand_.top();
        oprand_.pop();
        TreeNode* Lnum = oprand_.top();
        oprand_.pop();
        t->child_[0] = Lnum;
        t->child_[1] = Rnum;
        oprand_.push(t);
    }
    operator_.push(currentP);
    getExpResult_ = true;
    return;
}
void SynAnalysis::process83() {
    symbol_.push(OtherTerm);
    symbol_.push(Term);
    return;
}
void SynAnalysis::process84() {
    if (token_list_[now_]->lex_ == RPAREN && expflag_ != 0) {
        while (operator_.top()->attr_.exp_attr.op != LPAREN) {
            TreeNode* t = operator_.top();
            operator_.pop();
            //cout<<token_list_[now_]->line_<<endl;
            TreeNode* Rnum = oprand_.top();
            oprand_.pop();
            TreeNode* Lnum = oprand_.top();
            oprand_.pop();
            t->child_[0] = Lnum;
            t->child_[1] = Rnum;
            oprand_.push(t);
        }
        TreeNode* t1 = operator_.top();
        operator_.pop();
        delete t1;
        expflag_--;
        return;
    }

    if (getExpResult_ || getExpResult2_) {
        while (operator_.top()->attr_.exp_attr.op != END) {
            TreeNode* t = operator_.top();
            operator_.pop();
            TreeNode* Rnum = oprand_.top();
            oprand_.pop();
            TreeNode* Lnum = oprand_.top();
            oprand_.pop();
            t->child_[0] = Lnum;
            t->child_[1] = Rnum;
            oprand_.push(t);
        }
        TreeNode* t1 = operator_.top();
        operator_.pop();
        delete t1;
        currentP = oprand_.top();
        oprand_.pop();
        TreeNode** t2 = syn_tree_.top();
        syn_tree_.pop();
        *t2 = currentP;
        getExpResult2_ = false;
        return;
    }

}
void SynAnalysis::process85() {
    symbol_.push(Exp);
    symbol_.push(AddOp);
    currentP = new TreeNode();
    currentP->lineno_ = token_list_[now_]->line_;
    currentP->node_kind_ = ExpK;
    currentP->kind_ = OpK;
    currentP->attr_.exp_attr.op = token_list_[now_]->lex_;
//    currentP->child_.push_back(NULL);
//    currentP->child_.push_back(NULL);
    while (priosity(operator_.top()->attr_.exp_attr.op) >= priosity(currentP->attr_.exp_attr.op)) {
        TreeNode* t = operator_.top();
        operator_.pop();
        TreeNode* Rnum = oprand_.top();
        oprand_.pop();
        TreeNode* Lnum = oprand_.top();
        oprand_.pop();
        t->child_[0] = Lnum;
        t->child_[1] = Rnum;
        oprand_.push(t);
    }
    operator_.push(currentP);
    return;
}
void SynAnalysis::process86() {
    symbol_.push(OtherFactor);
    symbol_.push(Factor);
    return;
}
void SynAnalysis::process87() {
    return;
}
void SynAnalysis::process88() {
    symbol_.push(Term);
    symbol_.push(MultOp);
    currentP = new TreeNode();
    currentP->lineno_ = token_list_[now_]->line_;
    currentP->node_kind_ = ExpK;
    currentP->kind_ = OpK;
    currentP->attr_.exp_attr.op = token_list_[now_]->lex_;
//    currentP->child_.push_back(NULL);
//    currentP->child_.push_back(NULL);
    while (priosity(operator_.top()->attr_.exp_attr.op) >= priosity(currentP->attr_.exp_attr.op)) {
        TreeNode* t = operator_.top();
        operator_.pop();
        TreeNode* Rnum = oprand_.top();
        oprand_.pop();
        TreeNode* Lnum = oprand_.top();
        oprand_.pop();
        t->child_[0] = Lnum;
        t->child_[1] = Rnum;
        oprand_.push(t);
    }
    operator_.push(currentP);
    return;
}
void SynAnalysis::process89() {
    symbol_.push(RPAREN);
    symbol_.push(Exp);
    symbol_.push(LPAREN);
    currentP = new TreeNode();
    currentP->node_kind_ = ExpK;
    currentP->kind_ = OpK;
    currentP->attr_.exp_attr.op = LPAREN;
    operator_.push(currentP);
    expflag_++;
    return;
}
void SynAnalysis::process90() {
    symbol_.push(INTC);
    currentP = new TreeNode();
    currentP->lineno_ = token_list_[now_]->line_;
    currentP->node_kind_ = ExpK;
    currentP->kind_ = ConstK;
    currentP->attr_.exp_attr.val = getFromStr(token_list_[now_]->sem_);
    oprand_.push(currentP);
    return;
}
void SynAnalysis::process91() {
    symbol_.push(Variable);
    return;
}
void SynAnalysis::process92() {
    symbol_.push(VariMore);
    symbol_.push(ID);
    currentP = new TreeNode();
    currentP->lineno_ = token_list_[now_]->line_;
    currentP->node_kind_ = ExpK;
    currentP->kind_ = VariK;
    currentP->name_.push_back(token_list_[now_]->sem_);
    currentP->idnum_++;
    oprand_.push(currentP);
    return;
}
void SynAnalysis::process93() {
    currentP->attr_.exp_attr.var_kind = IdV;
    return;
}
void SynAnalysis::process94() {
    symbol_.push(RMIDPAREN);
    symbol_.push(Exp);
    symbol_.push(LMIDPAREN);
    currentP->attr_.exp_attr.var_kind = ArrayMembV;
//    currentP->child_.push_back(NULL);
    syn_tree_.push(&(currentP->child_[0]));
    TreeNode* t = new TreeNode();
    t->node_kind_ = ExpK;
    t->kind_ = OpK;
    t->attr_.exp_attr.op = END;
    operator_.push(t);
    getExpResult2_ = true;
    return;
}
void SynAnalysis::process95() {
    symbol_.push(FieldVar);
    symbol_.push(DOT);
    currentP->attr_.exp_attr.var_kind = FieldMembV;
//   currentP->child_.push_back(NULL);
    syn_tree_.push(&(currentP->child_[0]));
    return;
}
void SynAnalysis::process96() {
    symbol_.push(FieldVarMore);
    symbol_.push(ID);
    currentP = new TreeNode();
    currentP->lineno_ = token_list_[now_]->line_;
    currentP->node_kind_ = ExpK;
    currentP->kind_ = VariK;
    currentP->name_.push_back(token_list_[now_]->sem_);
    currentP->idnum_++;
    TreeNode** t = syn_tree_.top();
    syn_tree_.pop();
    *t = currentP;
    return;
}
void SynAnalysis::process97() {
    currentP->attr_.exp_attr.var_kind = IdV;
    return;
}
void SynAnalysis::process98() {
    symbol_.push(RMIDPAREN);
    symbol_.push(Exp);
    symbol_.push(LMIDPAREN);
    currentP->attr_.exp_attr.var_kind = ArrayMembV;
//    currentP->child_.push_back(NULL);
    syn_tree_.push(&(currentP->child_[0]));
    TreeNode* t = new TreeNode();
    t->node_kind_ = ExpK;
    t->kind_ = OpK;
    t->attr_.exp_attr.op = END;
    operator_.push(t);
    getExpResult2_ = true;
    return;
}
void SynAnalysis::process99() {
    symbol_.push(LT);
    return;
}
void SynAnalysis::process100() {
    symbol_.push(EQ);
    return;
}
void SynAnalysis::process101() {
    symbol_.push(ADD);
    return;
}
void SynAnalysis:: process102() {
    symbol_.push(MINUS);
    return;
}
void SynAnalysis::process103() {
    symbol_.push(TIMES);
    return;
}
void SynAnalysis::process104() {
    symbol_.push(DIV);
    return;
}


