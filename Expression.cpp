#include "Expression.h"
#include "MainProcess.h"
#include <iostream>
using namespace std;

string getExpType(Expression* exp) {
    if (!(exp->type.empty())) {
        return exp->type;
    }
    MainProcess& process = MainProcess::get_instance();
    return process.getEntryInSymbolTable(exp->name)->type;
}

void checkIfBoolUn(Expression* exp) {
    string type = getExpType(exp);
    if (type == "BOOL") {
        return;
    }
    output::errorMismatch(yylineno);
    exit(1);
}

void checkIfBoolBin(Expression* exp1, Expression* exp2) {
    if(!(getExpType(exp1) == "BOOL" && getExpType(exp2) == "BOOL")) {
        output::errorMismatch(yylineno);
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
    if (size <= 255) {
        return;
    }
    output::errorByteTooLarge(yylineno, to_string(size));
    exit(1);
}

void checkIfFuncAlreadyInSymbolTable(Expression* id) {
    MainProcess& process = MainProcess::get_instance();
    SymbolTableEntry* entry = process.getEntryInSymbolTable(id->name);
    if (entry == nullptr) {
        return;
    }
    output::errorDef(yylineno, id->name);
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
        output::errorMismatch(yylineno);
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
        output::errorMismatch(yylineno);
        exit(1);
    }
    return nullptr;
}
Expression* handleByte(Expression* expression) {
    if (stoi(expression->name) <= 255) {
        return new Expression(expression->name, "BYTE");
    }
    output::errorByteTooLarge(yylineno, expression->name);
    exit(1);
}



void handleCall(Expression* id, Expression* args) {
    MainProcess& process = MainProcess::get_instance();
    SymbolTableEntry* entry = process.getEntryInSymbolTable(id->name);
    if (entry == nullptr) {
        output::errorUndefFunc(yylineno, id->name);
        exit(1);
    }
    else if (!entry->is_function) {
        output::errorUndefFunc(yylineno, id->name);
        exit(1);
    }
    SymbolTableEntryFunction* function_entry = dynamic_cast<class SymbolTableEntryFunction*>(entry);
    int number_of_args = function_entry->arguments_types.size();
    if (args != nullptr) {
        ExpressionFunction* args_list = dynamic_cast<class ExpressionFunction*>(args);
        if (args_list->arguments_names.size() != number_of_args) {
            output::errorPrototypeMismatch(yylineno, function_entry->name, function_entry->arguments_types);
            exit(1);
        }

        reverse(args_list->arguments_names.begin(), args_list->arguments_names.end());
        reverse(args_list->arguments_types.begin(), args_list->arguments_types.end());

        for (int i = 0; i < number_of_args; i++) {
            if (function_entry->arguments_types[i] != args_list->arguments_types[i])
            {
                if((function_entry->arguments_types[i] == "INT") && (args_list->arguments_types[i] == "BYTE"))
                {
                    continue;
                }
                output::errorPrototypeMismatch(yylineno, function_entry->name, function_entry->arguments_types);
                exit(1);
            }
        }
    }
    if (args == nullptr && number_of_args != 0) {
        output::errorPrototypeMismatch(yylineno, function_entry->name, function_entry->arguments_types);
        exit(1);
    }
}

void checkID(Expression* id) {
    MainProcess& process = MainProcess::get_instance();
    SymbolTableEntry* entry = process.getEntryInSymbolTable(id->name);
    if (entry == nullptr) {
        output::errorUndef(yylineno, id->name);
        exit(1);
    }
    else if (entry->is_function) {
        output::errorUndef(yylineno, id->name);
        exit(1);
    }
}

void addArgInDeclaration(Expression* args_list, Expression* new_arg) {
    ExpressionFunction* function_args_list = dynamic_cast<class ExpressionFunction*>(args_list);
    if (find(function_args_list->arguments_names.begin(), function_args_list->arguments_names.end(), new_arg->name) != function_args_list->arguments_names.end()) {
        output::errorDef(yylineno, new_arg->name);
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


