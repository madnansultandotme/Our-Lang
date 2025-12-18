#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <sstream>
#include <fstream>
#include <cctype>
#include <stdexcept>

// ============================================================================
// Token Types and Lexer
// ============================================================================

enum class TokenType {
    // Keywords
    BANAO, KAAM, AGAR, WARNAH, DAURA, WAPAS, DEKH, LOU, HAAN, NA, BAND,
    // Literals and Identifiers
    NUMBER, STRING, IDENTIFIER,
    // Operators
    PLUS, MINUS, STAR, SLASH, PERCENT, ASSIGN,
    PLUS_ASSIGN, MINUS_ASSIGN, STAR_ASSIGN, SLASH_ASSIGN,
    EQ, NE, LT, GT, LE, GE, AND, OR, NOT,
    // Delimiters
    LPAREN, RPAREN, LBRACE, RBRACE, LBRACKET, RBRACKET,
    SEMICOLON, COMMA, COLON, DOT,
    // Special
    EOF_TOKEN, UNKNOWN
};

struct Token {
    TokenType type;
    std::string value;
    int line;
    int column;

    Token(TokenType t = TokenType::UNKNOWN, const std::string& v = "", int l = 0, int c = 0)
        : type(t), value(v), line(l), column(c) {}
};

class Lexer {
private:
    std::string source;
    size_t pos;
    int line, column;

    std::unordered_map<std::string, TokenType> keywords = {
        {"banao", TokenType::BANAO},
        {"kaam", TokenType::KAAM},
        {"agar", TokenType::AGAR},
        {"warnah", TokenType::WARNAH},
        {"daura", TokenType::DAURA},
        {"wapas", TokenType::WAPAS},
        {"dekh", TokenType::DEKH},
        {"lou", TokenType::LOU},
        {"haan", TokenType::HAAN},
        {"na", TokenType::NA},
        {"band", TokenType::BAND}
    };

public:
    Lexer(const std::string& src) : source(src), pos(0), line(1), column(1) {}

    Token nextToken() {
        skipWhitespaceAndComments();

        if (pos >= source.length()) {
            return Token(TokenType::EOF_TOKEN, "", line, column);
        }

        int tokenLine = line;
        int tokenCol = column;
        char ch = source[pos];

        // String literals
        if (ch == '\'' || ch == '"') {
            return scanString(ch);
        }

        // Numbers
        if (std::isdigit(ch)) {
            return scanNumber();
        }

        // Identifiers and keywords
        if (std::isalpha(ch) || ch == '_') {
            return scanIdentifierOrKeyword();
        }

        // Single and multi-character operators
        pos++;
        column++;

        switch (ch) {
            case '+': return source[pos] == '=' ? (pos++, column++, Token(TokenType::PLUS_ASSIGN, "+=", tokenLine, tokenCol))
                                                  : Token(TokenType::PLUS, "+", tokenLine, tokenCol);
            case '-': return source[pos] == '=' ? (pos++, column++, Token(TokenType::MINUS_ASSIGN, "-=", tokenLine, tokenCol))
                                                  : Token(TokenType::MINUS, "-", tokenLine, tokenCol);
            case '*': return source[pos] == '=' ? (pos++, column++, Token(TokenType::STAR_ASSIGN, "*=", tokenLine, tokenCol))
                                                  : Token(TokenType::STAR, "*", tokenLine, tokenCol);
            case '/': return source[pos] == '=' ? (pos++, column++, Token(TokenType::SLASH_ASSIGN, "/=", tokenLine, tokenCol))
                                                  : Token(TokenType::SLASH, "/", tokenLine, tokenCol);
            case '%': return Token(TokenType::PERCENT, "%", tokenLine, tokenCol);
            case '=': return source[pos] == '=' ? (pos++, column++, Token(TokenType::EQ, "==", tokenLine, tokenCol))
                                                  : Token(TokenType::ASSIGN, "=", tokenLine, tokenCol);
            case '!': return source[pos] == '=' ? (pos++, column++, Token(TokenType::NE, "!=", tokenLine, tokenCol))
                                                  : Token(TokenType::NOT, "!", tokenLine, tokenCol);
            case '<': return source[pos] == '=' ? (pos++, column++, Token(TokenType::LE, "<=", tokenLine, tokenCol))
                                                  : Token(TokenType::LT, "<", tokenLine, tokenCol);
            case '>': return source[pos] == '=' ? (pos++, column++, Token(TokenType::GE, ">=", tokenLine, tokenCol))
                                                  : Token(TokenType::GT, ">", tokenLine, tokenCol);
            case '&': return source[pos] == '&' ? (pos++, column++, Token(TokenType::AND, "&&", tokenLine, tokenCol))
                                                  : Token(TokenType::UNKNOWN, "&", tokenLine, tokenCol);
            case '|': return source[pos] == '|' ? (pos++, column++, Token(TokenType::OR, "||", tokenLine, tokenCol))
                                                  : Token(TokenType::UNKNOWN, "|", tokenLine, tokenCol);
            case '(': return Token(TokenType::LPAREN, "(", tokenLine, tokenCol);
            case ')': return Token(TokenType::RPAREN, ")", tokenLine, tokenCol);
            case '{': return Token(TokenType::LBRACE, "{", tokenLine, tokenCol);
            case '}': return Token(TokenType::RBRACE, "}", tokenLine, tokenCol);
            case '[': return Token(TokenType::LBRACKET, "[", tokenLine, tokenCol);
            case ']': return Token(TokenType::RBRACKET, "]", tokenLine, tokenCol);
            case ';': return Token(TokenType::SEMICOLON, ";", tokenLine, tokenCol);
            case ',': return Token(TokenType::COMMA, ",", tokenLine, tokenCol);
            case ':': return Token(TokenType::COLON, ":", tokenLine, tokenCol);
            case '.': return Token(TokenType::DOT, ".", tokenLine, tokenCol);
            default:  return Token(TokenType::UNKNOWN, std::string(1, ch), tokenLine, tokenCol);
        }
    }

private:
    void skipWhitespaceAndComments() {
        while (pos < source.length()) {
            if (std::isspace(source[pos])) {
                if (source[pos] == '\n') {
                    line++;
                    column = 1;
                } else {
                    column++;
                }
                pos++;
            } else if (source[pos] == '/' && pos + 1 < source.length() && source[pos + 1] == '/') {
                // Skip comment
                while (pos < source.length() && source[pos] != '\n') {
                    pos++;
                }
            } else {
                break;
            }
        }
    }

