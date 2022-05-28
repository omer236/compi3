#pragma once
#pragma once
#ifndef EXPRESSION_H
#define EXPRESSION_H
#include <string>
#include <algorithm>
#include "hw3_output.hpp"

// TODO: CHECK IF NEED TO REPLACE THIS TO yylloc.first_line
extern int yylineno;

class Expression {
public:
	std::string name;
	std::string type;
	bool is_func = false;
	bool is_const = false;
	Expression() = default;
	Expression(bool is_const) : is_const(is_const) {}
	Expression(const std::string name, const std::string type) :name(name), type(type), is_func(false), is_const(false) {}
	Expression(const std::string name, const std::string type, bool is_func) :name(name), type(type), is_const(false), is_func(is_func) {}
	Expression(const std::string name, const std::string type, bool is_const, bool is_func) : name(name), type(type), is_const(is_const), is_func(is_func) {}
	virtual ~Expression() = default;
};

#define YYSTYPE Expression*

void checkTypeBool(Expression* exp);
void checkTypeBool(Expression* exp1, Expression* exp2);
void checkByteSize(int size);
Expression* handleLogical(Expression* exp1, Expression* exp2 = nullptr);
Expression* handleBinop(Expression* exp1, Expression* exp2, std::string op);
Expression* handleRelop(Expression* exp1, Expression* exp2);
Expression* handleByte(Expression* exp);
Expression* handleCast(std::string cast_type, Expression* exp);
void handleCall(Expression* id, Expression* args = nullptr);
void handleID(Expression* id);
void checkFuncInSymbolTable(Expression* id);
void addArgInCall(Expression* exp, Expression* arg);
void addArgInDeclaration(Expression* exp, Expression* arg);
std::string find_type(Expression* exp);

class ExpressionFunction : public Expression {
public:
	std::vector<std::string> args_name;
	std::vector<std::string> args_type;
	std::vector<bool> args_is_const;
	ExpressionFunction() = default;
	ExpressionFunction(std::string arg_name, std::string arg_type, bool arg_is_const) {
		args_name.push_back(arg_name);
		args_type.push_back(arg_type);
		args_is_const.push_back(arg_is_const);
	}
	ExpressionFunction(const std::string name, const std::string type, bool is_const, bool is_func) :Expression(name, type, is_const, is_func), args_name(), args_type(), args_is_const() {}
	ExpressionFunction(Expression* exp) : Expression() {
		args_name.push_back(exp->name);
		std::string type = find_type(exp);
		args_type.push_back(type);
		args_is_const.push_back(exp->is_const);
	}
	~ExpressionFunction() override = default;
};

#endif