#include <cctype>
#include <sstream>
#include <algorithm>
#include <iostream>

#include "Tokenizer.hpp"


Tokenstream::Tokenstream(const std::string &input){
    head = nullptr;
    end = nullptr;
    Tokenize(input);
}

Token* Tokenstream::GetHead() const{
    return head;
}

Token* Tokenstream::MatchToken(TokenType typein){
    // returns and consumes the head token if it matches the type
    if (head && head->type == typein){
        Token* current = head;
        head = head->next;
        current->next = nullptr;
        return current;
    }
    return nullptr;
}

void Tokenstream::AppendToken(TokenType type, const std::string& value){
    Token* t = new Token{type, value};

    if(!head && !end){ // first token
        head = t;
        end = t;
    }
    else{
        end->next = t;
        end = t;
    }

}

void Tokenstream::Tokenize(const std::string& input){
    size_t i = 0;
    while(i < input.size()){
        char c = input[i];
        if (std::isspace(c)) {
            ++i;
            continue;
        }
        if(std::isalpha(c)){
            std::string word;
            while(i < input.size() && std::isalnum(input[i])){
                word += input[i++];
            }
            if(word == "AND"){
                AppendToken(TokenType::ANDOP, "");
                Tokens.push_back("AND");
            } 
            else if(word == "OR"){
                AppendToken(TokenType::OROP, "");
                Tokens.push_back("OR");
            } 
            else if(word == "NOT"){
                AppendToken(TokenType::NOTOP, "-");
                Tokens.push_back("NOT");
            }
            else{
                AppendToken(TokenType::WORD, word);
                Tokens.push_back(word);
            }
        }
        else{
            switch(c){
                case '&':
                    if(i + 1 < input.size() && input[i + 1] == '&'){ // skip possible second &
                        i++;
                    }
                    AppendToken(TokenType::ANDOP, "");
                    Tokens.push_back("AND");
                    break;
                case '|':
                    if(i + 1 < input.size() && input[i + 1] == '|'){
                        i++;
                    }
                    AppendToken(TokenType::OROP, "");
                    Tokens.push_back("OR");
                    break;
                case '-':
                    AppendToken(TokenType::NOTOP, "-");
                    Tokens.push_back("NOT");
                    break;
                case '"':
                    AppendToken(TokenType::QUOTE, "");
                    Tokens.push_back("QUOTE");
                    break;
                case '(':
                    AppendToken(TokenType::LPAREN, ""); // ranker does not need to know where parentheses are
                    break;
                case ')':
                    AppendToken(TokenType::RPAREN, "");
                    break;
                default: 
                {
                    std::string word;
                    while (i < input.size() && !std::isspace(input[i]) && !std::ispunct(input[i])) {
                        word += input[i++];
                    }
                    if (!word.empty()) {
                        AppendToken(TokenType::WORD, word);
                        continue;
                    }
                }
            }
            i++;
        }
    }
    AppendToken(TokenType::END, "");
}

std::vector<std::string> Tokenstream::getTokens(){
    return Tokens;
}

// debugging function
void Tokenstream::PrintTokens(){
    Token* curr = head;
    while(curr->type != TokenType::END){
        if(curr->type == TokenType::OROP){
            std::cout<<" OR";
        }
        else if(curr->type == TokenType::QUOTE){
            std::cout<<" \"";
        }
        else if(curr->type == TokenType::ANDOP){
            std::cout<<" AND";
        }
        else if(curr->type == TokenType::LPAREN){
            std::cout<<" (";
        }
        else if(curr->type == TokenType::RPAREN){
            std::cout<<" )";
        }
        else if(curr->type == TokenType::NOTOP){
            std::cout<<" NOT";
        }
        else if(curr->type == TokenType::WORD){
            std::cout<<" "<<curr->value;
        }
        curr = curr->next;
    }
    std::cout<<std::endl;
}