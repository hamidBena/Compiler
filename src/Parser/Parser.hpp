#pragma once

#include "SyntaxTree.hpp"
#include "Lexer.hpp"

#include <vector>
#include <stdexcept>
#include <unordered_set>

// Assuming Token and ASTNode classes are already defined
class Parser {
private:
    std::vector<Token> tokens;
	std::unordered_set<std::string> typeTable;
	std::unordered_set<std::string> primitiveTypeTable = {"float", "void"};
    size_t current = 0; // Tracks current position in the token list

public:
    explicit Parser(std::vector<Token>& tokens) : tokens(tokens) {
		for(auto type : primitiveTypeTable){
			typeTable.emplace(type);
		}
	}

    Program* Parse() {// Entry point for parsing either a function or a class definition
		Program* root = new Program;
		while(!isAtEnd()){
			if(peek().value == "class"){
				root->Code.emplace_back(parseClass());
			}else{
				root->Code.emplace_back(parseFunction()); 
			}
		}
		return root;
    }

private:
    ASTNode* parseStatement() {
        // Example: Detects an assignment statement like "x = 5 + 3;"
        if(check(TokenType::o_brace)){
			return parseCompound();
		}else if(check(TokenType::_if)){
			return parseIf();
		}else if(check(TokenType::_while)){
			return parseWhile();
		}else if(check(TokenType::_for)){
			return parseFor();
		}else if(check(TokenType::_return)){
			return parseReturn();
		}else if(typeTable.find(peek().value) != typeTable.end()){	//if it is a data type, that means it is a variable declaration
			return parseDefinition();
		}else if(check(TokenType::_break) || check(TokenType::_continue)){
			return parseLoopControl();
		}else{
			return parseExpressionStmt();
		}
        throw std::runtime_error("Unexpected token at start of statement.");
    }

	//---------------------------------------------------------------------------------------------------------------------------------------------------------------
	#pragma region expressionHandler
	ASTNode* parseExpression() {
		return parseAssignment();  // Start from the lowest precedence
	}
	
	ASTNode* parseAssignment() {
		ASTNode* left = parseComparison(); // First parse a comparison expression
	
		// Check for assignment operator "=".
		if (check(TokenType::_operator) && peek().value == "=") {
			std::string op = advance().value;  // consume "="
			ASTNode* right = parseAssignment(); // right-associative for assignment
			return new BinaryExpr(op, left, right);
		}
		return left;
	}
	
	ASTNode* parseComparison() {
		ASTNode* left = parseAddition(); // Next, parse addition/subtraction
	
		// Suppose comparison operators are "==", "!=", "<", "<=", ">", ">="
		while (check(TokenType::_operator) &&
			  (peek().value == "==" || peek().value == "!=" ||
			   peek().value == "<"  || peek().value == "<=" ||
			   peek().value == ">"  || peek().value == ">=")) {

			std::string op = advance().value;  // consume the operator
			ASTNode* right = parseAddition();
			left = new BinaryExpr(op, left, right);
		}
		return left;
	}
	
	ASTNode* parseAddition() {
		ASTNode* left = parseMultiplication();  // Higher precedence: multiplication
	
		// Check for addition or subtraction operators
		while (check(TokenType::_operator) &&
			  (peek().value == "+" || peek().value == "-")) {
			std::string op = advance().value;  // consume the operator
			ASTNode* right = parseMultiplication();
			left = new BinaryExpr(op, left, right);
		}
		return left;
	}
	
	ASTNode* parseMultiplication() {
		ASTNode* left = parseUnary();  // Start with a primary expression
	
		// Check for multiplication, division, modulus, or exponentiation operators
		while (check(TokenType::_operator) &&
			  (peek().value == "*" || peek().value == "/" ||
			   peek().value == "%" || peek().value == "^")) {
			std::string op = advance().value;  // consume the operator
			ASTNode* right = parseUnary();
			left = new BinaryExpr(op, left, right);
		}
		return left;
	}

	ASTNode* parseUnary() {
		if (check(TokenType::_operator) && (peek().value == "++" || peek().value == "--")) {
			std::string op = advance().value;
			ASTNode* operand = parseUnary();  // Recursively parse the next expression
			return new PrefixExpr(op, operand);
		}
		return parsePrimary();  // If no prefix operator, parse normally
	}	
	
