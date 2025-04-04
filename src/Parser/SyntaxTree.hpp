#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include "Type.hpp"

inline void printIndent(int indent) {
    for (int i = 0; i < indent; ++i)
        std::cout << "  ";  // Four spaces per indent level
}

// Base class for all AST nodes
class ASTNode {
public:
    virtual ~ASTNode() = default;
    // Default parameter added for indentation
    virtual void print(int indent = 0) const = 0;
    // You can still call print() with no arguments:
    void print() const { print(0); }
};

//level 1
class LiteralExpr : public ASTNode {
	public:
		int value;
		
		LiteralExpr(int val) : value(val) {}
	
		void print(int indent = 0) const override {
			printIndent(indent);
			std::cout << "Literal(" << value << ")" << std::endl;
		}
	};
	
	class VariableExpr : public ASTNode {
	public:
		std::string name;
	
		VariableExpr(const std::string& name) : name(name) {}
	
		void print(int indent = 0) const override {
			printIndent(indent);
			std::cout << "identifier(" << name << ")" << std::endl;
		}
	};
	
	class BinaryExpr : public ASTNode {
	public:
		std::string op;
		ASTNode* left;
		ASTNode* right;
	
		BinaryExpr(const std::string& op, ASTNode* left, ASTNode* right)
			: op(op), left(left), right(right) {}
	
		~BinaryExpr() {
			delete left;
			delete right;
		}
	
		void print(int indent = 0) const override {
			printIndent(indent);
			std::cout << "BinaryExpr(" << op << ")" << std::endl;
			left->print(indent + 1);
			right->print(indent + 1);
		}
	};
	
	class UnaryExpr : public ASTNode {
	public:
		std::string op;
		ASTNode* expr;
		
		UnaryExpr(const std::string& operatorSymbol, ASTNode* expression)
			: op(operatorSymbol), expr(expression) {}
		
		~UnaryExpr() {
			delete expr;
		}
		
		void print(int indent = 0) const override {
			printIndent(indent);
			std::cout << "UnaryExpr(" << op << ")" << std::endl;
			expr->print(indent + 1);
		}
	};
	
	class PostfixExpr : public ASTNode {
	public:
		std::string op;
		ASTNode* operand;
		
		PostfixExpr(const std::string& op, ASTNode* operand) : op(op), operand(operand) {}
		
		~PostfixExpr() { delete operand; }
		
		void print(int indent = 0) const override {
			printIndent(indent);
			std::cout << "PostfixExpr(" << op << ")" << std::endl;
			operand->print(indent + 1);
		}
	};
	
	class PrefixExpr : public ASTNode {
	public:
		std::string op;
		ASTNode* operand;
		
		PrefixExpr(const std::string& op, ASTNode* operand) : op(op), operand(operand) {}
		
		~PrefixExpr() { delete operand; }
		
		void print(int indent = 0) const override {
			printIndent(indent);
			std::cout << "PrefixExpr(" << op << ")" << std::endl;
			operand->print(indent + 1);
		}
	};
	
	class IndexExpr : public ASTNode {
	public:
		ASTNode* target;  // The array or list being indexed
		ASTNode* index;   // The index expression
		
		IndexExpr(ASTNode* target, ASTNode* index)
			: target(target), index(index) {}
		
		~IndexExpr() {
			delete target;
			delete index;
		}
		
		void print(int indent = 0) const override {
			printIndent(indent);
			std::cout << "IndexExpr" << std::endl;
			printIndent(indent + 1);
			std::cout << "Target:" << std::endl;
			target->print(indent + 2);
			printIndent(indent + 1);
			std::cout << "Index:" << std::endl;
			index->print(indent + 2);
		}
	};
	
	class FunctionCallExpr : public ASTNode {
	public:
		ASTNode* callee;  // The function being called
		std::vector<ASTNode*> arguments;
		
		FunctionCallExpr(ASTNode* callee, std::vector<ASTNode*> args)
			: callee(callee), arguments(std::move(args)) {}
		
		~FunctionCallExpr() {
			delete callee;
			for (ASTNode* arg : arguments)
				delete arg;
		}
		
		void print(int indent = 0) const override {
			printIndent(indent);
			std::cout << "FunctionCallExpr" << std::endl;
			printIndent(indent + 1);
			std::cout << "Callee:" << std::endl;
			callee->print(indent + 2);
			printIndent(indent + 1);
			std::cout << "Arguments:" << std::endl;
			if (!arguments.empty()){
				for (ASTNode* arg : arguments)
					arg->print(indent + 2);
			} else {
				printIndent(indent + 2);
				std::cout << "No arguments." << std::endl;
			}
		}
	};
	
	class ClassInstanceExpr : public ASTNode {
	public:
		StructType* structType;  // Type of the struct
		std::unordered_map<std::string, ASTNode*> fieldValues;  // Field initializations
		
		ClassInstanceExpr(StructType* structType, std::unordered_map<std::string, ASTNode*> fieldValues)
			: structType(structType), fieldValues(std::move(fieldValues)) {}
		
		~ClassInstanceExpr() {
			for (auto& field : fieldValues) {
				delete field.second;
			}
		}
		
		void print(int indent = 0) const override {
			printIndent(indent);
			std::cout << "StructInstanceExpr(" << structType->name << ")" << std::endl;
			for (auto& field : fieldValues) {
				printIndent(indent + 1);
				std::cout << field.first << " =" << std::endl;
				field.second->print(indent + 2);
			}
		}
	};
	
	class ClassFieldAccessExpr : public ASTNode {
	public:
		ASTNode* structInstance;  // The struct variable being accessed
		std::string fieldName;    // The field name
		
		ClassFieldAccessExpr(ASTNode* structInstance, const std::string& fieldName)
			: structInstance(structInstance), fieldName(fieldName) {}
		
		~ClassFieldAccessExpr() {
			delete structInstance;
		}
		
		void print(int indent = 0) const override {
			printIndent(indent);
			std::cout << "ClassFieldAccessExpr" << std::endl;
			printIndent(indent + 1);
			std::cout << "Parent:" << std::endl;
			structInstance->print(indent + 2);
			printIndent(indent + 1);
			std::cout << "Field: " << fieldName << std::endl;
		}
	};
	

