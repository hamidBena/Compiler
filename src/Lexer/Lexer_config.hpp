#pragma once
#include <string>
#include <unordered_map>

enum class TokenType {
    // Keywords
    _function, _if, _else, _for, _while, _return,
    _true, _false, _break, _continue, _let,

    // Literals
    int_lit,

    // operators
    _operator,

    // Punctuation
    o_paren, c_paren, o_brace, c_brace, o_bracket,
	c_bracket, semicolon, comma, colon, exclamation,

    // Identifiers
    identifier,

    // Special
    _unknown
};

struct Token {
	TokenType type;
	int line;
	std::string value;
	std::string scoped_value;

	bool is(TokenType expectedType) const {
        return type == expectedType;
    }

	bool isOneOf(std::initializer_list<TokenType> expectedTypes) const {
        for (auto expectedType : expectedTypes) {
            if (type == expectedType) return true;
        }
        return false;
    }

	bool operator==(const Token& other) const {
    return type == other.type && line == other.line && value == other.value;
	}
};


// language keywords:
const std::unordered_map<std::string, TokenType> string_to_type = {
        {"def", TokenType::_function},
        {"if", TokenType::_if},
        {"else", TokenType::_else},
        {"for", TokenType::_for},
        {"while", TokenType::_while},
        {"return", TokenType::_return},
        {"true", TokenType::_true},
        {"false", TokenType::_false},
        {"break", TokenType::_break},
        {"continue", TokenType::_continue},
        {"let", TokenType::_let},
};

// Function that maps a word to a token (for keywords/identifiers)
Token mapStringToToken(const std::string &input, int line) {
    Token token;
    token.line = line;
    token.value = input;

    // Look up in keyword map
    auto it = string_to_type.find(input);
    if (it != string_to_type.end()) {
        token.type = it->second;
    } else {
        token.type = TokenType::identifier;
    }
    return token;
}

bool isOperatorChar(char c) {
    // Add any characters that might appear in your operators.
    static const std::string operatorChars = "+-*/%^<>=!.";
    return operatorChars.find(c) != std::string::npos;
}
