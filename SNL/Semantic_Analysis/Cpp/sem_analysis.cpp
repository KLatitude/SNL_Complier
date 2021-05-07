#include "../Head/sem_analysis.h"
#include <iostream>
#include <unordered_set>
using namespace std;

void SemAnalysis::createTable() {
    level_++;
    off_ = init_off_;
    SymbolTable* t = new SymbolTable();
    scope_.push_back(t);
    while (level_ >= (int)rec_sym_table_.size())
        rec_sym_table_.push_back(vector<SymbolTable*>());
    rec_sym_table_[level_].push_back(t);
    return;
}

void SemAnalysis::destroyTable() {
    level_--;
    if (!scope_.empty())
        scope_.pop_back();
    return;
}

bool SemAnalysis::enter(string id, AttributeIR* attribP, SymbolTable** entry) {
    bool present = false;
    present = findEntry(id, 0, entry);
    if (present) {
        analysisError(NULL, "id has repeat declaration", id);
        return false;
    }
    SymbolTable* cur = new SymbolTable();
    cur->idname_ = id;
    cur->next_ = NULL;
    cur->attrIR_->kind_ = attribP->kind_;
    /*
    if (attribP->kind_ == "typeKind") {

    }
    if (attribP->kind_ == "varKind") {
        cur->attrIR_->var_attr_ = new Var();
        cur->attrIR_->var_attr_->access_ = attribP->var_attr_->access_;
        cur->attrIR_->var_attr_->level_ = attribP->var_attr_->level_;
        cur->attrIR_->var_attr_->off_ = attribP->var_attr_->off_;
    }
    if (attribP->kind_ == "procKind") {

    }
    */
    SymbolTable* pre = scope_[level_];
    SymbolTable* q = pre->next_;
    while (q != NULL) {
        pre = q;
        q = q->next_;
    }
    pre->next_ = cur;
    *entry = cur;
    return true;
}

bool SemAnalysis::findEntry(string id, int flag, SymbolTable** entry) {
    if (flag == 0) //find in current level
        return searchTable(id, level_, entry);
    int x = level_;
    bool sign = false;
    while (x >= 0) {
        if (searchTable(id, x, entry))
            sign = true;
        x--;
    }
    return sign;
}

bool SemAnalysis::searchTable(string id, int current_level, SymbolTable** entry) {
    int n = scope_.size();
    if (current_level > n)
        return false;
    SymbolTable* t = scope_[current_level]->next_;
    while (t != NULL) {
        if (id == t->idname_) {
            *entry = t;
            return true;
        }
        t = t->next_;
    }
    return false;
}

bool SemAnalysis::findField(string id, FieldChain* head, FieldChain** entry) {
    while (head != NULL) {
        if (id == head->idname_) {
            *entry = head;
            return true;
        }
    }
    return false;
}

void SemAnalysis::printOneTypeIR(TypeIR* ptr) {

    cout<<ptr->size_<<"\t"<<ptr->kind_;
    if (ptr->kind_ == "arrayTy")
        cout<<"\t"<<ptr->array_attr_->low<<"\t"<<ptr->array_attr_->up<<"\t"<<ptr->array_attr_->elem_ty_->kind_;
    //cout<<endl;
    if (ptr->kind_ == "recordTy") {
        FieldChain* field = ptr->body_;
        while (field != NULL) {
            cout<<endl;
            for (int i = 0; i < 3; i++)
                cout<<"\t";
            cout<<field->idname_<<"\t"<<field->offset_<<"\t";
            printOneTypeIR(field->unit_type_);
            field = field->next_;
        }
    }
    return;
}

void SemAnalysis::printOneSymTable(SymbolTable* head) {
    while (head != NULL) {
        cout<<"\t";
        cout<<head->idname_<<"\t"<<head->attrIR_->kind_;
        if (head->attrIR_->var_attr_ != NULL)
            cout<<"\t"<<head->attrIR_->var_attr_->access_<<"\t"<<head->attrIR_->var_attr_->level_<<"\t"<<head->attrIR_->var_attr_->off_;
        if (head->attrIR_->proc_attr_ != NULL)
            cout<<"\t"<<head->attrIR_->proc_attr_->level_;
        cout<<endl;
        if (head->attrIR_->id_type_ != NULL) {
            for (int i = 0; i < 2; i++)
                cout<<"\t";
            printOneTypeIR(head->attrIR_->id_type_);
            cout<<endl;
        }
        head = head->next_;
    }
}

