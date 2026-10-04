//
// Created by simon on 10.09.2026.
//

#include "scanner.h"

#include <ctype.h>

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

    if (isalpha((unsigned char)c))
    {
        if (c == 'r') // WICHTIG: Register beginnen IMMER mit einem kleinen R: R3 -> falsch r3 -> korrekt TODO: In der Doku nachtragen
        {
            token.type = TOK_REGISTER;

            long regnum = 0;

            advance(s); // 'r' verbrauchen

            while (isdigit(peek(s)))
            {
                char d = advance(s);
                regnum = regnum * 10 + (d - '0');
            }

            token.value = regnum;
        }
        else
        {
            token.type = TOK_IDENT;
            token.text[0] = c;

            while (isalnum(peek(s)) || peek(s) == ':') {
                if (peek(s) == ':')
                {
                    advance(s); // ':' verbrauchen
                    token.text[counter++] = '\0';
                    token.type = TOK_LABEL;
                    return token;
                }

                token.text[counter++] = advance(s);
            }
            token.type = s -> mnemonic_seen ? TOK_LABEL_IDENT : TOK_IDENT;
            s -> mnemonic_seen = 1;
        }

        return token;
    }

    if (isdigit((unsigned char)c))
    {
        token.type = TOK_NUMBER;

        advance(s);

        if (c == '0' && peek(s) == 'x')
        {
            advance(s); // 'x' verbrauchen

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
            token.text[0] = c;
            token.text[1] = '\0';
            token.line = s -> line;
            break;
    }

    advance(s);

    return token;
}