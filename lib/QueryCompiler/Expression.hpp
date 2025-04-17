#pragma once

#include <stdint.h>
#include <vector>
#include <string>


#include "ISR.hpp"
#include "PostingList.hpp"

// base class
class Expression {
public:

   virtual ~Expression();
   virtual ISR* Eval() const = 0; // placeholder for actual ISRs

};
// class Expression

// <Constraint> ::= <BaseConstraint> { <OrOp> <BaseConstraint> }
class Constraint: public Expression {
protected:

   std::vector<Expression*> BCs;

public:

   Constraint(std::vector<Expression*> BCin);
   ~Constraint();
   ISR* Eval() const override;
};
// class Constraint

// <BaseConstraint> ::= <SimpleConstaint> { [ <AndOp> ] <SimpleConstraint> }
class BaseConstraint: public Expression {
protected:

   std::vector<Expression*> SCs;

public:

   BaseConstraint(std::vector<Expression*> SCin);
   ~BaseConstraint();
   ISR* Eval() const override;

};
// class BaseConstraint

// <SimpleConstraint> ::= <Phrase> | <NestedConstraint> | <UnaryOp> <SimpleConstraint> | <SearchWord>
class SimpleConstraint: public Expression {
protected:

   char type; // 'p' for phrase, 'n' for nested, '-'/'+' for unary+simple, 's' for search
   Expression* inner;

public:

   SimpleConstraint(char typein, Expression* innerin);
   ~SimpleConstraint();
   ISR* Eval() const override; // calls a different ISR depending on type

};
// class SimpleConstraint

// <Phrase> ::= '"' { <SearchWord> } '"'
class Phrase: public Expression {
protected:

    std::vector<Expression*> SWs;

public:

    Phrase(std::vector<Expression*> SWin);
    ~Phrase();
    ISR* Eval() const override;

};
// class Phrase

// <NestedConstraint> ::= '(' <Constraint> ')'
class NestedConstraint: public Expression {
protected:

    Expression* inner;

public:

    NestedConstraint(Expression* innerin);
    ~NestedConstraint();
    ISR* Eval() const override;
    
};
// class NestedConstraint

// <SearchWord> lowest level
class SearchWord: public Expression {
protected:

    std::string value;
    const std::unordered_map<std::string, PostingList> index;

public:

    SearchWord(std::string valuein, const std::unordered_map<std::string, PostingList> indexin);
    ISR* Eval() const override;
    
};
// class SearchWord
