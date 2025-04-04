#pragma once

#include <iostream>
#include <vector>
#include <string>

// Base class for all AST nodes
class ASTNode {
public:
    virtual ~ASTNode() = default;
    virtual void print() const = 0;
};

// Literal expression
class LiteralExpr : public ASTNode {
	public:
	int value;
	
	LiteralExpr(int val) : value(val) {}

	void print() const override {
		std::cout << "Literal(" << value << ")" << std::endl;
	}
};

// Variable reference (e.g., x, y)
class VariableExpr : public ASTNode {
public:
    std::string name;

    VariableExpr(const std::string& name) : name(name) {}

    void print() const override {
        std::cout << "Variable(" << name << ")" << std::endl;
    }
};

// Binary expression (e.g., x + y)
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

    void print() const override {
        std::cout << "BinaryExpr(" << op << ")" << std::endl;
        left->print();
        right->print();
    }
};

// Assignment statement (e.g., x = 5)
class AssignmentStmt : public ASTNode {
public:
    std::string varName;
    ASTNode* expr;

    AssignmentStmt(const std::string& varName, ASTNode* expr)
        : varName(varName), expr(expr) {}

    ~AssignmentStmt() {
        delete expr;
    }

    void print() const override {
        std::cout << "Assignment(" << varName << " = )" << std::endl;
        expr->print();
    }
};

// Block statement (holds multiple statements)
class BlockStmt : public ASTNode {
public:
    std::vector<ASTNode*> statements; // Correct usage!

    ~BlockStmt() {
        for (ASTNode* stmt : statements) {
            delete stmt;
        }
    }

    void addStatement(ASTNode* stmt) {
        statements.push_back(stmt);
    }

    void print() const override {
        std::cout << "Block {" << std::endl;
        for (ASTNode* stmt : statements) {
            stmt->print();
        }
        std::cout << "}" << std::endl;
    }
};
