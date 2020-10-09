#pragma once
#include <variant>
#include <istream>
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

    enum class TokenType: int {
        END_OF_STMT = 0,
        SYMBOL,
        NUMBER,
    };

    struct Token {
        TokenType type;
        std::variant<std::monostate, double, Symbol> data;
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