    Token scanString(char quote) {
        int tokenLine = line;
        int tokenCol = column;
        pos++;
        column++;
        std::string value;

        while (pos < source.length() && source[pos] != quote) {
            if (source[pos] == '\n') {
                line++;
                column = 1;
            } else {
                column++;
            }
            value += source[pos];
            pos++;
        }

        if (pos < source.length()) {
            pos++;
            column++;
        }

        return Token(TokenType::STRING, value, tokenLine, tokenCol);
    }

    Token scanNumber() {
        int tokenLine = line;
        int tokenCol = column;
        std::string value;

        while (pos < source.length() && (std::isdigit(source[pos]) || source[pos] == '.')) {
            value += source[pos];
            pos++;
            column++;
        }

        return Token(TokenType::NUMBER, value, tokenLine, tokenCol);
    }

    Token scanIdentifierOrKeyword() {
        int tokenLine = line;
        int tokenCol = column;
        std::string value;

        while (pos < source.length() && (std::isalnum(source[pos]) || source[pos] == '_')) {
            value += source[pos];
            pos++;
            column++;
        }

        auto it = keywords.find(value);
        if (it != keywords.end()) {
            return Token(it->second, value, tokenLine, tokenCol);
        }

        return Token(TokenType::IDENTIFIER, value, tokenLine, tokenCol);
    }
};

// ============================================================================
// Type System
// ============================================================================

enum class DataType {
    UNKNOWN, NUMBER, STRING, BOOLEAN, ARRAY, OBJECT, VOID, NIL
};

std::string dataTypeToString(DataType type) {
    switch (type) {
        case DataType::NUMBER: return "number";
        case DataType::STRING: return "string";
        case DataType::BOOLEAN: return "boolean";
        case DataType::ARRAY: return "array";
        case DataType::OBJECT: return "object";
        case DataType::VOID: return "void";
        case DataType::NIL: return "nil";
        default: return "unknown";
    }
}

// ============================================================================
// Symbol Table and Scope Management
// ============================================================================

struct Symbol {
    std::string name;
    DataType type;
    bool isFunction;
    bool isInitialized;
    std::vector<DataType> paramTypes;
    DataType returnType;

    Symbol() : name(""), type(DataType::UNKNOWN), isFunction(false), isInitialized(false), returnType(DataType::VOID) {}

    Symbol(const std::string& n, DataType t, bool func = false, bool init = true)
        : name(n), type(t), isFunction(func), isInitialized(init), returnType(DataType::VOID) {}
};

class SymbolTable {
private:
    std::vector<std::unordered_map<std::string, Symbol>> scopes;

public:
    SymbolTable() {
        scopes.push_back(std::unordered_map<std::string, Symbol>());
        initBuiltins();
    }

    void enterScope() {
        scopes.push_back(std::unordered_map<std::string, Symbol>());
    }

    void exitScope() {
        if (scopes.size() > 1) {
            scopes.pop_back();
        }
    }

    bool define(const std::string& name, DataType type, bool isFunc = false, bool isInit = true) {
        // Check if already defined in current scope
        if (scopes.back().find(name) != scopes.back().end()) {
            return false; // Already defined
        }
        scopes.back()[name] = Symbol(name, type, isFunc, isInit);
        return true;
    }

    bool lookup(const std::string& name, Symbol& symbol) {
        for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
            auto found = it->find(name);
            if (found != it->end()) {
                symbol = found->second;
                return true;
            }
        }
        return false;
    }

    bool update(const std::string& name) {
        for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
            auto found = it->find(name);
            if (found != it->end()) {
                found->second.isInitialized = true;
                return true;
            }
        }
        return false;
    }

    void addFunctionSignature(const std::string& name, const std::vector<DataType>& params, DataType returnType) {
        Symbol sym(name, DataType::VOID, true);
        sym.paramTypes = params;
        sym.returnType = returnType;
        scopes[0][name] = sym;
    }

private:
    void initBuiltins() {
        // Built-in functions
        addFunctionSignature("dekh", {DataType::UNKNOWN}, DataType::VOID);
        addFunctionSignature("lou", {DataType::STRING}, DataType::NUMBER);
        addFunctionSignature("nikal", {DataType::UNKNOWN}, DataType::NUMBER);
        addFunctionSignature("band", {}, DataType::VOID);
        addFunctionSignature("abs", {DataType::NUMBER}, DataType::NUMBER);
        addFunctionSignature("sqrt", {DataType::NUMBER}, DataType::NUMBER);
        addFunctionSignature("pow", {DataType::NUMBER, DataType::NUMBER}, DataType::NUMBER);
        addFunctionSignature("max", {DataType::NUMBER, DataType::NUMBER}, DataType::NUMBER);
        addFunctionSignature("min", {DataType::NUMBER, DataType::NUMBER}, DataType::NUMBER);
        addFunctionSignature("round", {DataType::NUMBER}, DataType::NUMBER);
        addFunctionSignature("random", {}, DataType::NUMBER);
    }
};

