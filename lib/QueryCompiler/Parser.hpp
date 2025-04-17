#pragma once

#include <string>
#include <unordered_map>

#include "Tokenizer.hpp"
#include "Expression.hpp"
#include "PostingList.hpp"

class Parser {
private:
    Tokenstream stream; // tokenstream to parse
    const std::unordered_map<std::string, PostingList> index; // just for testing, input the index

    // Find various parts of the grammar, return nullptr if not found
    Expression *FindConstraint();

    Expression *FindBaseConstraint();

    Expression *FindSimpleConstraint();

    Expression *FindPhrase();

    Expression *FindNestedConstraint();

    Expression *FindSearchWord();

public:
    // construct stream based on a query
    Parser(const std::string &query, const std::unordered_map<std::string, PostingList> indexin);

    // Builds the tree
    Expression *Parse();
};
