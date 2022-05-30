#include "Expression.h"
#include "MainProcess.h"
#include <iostream>
using namespace std;
using namespace output;

string getExpType(Expression* exp) {
    if (exp->type.size()!=0) {
        return exp->type;
    }
    return MainProcess::get_instance().getEntryInSymbolTable(exp->name)->type;
}

void checkIfBoolUn(Expression* exp) {
    string type = getExpType(exp);
    if (type == "BOOL") {
        return;
    }
    errorMismatch(yylineno);
    exit(1);
}

void checkIfBoolBin(Expression* exp1, Expression* exp2) {
    if(!(getExpType(exp2) == "BOOL"&&getExpType(exp1) == "BOOL")) {
        errorMismatch(yylineno);
        exit(1);
    }
}
Expression* logicalExpression(Expression* exp1, Expression* exp2) {
    if (exp2 != nullptr) {
        checkIfBoolBin(exp1, exp2);
    }
    else {
        checkIfBoolUn(exp1);
    }
    return new Expression("", "BOOL");
}
void checkByteSize(int size) {
    if (size < 256) {
        return;
    }
    errorByteTooLarge(yylineno, to_string(size));
    exit(1);
}

void checkIfFuncAlreadyInSymbolTable(Expression* id) {
    SymbolTableEntry* current_entry = MainProcess::get_instance().getEntryInSymbolTable(id->name);
    if (current_entry == nullptr) {
        return;
    }
    errorDef(yylineno, id->name);
    exit(1);
}

Expression* handleBinop(Expression* exp1, Expression* exp2) {
    string expression_type = getExpType(exp1);
    string expression_type1 = getExpType(exp2);
    if (expression_type == "BYTE" && expression_type1 == "BYTE") {
        return new Expression("", "BYTE");
    }
    else if ((expression_type == "INT") && (expression_type1 == "INT" || expression_type1 == "BYTE")) {
        return new Expression("", "INT");
    }
    else if (expression_type == "BYTE" && expression_type1 == "INT") {
        return new Expression("", "INT");
    }
    else {
        errorMismatch(yylineno);
        exit(1);
    }
    return nullptr;
}

Expression* handleCast(string cast_type, Expression* expression) {
    if (cast_type == "BYTE") {
        checkByteSize(stoi(expression->name));
    }
    return new Expression(expression->name, cast_type);
}
Expression* handleRelop(Expression* expression1, Expression* expression2) {
    string expression_type = getExpType(expression1);
    string expression_type1 = getExpType(expression2);
    if ((expression_type == "INT" || expression_type == "BYTE") && (expression_type1 == "BYTE"||expression_type1 == "INT" )) {
        return new Expression("", "BOOL");
    }
    else {
        errorMismatch(yylineno);
        exit(1);
    }
    return nullptr;
}
Expression* handleByte(Expression* expression) {
    if (stoi(expression->name) < 256) {
        return new Expression(expression->name, "BYTE");
    }
    errorByteTooLarge(yylineno, expression->name);
    exit(1);
}

void handleCall(Expression* id, Expression* args) {
    MainProcess& process = MainProcess::get_instance();
    SymbolTableEntry* entry = process.getEntryInSymbolTable(id->name);
    if (entry == nullptr) {
        errorUndefFunc(yylineno, id->name);
        exit(1);
    }
    else if (entry->is_function== false) {
        errorUndefFunc(yylineno, id->name);
        exit(1);
    }
    SymbolTableEntryFunction* function_entry = dynamic_cast<class SymbolTableEntryFunction*>(entry);
    int num_arguments = function_entry->arguments_types.size();
    if (args != nullptr) {
        ExpressionFunction* args_list = dynamic_cast<class ExpressionFunction*>(args);
        if (args_list->arguments_names.size() != num_arguments) {
            errorPrototypeMismatch(yylineno, function_entry->name, function_entry->arguments_types);
            exit(1);
        }
        reverse(args_list->arguments_names.begin(), args_list->arguments_names.end());
        reverse(args_list->arguments_types.begin(), args_list->arguments_types.end());

        for (int i = 0; i < num_arguments; i++) {
            if (function_entry->arguments_types[i] != args_list->arguments_types[i]&&!(((function_entry->arguments_types[i] == "INT") && (args_list->arguments_types[i] == "BYTE"))))
            {
                errorPrototypeMismatch(yylineno, function_entry->name, function_entry->arguments_types);
                exit(1);
            }
        }
    }
    else if (!(num_arguments == 0)) {
        errorPrototypeMismatch(yylineno, function_entry->name, function_entry->arguments_types);
        exit(1);
    }
}

void checkID(Expression* id) {
    MainProcess& process = MainProcess::get_instance();
    SymbolTableEntry* entry = process.getEntryInSymbolTable(id->name);
    if (entry == nullptr) {
        errorUndef(yylineno, id->name);
        exit(1);
    }
    else if (entry->is_function) {
        errorUndef(yylineno, id->name);
        exit(1);
    }
}

void addArgInDeclaration(Expression* args_list, Expression* new_arg) {
    ExpressionFunction* function_args_list = dynamic_cast<class ExpressionFunction*>(args_list);
    if (find(function_args_list->arguments_names.begin(), function_args_list->arguments_names.end(), new_arg->name) != function_args_list->arguments_names.end()) {
        errorDef(yylineno, new_arg->name);
        exit(0);
    }
    function_args_list->arguments_types.push_back(new_arg->type);
    function_args_list->arguments_names.push_back(new_arg->name);
}

void addArgToFunction(Expression* exp, Expression* arg)
{
    ExpressionFunction* function_entry = dynamic_cast<class ExpressionFunction*>(exp);
    function_entry->arguments_names.push_back(arg->name);
    string type = getExpType(arg);
    function_entry->arguments_types.push_back(type);
}