//level 2
class BlockStmt : public ASTNode {
	public:
		std::vector<ASTNode*> statements;
	
		~BlockStmt() {
			for (ASTNode* stmt : statements) {
				delete stmt;
			}
		}
	
		void addStatement(ASTNode* stmt) {
			statements.push_back(stmt);
		}
	
		void print(int indent = 0) const override {
			printIndent(indent);
			std::cout << "Block {" << std::endl;
			for (ASTNode* stmt : statements) {
				stmt->print(indent + 1);
			}
			printIndent(indent);
			std::cout << "}" << std::endl;
		}
	};
	
	class CompoundStmt : public ASTNode {
	public:
		std::vector<ASTNode*> statements;
		
		CompoundStmt(std::vector<ASTNode*> stmts)
			: statements(std::move(stmts)) {}
		
		~CompoundStmt() {
			for (ASTNode* stmt : statements)
				delete stmt;
		}
		
		void print(int indent = 0) const override {
			printIndent(indent);
			std::cout << "CompoundStmt" << std::endl;
			for (ASTNode* stmt : statements)
				stmt->print(indent + 1);
			std::cout << std::endl;
		}
	};
	
	class ExprStmt : public ASTNode {
	public:
		ASTNode* expr;
		
		ExprStmt(ASTNode* expr) : expr(expr) {}
		
		~ExprStmt() {
			delete expr;
		}
		
		void print(int indent = 0) const override {
			printIndent(indent);
			std::cout << "ExprStmt" << std::endl;
			expr->print(indent + 1);
			std::cout << std::endl;
		}
	};
	
	class IfStmt : public ASTNode {
	public:
		ASTNode* condition;
		ASTNode* thenBranch;
		ASTNode* elseBranch;
		
		IfStmt(ASTNode* cond, ASTNode* thenB, ASTNode* elseB = nullptr)
			: condition(cond), thenBranch(thenB), elseBranch(elseB) {}
		
		~IfStmt() {
			delete condition;
			delete thenBranch;
			if (elseBranch) delete elseBranch;
		}
		
		void print(int indent = 0) const override {
			printIndent(indent);
			std::cout << "IfStmt" << std::endl;
			printIndent(indent + 1);
			std::cout << "Condition:" << std::endl;
			condition->print(indent + 2);
			printIndent(indent + 1);
			std::cout << "Then:" << std::endl;
			thenBranch->print(indent + 2);
			if (elseBranch) {
				printIndent(indent + 1);
				std::cout << "Else:" << std::endl;
				elseBranch->print(indent + 2);
			}
			std::cout << std::endl;
		}
	};
	
	class WhileStmt : public ASTNode {
	public:
		ASTNode* condition;
		ASTNode* body;
		
		WhileStmt(ASTNode* cond, ASTNode* body)
			: condition(cond), body(body) {}
		
		~WhileStmt() {
			delete condition;
			delete body;
		}
		
		void print(int indent = 0) const override {
			printIndent(indent);
			std::cout << "WhileStmt" << std::endl;
			printIndent(indent + 1);
			std::cout << "Condition:" << std::endl;
			condition->print(indent + 2);
			printIndent(indent + 1);
			std::cout << "Body:" << std::endl;
			body->print(indent + 2);
			std::cout << std::endl;
		}
	};
	
	class ForStmt : public ASTNode {
	public:
		ASTNode* initializer;
		ASTNode* condition;
		ASTNode* incrementor;
		ASTNode* body;
	
		ForStmt(ASTNode* init, ASTNode* cond, ASTNode* inc, ASTNode* body)
			: initializer(init), condition(cond), incrementor(inc), body(body) {}
	
		~ForStmt() {
			delete initializer;
			delete condition;
			delete incrementor;
			delete body;
		}
	
		void print(int indent = 0) const override {
			printIndent(indent);
			std::cout << "ForStmt" << std::endl;
			printIndent(indent + 1);
			std::cout << "Initializer:" << std::endl;
			initializer->print(indent + 2);
			printIndent(indent + 1);
			std::cout << "Condition:" << std::endl;
			condition->print(indent + 2);
			printIndent(indent + 1);
			std::cout << "Incrementor:" << std::endl;
			incrementor->print(indent + 2);
			printIndent(indent + 1);
			std::cout << "Body:" << std::endl;
			body->print(indent + 2);
			std::cout << std::endl;
		}
	};
	
	class ReturnStmt : public ASTNode {
	public:
		ASTNode* expression;
		
		ReturnStmt(ASTNode* expr = nullptr)
			: expression(expr) {}
		
		~ReturnStmt() {
			if(expression) delete expression;
		}
		
		void print(int indent = 0) const override {
			printIndent(indent);
			std::cout << "ReturnStmt" << std::endl;
			if (expression) {
				printIndent(indent + 1);
				std::cout << "Expression:" << std::endl;
				expression->print(indent + 2);
			} else {
				printIndent(indent + 1);
				std::cout << "No expression (return)" << std::endl;
			}
			std::cout << std::endl;
		}
	};
	
	class DefinitionStmt : public ASTNode {
	public:
		std::string dataType;
		ASTNode* expression;
		
		DefinitionStmt(ASTNode* exp, std::string type)
			: expression(exp), dataType(type){}
		
		~DefinitionStmt() {
			delete expression;
		}
		
		void print(int indent = 0) const override {
			printIndent(indent);
			std::cout << "DefinitionStmt" << std::endl;
			printIndent(indent + 1);
			std::cout << "dataType: " << dataType << std::endl;
			printIndent(indent + 1);
			std::cout << "Initializer:" << std::endl;
			expression->print(indent + 2);
			std::cout << std::endl;
		}
	};
	
	class BreakStmt : public ASTNode {
	public:
		void print(int indent = 0) const override {
			printIndent(indent);
			std::cout << "BreakStmt" << std::endl;
		}
	};
	
	class ContinueStmt : public ASTNode {
	public:
		void print(int indent = 0) const override {
			printIndent(indent);
			std::cout << "ContinueStmt" << std::endl;
		}
	};
	