// ============================================================================
// AST Node Definitions
// ============================================================================

struct ASTNode {
    virtual ~ASTNode() = default;
    virtual DataType getType() const { return DataType::UNKNOWN; }
};

struct Expression : public ASTNode {
    DataType type = DataType::UNKNOWN;
    virtual DataType getType() const override { return type; }
};

struct NumberLiteral : public Expression {
    double value;
    NumberLiteral(double v) : value(v) { type = DataType::NUMBER; }
};

struct StringLiteral : public Expression {
    std::string value;
    StringLiteral(const std::string& v) : value(v) { type = DataType::STRING; }
};

struct BooleanLiteral : public Expression {
    bool value;
    BooleanLiteral(bool v) : value(v) { type = DataType::BOOLEAN; }
};

struct Identifier : public Expression {
    std::string name;
    Identifier(const std::string& n) : name(n) {}
};

struct BinaryOp : public Expression {
    std::unique_ptr<Expression> left;
    std::string op;
    std::unique_ptr<Expression> right;

    BinaryOp(std::unique_ptr<Expression> l, const std::string& o, std::unique_ptr<Expression> r)
        : left(std::move(l)), op(o), right(std::move(r)) {}
};

struct UnaryOp : public Expression {
    std::string op;
    std::unique_ptr<Expression> operand;

    UnaryOp(const std::string& o, std::unique_ptr<Expression> expr)
        : op(o), operand(std::move(expr)) {}
};

struct Assignment : public Expression {
    std::string name;
    std::unique_ptr<Expression> value;

    Assignment(const std::string& n, std::unique_ptr<Expression> v)
        : name(n), value(std::move(v)) {}
};

struct FunctionCall : public Expression {
    std::string name;
    std::vector<std::unique_ptr<Expression>> args;

    FunctionCall(const std::string& n) : name(n) {}
};

struct ArrayLiteral : public Expression {
    std::vector<std::unique_ptr<Expression>> elements;

    ArrayLiteral() { type = DataType::ARRAY; }
};

struct ObjectLiteral : public Expression {
    std::vector<std::pair<std::string, std::unique_ptr<Expression>>> members;

    ObjectLiteral() { type = DataType::OBJECT; }
};

struct ArrayAccess : public Expression {
    std::string arrayName;
    std::unique_ptr<Expression> index;

    ArrayAccess(const std::string& n, std::unique_ptr<Expression> idx)
        : arrayName(n), index(std::move(idx)) {}
};

struct Statement : public ASTNode {
};

struct VariableDeclaration : public Statement {
    std::string name;
    std::unique_ptr<Expression> initializer;

    VariableDeclaration(const std::string& n, std::unique_ptr<Expression> init)
        : name(n), initializer(std::move(init)) {}
};

struct FunctionDeclaration : public Statement {
    std::string name;
    std::vector<std::string> params;
    std::vector<std::unique_ptr<Statement>> body;

    FunctionDeclaration(const std::string& n) : name(n) {}
};

struct IfStatement : public Statement {
    std::unique_ptr<Expression> condition;
    std::vector<std::unique_ptr<Statement>> thenBranch;
    std::vector<std::unique_ptr<Statement>> elseBranch;

    IfStatement(std::unique_ptr<Expression> cond)
        : condition(std::move(cond)) {}
};

struct LoopStatement : public Statement {
    std::unique_ptr<Expression> condition;
    std::vector<std::unique_ptr<Statement>> body;

    LoopStatement(std::unique_ptr<Expression> cond)
        : condition(std::move(cond)) {}
};

struct ReturnStatement : public Statement {
    std::unique_ptr<Expression> value;

    ReturnStatement(std::unique_ptr<Expression> val = nullptr)
        : value(std::move(val)) {}
};

struct ExpressionStatement : public Statement {
    std::unique_ptr<Expression> expr;

    ExpressionStatement(std::unique_ptr<Expression> e)
        : expr(std::move(e)) {}
};

struct Program : public ASTNode {
    std::vector<std::unique_ptr<Statement>> statements;
};

// ============================================================================
// Parser (Recursive Descent)
// ============================================================================

class Parser {
private:
    std::vector<Token> tokens;
    size_t current;

public:
    Parser(const std::vector<Token>& toks) : tokens(toks), current(0) {}

    std::unique_ptr<Program> parse() {
        auto program = std::make_unique<Program>();

        while (!isAtEnd()) {
            if (peek().type == TokenType::EOF_TOKEN) break;

            auto stmt = parseStatement();
            if (stmt) {
                program->statements.push_back(std::move(stmt));
            }
        }

        return program;
    }

private:
    std::unique_ptr<Statement> parseStatement() {
        if (match(TokenType::BANAO)) {
            return parseVariableDeclaration();
        }
        if (match(TokenType::KAAM)) {
            return parseFunctionDeclaration();
        }
        if (match(TokenType::AGAR)) {
            return parseIfStatement();
        }
        if (match(TokenType::DAURA)) {
            return parseLoopStatement();
        }
        if (match(TokenType::WAPAS)) {
            return parseReturnStatement();
        }
        if (check(TokenType::LBRACE)) {
            consume(TokenType::LBRACE, "Expected '{'");
            auto stmts = std::make_unique<std::vector<std::unique_ptr<Statement>>>();
            while (!check(TokenType::RBRACE) && !isAtEnd()) {
                if (auto stmt = parseStatement()) {
                    stmts->push_back(std::move(stmt));
                }
            }
            consume(TokenType::RBRACE, "Expected '}'");
            return nullptr; // Block statements handled differently
        }

        // Handle built-in function calls without assignment (like dekh, band, etc.)
        if (check(TokenType::IDENTIFIER) || check(TokenType::DEKH) || check(TokenType::LOU) ||
            check(TokenType::BAND)) {
            return parseExpressionStatement();
        }

        return parseExpressionStatement();
    }

