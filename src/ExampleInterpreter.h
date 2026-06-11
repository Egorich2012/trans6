#pragma once

#include <any>
#include <map>
#include <vector>
#include <string>
#include <iostream>
#include <stdexcept>
#include "ExampleBaseVisitor.h"

class Interpreter : public ExampleBaseVisitor {
public:
    std::map<std::string, int> variables;

    std::any visitProgram(ExampleParser::ProgramContext *ctx) override {
        visit(ctx->stmtList());
        return 0;
    }

    std::any visitStmtList(ExampleParser::StmtListContext *ctx) override {
        for (auto s : ctx->stmt()) visit(s);
        return 0;
    }

    std::any visitStmt(ExampleParser::StmtContext *ctx) override {
        if (ctx->doWhileStmt()) return visit(ctx->doWhileStmt());
        if (ctx->printStmt()) return visit(ctx->printStmt());
        if (ctx->block()) return visit(ctx->block());
        return 0;
    }

    std::any visitBlock(ExampleParser::BlockContext *ctx) override {
        return visit(ctx->stmtList());
    }

    std::any visitDoWhileStmt(ExampleParser::DoWhileStmtContext *ctx) override {
        do {
            visit(ctx->stmt());
        } while (std::any_cast<int>(visit(ctx->expr())) != 0);
        return 0;
    }

    std::any visitPrintStmt(ExampleParser::PrintStmtContext *ctx) override {
        auto args = std::any_cast<std::vector<int>>(visit(ctx->argList()));
        for (size_t i = 0; i < args.size(); ++i) {
            std::cout << args[i] << (i + 1 < args.size() ? " " : "");
        }
        std::cout << std::endl;
        return 0;
    }

    std::any visitArgList(ExampleParser::ArgListContext *ctx) override {
        std::vector<int> result;
        result.push_back(std::any_cast<int>(visit(ctx->expr())));
        auto tail = std::any_cast<std::vector<int>>(visit(ctx->argListTail()));
        result.insert(result.end(), tail.begin(), tail.end());
        return result;
    }

    std::any visitArgListTail(ExampleParser::ArgListTailContext *ctx) override {
        std::vector<int> result;
        if (ctx->expr()) {
            result.push_back(std::any_cast<int>(visit(ctx->expr())));
            auto tail = std::any_cast<std::vector<int>>(visit(ctx->argListTail()));
            result.insert(result.end(), tail.begin(), tail.end());
        }
        return result;
    }

    std::any visitExpr(ExampleParser::ExprContext *ctx) override {
        int left = std::any_cast<int>(visit(ctx->unary()));
        auto tail = ctx->exprTail();
        if (tail->relop() == nullptr) return left;
        
        std::string op = tail->relop()->getText();
        int right = std::any_cast<int>(visit(tail->unary()));
        
        if (op == "<") return (left < right) ? 1 : 0;
        if (op == ">") return (left > right) ? 1 : 0;
        return 0;
    }

    std::any visitUnary(ExampleParser::UnaryContext *ctx) override {
        if (ctx->children.size() == 2 && ctx->primary() != nullptr) {
            std::string op = ctx->children[0]->getText();
            auto primaryCtx = ctx->primary();
            if (primaryCtx->VAR()) {
                std::string name = primaryCtx->VAR()->getText();
                if (variables.find(name) == variables.end())
                    throw std::runtime_error("Undefined variable: " + name);
                if (op == "++") {
                    variables[name]++;
                } else if (op == "--") {
                    variables[name]--;
                }
                return variables[name];
            }
        }

        return visit(ctx->primary());
    }

    std::any visitPrimary(ExampleParser::PrimaryContext *ctx) override {
        if (ctx->VAR()) {
            std::string name = ctx->VAR()->getText();
            if (variables.find(name) == variables.end())
                throw std::runtime_error("Undefined variable: " + name);
            return variables[name];
        }
        if (ctx->NUM()) {
            return std::stoi(ctx->NUM()->getText());
        }
        return visit(ctx->expr());
    }
};
