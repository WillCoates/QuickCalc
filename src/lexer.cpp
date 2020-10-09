#include "lexer.hpp"
#include <cctype>
#include <array>
#include <stdexcept>
#include <sstream>

using namespace quickcalc;

namespace {
    // If modifying check Symbol enum in lexer.hpp
    const char SYMBOLS[] = {
        '+',
        '-',
        '*',
        '/',
        '&',
        '|',
        '^',
        '~',
        '(',
        ')',
    };
    constexpr int SYMBOL_COUNT = sizeof(SYMBOLS) / sizeof(char);
}

ILexer::~ILexer() {
}

/**
 * @brief Construct a new lexical analyzer
 * 
 * @param input An input stream to read tokens from. **Must** live as long as the lexer.
 */
Lexer::Lexer(std::istream &input): _input(input), _ready(false), _line(1), _col(1) {
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
    if (!_ready) {
        _pending = readToken();
        _ready = true;
    }
    return _pending;
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

    if (c == EOF || c == ';') {
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
        for (int i = 0; i < SYMBOL_COUNT; i++) {
            if (c == SYMBOLS[i]) {
                return { TokenType::SYMBOL, static_cast<Symbol>(i) };
            }
        }
        throw std::runtime_error(generateError("Bad character", c));
    }
}

int Lexer::getChar() {
    return _input.peek();
}

int Lexer::digestChar() {
    int c = _input.get();
    // Keep track of where we are in input
    if (c == '\n') {
        _col = 1;
        _line++;
    } else if (c != '\r') {
        _col++;
    }
    return c;
}

std::string Lexer::generateError(const std::string &msg, int c) {
    std::ostringstream error;
    error << msg << " '" << static_cast<char>(c & 0xFF) << "' Line " << _line << " Col " << _col;
    return error.str();
}