    std::unique_ptr<Statement> parseVariableDeclaration() {
        Token nameToken = consume(TokenType::IDENTIFIER, "Expected identifier");
        std::unique_ptr<Expression> initializer = nullptr;

        if (match(TokenType::ASSIGN)) {
            initializer = parseExpression();
        }

        consume(TokenType::SEMICOLON, "Expected ';' after variable declaration");
        return std::make_unique<VariableDeclaration>(nameToken.value, std::move(initializer));
    }

    std::unique_ptr<Statement> parseFunctionDeclaration() {
        Token nameToken = consume(TokenType::IDENTIFIER, "Expected function name");
        auto func = std::make_unique<FunctionDeclaration>(nameToken.value);

        consume(TokenType::LPAREN, "Expected '(' after function name");
        if (!check(TokenType::RPAREN)) {
            do {
                Token param = consume(TokenType::IDENTIFIER, "Expected parameter name");
                func->params.push_back(param.value);
            } while (match(TokenType::COMMA));
        }
        consume(TokenType::RPAREN, "Expected ')' after parameters");

        consume(TokenType::LBRACE, "Expected '{' before function body");
        while (!check(TokenType::RBRACE) && !isAtEnd()) {
            if (auto stmt = parseStatement()) {
                func->body.push_back(std::move(stmt));
            }
        }
        consume(TokenType::RBRACE, "Expected '}' after function body");

        return func;
    }

    std::unique_ptr<Statement> parseIfStatement() {
        consume(TokenType::LPAREN, "Expected '(' after 'agar'");
        auto condition = parseExpression();
        consume(TokenType::RPAREN, "Expected ')' after if condition");

        auto ifStmt = std::make_unique<IfStatement>(std::move(condition));

        consume(TokenType::LBRACE, "Expected '{' before if body");
        while (!check(TokenType::RBRACE) && !isAtEnd()) {
            if (auto stmt = parseStatement()) {
                ifStmt->thenBranch.push_back(std::move(stmt));
            }
        }
        consume(TokenType::RBRACE, "Expected '}' after if body");

        if (match(TokenType::WARNAH)) {
            consume(TokenType::LBRACE, "Expected '{' before else body");
            while (!check(TokenType::RBRACE) && !isAtEnd()) {
                if (auto stmt = parseStatement()) {
                    ifStmt->elseBranch.push_back(std::move(stmt));
                }
            }
            consume(TokenType::RBRACE, "Expected '}' after else body");
        }

        return ifStmt;
    }

    std::unique_ptr<Statement> parseLoopStatement() {
        consume(TokenType::LPAREN, "Expected '(' after 'daura'");
        auto condition = parseExpression();
        consume(TokenType::RPAREN, "Expected ')' after loop condition");

        auto loopStmt = std::make_unique<LoopStatement>(std::move(condition));

        consume(TokenType::LBRACE, "Expected '{' before loop body");
        while (!check(TokenType::RBRACE) && !isAtEnd()) {
            if (auto stmt = parseStatement()) {
                loopStmt->body.push_back(std::move(stmt));
            }
        }
        consume(TokenType::RBRACE, "Expected '}' after loop body");

        return loopStmt;
    }

    std::unique_ptr<Statement> parseReturnStatement() {
        std::unique_ptr<Expression> value = nullptr;

        if (!check(TokenType::SEMICOLON)) {
            value = parseExpression();
        }

        consume(TokenType::SEMICOLON, "Expected ';' after return statement");
        return std::make_unique<ReturnStatement>(std::move(value));
    }

    std::unique_ptr<Statement> parseExpressionStatement() {
        auto expr = parseExpression();
        consume(TokenType::SEMICOLON, "Expected ';' after expression statement");
        return std::make_unique<ExpressionStatement>(std::move(expr));
    }

    std::unique_ptr<Expression> parseExpression() {
        return parseAssignment();
    }

    std::unique_ptr<Expression> parseAssignment() {
        auto expr = parseLogicalOr();

        if (match(TokenType::ASSIGN)) {
            if (auto id = dynamic_cast<Identifier*>(expr.get())) {
                auto value = parseAssignment();
                return std::make_unique<Assignment>(id->name, std::move(value));
            } else {
                throw std::runtime_error("Invalid assignment target");
            }
        } else if (match(TokenType::PLUS_ASSIGN) || match(TokenType::MINUS_ASSIGN) ||
                   match(TokenType::STAR_ASSIGN) || match(TokenType::SLASH_ASSIGN)) {
            if (auto id = dynamic_cast<Identifier*>(expr.get())) {
                std::string op = previous().value;
                op = op.substr(0, op.length() - 1); // Remove '='
                auto value = parseAssignment();
                auto binOp = std::make_unique<BinaryOp>(std::move(expr), op, std::move(value));
                return std::make_unique<Assignment>(id->name, std::move(binOp));
            }
        }

        return expr;
    }

