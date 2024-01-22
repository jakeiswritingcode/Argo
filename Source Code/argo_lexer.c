#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

// Token types
enum TokenType {
    NUMBER,
    PLUS,
    MINUS,
    MULTIPLY,
    DIVIDE,
    LPAREN,
    RPAREN,
    END
};

// Token structure
struct Token {
    enum TokenType type;
    int value;  // For NUMBER tokens
};

// Function to tokenize the input
struct Token getNextToken(const char** input) {
    while (isspace(**input)) {
        (*input)++;
    }

    if (**input == '\0') {
        return (struct Token){.type = END};
    }

    if (isdigit(**input)) {
        int value = 0;
        while (isdigit(**input)) {
            value = value * 10 + (**input - '0');
            (*input)++;
        }
        return (struct Token){.type = NUMBER, .value = value};
    }

    switch (**input) {
        case '+':
            (*input)++;
            return (struct Token){.type = PLUS};
        case '-':
            (*input)++;
            return (struct Token){.type = MINUS};
        case '*':
            (*input)++;
            return (struct Token){.type = MULTIPLY};
        case '/':
            (*input)++;
            return (struct Token){.type = DIVIDE};
        case '(':
            (*input)++;
            return (struct Token){.type = LPAREN};
        case ')':
            (*input)++;
            return (struct Token){.type = RPAREN};
        default:
            fprintf(stderr, "Invalid character: %c\n", **input);
            exit(EXIT_FAILURE);
    }
}

int main() {
    const char* input = "3 + (4 * 5) - 2";

    while (1) {
        struct Token token = getNextToken(&input);

        switch (token.type) {
            case NUMBER:
                printf("NUMBER: %d\n", token.value);
                break;
            case PLUS:
                printf("PLUS\n");
                break;
            case MINUS:
                printf("MINUS\n");
                break;
            case MULTIPLY:
                printf("MULTIPLY\n");
                break;
            case DIVIDE:
                printf("DIVIDE\n");
                break;
            case LPAREN:
                printf("LPAREN\n");
                break;
            case RPAREN:
                printf("RPAREN\n");
                break;
            case END:
                printf("END\n");
                exit(EXIT_SUCCESS);
            default:
                fprintf(stderr, "Unexpected token: %d\n", token.type);
                exit(EXIT_FAILURE);
        }
    }

    return 0;
}
