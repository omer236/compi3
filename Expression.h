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
	bool is_function = false;
	Expression() = default;
	Expression(const string name, const string type): name(name), type(type), is_function(false) {}
	Expression(const string name, const string type, bool is_func): name(name), type(type), is_function(is_func) {}
	virtual ~Expression() = default;
};

#define YYSTYPE Expression*

void checkIfBoolBin(Expression* exp1, Expression* exp2);
void checkIfBoolUn(Expression* exp);
Expression* logicalExpression(Expression* exp1, Expression* exp2 = nullptr);
void checkByteSize(int size);
void checkIfFuncAlreadyInSymbolTable(Expression* id);
Expression* handleBinop(Expression* exp1, Expression* exp2);
Expression* handleCast(string cast_type, Expression* exp);
Expression* handleRelop(Expression* exp1, Expression* exp2);
Expression* handleByte(Expression* exp);
void handleCall(Expression* id, Expression* args = nullptr);
void checkID(Expression* id);
void addArgInDeclaration(Expression* exp, Expression* arg);
void addArgToFunction(Expression* exp, Expression* arg);
string getExpType(Expression* exp);

class ExpressionFunction : public Expression {
public:
	vector<string> arguments_names;
	vector<string> arguments_types;
	ExpressionFunction() = default;
	ExpressionFunction(string arg_name, string arg_type) {
        arguments_names.push_back(arg_name);
        arguments_types.push_back(arg_type);
	}
	ExpressionFunction(const string name, const string type, bool is_func): Expression(name, type, is_func), arguments_names(), arguments_types(){}
	ExpressionFunction(Expression* exp) : Expression() {
        arguments_names.push_back(exp->name);
		string type = getExpType(exp);
        arguments_types.push_back(type);
	}
	~ExpressionFunction() override = default;
};

#endif