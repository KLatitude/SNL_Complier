#ifndef TYPE_IR_H_INCLUDED
#define TYPE_IR_H_INCLUDED

#include "field_chain.h"

class TypeIR {
public:
    int size_ = 0;
    std::string kind_ = "";
    TypeIR* index_ty_ = NULL;
    TypeIR* elem_ty_ = NULL;
    FieldChain* body_ = NULL;

    TypeIR() {}

    ~TypeIR() {
        if (index_ty_ != NULL)
            delete index_ty_;
        if (elem_ty_ != NULL)
            delete elem_ty_;
        if (body_ != NULL)
            delete body_;
    }
};

#endif // TYPE_IR_H_INCLUDED
