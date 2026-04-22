/*
 * File: statement.cpp
 * -------------------
 * This file implements the constructor and destructor for
 * the Statement class itself.  Your implementation must do
 * the same for the subclasses you define for each of the
 * BASIC statements.
 */

#include "statement.hpp"
#include <iostream>
#include <sstream>

/* Implementation of the Statement class */

Statement::Statement() = default;

Statement::~Statement() = default;

/* Implementation of RemStatement */

void RemStatement::execute(EvalState &state, Program &program) {
}

/* Implementation of LetStatement */

LetStatement::LetStatement(Expression *exp) : exp(exp) {}

LetStatement::~LetStatement() {
    delete exp;
}

void LetStatement::execute(EvalState &state, Program &program) {
    exp->eval(state);
}

/* Implementation of PrintStatement */

PrintStatement::PrintStatement(Expression *exp) : exp(exp) {}

PrintStatement::~PrintStatement() {
    delete exp;
}

void PrintStatement::execute(EvalState &state, Program &program) {
    std::cout << exp->eval(state) << std::endl;
}

/* Implementation of InputStatement */

InputStatement::InputStatement(std::string var) : var(var) {}

void InputStatement::execute(EvalState &state, Program &program) {
    while (true) {
        std::cout << " ? " << std::flush;
        std::string input;
        std::getline(std::cin, input);
        try {
            int value = stringToInteger(input);
            state.setValue(var, value);
            break;
        } catch (...) {
            std::cout << "INVALID NUMBER" << std::endl;
        }
    }
}

/* Implementation of EndStatement */

void EndStatement::execute(EvalState &state, Program &program) {
    throw std::runtime_error("END");
}

/* Implementation of GotoStatement */

GotoStatement::GotoStatement(int lineNumber) : lineNumber(lineNumber) {}

void GotoStatement::execute(EvalState &state, Program &program) {
    if (program.getSourceLine(lineNumber).empty()) {
        error("LINE NUMBER ERROR");
    }
    throw lineNumber;
}

/* Implementation of IfStatement */

IfStatement::IfStatement(Expression *lhs, std::string op, Expression *rhs, int lineNumber)
    : lhs(lhs), op(op), rhs(rhs), lineNumber(lineNumber) {}

IfStatement::~IfStatement() {
    delete lhs;
    delete rhs;
}

void IfStatement::execute(EvalState &state, Program &program) {
    int leftVal = lhs->eval(state);
    int rightVal = rhs->eval(state);
    bool condition = false;
    if (op == "=") {
        condition = (leftVal == rightVal);
    } else if (op == "<") {
        condition = (leftVal < rightVal);
    } else if (op == ">") {
        condition = (leftVal > rightVal);
    }
    if (condition) {
        if (program.getSourceLine(lineNumber).empty()) {
            error("LINE NUMBER ERROR");
        }
        throw lineNumber;
    }
}