void SemAnalysis::printAllSymTable() {
    for (int i = 0; i < (int)rec_sym_table_.size(); i++) {
        for (int j = 0; j < (int)rec_sym_table_[i].size(); j++) {
            cout<<"Level: "<<i<<endl;
            printOneSymTable(rec_sym_table_[i][j]->next_);
        }
    }
}


void SemAnalysis::analyze(TreeNode* currentP) {

    createTable();
    initialize();
    TreeNode* q = currentP->child_[1];

    while (q != NULL) {
        if (q->node_kind_ == TypeK)
            typeDecPart(q->child_[0]);
        else if (q->node_kind_ == VarK)
            varDecPart(q->child_[0]);
        else if (q->node_kind_ == ProcDecK)
            procDecPart(q);
        else
            analysisError(q, "no this node kind", "");
        q = q->sibling_;
    }

    q = currentP->child_[2];
    if (q->node_kind_ == StmLK)
        body(q);
    if (level_ != -1)
        destroyTable();

    //cout<<error_message.length()<<endl;
    if (error_sign)
        cout<<error_message<<endl;
}

void SemAnalysis::initialize() {
    intPtr = new TypeIR();
    intPtr->size_ = 1;
    intPtr->kind_ = "intTy";

    charPtr = new TypeIR();
    charPtr->size_ = 1;
    charPtr->kind_ = "charTy";

    boolPtr = new TypeIR();
    boolPtr->size_ = 1;
    boolPtr->kind_ = "boolTy";
}

TypeIR* SemAnalysis::typeProcess(TreeNode* t, SpecType kind) {
    TypeIR* type_ptr = NULL;
    if (kind == IntegerK) {
        type_ptr = new TypeIR();
        type_ptr->size_ = 1;
        type_ptr->kind_ = "intTy";
    } else if (kind == CharK) {
        type_ptr = new TypeIR();
        type_ptr->size_ = 1;
        type_ptr->kind_ = "charTy";
    } else if (kind == ArrayK) {
        type_ptr = arrayType(t);
    } else if (kind == RecordK) {
        type_ptr = recordType(t);
    } else if (kind == IdK) {
        type_ptr = nameType(t);
    } else {
        analysisError(t, "No this syn_tree type", "");
        return NULL;
    }
    return type_ptr;
}

TypeIR* SemAnalysis::nameType(TreeNode* t) {
    SymbolTable* entry = NULL;
    bool present = false;
    TypeIR* type_ptr = NULL;
    present = findEntry(t->type_name_, 1, &entry);
    if (!present)
        analysisError(t, "id no declaration", t->type_name_);
    else if (entry->attrIR_->kind_ != "typeKind")
        analysisError(t, "id not a type id", t->type_name_);
    else {
        type_ptr = new TypeIR;
        type_ptr->size_ = entry->attrIR_->id_type_->size_;
        type_ptr->kind_ = entry->attrIR_->id_type_->kind_;
        type_ptr->array_attr_ = entry->attrIR_->id_type_->array_attr_;
        type_ptr->body_ = entry->attrIR_->id_type_->body_;
    }
    return type_ptr;
}

TypeIR* SemAnalysis::arrayType(TreeNode* t) {
    TypeIR* type_ptr = new TypeIR();
    type_ptr->kind_ = "arrayTy";
    type_ptr->array_attr_ = new Array();
    type_ptr->array_attr_->index_ty_ = new TypeIR();
    type_ptr->array_attr_->index_ty_->size_ = 1;
    type_ptr->array_attr_->index_ty_->kind_ = "intTy";
    type_ptr->array_attr_->elem_ty_ = typeProcess(t, t->attr_.array_attr.child_type);

    int up = t->attr_.array_attr.up;
    int low = t->attr_.array_attr.low;
    if (low > up) {
        analysisError(t, "array up smaller than low", "");
        return NULL;
    } else {
        type_ptr->array_attr_->low = low;
        type_ptr->array_attr_->up = up;
    }
    type_ptr->size_ = (up - low + 1) * type_ptr->array_attr_->elem_ty_->size_;
    return type_ptr;
}

