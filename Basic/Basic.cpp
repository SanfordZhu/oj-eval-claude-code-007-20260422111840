/*
 * File: Basic.cpp
 * ---------------
 * This file is the starter project for the BASIC interpreter.
 */

#include <cctype>
#include <iostream>
#include <string>
#include <map>
#include "exp.hpp"
#include "parser.hpp"
#include "program.hpp"
#include "Utils/error.hpp"
#include "Utils/tokenScanner.hpp"
#include "Utils/strlib.hpp"

/* Function prototypes */

void processLine(std::string line, Program &program, EvalState &state);
void runProgram(Program &program, EvalState &state);
Statement *parseStatement(TokenScanner &scanner, std::string keyword = "");

/* Main program */

int main() {
    EvalState state;
    Program program;
    while (true) {
        try {
            std::string input;
            getline(std::cin, input);
            if (input.empty())
                continue;
            processLine(input, program, state);
        } catch (ErrorException &ex) {
            std::cout << ex.getMessage() << std::endl;
        } catch (std::runtime_error &ex) {
            if (std::string(ex.what()) == "QUIT") {
                break;
            }
        }
    }
    return 0;
}

/*
 * Function: processLine
 * Usage: processLine(line, program, state);
 * -----------------------------------------
 * Processes a single line entered by the user.
 */

void processLine(std::string line, Program &program, EvalState &state) {
    TokenScanner scanner;
    scanner.ignoreWhitespace();
    scanner.scanNumbers();
    scanner.setInput(line);

    if (!scanner.hasMoreTokens()) {
        return;
    }

    std::string firstToken = scanner.nextToken();

    TokenType type = scanner.getTokenType(firstToken);

    if (type == NUMBER) {
        int lineNumber = stringToInteger(firstToken);
        if (!scanner.hasMoreTokens()) {
            program.removeSourceLine(lineNumber);
        } else {
            program.addSourceLine(lineNumber, line);
        }
        return;
    }

    std::string keyword = toUpperCase(firstToken);

    if (keyword == "QUIT") {
        throw std::runtime_error("QUIT");
    } else if (keyword == "LIST") {
        program.list();
    } else if (keyword == "CLEAR") {
        program.clear();
        state.Clear();
    } else if (keyword == "RUN") {
        runProgram(program, state);
    } else if (keyword == "HELP") {
        std::cout << "BASIC commands: REM, LET, PRINT, INPUT, END, GOTO, IF, THEN, RUN, LIST, CLEAR, QUIT" << std::endl;
    } else if (keyword == "REM") {
    } else if (keyword == "LET") {
        Statement *stmt = parseStatement(scanner, keyword);
        stmt->execute(state, program);
        delete stmt;
    } else if (keyword == "PRINT") {
        Statement *stmt = parseStatement(scanner, keyword);
        stmt->execute(state, program);
        delete stmt;
    } else if (keyword == "INPUT") {
        Statement *stmt = parseStatement(scanner, keyword);
        stmt->execute(state, program);
        delete stmt;
    } else if (keyword == "END") {
        throw std::runtime_error("END");
    } else if (keyword == "GOTO") {
        error("SYNTAX ERROR");
    } else if (keyword == "IF") {
        error("SYNTAX ERROR");
    } else {
        error("SYNTAX ERROR");
    }
}

void runProgram(Program &program, EvalState &state) {
    int currentLine = program.getFirstLineNumber();
    while (currentLine != -1) {
        std::string line = program.getSourceLine(currentLine);
        TokenScanner scanner;
        scanner.ignoreWhitespace();
        scanner.scanNumbers();
        scanner.setInput(line);

        Statement *stmt = program.getParsedStatement(currentLine);
        if (stmt == nullptr) {
            if (!scanner.hasMoreTokens()) {
                currentLine = program.getNextLineNumber(currentLine);
                continue;
            }
            std::string firstToken = scanner.nextToken();
            if (scanner.getTokenType(firstToken) == NUMBER) {
                stmt = parseStatement(scanner);
            } else {
                scanner.saveToken(firstToken);
                stmt = parseStatement(scanner);
            }
            program.setParsedStatement(currentLine, stmt);
        }

        try {
            stmt->execute(state, program);
            currentLine = program.getNextLineNumber(currentLine);
        } catch (int lineNumber) {
            currentLine = lineNumber;
        } catch (std::runtime_error &ex) {
            if (std::string(ex.what()) == "END") {
                break;
            }
            throw;
        }
    }
}

Statement *parseStatement(TokenScanner &scanner, std::string keyword) {
    if (keyword.empty()) {
        if (!scanner.hasMoreTokens()) {
            error("SYNTAX ERROR");
        }
        keyword = toUpperCase(scanner.nextToken());
    }

    if (keyword == "REM") {
        return new RemStatement();
    } else if (keyword == "LET") {
        Expression *exp = parseExp(scanner);
        return new LetStatement(exp);
    } else if (keyword == "PRINT") {
        Expression *exp = parseExp(scanner);
        return new PrintStatement(exp);
    } else if (keyword == "INPUT") {
        if (!scanner.hasMoreTokens()) {
            error("SYNTAX ERROR");
        }
        std::string var = scanner.nextToken();
        if (scanner.getTokenType(var) != WORD) {
            error("SYNTAX ERROR");
        }
        return new InputStatement(var);
    } else if (keyword == "END") {
        return new EndStatement();
    } else if (keyword == "GOTO") {
        if (!scanner.hasMoreTokens()) {
            error("SYNTAX ERROR");
        }
        std::string token = scanner.nextToken();
        if (scanner.getTokenType(token) != NUMBER) {
            error("SYNTAX ERROR");
        }
        return new GotoStatement(stringToInteger(token));
    } else if (keyword == "IF") {
        Expression *lhs = readEWithoutComparison(scanner, 0);
        if (!scanner.hasMoreTokens()) {
            error("SYNTAX ERROR");
        }
        std::string op = scanner.nextToken();
        if (op != "=" && op != "<" && op != ">") {
            error("SYNTAX ERROR");
        }
        Expression *rhs = readEWithoutComparison(scanner, 0);
        if (!scanner.hasMoreTokens()) {
            error("SYNTAX ERROR");
        }
        std::string thenToken = toUpperCase(scanner.nextToken());
        if (thenToken != "THEN") {
            error("SYNTAX ERROR");
        }
        if (!scanner.hasMoreTokens()) {
            error("SYNTAX ERROR");
        }
        std::string lineToken = scanner.nextToken();
        if (scanner.getTokenType(lineToken) != NUMBER) {
            error("SYNTAX ERROR");
        }
        int lineNumber = stringToInteger(lineToken);
        return new IfStatement(lhs, op, rhs, lineNumber);
    } else {
        error("SYNTAX ERROR");
    }
    error("SYNTAX ERROR");
    return nullptr;
}