	ASTNode* parsePrimary() {
		if (match(TokenType::int_lit)) {
			return new LiteralExpr(std::stoi(previous().value));
		}
		if (match(TokenType::identifier)) {
			ASTNode* node = new VariableExpr(previous().value);
	
			// Handle indexing (arr[expr])
			while (check(TokenType::o_bracket)) { // '[' detected
				advance();
				ASTNode* index = parseExpression(); // Parse the index expression
				consume(TokenType::c_bracket, "Expected ']' after index.");
				node = new IndexExpr(node, index); // Wrap in IndexExpr
			}
	
			// Handle member access (obj.field)
			while (check(TokenType::_operator) && peek().value == ".") { // '.' detected
				advance();
				std::string field = advance().value;
				node = new ClassFieldAccessExpr(node, field); // Wrap in MemberAccessExpr
			}

			// Handle postfix expressions (var++/var--)
			while (check(TokenType::_operator) && (peek().value == "++" || peek().value == "--")) {
				advance();
				std::string op = previous().value;
				node = new PostfixExpr(op, node);  // Wrap in PostfixExpr
			}

			while (check(TokenType::o_paren)) { // '(' detected
				advance();
				std::vector<ASTNode*> arguments;
				if (!check(TokenType::c_paren)) {
					do {
						arguments.push_back(parseExpression());
					} while (match(TokenType::comma));
				}
				consume(TokenType::c_paren, "Expected ')' after function call arguments.");
				
				node = new FunctionCallExpr(node, arguments);
			}
	
			return node;
		}
		if (match(TokenType::o_paren)) {  // Handle parenthesized expressions
			ASTNode* expr = parseExpression();
			consume(TokenType::c_paren, "Expected ')' after expression.");
			return expr;
		}
		throw std::runtime_error("Expected a number, variable, or '('.");
	}
	#pragma endregion
	//---------------------------------------------------------------------------------------------------------------------------------------------------------------
	ASTNode* parseFunction() {
		if(typeTable.find(peek().value) == typeTable.end()){
			throw std::runtime_error("Expected return datatype for function.");
		}

		std::string returnType = advance().value;	

		if (check(TokenType::identifier)) {
			std::string functionName = advance().value;
	
			if (!match(TokenType::o_paren)) {
				throw std::runtime_error("Expected '(' after function name.");
			}
	
			std::vector<std::pair<std::string, std::string>> params; // (name, type)
	
			// Parse optional parameters
			if (!check(TokenType::c_paren)) { // If not immediately closed, parse params
				do {
					if (typeTable.find(peek().value) == typeTable.end()) {
						throw std::runtime_error("Expected a type.");
					}
					std::string paramType = advance().value;

					if (!check(TokenType::identifier)) {
						throw std::runtime_error("Expected a type.");
					}
					std::string paramName = advance().value;
	
					params.push_back({paramName, paramType}); // No explicit type in this syntax
	
				} while (match(TokenType::comma));
			}
	
			if (!match(TokenType::c_paren)) {
				throw std::runtime_error("Expected ')' after parameters.");
			}
	
			// Parse function body (assumed to be a statement)
			ASTNode* body = parseStatement();
	
			return new FunctionDecl(functionName, params, returnType, body);
		}
	
		throw std::runtime_error("Unexpected token at start of Function declaration.");
	}
	
	ASTNode* parseCompound() {
		consume(TokenType::o_brace, "Expected '{' at the start of a compound statement.");
	
		std::vector<ASTNode*> statements;
	
		while (!check(TokenType::c_brace) && !isAtEnd()) {
			statements.push_back(parseStatement());
		}

		consume(TokenType::c_brace, "Expected '}' at the end of a compound statement.");
		return new CompoundStmt(statements);
	}

	ASTNode* parseIf() {
		if (!match(TokenType::_if)) {
			throw std::runtime_error("Expected 'if' keyword.");
		}
	
		if (!match(TokenType::o_paren)) {
			throw std::runtime_error("Expected '(' after 'if'.");
		}
	
		ASTNode* condition = parseStatement(); // Parse the condition
	
		if (!match(TokenType::c_paren)) {
			throw std::runtime_error("Expected ')' after condition.");
		}
	
		ASTNode* thenStmt = parseStatement(); // Parse the statement/block after `if`
	
		ASTNode* elseStmt = nullptr;
		if (match(TokenType::_else)) {
			elseStmt = parseStatement(); // Parse the statement/block after `else`
		}
	
		return new IfStmt(condition, thenStmt, elseStmt);
	}
	
