#pragma once
#include "ExampleBaseVisitor.h"
#include <any>
#include <string>
#include <map>
#include <stdexcept>

class ExampleInterpreter : public ExampleBaseVisitor {
private:
    std::map<std::string, int> vars; // хранилище переменных

public:
    int printCount = 0;
    int loopCount = 0;

    // program: stmtList EOF
    std::any visitProgram(ExampleParser::ProgramContext *ctx) override {
        visit(ctx->stmtList());
        std::cout << "Prints: " << printCount << std::endl;
        std::cout << "Loops: " << loopCount << std::endl;
        return 0;
    }

    // stmtList: stmt stmtList | ε
    std::any visitStmtList(ExampleParser::StmtListContext *ctx) override {
        if (ctx->stmt()) {
            visit(ctx->stmt());
            if (ctx->stmtList())
                visit(ctx->stmtList());
        }
        return 0;
    }

    // stmt: doWhileStmt | printStmt | block
    std::any visitStmt(ExampleParser::StmtContext *ctx) override {
        if (ctx->doWhileStmt())
            return visit(ctx->doWhileStmt());
        if (ctx->printStmt())
            return visit(ctx->printStmt());
        if (ctx->block())
            return visit(ctx->block());
        return 0;
    }

    // block: { stmtList }
    std::any visitBlock(ExampleParser::BlockContext *ctx) override {
        return visit(ctx->stmtList());
    }

    // doWhileStmt: 'do' stmt 'while' '(' expr ')' ';'
    std::any visitDoWhileStmt(ExampleParser::DoWhileStmtContext *ctx) override {
        loopCount++;
        do {
            visit(ctx->stmt());
        } while (std::any_cast<bool>(visit(ctx->expr())));
        return 0;
    }

    // printStmt: 'print' '(' argList ')' ';'
    std::any visitPrintStmt(ExampleParser::PrintStmtContext *ctx) override {
        printCount++;
        visit(ctx->argList());
        return 0;
    }

    // argList: expr argListTail
    std::any visitArgList(ExampleParser::ArgListContext *ctx) override {
        std::cout << std::any_cast<int>(visit(ctx->expr()));
        if (ctx->argListTail())
            visit(ctx->argListTail());
        return 0;
    }

    // argListTail: ',' expr argListTail | ε
    std::any visitArgListTail(ExampleParser::ArgListTailContext *ctx) override {
        if (ctx->expr()) {
            std::cout << " " << std::any_cast<int>(visit(ctx->expr()));
            if (ctx->argListTail())
                visit(ctx->argListTail());
        }
        return 0;
    }

    // expr: unary exprTail
    std::any visitExpr(ExampleParser::ExprContext *ctx) override {
        int left = std::any_cast<int>(visit(ctx->unary()));
        if (ctx->exprTail()) {
            std::string op = ctx->exprTail()->relop()->getText();
            int right = std::any_cast<int>(visit(ctx->exprTail()->unary()));
            if (op == "<") return left < right;
            if (op == ">") return left > right;
        }
        return left;
    }

    // unary: '++' primary | '--' primary | primary
    std::any visitUnary(ExampleParser::UnaryContext *ctx) override {
        if (ctx->primary()) {
            std::string var = ctx->primary()->getText();
            if (ctx->getChild(0)->getText() == "++") {
                return ++vars[var];
            } else if (ctx->getChild(0)->getText() == "--") {
                return --vars[var];
            } else {
                return std::any_cast<int>(visit(ctx->primary()));
            }
        }
        return 0;
    }

    // primary: VAR | NUM | '(' expr ')'
    std::any visitPrimary(ExampleParser::PrimaryContext *ctx) override {
        if (ctx->VAR()) {
            std::string var = ctx->VAR()->getText();
            if (vars.find(var) == vars.end()) vars[var] = 0;
            return vars[var];
        }
        if (ctx->NUM()) {
            return std::stoi(ctx->NUM()->getText());
        }
        if (ctx->expr()) {
            return std::any_cast<int>(visit(ctx->expr()));
        }
        return 0;
    }
};