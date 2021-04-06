#pragma once
#include <stdint.h>
#include <stddef.h>

typedef enum symbol_type {
    EPSILON_SYM, EOF_SYM, AUG_START_SYM, TERMINAL_SYM, NON_TERMINAL_SYM
} SymbolType;

typedef enum action_type {
    ACCEPT_ACT, SHIFT_ACT, REDUCE_ACT
} ActionType;

struct symbols;

typedef struct symbol {
    SymbolType type;
    char *name;
    struct symbols *first;
    struct symbols *follow;
} Symbol;

typedef struct symbols {
    Symbol *sym;
    struct symbols *next;
} Symbols; // TODO: hash set

typedef struct production {
    Symbol *head;
    Symbols *body;
} Production;

typedef struct productions {
    Production *prod;
    struct productions *next;
} Productions; // TODO: hash set (hash by head)

typedef struct item {
    Production *prod;
    uint32_t index;
} Item;

typedef struct grammar {
    Symbol *augstart;
    Symbols *syms;
    Productions *prods;
} Grammar;

Symbol *nonterminal(char *name);
Symbol *terminal(char *name);
Symbol *augstart(void);
Symbol *eof(void);
Production *production(Symbol *head, uint32_t body_size, ...);
Item *item(Production *prod, uint32_t index);
Grammar *grammar(Symbol *start, uint32_t prod_count, ...);

void *malloc_or_die(size_t size);
