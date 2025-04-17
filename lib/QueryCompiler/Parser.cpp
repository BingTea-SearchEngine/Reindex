#include <vector>

#include "Parser.hpp"


Parser::Parser(const std::string &query, const std::unordered_map<std::string, PostingList> indexin): stream(query), index(indexin) {}

// <Constraint> ::= <BaseConstraint> { <OrOp> <BaseConstraint> }
Expression* Parser::FindConstraint(){
    std::vector<Expression*> BCs; // accumulate base constraints
    Expression* BC = FindBaseConstraint(); // find first <BaseConstraint>
    if(BC){
        BCs.push_back(BC);
    }
    else{
        return nullptr;
    }
    while(stream.MatchToken(TokenType::OROP)){ // can repeat an arbitrary amount of times
        BC = FindBaseConstraint(); // there needs to be a <BaseConstraint> after every OROP
        if(BC){
            BCs.push_back(BC);
        }
        else{
            return nullptr;
        }
    }
    return new Constraint(BCs); // new constraint of all BCs ORed together
}

// <BaseConstraint> ::= <SimpleConstaint> { [ <AndOp> ] <SimpleConstraint> }
Expression* Parser::FindBaseConstraint(){
    std::vector<Expression*> SCs; // accumulate simple constraints
    Expression* SC = FindSimpleConstraint(); // find first <SimpleConstraint>
    if(SC){
        SCs.push_back(SC);
    }
    else{
        return nullptr;
    }
    while(true){
        if(stream.MatchToken(TokenType::ANDOP)){
            SC = FindSimpleConstraint(); // if there is an ANDOP, a <SimpleConstraint> MUST follow
            if(SC){
                SCs.push_back(SC);
            }
            else{
                return nullptr;
            }
        }
        else if(SC = FindSimpleConstraint()){ // it is also legal to have another simple constraint with no ANDOP inbetween
            SCs.push_back(SC);
        }
        else{
            break; // break out of the loop if we hit anything else
        }
    }
    return new BaseConstraint(SCs); // new Base Constraint of all SCs ANDed together
}

// <SimpleConstraint> ::= <Phrase> | <NestedConstraint> | <UnaryOp> <SimpleConstraint> | <SearchWord>
Expression* Parser::FindSimpleConstraint() {
    char type = 'p';
    Expression* inner = FindPhrase();
    if(!inner){
        inner = FindNestedConstraint();
        type = 'n';
    } 
    if(!inner){
        inner = FindSearchWord();
        type = 's';
    } 
    if(inner){
        return new SimpleConstraint(type, inner);
    }

    Token* t = stream.MatchToken(TokenType::UNARYOP);
    if(t){
        inner = FindSimpleConstraint();
        if(inner){
            return new SimpleConstraint(t->value[0], inner); // value is either '+' or '-'
        }
        return nullptr;
    }

    return nullptr;
}

// <Phrase> ::= '"' { <SearchWord> } '"'
Expression* Parser::FindPhrase() {
    Token* openQuote = stream.MatchToken(TokenType::QUOTE);
    if(!openQuote){
        return nullptr;
    }
    std::vector<Expression*> SWs;
    while(true){
        Token* closingQuote = stream.MatchToken(TokenType::QUOTE);
        if(closingQuote){
            break;
        }
        Token* endToken = stream.MatchToken(TokenType::END);
        if(endToken){
            return nullptr;
        }
        Expression* sw = FindSearchWord();
        if(!sw){
            return nullptr;
        }
        SWs.push_back(sw);
    }
    return new Phrase(SWs);
}


// <NestedConstraint> ::= '(' <Constraint> ')'
Expression* Parser::FindNestedConstraint(){
    if(stream.MatchToken(TokenType::LPAREN)){
        Expression* inner = FindConstraint();
        if(inner){
            if(stream.MatchToken(TokenType::RPAREN)){
                return new NestedConstraint(inner);
            }
        }
    }
    return nullptr;
}

// <SearchWord> lowest level
Expression* Parser::FindSearchWord(){
    Token* t = stream.MatchToken(TokenType::WORD);
    if(t){
        return new SearchWord(t->value, index);
    }
    return nullptr;
}

Expression* Parser::Parse(){
    Expression* expr = FindConstraint();
    if(!stream.GetHead() || stream.GetHead()->type != TokenType::END){
        return nullptr;
    }
    return expr;
}

