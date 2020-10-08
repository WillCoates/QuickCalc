#pragma once
#include <variant>
#include <istream>

namespace quickcalc {
    enum class Symbol: int {
        ADD = 0,
        SUBTRACT,
        MULTIPLY,
        DIVIDE,
        EXPONENT,
        AND,
        OR,
        XOR,
        NOT,
    };

    enum class TokenType: int {
        END_OF_STMT = 0,
        UNARY_OPERATOR,
        BINARY_OPERATOR,
        NUMBER,
        BRACKET_OPEN,
        BRACKET_CLOSE,
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

    public:
        explicit Lexer(std::istream &input);
        Token read() override;
        Token peek() override;
        
    private:
        Token readToken();
        int getChar();
        int digestChar();
    };
}
