#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Token types
enum TokenType {
    IDENTIFIER,
    KEYWORD,
    INT,
    FLOAT,
    STRING,
    LPAREN,
    RPAREN,
    LBRACE,
    RBRACE,
    SEMICOLON,
    END
};

// Token structure
struct Token {
    enum TokenType type;
    char lexeme[100];  // For identifiers and keywords
};

// AST Node structure
struct ASTNode {
    char label[100];
    struct ASTNode* left;
    struct ASTNode* right;
};

// Function to check if a string is a keyword
int isKeyword(const char* word) {
    const char* keywords[] = {"class", "public", "private", "int", "float", "string"};
    for (int i = 0; i < sizeof(keywords) / sizeof(keywords[0]); i++) {
        if (strcmp(word, keywords[i]) == 0) {
            return 1;  // It's a keyword
        }
    }
    return 0;  // It's not a keyword
}

// Function to tokenize the input
struct Token getNextToken(const char** input) {
    while (isspace(**input)) {
        (*input)++;
    }

    if (**input == '\0') {
        return (struct Token){.type = END};
    }

    if (isalpha(**input) || **input == '_') {
        // Identifier or keyword
        char lexeme[100];
        int i = 0;
        while (isalnum(**input) || **input == '_') {
            lexeme[i++] = **input;
            (*input)++;
        }
        lexeme[i] = '\0';

        struct Token token;
        token.type = isKeyword(lexeme) ? KEYWORD : IDENTIFIER;
        strcpy(token.lexeme, lexeme);
        return token;
    }

    switch (**input) {
        case '(':
            (*input)++;
            return (struct Token){.type = LPAREN};
        case ')':
            (*input)++;
            return (struct Token){.type = RPAREN};
        case '{':
            (*input)++;
            return (struct Token){.type = LBRACE};
        case '}':
            (*input)++;
            return (struct Token){.type = RBRACE};
        case ';':
            (*input)++;
            return (struct Token){.type = SEMICOLON};
        default:
            fprintf(stderr, "Invalid character: %c\n", **input);
            exit(EXIT_FAILURE);
    }
}

// Function to create a new AST node
struct ASTNode* createNode(const char* label) {
    struct ASTNode* node = (struct ASTNode*)malloc(sizeof(struct ASTNode));
    strcpy(node->label, label);
    node->left = NULL;
    node->right = NULL;
    return node;
}

// Grammar rules for the parser
struct ASTNode* parseClass(const char** input);
struct ASTNode* parseClassBody(const char** input);
struct ASTNode* parseMember(const char** input);
struct ASTNode* parseType(const char** input);
struct ASTNode* parseIdentifier(const char** input);

// Function to parse class declarations
struct ASTNode* parseClass(const char** input) {
    struct ASTNode* classNode = createNode("ClassDeclaration");
    
    // Expect 'class' keyword
    struct Token classToken = getNextToken(input);
    if (classToken.type != KEYWORD || strcmp(classToken.lexeme, "class") != 0) {
        fprintf(stderr, "Error: Expected 'class' keyword.\n");
        exit(EXIT_FAILURE);
    }

    // Expect class name (identifier)
    struct Token classNameToken = getNextToken(input);
    if (classNameToken.type != IDENTIFIER) {
        fprintf(stderr, "Error: Expected class name (identifier).\n");
        exit(EXIT_FAILURE);
    }

    // Set class name as label for the AST node
    strcpy(classNode->label, classNameToken.lexeme);

    // Expect '{'
    struct Token openBraceToken = getNextToken(input);
    if (openBraceToken.type != LBRACE) {
        fprintf(stderr, "Error: Expected '{'.\n");
        exit(EXIT_FAILURE);
    }

    // Parse class body
    classNode->left = parseClassBody(input);

    // Expect '}'
    struct Token closeBraceToken = getNextToken(input);
    if (closeBraceToken.type != RBRACE) {
        fprintf(stderr, "Error: Expected '}'.\n");
        exit(EXIT_FAILURE);
    }

