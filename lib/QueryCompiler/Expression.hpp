#pragma once

#include <stdint.h>
#include <string>
#include <vector>

#include "ISR.hpp"
#include "PostingList.hpp"

// base class
class Expression {
   public:
    virtual ~Expression();
    virtual ISR* Eval() const = 0;  // placeholder for actual ISRs

    virtual std::string GetString()
        const = 0;  // prints ISR structure for debugging
};
// class Expression

// <Constraint> ::= <BaseConstraint> { <OrOp> <BaseConstraint> }
class Constraint : public Expression {
   protected:
    std::vector<Expression*> BCs;

   public:
    Constraint(std::vector<Expression*> BCin);
    ~Constraint();
    ISR* Eval() const override;

    std::string GetString() const override;
};
// class Constraint

// <BaseConstraint> ::= <SimpleConstaint> { [ <AndOp> ] <SimpleConstraint> } | <SimpleConstraint> <NotOp> <SimpleConstraint>
class BaseConstraint : public Expression {
   protected:
    std::vector<Expression*> SCs;
    std::string type;

   public:
    BaseConstraint(std::vector<Expression*> SCin, std::string typein);
    ~BaseConstraint();
    ISR* Eval() const override;

    std::string GetString() const override;
};
// class BaseConstraint

// <SimpleConstraint> ::= <Phrase> | <NestedConstraint> | <SearchWord>
class SimpleConstraint : public Expression {
   protected:
    Expression* inner;

   public:
    SimpleConstraint(Expression* innerin);
    ~SimpleConstraint();
    ISR* Eval() const override;

    std::string GetString() const override;
};
// class SimpleConstraint

// <Phrase> ::= '"' { <SearchWord> } '"'
class Phrase : public Expression {
   protected:
    std::vector<Expression*> SWs;

   public:
    Phrase(std::vector<Expression*> SWin);
    ~Phrase();
    ISR* Eval() const override;

    std::string GetString() const override;
};
// class Phrase

// <NestedConstraint> ::= '(' <Constraint> ')'
class NestedConstraint : public Expression {
   protected:
    Expression* inner;

   public:
    NestedConstraint(Expression* innerin);
    ~NestedConstraint();
    ISR* Eval() const override;

    std::string GetString() const override;
};
// class NestedConstraint

// <SearchWord> lowest level
class SearchWord : public Expression {
   protected:
    std::string value;
    const std::unordered_map<std::string, PostingList>* index;

public:

    SearchWord(std::string valuein, const std::unordered_map<std::string, PostingList>* indexin);
    ISR* Eval() const override;

    std::string GetString() const override;
};
// class SearchWord
