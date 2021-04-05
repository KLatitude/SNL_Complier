#ifndef SCANNER_H_INCLUDED
#define SCANNER_H_INCLUDED

#include <string>
#include "lex_type.h"
#include "token.h"

enum State {
    START, INNUM, INID, INSINGLE, INCHAR, INDOT, INCOMMENT, INASSIGN
};

extern std::string reserved_words[];

class Scanner {
private:
    int line_;
    int next_;
    std::string context_;
public:
    Scanner();
    Scanner(std::string context);
    Token scan();
    bool isNumber(char ch);
    bool isLetter(char ch);
    LexType reservedLookup(std::string str);
};


#endif // SCANNER_H_INCLUDED
