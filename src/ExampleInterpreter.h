#pragma once

#include <any>
#include <iostream>
#include "ExampleBaseVisitor.h"

class ExampleInterpreter : public ExampleBaseVisitor {
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

    // stmt
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

    // doWhileStmt
    std::any visitDoWhileStmt(ExampleParser::DoWhileStmtContext *ctx) override {
        loopCount++;

        visit(ctx->stmt()); // тело
        visit(ctx->expr()); // условие

        return 0;
    }

    // printStmt
    std::any visitPrintStmt(ExampleParser::PrintStmtContext *ctx) override {
        printCount++;

        visit(ctx->argList());
        return 0;
    }

    // argList
    std::any visitArgList(ExampleParser::ArgListContext *ctx) override {
        visit(ctx->expr());
        if (ctx->argListTail())
            visit(ctx->argListTail());
        return 0;
    }

    std::any visitArgListTail(ExampleParser::ArgListTailContext *ctx) override {
        if (ctx->expr()) {
            visit(ctx->expr());
            if (ctx->argListTail())
                visit(ctx->argListTail());
        }
        return 0;
    }

    // expr (минимально — просто обходим)
    std::any visitExpr(ExampleParser::ExprContext *ctx) override {
        visit(ctx->unary());
        if (ctx->exprTail())
            visit(ctx->exprTail());
        return 0;
    }

    std::any visitExprTail(ExampleParser::ExprTailContext *ctx) override {
        if (ctx->unary())
            visit(ctx->unary());
        return 0;
    }

    std::any visitUnary(ExampleParser::UnaryContext *ctx) override {
        return visit(ctx->primary());
    }

    std::any visitPrimary(ExampleParser::PrimaryContext *ctx) override {
        return 0;
    }
};