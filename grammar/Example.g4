grammar Example;

program: stmtList EOF;

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

doWhileStmt
    : 'do' stmt 'while' '(' expr ')' ';'
    ;

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

VAR: [a-zA-Z]+;
NUM: [0-9]+;
WS: [ \t\r\n]+ -> skip;