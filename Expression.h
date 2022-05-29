#pragma once
#pragma once
#ifndef EXPRESSION_H
#define EXPRESSION_H
#include <string>
#include <algorithm>
#include "hw3_output.hpp"

using namespace std;

extern int yylineno;

class Expression {
public:
	string name;
	string type;
	bool is_func = false;
	Expression() = default;
	Expression(const string name, const string type): name(name), type(type), is_func(false) {}
	Expression(const string name, const string type, bool is_func): name(name), type(type), is_func(is_func) {}
	virtual ~Expression() = default;
};

#define YYSTYPE Expression*

void checkIfBoolBin(Expression* exp1, Expression* exp2);
void checkIfBoolUn(Expression* exp);
void checkByteSize(int size);
Expression* logicalExpression(Expression* exp1, Expression* exp2 = nullptr);
Expression* handleBinop(Expression* exp1, Expression* exp2, string op);
Expression* handleRelop(Expression* exp1, Expression* exp2);
Expression* handleByte(Expression* exp);
Expression* handleCast(string cast_type, Expression* exp);
void handleCall(Expression* id, Expression* args = nullptr);
void checkID(Expression* id);
void checkIfFuncAlreadyInSymbolTable(Expression* id);
void addArgToFunction(Expression* exp, Expression* arg);
void addArgInDeclaration(Expression* exp, Expression* arg);
string getExpType(Expression* exp);

class ExpressionFunction : public Expression {
public:
	vector<string> args_name;
	vector<string> args_type;
	ExpressionFunction() = default;
	ExpressionFunction(string arg_name, string arg_type) {
		args_name.push_back(arg_name);
		args_type.push_back(arg_type);
	}
	ExpressionFunction(const string name, const string type, bool is_func): Expression(name, type, is_func), args_name(), args_type(){}
	ExpressionFunction(Expression* exp) : Expression() {
		args_name.push_back(exp->name);
		string type = getExpType(exp);
		args_type.push_back(type);
	}
	~ExpressionFunction() override = default;
};

#endif