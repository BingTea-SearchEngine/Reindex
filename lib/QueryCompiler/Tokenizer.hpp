#pragma once
#include <string>
#include <vector>

enum class TokenType {
    WORD,       // individual words
    QUOTE,      // '"'
    ANDOP,      // AND, &, &&
    OROP,       // OR, |, ||
    LPAREN,     // (
    RPAREN,     // )
    NOTOP,      // NOT, -
    END         // end of input
};

struct Token {
    TokenType type;
    std::string value;
    Token* next = nullptr;
};

class Tokenstream {
public:
    Tokenstream(const std::string& input);

    Token* GetHead() const;
    Token* MatchToken(TokenType typein);

private:
    Token* head;
    Token* end;
    std::vector<std::string> Tokens; // tokenized input in vector form for the ranker

    void Tokenize(const std::string& input);
    void AppendToken(TokenType type, const std::string& value);
    std::vector<std::string> getTokens(); // return the vector of tokens
    
    void PrintTokens();
};
