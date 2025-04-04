#pragma once

#include <string>
#include <optional>
#include <vector>
#include <iostream>
#include "Lexer_config.hpp"

class Lexer{
public:
	std::vector<Token> tokens;

void clear_tokens(){
	tokens.clear();
}

std::vector<Token> tokenize(const std::vector<std::string>& file) {
    std::vector<Token> tokens;
    int lineNum = 1;

    for (const std::string& line : file) {
        size_t i = 0;
        while (i < line.size()) {
            char c = line[i];

            // Skip whitespace
            if (std::isspace(c)) {
                ++i;
                continue;
            }

            // Handle words (identifiers or keywords)
            if (std::isalpha(c)) {
                size_t start = i;
                while (i < line.size() && (std::isalnum(line[i]) || line[i] == '_')) {
                    ++i;
                }
                std::string word = line.substr(start, i - start);
                tokens.push_back(mapStringToToken(word, lineNum));
                continue;
            }

            // Handle integer literals
            if (std::isdigit(c)) {
                size_t start = i;
                while (i < line.size() && (std::isdigit(line[i]) || line[i] == '.')) {
                    ++i;
                }
                std::string number = line.substr(start, i - start);
                Token token;
                token.line = lineNum;
                token.value = number;
                token.type = TokenType::int_lit;
                tokens.push_back(token);
                continue;
            }

            // Handle operators
            if (isOperatorChar(c)) {
                size_t start = i;
                while (i < line.size() && isOperatorChar(line[i])) {
                    ++i;
                }
                std::string op = line.substr(start, i - start);
                Token token;
                token.line = lineNum;
                token.value = op;
				token.type = TokenType::_operator;
                tokens.push_back(token);
                continue;
            }

            // Handle punctuation and operators (single-character tokens for now)
            {
                Token token;
                token.line = lineNum;
                token.value = std::string(1, c);

                switch(c) {
                    case '(': token.type = TokenType::o_paren; break;
                    case ')': token.type = TokenType::c_paren; break;
                    case '{': token.type = TokenType::o_brace; break;
                    case '}': token.type = TokenType::c_brace; break;
                    case '[': token.type = TokenType::o_bracket; break;
                    case ']': token.type = TokenType::c_bracket; break;
                    case ';': token.type = TokenType::semicolon; break;
                    case ',': token.type = TokenType::comma; break;
                    case ':': token.type = TokenType::colon; break;
                    default:
                        token.type = TokenType::_unknown;
                        break;
                }
                tokens.push_back(token);
                ++i; // Move past the current character
            }
        }
        ++lineNum;
    }
	return tokens;
}
};

void print_tokens(std::vector<Token>& tokens){
	for (const Token& token : tokens) {
		std::cout << "Line " << token.line << ": " << token.value
				<< " (Type: " << static_cast<int>(token.type) << ")\n";
	}
}