//level 3
class Program : public ASTNode {
	public:
		std::vector<ASTNode*> Code;
	
		void print(int indent = 0) const override {
			for(auto& node : Code){
				node->print(indent);
			}
		}
	};
	
	class FunctionDecl : public ASTNode {
	public:
		std::string name;
		std::vector<std::pair<std::string, std::string>> params;
		std::string returnType;
		ASTNode* body;
		
		FunctionDecl(const std::string& name, const std::vector<std::pair<std::string, std::string>>& params,
					 const std::string& returnType, ASTNode* body)
			: name(name), params(params), returnType(returnType), body(body) {}
		
		~FunctionDecl() {
			delete body;
		}
		
		void print(int indent = 0) const override {
			printIndent(indent);
			std::cout << "FunctionDecl(" << name << ")" << std::endl;
			printIndent(indent + 1);
			std::cout << "Return Type: " << returnType << std::endl;
			printIndent(indent + 1);
			std::cout << "Params: ";
			for (auto& param : params) {
				std::cout << param.second << " " << param.first << ", ";
			}
			std::cout << std::endl;
			printIndent(indent + 1);
			std::cout << "Body:" << std::endl;
			body->print(indent + 2);
			std::cout << std::endl;
		}
	};
	
	class ClassDecl : public ASTNode {
	public:
		std::string name;
		StructType* structType;
		
		ClassDecl(const std::string& name, StructType* structType)
			: name(name), structType(structType) {}
		
		~ClassDecl() {
			delete structType;
		}
		
		void print(int indent = 0) const override {
			printIndent(indent);
			std::cout << "Class(" << name << ")" << std::endl;
			structType->print();  // Assuming StructType::print() handles its own formatting.
			std::cout << std::endl;
		}
	};
	