#pragma once
#include <variant>
#include <istream>
#include <ostream>
#include <string>

namespace quickcalc {
    // If modifying check SYMBOLS in lexer.cpp
    enum class Symbol: int {
        ADD = 0,
        SUBTRACT,
        MULTIPLY,
        DIVIDE,
        AND,
        OR,
        XOR,
        NOT,
        BRACKET_OPEN,
        BRACKET_CLOSE,
    };

    // If modifying check KEYWORDS in lexer.cpp
    enum class Keyword: int {
        LET,
    };

    // If modifying check TOKEN_TYPES in lexer.cpp
    enum class TokenType: int {
        END_OF_STMT = 0,
        SYMBOL,
        NUMBER,
        NAME,
        KEYWORD,
    };

    struct Token {
        int column;
        int line;
        TokenType type;
        std::variant<std::monostate, double, Symbol, Keyword, std::string> data;
    };

    struct ILexer {
        virtual ~ILexer();
        virtual Token read() = 0;
        virtual Token peek() = 0;
    };

    class Lexer: public ILexer {
        std::istream &_input;
        Token _pending;
        bool _ready;
        int _line, _col;

    public:
        explicit Lexer(std::istream &input);
        Token read() override;
        Token peek() override;
        
    private:
        Token readToken();
        int getChar();
        int digestChar();
        std::string generateError(const std::string &msg, int c);
    };
}

std::ostream &operator<<(std::ostream &stream, quickcalc::Symbol symbol);
std::ostream &operator<<(std::ostream &stream, quickcalc::TokenType type);