TypeIR* SemAnalysis::recordType(TreeNode* t) {
    TypeIR* type_ptr = new TypeIR();
    type_ptr->kind_ = "recordTy";
    FieldChain* body = NULL;
    FieldChain* ptr1 = NULL;
    FieldChain* ptr2 = NULL;
    TreeNode* p = t;
    t = t->child_[0];
    unordered_set<string> ust;
    while (t != NULL) {
        //deal with like int a, b;
        for (int i = 0; i < t->idnum_; i++) {
            if (ust.find(t->name_[i]) != ust.end()) {
                analysisError(t, "id has repeat declaration in record", p->name_[0] + "." + t->name_[i]);
                continue;
            }
            ust.insert(t->name_[i]);
            ptr2 = new FieldChain();
            ptr2->idname_ = t->name_[i];
            ptr2->unit_type_ = typeProcess(t, t->kind_);
            if (body == NULL) {
                body = ptr2;
                ptr1 = ptr2;
            } else {
                ptr2->offset_ = ptr1->offset_ + ptr1->unit_type_->size_;
                ptr1->next_ = ptr2;
                ptr1 = ptr2;
            }
        }
        t = t->sibling_;
    }
    type_ptr->size_ = ptr2->offset_ + ptr2->unit_type_->size_;
    type_ptr->body_ = body;
    return type_ptr;
}

void SemAnalysis::typeDecPart(TreeNode* t) {
    bool present = false;
    AttributeIR* attrib = new AttributeIR();
    SymbolTable* entry = NULL;
    attrib->kind_ = "typeKind";
    while (t != NULL) {
        entry = NULL;
        //cout<<t->name_[0]<<endl;
        present = enter(t->name_[0], attrib, &entry);
        if (!present) {
            analysisError(t, "id has repeat declaration", t->name_[0]);
            break;
        } else
            entry->attrIR_->id_type_ = typeProcess(t, t->kind_);
        t = t->sibling_;
    }
    delete attrib;
    return;
}

void SemAnalysis::varDecList(TreeNode* t) {
    SymbolTable* entry = NULL;
    AttributeIR* attrib = new AttributeIR();
    attrib->kind_ = "varKind";
    while (t != NULL) {
        for (int i = 0; i < t->idnum_; i++) {
            bool present = enter(t->name_[i], attrib, &entry);
            if (!present) {
                analysisError(t, "id has repeat declaration", t->name_[i]);
                delete attrib;
                return;
            }
            entry->attrIR_->id_type_ = typeProcess(t, t->kind_);
            if (t->attr_.proc_attr.paramt == varparamtype) {
                entry->attrIR_->var_attr_ = new Var();
                entry->attrIR_->var_attr_->access_ = "indir";
                entry->attrIR_->var_attr_->level_ = level_;
                entry->attrIR_->var_attr_->off_ = off_;
                off_ += 1;
            } else {
                entry->attrIR_->var_attr_ = new Var();
                entry->attrIR_->var_attr_->access_ = "dir";
                entry->attrIR_->var_attr_->level_ = level_;
                entry->attrIR_->var_attr_->off_ = off_;
                off_ += entry->attrIR_->id_type_->size_;
            }
        }
        t = t->sibling_;
    }
	delete attrib;
    return;
}

void SemAnalysis::varDecPart(TreeNode* t) {
    varDecList(t);
}

void SemAnalysis::procDecPart(TreeNode* t) {
    TreeNode* p = t;
    SymbolTable* entry = headProcess(t);
    t = p;
    t = t->child_[1];
    while (t != NULL) {
        if (t->node_kind_ == TypeK)
            typeDecPart(t->child_[0]);
        if (t->node_kind_ == VarK)
            varDecPart(t->child_[0]);
        if (t->node_kind_ == ProcDecK)
            procDecPart(t);
        t = t->sibling_;
    }
    t = p;
    body(t->child_[2]);
    if (level_ != -1)
        destroyTable();
    return;
}

