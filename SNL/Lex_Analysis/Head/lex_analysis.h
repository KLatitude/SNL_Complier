#ifndef LEX_ANALYSIS_H_INCLUDED
#define LEX_ANALYSIS_H_INCLUDED

#include <string>
#include <vector>
#include "token.h"
class LexAnalysis {
private:
    std::vector<Token> token_list_;
public:
    std::vector<Token>& getTokenList(std::string source_file);
    void printTokenList(std::string target_file);
};


#endif // LEX_ANALYSIS_H_INCLUDED
