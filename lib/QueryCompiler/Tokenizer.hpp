#pragma once
#include <string>

enum class TokenType {
    WORD,       // individual words
    QUOTE,      // '"'
    ANDOP,      // AND, &, &&
    OROP,       // OR, |, ||
    LPAREN,     // (
    RPAREN,     // )
    UNARYOP,    // NOT, -, +
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

    void Tokenize(const std::string& input);
    void AppendToken(TokenType type, const std::string& value);
    
    void PrintTokens();
};
