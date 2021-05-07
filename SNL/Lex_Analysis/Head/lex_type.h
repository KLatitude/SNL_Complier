#ifndef LEX_TYPE_H_INCLUDED
#define LEX_TYPE_H_INCLUDED

#include <string>

typedef enum {
    //reserved word
    PROGRAM, TYPE, VAR, PROCEDURE, BEGIN, END, ARRAY, OF, RECORD,
    IF, THEN, ELSE, FI, WHILE, DO, ENDWH, READ, WRITE, RETURN, INTEGER, CHAR,

    //operator
    ADD, MINUS, TIMES, DIV, LPAREN, RPAREN, LMIDPAREN, RMIDPAREN,
    ASSIGN, EQ, LT, SEMI, COLON, COMMA, UNDERANGE, DOT, // := = < ; : , .. .

    ID, INTC, CHARC,

    ERROR, ENDFILE
}LexType;

std::string getLexType(LexType lex);

#endif // LEX_TYPE_H_INCLUDED