SymbolTable* SemAnalysis::headProcess(TreeNode* t) {
    AttributeIR* attrib = new AttributeIR();
    attrib->kind_ = "procKind";
    bool present = true;
    SymbolTable* entry = NULL;
    present = enter(t->name_[0], attrib, &entry);
    if (!present) {
        analysisError(t, "id has repeat declaration", t->name_[0]);
        return NULL;
    }
    entry->attrIR_->proc_attr_ = new Proc();
    entry->attrIR_->proc_attr_->level_ = level_ + 1;
    entry->attrIR_->proc_attr_->param_ = paraDecList(t);
    delete attrib;
    return entry;
}

ParamTable* SemAnalysis::paraDecList(TreeNode* t) {
    createTable();
    if (t->child_[0] != NULL)
        varDecPart(t->child_[0]);
    ParamTable* head = NULL;
    ParamTable* ptr1 = NULL;
    ParamTable* ptr2 = NULL;
    SymbolTable* ptr0 = scope_[level_]->next_;
    while (ptr0 != NULL) {
        ptr2 = new ParamTable();
        ptr2->entry_ = ptr0;
        if (head == NULL) {
            head = ptr2;
            ptr1 = ptr2;
        } else {
            ptr1->next_ = ptr2;
            ptr1 = ptr2;
        }
        ptr0 = ptr0->next_;
    }
    return head;
}

void SemAnalysis::body(TreeNode* t) {
    if (t->node_kind_ == StmLK)
        t = t->child_[0];
    while (t != NULL) {
        statement(t);
        t = t->sibling_;
    }
    return;
}

void SemAnalysis::statement(TreeNode* t) {
    if (t->kind_ == AssignK)
        assignStatement(t);
    else if (t->kind_ == CallK)
        callStatement(t);
    else if (t->kind_ == ReadK)
        readStatement(t);
    else if (t->kind_ == WriteK)
        writeStatement(t);
    else if (t->kind_ == IfK)
        ifStatement(t);
    else if (t->kind_ == WhileK)
        whileStatement(t);
    else if (t->kind_ == ReturnK)
        returnStatement(t);
    else
        analysisError(t, "no this statement", "");
    return;
}

TypeIR* SemAnalysis::expr(TreeNode* t, std::string& kind) {
    bool present = false;
    TypeIR* type_ptr = NULL;
    SymbolTable* entry = NULL;
    if (t != NULL) {
        if (t->kind_ == ConstK) {
            type_ptr = intPtr;
            if (kind != "")
                kind = "dir";
        } else if (t->kind_ == VariK) {
            if (t->child_[0] == NULL) {
                present = findEntry(t->name_[0], 1, &entry);
                if (present) {
                    if (entry->attrIR_->kind_ != "varKind")
                        analysisError(t, "id is not a variable", t->name_[0]);
                    else {
                        type_ptr = entry->attrIR_->id_type_;
                        if (kind != "")
                            kind = "indir";
                    }
                } else
                    analysisError(t, "id has not declaration", t->name_[0]);
            } else {
                if (t->attr_.exp_attr.var_kind == ArrayMembV)
                    type_ptr = arrayVar(t);
                else if (t->attr_.exp_attr.var_kind == FieldMembV)
                    type_ptr = recordType(t);
            }
        } else if (t->kind_ == OpK) {
            string str = "";
            TypeIR* ptr1 = expr(t->child_[0], str);
            TypeIR* ptr2 = expr(t->child_[1], str);
            if (ptr1 == NULL || ptr2 == NULL)
                return NULL;
            present = compat(ptr1, ptr2);
            if (present) {
                if (t->attr_.exp_attr.op == LT || t->attr_.exp_attr.op == EQ)
                    type_ptr = boolPtr;
                else if (t->attr_.exp_attr.op == ADD || t->attr_.exp_attr.op == MINUS || t->attr_.exp_attr.op == TIMES || t->attr_.exp_attr.op == DIV)
                    type_ptr = intPtr;
                if (kind != "")
                    kind = "dir";
            } else {
                analysisError(t, "operator's left and right are not compatible", "");
            }
        }
    }
    return type_ptr;
}