    std::unique_ptr<Expression> parseLogicalOr() {
        auto left = parseLogicalAnd();

        while (match(TokenType::OR)) {
            std::string op = previous().value;
            auto right = parseLogicalAnd();
            left = std::make_unique<BinaryOp>(std::move(left), op, std::move(right));
        }

        return left;
    }

    std::unique_ptr<Expression> parseLogicalAnd() {
        auto left = parseEquality();

        while (match(TokenType::AND)) {
            std::string op = previous().value;
            auto right = parseEquality();
            left = std::make_unique<BinaryOp>(std::move(left), op, std::move(right));
        }

        return left;
    }

    std::unique_ptr<Expression> parseEquality() {
        auto left = parseComparison();

        while (match(TokenType::EQ) || match(TokenType::NE)) {
            std::string op = previous().value;
            auto right = parseComparison();
            left = std::make_unique<BinaryOp>(std::move(left), op, std::move(right));
        }

        return left;
    }

    std::unique_ptr<Expression> parseComparison() {
        auto left = parseTerm();

        while (match(TokenType::LT) || match(TokenType::LE) ||
               match(TokenType::GT) || match(TokenType::GE)) {
            std::string op = previous().value;
            auto right = parseTerm();
            left = std::make_unique<BinaryOp>(std::move(left), op, std::move(right));
        }

        return left;
    }

    std::unique_ptr<Expression> parseTerm() {
        auto left = parseFactor();

        while (match(TokenType::PLUS) || match(TokenType::MINUS)) {
            std::string op = previous().value;
            auto right = parseFactor();
            left = std::make_unique<BinaryOp>(std::move(left), op, std::move(right));
        }

        return left;
    }

    std::unique_ptr<Expression> parseFactor() {
        auto left = parseUnary();

        while (match(TokenType::STAR) || match(TokenType::SLASH) || match(TokenType::PERCENT)) {
            std::string op = previous().value;
            auto right = parseUnary();
            left = std::make_unique<BinaryOp>(std::move(left), op, std::move(right));
        }

        return left;
    }

    std::unique_ptr<Expression> parseUnary() {
        if (match(TokenType::NOT) || match(TokenType::MINUS)) {
            std::string op = previous().value;
            auto expr = parseUnary();
            return std::make_unique<UnaryOp>(op, std::move(expr));
        }

        return parsePostfix();
    }

    std::unique_ptr<Expression> parsePostfix() {
        auto expr = parsePrimary();

        while (true) {
            if (match(TokenType::LBRACKET)) {
                auto index = parseExpression();
                consume(TokenType::RBRACKET, "Expected ']' after array index");
                if (auto id = dynamic_cast<Identifier*>(expr.get())) {
                    expr = std::make_unique<ArrayAccess>(id->name, std::move(index));
                }
            } else if (check(TokenType::LPAREN) && dynamic_cast<Identifier*>(expr.get())) {
                auto id = dynamic_cast<Identifier*>(expr.get());
                match(TokenType::LPAREN);
                auto funcCall = std::make_unique<FunctionCall>(id->name);

                if (!check(TokenType::RPAREN)) {
                    do {
                        funcCall->args.push_back(parseExpression());
                    } while (match(TokenType::COMMA));
                }

                consume(TokenType::RPAREN, "Expected ')' after function arguments");
                expr = std::move(funcCall);
            } else {
                break;
            }
        }

        return expr;
    }

    std::unique_ptr<Expression> parsePrimary() {
        if (match(TokenType::HAAN)) {
            return std::make_unique<BooleanLiteral>(true);
        }

        if (match(TokenType::NA)) {
            return std::make_unique<BooleanLiteral>(false);
        }

        if (match(TokenType::NUMBER)) {
            return std::make_unique<NumberLiteral>(std::stod(previous().value));
        }

        if (match(TokenType::STRING)) {
            return std::make_unique<StringLiteral>(previous().value);
        }

        if (match(TokenType::IDENTIFIER)) {
            return std::make_unique<Identifier>(previous().value);
        }

        // Handle built-in function keywords as identifiers
        if (match(TokenType::DEKH)) {
            return std::make_unique<Identifier>("dekh");
        }

        if (match(TokenType::LOU)) {
            return std::make_unique<Identifier>("lou");
        }

        if (match(TokenType::BAND)) {
            return std::make_unique<Identifier>("band");
        }

        if (match(TokenType::LBRACKET)) {
            auto arrayLit = std::make_unique<ArrayLiteral>();
            if (!check(TokenType::RBRACKET)) {
                do {
                    arrayLit->elements.push_back(parseExpression());
                } while (match(TokenType::COMMA));
            }
            consume(TokenType::RBRACKET, "Expected ']' after array elements");
            return arrayLit;
        }

        if (match(TokenType::LBRACE)) {
            auto objLit = std::make_unique<ObjectLiteral>();
            if (!check(TokenType::RBRACE)) {
                do {
                    Token key = consume(TokenType::IDENTIFIER, "Expected property name");
                    consume(TokenType::COLON, "Expected ':' after property name");
                    auto value = parseExpression();
                    objLit->members.push_back({key.value, std::move(value)});
                } while (match(TokenType::COMMA));
            }
            consume(TokenType::RBRACE, "Expected '}' after object properties");
            return objLit;
        }

        if (match(TokenType::LPAREN)) {
            auto expr = parseExpression();
            consume(TokenType::RPAREN, "Expected ')' after expression");
            return expr;
        }

        throw std::runtime_error("Expected expression at token: " + peek().value);
    }

