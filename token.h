#ifndef TOKEN_H_INCLUDED
#define TOKEN_H_INCLUDED

#include <string>
#include "lex_type.h"

class Token {
public:
    int line_; //行数
    LexType lex_; //词法信息, 即单词种类
    std::string sem_; //语义信息

    Token(int line, LexType lex, std::string sem){
        this->line_ = line;
        this->lex_ = lex;
        this->sem_ = sem;
    }
};

#endif // TOKEN_H_INCLUDED
