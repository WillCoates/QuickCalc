#include "lexer.hpp"
#include <cctype>
#include <array>

using namespace quickcalc;

ILexer::~ILexer() {
}

/**
 * @brief Construct a new lexical analyzer
 * 
 * @param input An input stream to read tokens from. **Must** live as long as the lexer.
 */
Lexer::Lexer(std::istream &input): _input(input) {
    _pending = readToken();
}

/**
 * @brief Reads the next token from the stream
 * 
 * @return Token The token read
 */
Token Lexer::read() {
    Token temp = _pending;
    _pending = readToken();
    return temp;
}

/**
 * @brief Reads the next token from the stream, without progressing
 * 
 * @return Token The token read
 */
Token Lexer::peek() {
    return _pending;
}

/**
 * @brief Reads a token from the stream, internally used by read and ctor
 * 
 * @return Token 
 */
Token Lexer::readToken() {
    int c = getChar();
    if (c == EOF) {
        return {};
    } else if (isdigit(c)) {
        // Max length of string double is 325 chars + 1 null char, 2 extra chars for padding and good luck
        std::array<char, 328> numBuffer = {};
        int offset = 0;
        while (isdigit(c) || c == '.') {
            numBuffer[offset++] = digestChar();
            c = getChar();
        }
        double value = strtod(numBuffer.data(), nullptr);
        return { TokenType::NUMBER, value };
    }
    return {};
}

int Lexer::getChar() {
    return _input.peek();
}

int Lexer::digestChar() {
    return _input.get();
}
