#ifndef FIELD_CHAIN_H_INCLUDED
#define FIELD_CHAIN_H_INCLUDED

#include "type_ir.h"
class TypeIR;

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

#endif // FIELD_CHAIN_H_INCLUDED
