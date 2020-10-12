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
    
    // If modifying check Symbol enum in lexer.hpp
    const char *KEYWORDS[] = {
        "let",
    };

    constexpr int KEYWORD_COUNT = sizeof(KEYWORDS) / sizeof(char*);

    // If modifying check TokenType enum in lexer.hpp
    const char *TOKEN_TYPES[] = {
        "End of statement",
        "Symbol",
        "Number",
        "Name",
        "Keyword",
    };

    constexpr int TOKEN_TYPE_COUNT = sizeof(TOKEN_TYPES) / sizeof(char*);
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

    int startCol = _col, startLine = _line;

    if (c == EOF || c == ';') {
        digestChar();
        return { startCol, startLine };
    } else if (isdigit(c)) {
        // Max length of string double is 325 chars + 1 null char, 2 extra chars for padding and good luck
        std::array<char, 328> numBuffer = {};
        int offset = 0;
        while (isdigit(c) || c == '.' || c == 'e' || c == 'E') {
            numBuffer[offset++] = digestChar();
            c = getChar();
        }
        numBuffer[numBuffer.size() - 1] = 0; // Stop buffer overrun
        double value = strtod(numBuffer.data(), nullptr);
        return { startCol, startLine, TokenType::NUMBER, value };
    } else {
        // Symbol
        for (int i = 0; i < SYMBOL_COUNT; i++) {
            if (c == SYMBOLS[i]) {
                digestChar();
                return { startCol, startLine, TokenType::SYMBOL, static_cast<Symbol>(i) };
            }
        }
        // Keyword or name
        if (isalpha(c)) {
            std::array<char, 128> nameBuffer = {};
            int offset = 0;
            while (isalnum(c) || c == '_') {
                nameBuffer[offset++] = digestChar();
                c = getChar();
            }
            nameBuffer[nameBuffer.size() - 1] = 0;
            // Check if keyword
            for (int i = 0; i < KEYWORD_COUNT; i++) {
                if (strncmp(nameBuffer.data(), KEYWORDS[i], nameBuffer.size()) == 0) {
                    return { startCol, startLine, TokenType::KEYWORD, static_cast<Keyword>(i) };
                }
            }
            return { startCol, startLine, TokenType::NAME, std::string(nameBuffer.data()) };
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
    } else if (c != '\r' && c != EOF) {
        _col++;
    }
    return c;
}

std::string Lexer::generateError(const std::string &msg, int c) {
    std::ostringstream error;
    error << msg << " '" << static_cast<char>(c & 0xFF) << "' Line " << _line << " Col " << _col;
    return error.str();
}

std::ostream &operator<<(std::ostream &stream, Symbol symbol) {
    int offset = static_cast<int>(symbol);

    if (offset < 0 || offset >= SYMBOL_COUNT) {
        return stream << "Bad symbol";
    }

    return stream << SYMBOLS[offset];
}

std::ostream &operator<<(std::ostream &stream, TokenType type) {
    int offset = static_cast<int>(type);

    if (offset < 0 || offset >= TOKEN_TYPE_COUNT) {
        return stream << "Bad token type";
    }

    return stream << TOKEN_TYPES[offset];
}

std::ostream &operator<<(std::ostream &stream, quickcalc::Keyword keyword) {
    int offset = static_cast<int>(keyword);

    if (offset < 0 || offset >= KEYWORD_COUNT) {
        return stream << "Bad keyword";
    }

    return stream << KEYWORDS[offset];
}
