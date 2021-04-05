#ifndef TOKEN_H_INCLUDED
#define TOKEN_H_INCLUDED

#include <string>
#include "lex_type.h"

class Token {
public:
    int line_; //����
    LexType lex_; //�ʷ���Ϣ, ����������
    std::string sem_; //������Ϣ

    Token(int line, LexType lex, std::string sem){
        this->line_ = line;
        this->lex_ = lex;
        this->sem_ = sem;
    }
};

#endif // TOKEN_H_INCLUDED
