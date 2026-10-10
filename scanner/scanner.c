//
// Created by simon on 10.09.2026.
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

// Ein Register ist genau "R" gefolgt von mindestens einer Ziffer, z.B. "R0" oder "R31"
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

    token.line = s -> line; // fuer Fehlermeldungen

    if (isalpha((unsigned char)c))
    {
        // Erst das ganze Wort lesen, dann entscheiden, ob es ein Register ist.
        // Sonst wuerde z.B. das Label "restart" als Register gelesen.
        while (isalnum((unsigned char)peek(s)))
        {
            char d = advance(s);
            if (counter < (int)sizeof(token.text) - 1)
                token.text[counter++] = d;
        }
        token.text[counter] = '\0';

        if (peek(s) == ':')
        {
            advance(s); // ':' verbrauchen
            token.type = TOK_LABEL;
            return token;
        }

        if (is_register(token.text)) // WICHTIG: Register beginnen IMMER mit einem grossen R: r3 -> falsch, R3 -> korrekt
        {
            token.type = TOK_REGISTER;
            token.value = 0;
            // Abbruch, sobald die Nummer zu gross ist - sonst kann z.B. "R99999999999999999999" ueberlaufen
            for (int i = 1; token.text[i] != '\0' && token.value < KRON_REGISTER_COUNT; i++)
                token.value = token.value * 10 + (token.text[i] - '0');

            if (token.value >= KRON_REGISTER_COUNT)
            {
                char name[sizeof(token.text)];
                strcpy(name, token.text); // snprintf darf nicht aus token.text in token.text schreiben

                token.type = TOK_ERROR;
                snprintf(token.text, sizeof(token.text), "Ungueltiges Register %.10s", name); // %.10s: Meldung passt immer in token.text
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
            snprintf(token.text, sizeof(token.text), "Ungueltiges Zeichen '%c'", c);
            break;
    }

    advance(s);

    return token;
}