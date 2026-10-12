//
// Created by Simon Hirsch on 10.09.2026.
//

#ifndef SCANNER_H
#define SCANNER_H

typedef enum {
    TOK_IDENT,       // mnemonic, e.g. "ADD"
    TOK_REGISTER,    // R0 .. R31
    TOK_NUMBER,      // decimal or 0x..., stored in value
    TOK_LABEL,       // label definition, e.g. "loop:"
    TOK_LABEL_IDENT, // label reference, e.g. "loop" (without colon)
    TOK_COMMA,       // ,
    TOK_LPAREN,      // (
    TOK_RPAREN,      // )
    TOK_EOL,         // end of line
    TOK_EOF,         // end of file
    TOK_ERROR        // invalid character/token, text holds the error message
} TokenType;

typedef struct {
    TokenType type;
    char text[32];  // raw text or error message, e.g. "ADD", "loop"
    long value;      // TOK_NUMBER: the numeric value; TOK_REGISTER: the register number (0..31)
    int line;         // line number in the source file, for error messages
} Token;

typedef struct {
    const char *src;  // entire source text, null-terminated
    int pos;           // current index into src
    int line;           // current line number (starting at 1)
    int mnemonic_seen; // set once the current line's mnemonic has been read
} Scanner;

void scanner_init(Scanner *s, const char *src);
Token scanner_next(Scanner *s);

#endif