	ASTNode* parseWhile() {
		if (!match(TokenType::_while)) {
			throw std::runtime_error("Expected 'while' keyword.");
		}
	
		if (!match(TokenType::o_paren)) {
			throw std::runtime_error("Expected '(' after 'while'.");
		}
	
		ASTNode* condition = parseExpression(); // Parse the condition
	
		if (!match(TokenType::c_paren)) {
			throw std::runtime_error("Expected ')' after condition.");
		}
	
		ASTNode* body = parseStatement(); // Parse the statement/block after `if`
	
		return new WhileStmt(condition, body);
	}

	ASTNode* parseFor() {
		if (!match(TokenType::_for)) {
			throw std::runtime_error("Expected 'for' keyword.");
		}
	
		if (!match(TokenType::o_paren)) {
			throw std::runtime_error("Expected '(' after 'for'.");
		}
		
		ASTNode* initializer = parseExpression();
		consume(TokenType::semicolon, "Expected ; in 'for' loop.");
		ASTNode* condition = parseExpression(); // Parse the condition
		consume(TokenType::semicolon, "Expected ; in 'for' loop.");
		ASTNode* incrementor = parseExpression();

		if (!match(TokenType::c_paren)) {
			throw std::runtime_error("Expected ')' after condition.");
		}
	
		ASTNode* body = parseStatement();
	
		return new ForStmt(initializer, condition, incrementor, body);
	}
    
	ASTNode* parseExpressionStmt() {
		auto ret = parseExpression();
		consume(TokenType::semicolon, "Expected a ;");
		return ret;
	}
	
	ASTNode* parseReturn() {
		consume(TokenType::_return, "Expected a 'return' statement.");
		auto expression = parseExpressionStmt();
		consume(TokenType::semicolon, "Expected ;");
		return new ReturnStmt(expression);
	}

	ASTNode* parseDefinition() {
		std::string datatype = consume(TokenType::identifier, "Expected a datatype").value;
		auto expression = parseExpression();
		consume(TokenType::semicolon, "Expected ;");
		return new DefinitionStmt(expression, datatype);
	}

	ASTNode* parseClass() {
		if (!check(TokenType::identifier)) {
			throw std::runtime_error("Expected a class identifier (name).");
		}
		advance();
		std::string className = advance().value;
		typeTable.emplace(className);
		
		consume(TokenType::o_brace, "Expected '{' to begin class body.");
		
		StructType* structType = new StructType(className);
		
		while (!check(TokenType::c_brace) && !isAtEnd()) {
			std::string fieldType = consume(TokenType::identifier, "Expected a type").value;
			std::string fieldName = consume(TokenType::identifier, "Expected field name.").value;
			consume(TokenType::semicolon, "Expected ';' after field declaration.");

			if(typeTable.find(fieldType) == typeTable.end()){

			}
			PrimitiveType* Type = new PrimitiveType(fieldType);

			structType->addField(fieldName, Type);
		}
		
		// Consume the '}' that ends the class body
		consume(TokenType::c_brace, "Expected '}' after class body.");
		
		// Return a new ClassDecl node with the parsed class name and its struct type definition
		return new ClassDecl(className, structType);
	}

	ASTNode* parseLoopControl() {
		if (match(TokenType::_break)) {
			consume(TokenType::semicolon, "Expected ';' after 'break'.");
			return new BreakStmt();
		}
		if (match(TokenType::_continue)) {
			consume(TokenType::semicolon, "Expected ';' after 'continue'.");
			return new ContinueStmt();
		}
		throw std::runtime_error("Expected a loop control statement ('break' or 'continue').");
	}


	// Helper functions for token navigation
    bool match(TokenType type) {
        if (!isAtEnd() && peek().type == type) {
            advance();
            return true;
        }
        return false;
    }

	bool check(TokenType type){
		if (!isAtEnd() && peek().type == type) {
            return true;
        }
        return false;
	}

    Token consume(TokenType expected, const std::string& errorMessage) {
        if (match(expected)) return previous();
		std::string msg = "Parse Error: " + errorMessage + " [Line: " + std::to_string(tokens[current].line) + "]";
        throw std::runtime_error(msg);
    }

    Token peek() { return tokens[current]; }
    Token previous() { return tokens[current - 1]; }
    Token advance() { return tokens[current++]; }
    bool isAtEnd() { return current >= tokens.size(); }
};
