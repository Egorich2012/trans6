grammar Example;

// Корневое правило
program: stmtList EOF;

// ─────────────── statements ───────────────
stmt
    : doWhileStmt
    | printStmt
    | block
    ;

block: '{' stmtList '}';

stmtList
    : stmt stmtList
    | 
    ;

// ─────────────── do-while ───────────────
doWhileStmt
    : 'do' stmt 'while' '(' expr ')' ';'
    ;

// ─────────────── print ───────────────
printStmt
    : 'print' '(' argList ')' ';'
    ;

argList
    : expr argListTail
    ;

argListTail
    : ',' expr argListTail
    | 
    ;

// ─────────────── expressions ───────────────
expr
    : unary exprTail
    ;

exprTail
    : relop unary
    | 
    ;

relop
    : '<'
    | '>'
    ;

unary
    : '++' primary
    | '--' primary
    | primary
    ;

primary
    : VAR
    | NUM
    | '(' expr ')'
    ;

// ─────────────── tokens ───────────────
VAR: [a-zA-Z]+;
NUM: [0-9]+;
WS: [ \t\r\n]+ -> skip;