    bool match(TokenType type) {
        if (check(type)) {
            advance();
            return true;
        }
        return false;
    }

    bool check(TokenType type) {
        if (isAtEnd()) return false;
        return peek().type == type;
    }

    Token advance() {
        if (!isAtEnd()) current++;
        return previous();
    }

    bool isAtEnd() {
        return peek().type == TokenType::EOF_TOKEN;
    }

    Token peek() {
        if (current >= tokens.size()) {
            return Token(TokenType::EOF_TOKEN);
        }
        return tokens[current];
    }

    Token previous() {
        return tokens[current - 1];
    }

    Token consume(TokenType type, const std::string& message) {
        if (check(type)) return advance();
        throw std::runtime_error(message + " at line " + std::to_string(peek().line));
    }
};

// ============================================================================
// Semantic Analyzer
// ============================================================================

class SemanticAnalyzer {
private:
    SymbolTable symbolTable;
    std::vector<std::string> errors;
    DataType currentReturnType;
    bool inFunction;

public:
    SemanticAnalyzer() : currentReturnType(DataType::VOID), inFunction(false) {}

    bool analyze(Program* program) {
        try {
            for (auto& stmt : program->statements) {
                analyzeStatement(stmt.get());
            }

            // Check if main function exists
            Symbol mainSym("", DataType::VOID);
            if (!symbolTable.lookup("main", mainSym)) {
                errors.push_back("ERROR: Main function 'kaam main()' not found");
                return false;
            }

            return errors.empty();
        } catch (const std::exception& e) {
            errors.push_back("EXCEPTION: " + std::string(e.what()));
            return false;
        }
    }

    const std::vector<std::string>& getErrors() const {
        return errors;
    }

private:
    void analyzeStatement(Statement* stmt) {
        if (auto varDecl = dynamic_cast<VariableDeclaration*>(stmt)) {
            analyzeVariableDeclaration(varDecl);
        } else if (auto funcDecl = dynamic_cast<FunctionDeclaration*>(stmt)) {
            analyzeFunctionDeclaration(funcDecl);
        } else if (auto ifStmt = dynamic_cast<IfStatement*>(stmt)) {
            analyzeIfStatement(ifStmt);
        } else if (auto loopStmt = dynamic_cast<LoopStatement*>(stmt)) {
            analyzeLoopStatement(loopStmt);
        } else if (auto retStmt = dynamic_cast<ReturnStatement*>(stmt)) {
            analyzeReturnStatement(retStmt);
        } else if (auto exprStmt = dynamic_cast<ExpressionStatement*>(stmt)) {
            analyzeExpression(exprStmt->expr.get());
        }
    }

    void analyzeVariableDeclaration(VariableDeclaration* varDecl) {
        DataType varType = DataType::UNKNOWN;

        if (varDecl->initializer) {
            varType = analyzeExpression(varDecl->initializer.get());
        }

        if (!symbolTable.define(varDecl->name, varType)) {
            errors.push_back("ERROR: Variable '" + varDecl->name + "' already defined in current scope");
        }
    }

    void analyzeFunctionDeclaration(FunctionDeclaration* funcDecl) {
        // Define function with its parameters
        std::vector<DataType> paramTypes(funcDecl->params.size(), DataType::UNKNOWN);
        symbolTable.addFunctionSignature(funcDecl->name, paramTypes, DataType::VOID);

        // Enter function scope
        symbolTable.enterScope();
        bool prevInFunction = inFunction;
        DataType prevReturnType = currentReturnType;
        inFunction = true;
        currentReturnType = DataType::UNKNOWN;

        // Define parameters in new scope
        for (const auto& param : funcDecl->params) {
            symbolTable.define(param, DataType::UNKNOWN);
        }

        // Analyze function body
        for (auto& stmt : funcDecl->body) {
            analyzeStatement(stmt.get());
        }

        inFunction = prevInFunction;
        currentReturnType = prevReturnType;
        symbolTable.exitScope();
    }

    void analyzeIfStatement(IfStatement* ifStmt) {
        DataType condType = analyzeExpression(ifStmt->condition.get());
        if (condType != DataType::BOOLEAN && condType != DataType::UNKNOWN && condType != DataType::VOID) {
            errors.push_back("ERROR: If condition must be boolean, got " + dataTypeToString(condType));
        }

        symbolTable.enterScope();
        for (auto& stmt : ifStmt->thenBranch) {
            analyzeStatement(stmt.get());
        }
        symbolTable.exitScope();

        if (!ifStmt->elseBranch.empty()) {
            symbolTable.enterScope();
            for (auto& stmt : ifStmt->elseBranch) {
                analyzeStatement(stmt.get());
            }
            symbolTable.exitScope();
        }
    }

    void analyzeLoopStatement(LoopStatement* loopStmt) {
        DataType condType = analyzeExpression(loopStmt->condition.get());
        if (condType != DataType::BOOLEAN && condType != DataType::UNKNOWN && condType != DataType::VOID) {
            errors.push_back("ERROR: Loop condition must be boolean, got " + dataTypeToString(condType));
        }

        symbolTable.enterScope();
        for (auto& stmt : loopStmt->body) {
            analyzeStatement(stmt.get());
        }
        symbolTable.exitScope();
    }

    void analyzeReturnStatement(ReturnStatement* retStmt) {
        if (!inFunction) {
            errors.push_back("ERROR: Return statement outside function");
            return;
        }

        if (retStmt->value) {
            analyzeExpression(retStmt->value.get());
        }
    }

