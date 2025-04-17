#include "Expression.hpp"
#include <sstream>

#include "ISRAnd.hpp"
#include "ISROr.hpp"
#include "ISRPhrase.hpp"
#include "ISRWord.hpp"

Expression::~Expression(){}

// <Constraint> ::= <BaseConstraint> { <OrOp> <BaseConstraint> }
Constraint::Constraint(std::vector<Expression*> BCin): BCs(BCin) {}

Constraint::~Constraint(){
    for(int i = 0; i < BCs.size(); i++){
        delete BCs[i];
    }
}

ISR* Constraint::Eval() const{ // placeholder for an OR ISR
    if(BCs.size() == 1){ // collapse level if there is only one child
        return BCs[0]->Eval();
    }
    std::vector<ISR*> children;
    for(int i = 0; i < BCs.size(); i++){
        children.push_back(BCs[i]->Eval());
    }
    return new ISROr(children);
}
// class Constraint

// <BaseConstraint> ::= <SimpleConstaint> { [ <AndOp> ] <SimpleConstraint> }
BaseConstraint::BaseConstraint(std::vector<Expression*> SCin): SCs(SCin) {}

BaseConstraint::~BaseConstraint(){
    for(int i = 0; i < SCs.size(); i++){
        delete SCs[i];
    }
}

ISR* BaseConstraint::Eval() const{ // placeholder for an AND ISR
    if(SCs.size() == 1){ // collapse level if there is only one child
        return SCs[0]->Eval();
    }
    std::vector<ISR*> children;
    for(int i = 0; i < SCs.size(); i++){
        children.push_back(SCs[i]->Eval());
    }
    return new ISRAnd(children);
}
// class BaseConstraint

// <SimpleConstraint> ::= <Phrase> | <NestedConstraint> | <UnaryOp> <SimpleConstraint> | <SearchWord>
SimpleConstraint::SimpleConstraint(char typein, Expression* innerin): type(typein), inner(innerin) {}
   
SimpleConstraint::~SimpleConstraint(){
    delete inner;
}

ISR* SimpleConstraint::Eval() const{ // this just needs to evaluate inner
    if(type == 'p' || type == 'n' || type == 's'){
        return inner->Eval();
    }
    else{
        if(type == '-'){
            return nullptr // TODO: ADD ISRNot support here (eg return new ISRNot(inner->Eval()))
        }
        else{ // '+' is just evaluating inner
            return inner->Eval();
        }
    }
}
// class SimpleConstraint

// <Phrase> ::= '"' { <SearchWord> } '"'
Phrase::Phrase(std::vector<Expression*> SWin): SWs(SWin) {}

Phrase::~Phrase(){
    for(int i = 0; i < SWs.size(); i++){
        delete SWs[i];
    }
}
    
ISR* Phrase::Eval() const{ // placeholder for a PHRASE ISR
    if(SWs.size() == 1){ // collapse level if there is only one child
        return SWs[0]->Eval();
    }
    std::vector<ISR*> children;
    for(int i = 0; i < SWs.size(); i++){
        children.push_back(SWs[i]->Eval());
    }
    return new ISRPhrase(children);
}
// class Phrase

// <NestedConstraint> ::= '(' <Constraint> ')'
NestedConstraint::NestedConstraint(Expression* innerin): inner(innerin) {}

NestedConstraint::~NestedConstraint(){
    delete inner;
}
        
ISR* NestedConstraint::Eval() const{ // just needs to eval inner
    return inner->Eval();
}
// class NestedConstraint

// <SearchWord> lowest level
SearchWord::SearchWord(std::string valuein, const std::unordered_map<std::string, PostingList> indexin): value(valuein), index(indexin) {}
        
ISR* SearchWord::Eval() const{
    //return new ISRWord(index.getpostinglist(value)); // TODO: eventually get a posting list from the actual index
    return new ISRWord(index[value]);
}
// class SearchWord