TypeIR* SemAnalysis::arrayVar(TreeNode* t) {
    bool present = false;
    SymbolTable* entry = NULL;
    TypeIR* type_ptr = NULL;
    present = findEntry(t->name_[0], 1, &entry);
    if (present) {
        if (entry->attrIR_->kind_ != "varKind")
            analysisError(t, "id is not a variable", t->name_[0]);
        else if (entry->attrIR_->id_type_->kind_ != "arrayTy")
            analysisError(t, "id is not a array variable", t->name_[0]);
        else {
            string str = "";
            TypeIR* ptr0 = entry->attrIR_->id_type_->array_attr_->index_ty_;
            TypeIR* ptr1 = expr(t->child_[0], str);
            if (ptr0 == NULL || ptr1 == NULL)
                return NULL;
            present = compat(ptr0, ptr1);
            if (!present) {
                analysisError(t, "type is not match array index", "");
                return NULL;
            } else {
                type_ptr = entry->attrIR_->id_type_->array_attr_->elem_ty_;
            }
        }
    } else {
        analysisError(t, "id has not declaration", t->name_[0]);
    }

    return type_ptr;
}

TypeIR* SemAnalysis::arrayVarForRecord(TreeNode* t, FieldChain* entry) {
    TypeIR* type_ptr = NULL;
    if (entry->unit_type_->kind_ == "arrayTy") {
        string str = "";
        TypeIR* ptr1 = entry->unit_type_->array_attr_->index_ty_;
        TypeIR* ptr2 = expr(t->child_[0], str);
        if (ptr1 == NULL || ptr2 == NULL)
            return NULL;
        if (compat(ptr1, ptr2)) {
            type_ptr = entry->unit_type_->array_attr_->elem_ty_;
        } else {
            analysisError(t, "type is not match in array index", "");
            return NULL;
        }
    } else {
        analysisError(t, "id is not a array variable", t->name_[0]);
    }
    return type_ptr;
}

TypeIR* SemAnalysis::recordVar(TreeNode* t) {
    bool present = false;
    SymbolTable* entry = NULL;
    TypeIR* type_ptr = NULL;
    present = findEntry(t->name_[0], 1, &entry);
    if (present) {
        if (entry->attrIR_->kind_ != "varKind")
            analysisError(t, "id is not a variable", t->name_[0]);
        else if (entry->attrIR_->id_type_->kind_ == "recordTy")
            analysisError(t, "id is not a record variable", t->name_[0]);
        else {
            FieldChain* curP = entry->attrIR_->id_type_->body_;
            bool result = false;
            while (curP != NULL && !result) {
                if (t->child_[0]->name_[0] == curP->idname_)
                    result = true;
                if (result)
                    type_ptr = curP->unit_type_;
                else
                    curP = curP->next_;
            }
            if (!result)
                analysisError(t, "id is not a field type", t->child_[0]->name_[0]);
            else if (t->child_[0]->child_[0] != NULL)
                type_ptr = arrayVarForRecord(t->child_[0], curP); //--------------------------------need to modify
        }
    } else
        analysisError(t, "id has not declaration", t->name_[0]);
    return type_ptr;
}

void SemAnalysis::assignStatement(TreeNode* t) {
    bool present = false;
    SymbolTable* entry = NULL;
    TypeIR* l_ptr = NULL;
    TypeIR* r_ptr = NULL;

    TreeNode* child1 = t->child_[0];
    TreeNode* child2 = t->child_[1];
    if (child1->child_[0] == NULL) {
        present = findEntry(child1->name_[0], 1, &entry);
        if (present) {
            if (entry->attrIR_->kind_ != "varKind")
                analysisError(t, "left and right are not compatible in assign", "");
            else
                l_ptr = entry->attrIR_->id_type_;
        } else {
            analysisError(t, "id has not declaration", child1->name_[0]);
        }
    } else {
        if (child1->attr_.exp_attr.var_kind == ArrayMembV) {
            l_ptr = arrayVar(child1);
        } else if (child1->attr_.exp_attr.var_kind == FieldMembV) {
            l_ptr = recordVar(child1);
        }
    }
    if (l_ptr != NULL) {
        if (t->node_kind_ == StmtK && t->kind_ == AssignK) {
            string str = "";
            r_ptr = expr(child2, str);
            if (!compat(l_ptr, r_ptr))
                analysisError(t, "left and right are not compatible in assign", "");
        }
    }
}


