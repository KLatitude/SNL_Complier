#include "../Head/lex_type.h"

std::string getLexType(LexType lex) {
    std::string str_lex[] = {
        //reserved word
        "PROGRAM", "TYPE", "VAR", "PROCEDURE", "BEGIN", "END", "ARRAY", "OF", "RECORD",
        "IF", "THEN", "ELSE", "FI", "WHILE", "DO", "ENDWH", "READ", "WRITE", "RETURN", "INTEGER", "CHAR",

        //operator
        "ADD", "MINUS", "TIMES", "DIV", "LPAREN", "RPAREN", "LMIDPAREN", "RMIDPAREN",
        "ASSIGN", "EQ", "LT", "SEMI", "COLON", "COMMA", "UNDERANGE", "DOT",

        "ID", "INTC", "CHARC",

        "ERROR", "ENDFILE"
    };
    return str_lex[lex];
}
