#pragma once
#include <string>
#include <vector>
#include <unordered_set>
#include <regex>

const static std::regex validWordRegex("^[a-zA-Z0-9_+\\-.]*[a-zA-Z][a-zA-Z0-9_+\\-.]*$");
const static std::unordered_set<std::string> skipWords = {
    "a",     "an",     "the",     "and",     "or",    "but",   "if",      "then",   "else",
    "when",  "where",  "while",   "at",      "by",    "for",   "to",      "in",     "on",
    "from",  "up",     "down",    "with",    "about", "as",    "into",    "like",   "over",
    "after", "before", "between", "without", "under", "again", "further", "once",   "just",
    "only",  "same",   "so",      "too",     "very",  "can",   "will",    "should", "would",
    "could", "is",     "am",      "are",     "was",   "were",  "be",      "been",   "being",
    "have",  "has",    "had",     "do",      "does",  "did",   "not",     "i"};

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
    ~Tokenstream();

    Token* GetCurrent() const;
    const Token* MatchToken(TokenType typein);

private:
    Token* head;
    Token* end;
    Token* current;
    std::vector<std::string> Tokens; // tokenized input in vector form for the ranker

    void Tokenize(const std::string& input);
    void AppendToken(TokenType type, const std::string& value);
    std::vector<std::string> getTokens(); // return the vector of tokens
    
    void PrintTokens();
};