    DataType analyzeExpression(Expression* expr) {
        if (!expr) return DataType::UNKNOWN;

        if (auto numLit = dynamic_cast<NumberLiteral*>(expr)) {
            return DataType::NUMBER;
        }

        if (auto strLit = dynamic_cast<StringLiteral*>(expr)) {
            return DataType::STRING;
        }

        if (auto boolLit = dynamic_cast<BooleanLiteral*>(expr)) {
            return DataType::BOOLEAN;
        }

        if (auto id = dynamic_cast<Identifier*>(expr)) {
            Symbol sym("", DataType::UNKNOWN);
            if (symbolTable.lookup(id->name, sym)) {
                expr->type = sym.type;
                return sym.type;
            } else {
                errors.push_back("ERROR: Undefined variable '" + id->name + "'");
                return DataType::UNKNOWN;
            }
        }

        if (auto binOp = dynamic_cast<BinaryOp*>(expr)) {
            return analyzeBinaryOp(binOp);
        }

        if (auto unaryOp = dynamic_cast<UnaryOp*>(expr)) {
            return analyzeUnaryOp(unaryOp);
        }

        if (auto assign = dynamic_cast<Assignment*>(expr)) {
            return analyzeAssignment(assign);
        }

        if (auto funcCall = dynamic_cast<FunctionCall*>(expr)) {
            return analyzeFunctionCall(funcCall);
        }

        if (auto arrayLit = dynamic_cast<ArrayLiteral*>(expr)) {
            return DataType::ARRAY;
        }

        if (auto objLit = dynamic_cast<ObjectLiteral*>(expr)) {
            return DataType::OBJECT;
        }

        if (auto arrAccess = dynamic_cast<ArrayAccess*>(expr)) {
            Symbol sym("", DataType::UNKNOWN);
            if (symbolTable.lookup(arrAccess->arrayName, sym)) {
                if (sym.type != DataType::ARRAY && sym.type != DataType::UNKNOWN) {
                    errors.push_back("ERROR: Cannot index non-array type '" + arrAccess->arrayName + "'");
                }
                DataType indexType = analyzeExpression(arrAccess->index.get());
                if (indexType != DataType::NUMBER && indexType != DataType::UNKNOWN) {
                    errors.push_back("ERROR: Array index must be number, got " + dataTypeToString(indexType));
                }
                return DataType::UNKNOWN; // Element type unknown
            } else {
                errors.push_back("ERROR: Undefined array '" + arrAccess->arrayName + "'");
                return DataType::UNKNOWN;
            }
        }

        return DataType::UNKNOWN;
    }

    DataType analyzeBinaryOp(BinaryOp* binOp) {
        DataType leftType = analyzeExpression(binOp->left.get());
        DataType rightType = analyzeExpression(binOp->right.get());

        // Arithmetic operators
        if (binOp->op == "+" || binOp->op == "-" || binOp->op == "*" ||
            binOp->op == "/" || binOp->op == "%") {
            // Allow void or unknown for recursive function calls
            if (leftType != DataType::NUMBER && leftType != DataType::UNKNOWN && leftType != DataType::VOID) {
                errors.push_back("ERROR: Left operand of '" + binOp->op + "' must be number");
            }
            if (rightType != DataType::NUMBER && rightType != DataType::UNKNOWN && rightType != DataType::VOID) {
                errors.push_back("ERROR: Right operand of '" + binOp->op + "' must be number");
            }
            return DataType::NUMBER;
        }

        // Comparison operators
        if (binOp->op == "<" || binOp->op == "<=" || binOp->op == ">" || binOp->op == ">=") {
            if (leftType != DataType::NUMBER && leftType != DataType::UNKNOWN) {
                errors.push_back("ERROR: Left operand of '" + binOp->op + "' must be number");
            }
            if (rightType != DataType::NUMBER && rightType != DataType::UNKNOWN) {
                errors.push_back("ERROR: Right operand of '" + binOp->op + "' must be number");
            }
            return DataType::BOOLEAN;
        }

        // Equality operators
        if (binOp->op == "==" || binOp->op == "!=") {
            return DataType::BOOLEAN;
        }

        // Logical operators
        if (binOp->op == "&&" || binOp->op == "||") {
            if (leftType != DataType::BOOLEAN && leftType != DataType::UNKNOWN) {
                errors.push_back("ERROR: Left operand of '" + binOp->op + "' must be boolean");
            }
            if (rightType != DataType::BOOLEAN && rightType != DataType::UNKNOWN) {
                errors.push_back("ERROR: Right operand of '" + binOp->op + "' must be boolean");
            }
            return DataType::BOOLEAN;
        }

        return DataType::UNKNOWN;
    }

    DataType analyzeUnaryOp(UnaryOp* unaryOp) {
        DataType operandType = analyzeExpression(unaryOp->operand.get());

        if (unaryOp->op == "-") {
            if (operandType != DataType::NUMBER && operandType != DataType::UNKNOWN) {
                errors.push_back("ERROR: Operand of '-' must be number");
            }
            return DataType::NUMBER;
        }

        if (unaryOp->op == "!") {
            if (operandType != DataType::BOOLEAN && operandType != DataType::UNKNOWN) {
                errors.push_back("ERROR: Operand of '!' must be boolean");
            }
            return DataType::BOOLEAN;
        }

        return DataType::UNKNOWN;
    }

