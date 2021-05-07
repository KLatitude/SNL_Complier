#include "../Head/scanner.h"
#include <iostream>
#include <algorithm>
using namespace std;

std::string reserved_words[] = {
    "program", "type", "var", "procedure", "begin", "end", "array", "of", "record",
    "if", "then", "else", "fi", "while", "do", "endwh", "read", "write", "return", "integer", "char"
};

Scanner::Scanner() {
    this->line_ = 1;
    this->next_ = 0;
    this->context_ = "";
}

Scanner::Scanner(string context) {
    this->line_ = 1;
    this->next_ = 0;
    this->context_ = context;
}

bool Scanner::isLetter(char ch) {
    if (ch >= 'a' && ch <= 'z')
        return true;
    if (ch >= 'A' && ch <= 'Z')
        return true;
    return false;
}

bool Scanner::isNumber(char ch) {
    if (ch >= '0' && ch <= '9')
        return true;
    return false;
}

LexType Scanner::reservedLookup(string str) {
    for (int i = 0; i < 21; i++) {
        if (str == reserved_words[i]) {
            return (LexType)i;
        }
    }
    return ID;
}


Token* Scanner::scan() {
    State state = START;
    int length = context_.length();

    while (next_ < length) {
        switch (state) {
            case START: {
                if (context_[next_] == ' ' || context_[next_] == '\t' || context_[next_] == '\n') {
                    //¹ýÂË¿Õ¸ñ
                    while (next_ < length && (context_[next_] == ' ' || context_[next_] == '\t' || context_[next_] == '\n')) {
                        if (context_[next_] == '\n')
                            line_++;
                        next_++;
                    }
                } else if (isLetter(context_[next_])) {
                    state = INID;
                } else if (isNumber(context_[next_])) {
                    state = INNUM;
                } else {
                    switch (context_[next_++]) {
                        case '+': return new Token(line_, ADD, "+");
                        case '-': return new Token(line_, MINUS, "-");
                        case '*': return new Token(line_, TIMES, "*");
                        case '/': return new Token(line_, DIV, "/");
                        case '(': return new Token(line_, LPAREN, "(");
                        case ')': return new Token(line_, RPAREN, ")");
                        case ';': return new Token(line_, SEMI, ";");
                        case '[': return new Token(line_, LMIDPAREN, "[");
                        case ']': return new Token(line_, RMIDPAREN, "]");
                        case '=': return new Token(line_, EQ, "=");
                        case '<': return new Token(line_, LT, "<");
                        case ',': return new Token(line_, COMMA, ",");
                        case ':': state = INASSIGN; break;
                        case '{': state = INCOMMENT; break;
                        case '.': state = INDOT; break;
                        case '\'': state = INCHAR; break;
                        case EOF: return new Token(line_, ENDFILE, "");
                        default: return new Token(line_, ERROR, string(1, context_[--next_]));
                    }
                }
                break;
            }
            case INID: {
                string str = "";
                while (next_ < length && (isLetter(context_[next_]) || isNumber(context_[next_]))) {
                    str.push_back(context_[next_]);
                    next_++;
                }
                return new Token(line_, reservedLookup(str), str);
                break;
            }
            case INNUM: {
                string str = "";
                while (next_ < length && isNumber(context_[next_])) {
                    str.push_back(context_[next_]);
                    next_++;
                }
                return new Token(line_, INTC, str);
                break;
            }
            case INASSIGN: {
                if (context_[next_++] == '=') {
                    return new Token(line_, ASSIGN, ":=");
                } else {
                    next_ -= 2;
                    return new Token(line_, ERROR, string(1, context_[next_]));
                }
                break;
            }
            case INCOMMENT: {
                while (next_ < length && context_[next_] != '}') {
                    if (context_[next_] == '\n')
                        line_++;
                    next_++;
                }
                if (next_ == length)
                    return new Token(line_, ERROR, "No } ");
                else {
                    next_++;
                    state = START;
                }
                break;
            }
            case INDOT: {
                if (context_[next_] == '.') {
                    next_++;
                    return new Token(line_, UNDERANGE, "..");
                }
                return new Token(line_, DOT, ".");
//                else if (context_[next_] == EOF)
//                    return new Token(line_, DOT, ".");
//                else
//                    return new Token(line_, ERROR, ".");
                break;
            }
            case INCHAR: {
                if (isLetter(context_[next_]) || isNumber(context_[next_])) {
                    next_++;
                } else
                    return new Token(line_, ERROR, "'");
                if (next_ < length && context_[next_] == '\'') {
                    next_++;
                    return new Token(line_, CHARC, string(1, context_[next_ - 2]));
                } else
                    return new Token(line_, ERROR, "'");
                break;
            }
            default: break;
        }
    }
    return new Token(line_, ERROR, "No End!");
}
