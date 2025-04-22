#include <vector>

#include "Parser.hpp"

Parser::Parser(const std::string& query,
               const std::unordered_map<std::string, PostingList>* indexin)
    : stream(query), index(indexin) {}

// <Constraint> ::= <BaseConstraint> { <OrOp> <BaseConstraint> }
Expression* Parser::FindConstraint() {
    std::vector<Expression*> BCs;           // accumulate base constraints
    Expression* BC = FindBaseConstraint();  // find first <BaseConstraint>
    if (BC) {
        BCs.push_back(BC);
    } else {
        return nullptr;
    }
    while (stream.MatchToken(TokenType::OROP)) {  // can repeat an arbitrary amount of times
        BC = FindBaseConstraint();  // there needs to be a <BaseConstraint> after every OROP
        if (BC) {
            BCs.push_back(BC);
        } else {
            return nullptr;
        }
    }
    return new Constraint(BCs);  // new constraint of all BCs ORed together
}

// <BaseConstraint> ::= <SimpleConstraint> { [ <AndOp> ] <SimpleConstraint> } | <SimpleConstraint> <NotOp> <SimpleConstraint>
Expression* Parser::FindBaseConstraint() {
    std::vector<Expression*> SCs;             // accumulate simple constraints
    Expression* SC = FindSimpleConstraint();  // find first <SimpleConstraint>
    if (SC) {
        SCs.push_back(SC);
    } else {  // fail instantly if there is no opening <SimpleConstraint>
        return nullptr;
    }
    if (stream.MatchToken(TokenType::NOTOP)) {
        Expression* right = FindSimpleConstraint();
        if (right) {
            SCs.push_back(right);
            return new BaseConstraint(SCs, "Not");
        } else {
            return nullptr;
        }
    }
    while (true) {
        if (stream.MatchToken(TokenType::ANDOP)) {
            SC = FindSimpleConstraint();  // if there is an ANDOP, a <SimpleConstraint> MUST follow
            if (SC) {
                SCs.push_back(SC);
            } else {
                return nullptr;
            }
        } else if (
            SC =
                FindSimpleConstraint()) {  // it is also legal to have another simple constraint with no ANDOP inbetween
            SCs.push_back(SC);
        } else {
            break;  // break out of the loop if we hit anything else
        }
    }
    return new BaseConstraint(SCs, "And");  // new Base Constraint of all SCs ANDed together
}

// <SimpleConstraint> ::= <Phrase> | <NestedConstraint> | <SearchWord>
Expression* Parser::FindSimpleConstraint() {
    Expression* inner = FindPhrase();
    if (!inner) {
        inner = FindNestedConstraint();
    }
    if (!inner) {
        inner = FindSearchWord();
    }
    if (inner) {
        return new SimpleConstraint(inner);
    }
    return nullptr;
}

// <Phrase> ::= '"' { <SearchWord> } '"'
Expression* Parser::FindPhrase() {
    const Token* openQuote = stream.MatchToken(TokenType::QUOTE);
    if (!openQuote) {
        return nullptr;
    }
    std::vector<Expression*> SWs;
    while (true) {
        const Token* closingQuote = stream.MatchToken(TokenType::QUOTE);
        if (closingQuote) {
            break;
        }
        const Token* endToken = stream.MatchToken(TokenType::END);
        if (endToken) {
            return nullptr;
        }
        Expression* sw = FindSearchWord();
        if (!sw) {
            return nullptr;
        }
        SWs.push_back(sw);
    }
    return new Phrase(SWs);
}

// <NestedConstraint> ::= '(' <Constraint> ')'
Expression* Parser::FindNestedConstraint() {
    if (stream.MatchToken(TokenType::LPAREN)) {
        Expression* inner = FindConstraint();
        if (inner) {
            if (stream.MatchToken(TokenType::RPAREN)) {
                return new NestedConstraint(inner);
            }
        }
    }
    return nullptr;
}

// <SearchWord> lowest level
Expression* Parser::FindSearchWord() {
    const Token* t = stream.MatchToken(TokenType::WORD);
    if (t) {
        return new SearchWord(t->value, index);
    }
    return nullptr;
}

Expression* Parser::Parse() {
    Expression* expr = FindConstraint();
    if (!stream.GetCurrent() || stream.GetCurrent()->type != TokenType::END) {
        return nullptr;
    }
    return expr;
}
