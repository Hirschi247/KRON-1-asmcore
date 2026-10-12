//
// Created by Simon Hirsch on 10.09.2026.
//

#include "scanner.h"

#include <ctype.h>
#include <stdio.h>
#include <string.h>

#include "isa.h"

static char peek(Scanner *s)
{
    const char c = s -> src[s -> pos];

    if (c == '\0')
        return '\0';

    return c;
}

static char advance(Scanner *s)
{
    const char c = s -> src[s -> pos];

    if (c == '\0')
        return '\0';

    if (c == '\n')
        ++s -> line;

    ++s -> pos;

    return c;
}

static int hexval(char d)
{
    if (d >= '0' && d <= '9') return d - '0';
    if (d >= 'a' && d <= 'f') return d - 'a' + 10;
    if (d >= 'A' && d <= 'F') return d - 'A' + 10;
    return 0;
}

// A register is exactly "R" followed by at least one digit, e.g. "R0" or "R31"
static int is_register(const char *word)
{
    if (word[0] != 'R' || word[1] == '\0')
        return 0;

    for (int i = 1; word[i] != '\0'; i++)
        if (!isdigit((unsigned char)word[i]))
            return 0;

    return 1;
}

void scanner_init(Scanner *s, const char *src)
{
    s -> line = 1;
    s -> pos = 0;
    s -> src = src;
    s ->mnemonic_seen = 0;
}

Token scanner_next(Scanner *s)
{
    Token token = {0};

    char c = peek(s);

    int counter = 0;

    if (isspace((unsigned char)c))
    {
        while (peek(s) == ' ' || peek(s) == '\t' || peek(s) == '\r')
            advance(s);
        c = peek(s);
    }

    if (c == ';')
    {
        while (peek(s) != '\n' && peek(s) != '\0')
            advance(s);

        c = peek(s);
    }

    token.line = s -> line; // for error messages

    if (isalpha((unsigned char)c))
    {
        // Read the whole word first, then decide whether it is a register.
        // Otherwise e.g. the label "restart" would be read as a register.
        while (isalnum((unsigned char)peek(s)))
        {
            char d = advance(s);
            if (counter < (int)sizeof(token.text) - 1)
                token.text[counter++] = d;
        }
        token.text[counter] = '\0';

        if (peek(s) == ':')
        {
            advance(s); // consume ':'
            token.type = TOK_LABEL;
            return token;
        }

        if (is_register(token.text)) // NOTE: registers ALWAYS start with an uppercase R: r3 -> wrong, R3 -> correct
        {
            token.type = TOK_REGISTER;
            token.value = 0;
            // Stop once the number is too large - otherwise e.g. "R99999999999999999999" could overflow
            for (int i = 1; token.text[i] != '\0' && token.value < KRON_REGISTER_COUNT; i++)
                token.value = token.value * 10 + (token.text[i] - '0');

            if (token.value >= KRON_REGISTER_COUNT)
            {
                char name[sizeof(token.text)];
                strcpy(name, token.text); // snprintf must not read from and write to token.text at the same time

                token.type = TOK_ERROR;
                snprintf(token.text, sizeof(token.text), "Invalid register %.10s", name); // %.10s: message always fits into token.text
            }
            return token;
        }

        token.type = s -> mnemonic_seen ? TOK_LABEL_IDENT : TOK_IDENT;
        s -> mnemonic_seen = 1;

        return token;
    }

    if (isdigit((unsigned char)c))
    {
        token.type = TOK_NUMBER;

        advance(s);

        if (c == '0' && peek(s) == 'x')
        {
            advance(s); // consume 'x'

            long num = 0;

            while (isxdigit(peek(s)))
            {
                char d = advance(s);
                num = num * 16 + hexval(d);
            }

            token.value = num;
        }
        else
        {
            long num = c - '0';

            while (isdigit(peek(s)))
            {
                char d = advance(s);
                num = num * 10 + (d - '0');
            }

            token.value = num;
        }

        return token;
    }

    switch (c)
    {
        case ',':
            token.type = TOK_COMMA;
            break;

        case ':':
            token.type = TOK_LABEL;
            break;

        case '(':
            token.type = TOK_LPAREN;
            break;

        case ')':
            token.type = TOK_RPAREN;
            break;

        case '\0':
            token.type = TOK_EOF;
            break;

        case '\n':
            token.type = TOK_EOL;
            s -> mnemonic_seen = 0;
            break;

        default:
            token.type = TOK_ERROR;
            snprintf(token.text, sizeof(token.text), "Invalid character '%c'", c);
            break;
    }

    advance(s);

    return token;
}