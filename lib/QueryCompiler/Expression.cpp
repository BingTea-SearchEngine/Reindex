#include "Expression.hpp"
#include <sstream>

#include "ISRAnd.hpp"
#include "ISROr.hpp"
#include "ISRPhrase.hpp"
#include "ISRWord.hpp"
#include "ISRContainer.hpp"

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

std::string Constraint::GetString() const{ // print debugging
    if(BCs.size() == 1){ // collapse level if there is only one child
        return BCs[0]->GetString();
    }
    std::stringstream ss;
    ss << "OrISR(";
    for(int i = 0; i < BCs.size(); i++){
        if(i > 0){
            ss << ", ";
        }
        ss << BCs[i]->GetString();
    }
    ss <<")";
    return ss.str();
}
// class Constraint

// <BaseConstraint> ::= <SimpleConstaint> { [ <AndOp> ] <SimpleConstraint> } | <SimpleConstraint> <NotOp> <SimpleConstraint>
BaseConstraint::BaseConstraint(std::vector<Expression*> SCin, std::string typein): SCs(SCin), type(typein) {}

BaseConstraint::~BaseConstraint(){
    for(int i = 0; i < SCs.size(); i++){
        delete SCs[i];
    }
}

ISR* BaseConstraint::Eval() const{
    if(SCs.size() == 1){ // collapse level if there is only one child
        return SCs[0]->Eval();
    }
    std::vector<ISR*> children;
    for(int i = 0; i < SCs.size(); i++){
        children.push_back(SCs[i]->Eval());
    }
    if(type == "And"){
        return new ISRAnd(children);
    }
    else if(type == "Not"){
        return new ISRContainer(children[0], children[1]);
    }
    return nullptr; // should not reach this return
}

std::string BaseConstraint::GetString() const{
    if(SCs.size() == 1){ // collapse level if there is only one child
        return SCs[0]->GetString();
    }
    if(type == "And"){
        std::stringstream ss;
        ss << "AndISR(";
        for(int i = 0; i < SCs.size(); i++){
            if(i > 0){
                ss << ", ";
            }
            ss << SCs[i]->GetString();
        }
        ss <<")";
        return ss.str();
    }
    else if(type == "Not"){ // not ISR only has two fields
        return "NotISR(" + SCs[0]->GetString() + ", " + SCs[1]->GetString() + ")";
    }
}
// class BaseConstraint

// <SimpleConstraint> ::= <Phrase> | <NestedConstraint> | <SearchWord>
SimpleConstraint::SimpleConstraint(Expression* innerin): inner(innerin) {}
   
SimpleConstraint::~SimpleConstraint(){
    delete inner;
}

ISR* SimpleConstraint::Eval() const{ // this just needs to evaluate inner
    return inner->Eval();
}

std::string SimpleConstraint::GetString() const{ // this just needs to evaluate inner
    return inner->GetString();
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

std::string Phrase::GetString() const{ // placeholder for a PHRASE ISR
    if(SWs.size() == 1){ // collapse level if there is only one child
        return SWs[0]->GetString();
    }
    std::stringstream ss;
    ss << "PhraseISR(";
    for(int i = 0; i < SWs.size(); i++){
        if(i > 0){
            ss << ", ";
        }
        ss << SWs[i]->GetString();
    }
    ss <<")";
    return ss.str();
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

std::string NestedConstraint::GetString() const{ // just needs to eval inner
    return inner->GetString();
}
// class NestedConstraint

// <SearchWord> lowest level
SearchWord::SearchWord(std::string valuein, const std::unordered_map<std::string, PostingList>& indexin): value(valuein), index(indexin) {}
        
ISR* SearchWord::Eval() const{
    //return new ISRWord(index.getpostinglist(value)); // TODO: eventually get a posting list from the actual index
    auto it = index.find(value);
    if (it == index.end()) {
        return nullptr; 
    }
    return new ISRWord(it->second);
}

std::string SearchWord::GetString() const{ // placeholder for an OR ISR
    return "WordISR(" + value + ")";
}
// class SearchWord