void SemAnalysis::callStatement(TreeNode* t) {
    SymbolTable* entry = NULL;
    bool present = findEntry(t->child_[0]->name_[0], 1, &entry);
    if (!present)
        analysisError(t, "function has not declaration", t->child_[0]->name_[0]);
    else {
        if (entry->attrIR_->kind_ != "procKind")
            analysisError(t, "it is not a function", t->child_[0]->name_[0]);
        else {
            TreeNode* p = t->child_[1];
            ParamTable* param = entry->attrIR_->proc_attr_->param_;
            while (p != NULL && param != NULL) {
                string str = " ";
                TypeIR* type_ptr = expr(p, str);
                SymbolTable* param_entry = param->entry_;
                if (param_entry->attrIR_->var_attr_->access_ == "indir" && str == "dir")
                    analysisError(t, "param kind not match", "");
                if (!compat(type_ptr, param_entry->attrIR_->id_type_))
                    analysisError(t, "param type not match", "");
                p = p->sibling_;
                param = param->next_;
            }
            if (p != NULL || param != NULL)
                analysisError(t, "param's nums are not match", "");
        }
    }
}

void SemAnalysis::ifStatement(TreeNode* t) {
    string str = "";
    TypeIR* type_ptr = expr(t->child_[0], str);
    if (type_ptr != NULL) {
        if (type_ptr->kind_ != "boolTy")
            analysisError(t, "no bool expression in if statement", "");
        else {
            TreeNode* p = t->child_[1];
            while (p != NULL) {
                statement(p);
                p = p->sibling_;
            }
            p = t->child_[2];
            while (p != NULL) {
                statement(p);
                p = p->sibling_;
            }
        }
    } else {
        analysisError(t, "if statement error", "");
    }
}

void SemAnalysis::whileStatement(TreeNode* t) {
    string str = "";
    TypeIR* type_ptr = expr(t->child_[0], str);
    if (type_ptr != NULL) {
        if (type_ptr->kind_ != "boolTy")
            analysisError(t, "no bool expression in while statement", "");
        else {
            TreeNode* p = t->child_[1];
            while (p != NULL) {
                statement(p);
                p = p->sibling_;
            }
        }
    } else {
        analysisError(t, "while statement error", "");
    }
}

void SemAnalysis::readStatement(TreeNode* t) {
    SymbolTable* entry = NULL;
    bool present = findEntry(t->name_[0], 1, &entry);
    if (!present)
        analysisError(t, "id has not declaration in read statement", t->name_[0]);
    else if (entry->attrIR_->kind_ != "varKind")
        analysisError(t, "id is not a var in read statement", t->name_[0]);
    return;
}

void SemAnalysis::writeStatement(TreeNode* t) {
    string str = "";
    TypeIR* type_ptr = expr(t->child_[0], str);
    bool sign = false;
    if (type_ptr != NULL && type_ptr->kind_ != "boolTy")
        sign = true;
    if (!sign)
        analysisError(t, "write statement error", "");
}

void SemAnalysis::returnStatement(TreeNode* t) {
    if (level_ == 0)
        analysisError(t, "return statement can not in main program", "");
    return;
}

bool SemAnalysis::isError() {
    return error_sign;
}

void SemAnalysis::analysisError(TreeNode* t, std::string message, std::string str) {
    if (t != NULL)
        error_message += "Error: " + to_string(t->lineno_) + " --- ";
    error_message += message + " --- " + str + "\n";
    error_sign = true;
}

bool SemAnalysis::compat(TypeIR* lhs, TypeIR* rhs) {
    if (lhs == NULL && rhs == NULL)
        return true;
    else if (lhs == NULL || rhs == NULL)
        return false;

    //bool present = true;
    //cout<<lhs->kind_<<"-"<<rhs->kind_<<endl;
    if (lhs->kind_ != rhs->kind_)
        return false;
    return true;
}
