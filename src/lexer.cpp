#include "lexer.hpp"
#include <cctype>
#include <array>
#include <stdexcept>

using namespace quickcalc;
ILexer::~ILexer() {
}

/**
 * @brief Construct a new lexical analyzer
 * 
 * @param input An input stream to read tokens from. **Must** live as long as the lexer.
 */
Lexer::Lexer(std::istream &input): _input(input), _ready(false) {
}

/**
 * @brief Reads the next token from the stream
 * 
 * @return Token The token read
 */
Token Lexer::read() {
    if (_ready) {
        _ready = false;
        return _pending;
    } else {
        return readToken();
    }
}

/**
 * @brief Reads the next token from the stream, without progressing
 * 
 * @return Token The token read
 */
Token Lexer::peek() {
    if (_ready) {
        return _pending;
    } else {
        _pending = readToken();
        _ready = true;
        return _pending;
    }
}

/**
 * @brief Reads a token from the stream, internally used by read and peek
 * 
 * @return Token 
 */
Token Lexer::readToken() {
    int c = getChar();
    
    while (isspace(c)) {
        digestChar();
        c = getChar();
    }

    if (c == EOF) {
        return {};
    } else if (isdigit(c)) {
        // Max length of string double is 325 chars + 1 null char, 2 extra chars for padding and good luck
        std::array<char, 328> numBuffer = {};
        int offset = 0;
        while (isdigit(c) || c == '.' || c == 'e' || c == 'E') {
            numBuffer[offset++] = digestChar();
            c = getChar();
        }
        double value = strtod(numBuffer.data(), nullptr);
        return { TokenType::NUMBER, value };
    } else {
        // TODO: Report a better message
        throw std::runtime_error("Bad character");
    }
}

int Lexer::getChar() {
    return _input.peek();
}

int Lexer::digestChar() {
    return _input.get();
}