    return classNode;
}

// Function to parse class body
struct ASTNode* parseClassBody(const char** input) {
    struct ASTNode* classBodyNode = createNode("ClassBody");

    // Parse members in the class body
    while (1) {
        struct Token nextToken = getNextToken(input);
        if (nextToken.type == KEYWORD && (strcmp(nextToken.lexeme, "public") == 0 || strcmp(nextToken.lexeme, "private") == 0)) {
            // Parse member (public or private)
            struct ASTNode* memberNode = parseMember(input);
            classBodyNode->left = memberNode;
            classBodyNode = memberNode;
        } else if (nextToken.type == RBRACE) {
            // End of class body
            return classBodyNode;
        } else {
            fprintf(stderr, "Error: Unexpected token in class body.\n");
            exit(EXIT_FAILURE);
        }
    }
}

// Function to parse class members (public or private)
struct ASTNode* parseMember(const char** input) {
    // Expect 'public' or 'private' keyword
    struct Token accessModifierToken = getNextToken(input);
    if (accessModifierToken.type != KEYWORD || (strcmp(accessModifierToken.lexeme, "public") != 0 && strcmp(accessModifierToken.lexeme, "private") != 0)) {
        fprintf(stderr, "Error: Expected 'public' or 'private' keyword.\n");
        exit(EXIT_FAILURE);
    }

    // Parse type
    struct ASTNode* typeNode = parseType(input);

    // Parse identifier
    struct ASTNode* identifierNode = parseIdentifier(input);

    // Expect ';'
    struct Token semicolonToken = getNextToken(input);
    if (semicolonToken.type != SEMICOLON) {
        fprintf(stderr, "Error: Expected ';'.\n");
        exit(EXIT_FAILURE);
    }

    // Create a node for the member
    struct ASTNode* memberNode = createNode("MemberDeclaration");
    strcpy(memberNode->label, accessModifierToken.lexeme);
    memberNode->left = typeNode;
    memberNode->right = identifierNode;

    return memberNode;
}

// Function to parse type
struct ASTNode* parseType(const char** input) {
    struct ASTNode* typeNode = createNode("Type");

    // Expect type keyword
    struct Token typeToken = getNextToken(input);
    if (typeToken.type != KEYWORD || (strcmp(typeToken.lexeme, "int") != 0 && strcmp(typeToken.lexeme, "float") != 0 && strcmp(typeToken.lexeme, "string") != 0)) {
        fprintf(stderr, "Error: Expected type keyword (int, float, or string).\n");
        exit(EXIT_FAILURE);
    }

    // Set type as label for the AST node
    strcpy(typeNode->label, typeToken.lexeme);

    return typeNode;
}

// Function to parse identifier
struct ASTNode* parseIdentifier(const char** input) {
    // Expect identifier
    struct Token identifierToken = getNextToken(input);
    if (identifierToken.type != IDENTIFIER) {
        fprintf(stderr, "Error: Expected identifier.\n");
        exit(EXIT_FAILURE);
    }

    // Create a node for the identifier
    struct ASTNode* identifierNode = createNode("Identifier");
    strcpy(identifierNode->label, identifierToken.lexeme);

    return identifierNode;
}

// Function to print the AST in a readable format
void printAST(struct ASTNode* root, int level) {
    if (root != NULL) {
        for (int i = 0; i < level; i++) {
            printf("  ");
        }
        printf("[%s]\n", root->label);
        printAST(root->left, level + 1);
        printAST(root->right, level + 1);
    }
}

int main() {
    const char* input = "class MyClass { public: int x; private: float y; };";

    // Parse the input and build the AST
    struct ASTNode* classNode = parseClass(&input);

    // Print the AST
    printf("Abstract Syntax Tree:\n");
    printAST(classNode, 0);

    return 0;
}