    DataType analyzeAssignment(Assignment* assign) {
        Symbol sym("", DataType::UNKNOWN);
        if (!symbolTable.lookup(assign->name, sym)) {
            errors.push_back("ERROR: Undefined variable '" + assign->name + "'");
            return DataType::UNKNOWN;
        }

        DataType valueType = analyzeExpression(assign->value.get());

        if (sym.type != DataType::UNKNOWN && valueType != DataType::UNKNOWN &&
            sym.type != valueType) {
            errors.push_back("ERROR: Type mismatch in assignment to '" + assign->name +
                           "': expected " + dataTypeToString(sym.type) +
                           ", got " + dataTypeToString(valueType));
        }

        symbolTable.update(assign->name);
        return valueType;
    }

    DataType analyzeFunctionCall(FunctionCall* funcCall) {
        Symbol funcSym("", DataType::VOID);
        if (!symbolTable.lookup(funcCall->name, funcSym)) {
            errors.push_back("ERROR: Undefined function '" + funcCall->name + "'");
            return DataType::UNKNOWN;
        }

        if (!funcSym.isFunction) {
            errors.push_back("ERROR: '" + funcCall->name + "' is not a function");
            return DataType::UNKNOWN;
        }

        // Check argument count for built-ins
        if (funcCall->name == "dekh") {
            for (auto& arg : funcCall->args) {
                analyzeExpression(arg.get());
            }
            return DataType::VOID;
        }

        if (funcCall->name == "lou") {
            if (!funcCall->args.empty()) {
                analyzeExpression(funcCall->args[0].get());
            }
            return DataType::NUMBER;
        }

        if (funcCall->name == "nikal") {
            if (funcCall->args.size() != 1) {
                errors.push_back("ERROR: nikal() expects 1 argument, got " + std::to_string(funcCall->args.size()));
            } else {
                analyzeExpression(funcCall->args[0].get());
            }
            return DataType::NUMBER;
        }

        if (funcCall->name == "band") {
            return DataType::VOID;
        }

        if (funcCall->name == "abs" || funcCall->name == "sqrt" || funcCall->name == "round") {
            if (funcCall->args.size() != 1) {
                errors.push_back("ERROR: " + funcCall->name + "() expects 1 argument");
            } else {
                DataType argType = analyzeExpression(funcCall->args[0].get());
                if (argType != DataType::NUMBER && argType != DataType::UNKNOWN) {
                    errors.push_back("ERROR: " + funcCall->name + "() expects number argument");
                }
            }
            return DataType::NUMBER;
        }

        if (funcCall->name == "pow" || funcCall->name == "max" || funcCall->name == "min") {
            if (funcCall->args.size() != 2) {
                errors.push_back("ERROR: " + funcCall->name + "() expects 2 arguments");
            } else {
                for (auto& arg : funcCall->args) {
                    DataType argType = analyzeExpression(arg.get());
                    if (argType != DataType::NUMBER && argType != DataType::UNKNOWN) {
                        errors.push_back("ERROR: " + funcCall->name + "() expects number arguments");
                    }
                }
            }
            return DataType::NUMBER;
        }

        if (funcCall->name == "random") {
            return DataType::NUMBER;
        }

        // User-defined function
        if (funcCall->args.size() != funcSym.paramTypes.size()) {
            errors.push_back("ERROR: Function '" + funcCall->name + "' expects " +
                           std::to_string(funcSym.paramTypes.size()) + " arguments, got " +
                           std::to_string(funcCall->args.size()));
        }

        for (auto& arg : funcCall->args) {
            analyzeExpression(arg.get());
        }

        return funcSym.returnType;
    }
};

// ============================================================================
// Main Program
// ============================================================================

int main() {
    // Read code from test.txt file
    std::ifstream inputFile("test.txt");
    if (!inputFile.is_open()) {
        std::cerr << "ERROR: Cannot open test.txt file" << std::endl;
        return 1;
    }

    std::stringstream buffer;
    buffer << inputFile.rdbuf();
    std::string code = buffer.str();
    inputFile.close();

    std::cout << "=== Our-Lang V1 Semantic Analyzer ===" << std::endl << std::endl;
    std::cout << "Reading from: test.txt" << std::endl << std::endl;
    std::cout << "Source Code:" << std::endl << code << std::endl << std::endl;

    try {
        // Lexical Analysis
        std::cout << "--- Lexical Analysis ---" << std::endl;
        Lexer lexer(code);
        std::vector<Token> tokens;
        Token token = lexer.nextToken();
        while (token.type != TokenType::EOF_TOKEN) {
            tokens.push_back(token);
            token = lexer.nextToken();
        }
        tokens.push_back(token); // Add EOF token

        std::cout << "Tokens generated: " << tokens.size() << std::endl << std::endl;

        // Parsing
        std::cout << "--- Parsing (Recursive Descent) ---" << std::endl;
        Parser parser(tokens);
        auto program = parser.parse();
        std::cout << "AST generated successfully" << std::endl << std::endl;

        // Semantic Analysis
        std::cout << "--- Semantic Analysis ---" << std::endl;
        std::cout << "- Data Type Checking" << std::endl;
        std::cout << "- Scope Checking" << std::endl;
        std::cout << "- Variable Declaration Validation" << std::endl;
        std::cout << "- Function Declaration Validation" << std::endl;

        SemanticAnalyzer analyzer;
        bool success = analyzer.analyze(program.get());

        if (success) {
            std::cout << "\n✓ Semantic Analysis PASSED" << std::endl;
        } else {
            std::cout << "\n✗ Semantic Analysis FAILED" << std::endl;
            std::cout << "\nErrors found:" << std::endl;
            for (const auto& error : analyzer.getErrors()) {
                std::cout << "  " << error << std::endl;
            }
        }

    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
