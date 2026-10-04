//
// Created by Simon Hirsch on 10.09.2026.
//

#ifndef SCANNER_H
#define SCANNER_H

typedef enum {
    TOK_IDENT,       // Mnemonic z.B. "ADD", "loop"
    TOK_REGISTER,    // R0 .. R31
    TOK_NUMBER,      // dezimal oder 0x... , in value abgelegt
    TOK_LABEL,       // Name für eine Sprungmarke, z.B. "loop:"
    TOK_LABEL_IDENT, // Name für eine Sprungmarke, z.B. "loop" (ohne Doppelpunkt)
    TOK_COMMA,       // ,
    TOK_LPAREN,      // (
    TOK_RPAREN,      // )
    TOK_EOL,         // Zeilenende
    TOK_EOF,         // Ende der Datei
    TOK_ERROR        // ungueltiges Zeichen/Token, text traegt die Fehlermeldung
} TokenType;

typedef struct {
    TokenType type;
    char text[32];  // Rohtext bzw. Fehlermeldung, z.B. "ADD", "loop"
    long value;      // fuer TOK_NUMBER: der Zahlenwert; fuer TOK_REGISTER: die Registernummer (0..31)
    int line;         // Zeilennummer in der Quelldatei, fuer Fehlermeldungen
} Token;

typedef struct {
    const char *src;  // gesamter Quelltext, nullterminiert
    int pos;           // aktueller Index in src
    int line;           // aktuelle Zeilennummer (beginnend bei 1)
    int mnemonic_seen; // Flag, um zu verfolgen, ob ein Mnemonic in der aktuellen Zeile bereits gesehen wurde
} Scanner;

void scanner_init(Scanner *s, const char *src);
Token scanner_next(Scanner *s);

#endif
