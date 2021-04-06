#include "herd.h"
#include <stdarg.h>
#include <stdbool.h>

Symbols *add_sym(Symbol *sym, Symbols *syms) {
    Symbols *new_head = malloc_or_die(sizeof(Symbols));
    new_head->sym = sym;
    new_head->next = syms;
    return new_head;
}

Symbols *add_new_sym(Symbol *sym, Symbols *syms) {
    Symbols *head = syms;
    while (head) {
        if (head->sym == sym) {
            return syms;
        }
        head = head->next;
    }

    Symbols *new_head = malloc_or_die(sizeof(Symbols));
    new_head->sym = sym;
    new_head->next = syms;
    return new_head;
}

Productions *add_prod(Production *prod, Productions *prods) {
    Productions *new_head = malloc_or_die(sizeof(Productions));
    new_head->prod = prod;
    new_head->next = prods;
    return new_head;
}

void compute_first(Grammar *g) {

}

void compute_follow(Grammar *g) {

}

Symbol *symbol(SymbolType type, char *name) {
    Symbol *syms = malloc_or_die(sizeof(Symbol));
    syms->type = type;
    syms->name = name;
    syms->first = NULL;
    syms->follow = NULL;
    return syms;
}

Symbol *nonterminal(char *name) {
    return symbol(NON_TERMINAL_SYM, name);
}

Symbol *terminal(char *name) {
    return symbol(TERMINAL_SYM, name);
}

Symbol *augstart(void) {
    return symbol(AUG_START_SYM, NULL);
}

Symbol *eof(void) {
    return symbol(EOF_SYM, NULL);
}

Production *production(Symbol *head, uint32_t body_size, ...) {
    Symbols *body = malloc_or_die(sizeof(Symbols));
    va_list ap;
    va_start(ap, body_size);
    for (uint32_t i = 0; i < body_size; i++) {
        body = add_sym(va_arg(ap, Symbol *), body);
    }
    va_end(ap);
    Production *prod = malloc_or_die(sizeof(Production));
    prod->head = head;
    prod->body = body;
    return prod;
}

Grammar *grammar(Symbol *start, uint32_t prod_count, ...) {
    Productions *prods = malloc_or_die(sizeof(Productions));
    Symbols *syms = malloc_or_die(sizeof(Symbols));
    va_list ap;
    va_start(ap, prod_count);
    for (uint32_t i = 0; i < prod_count; i++) {
        Production *prod = va_arg(ap, Production *);
        prods = add_prod(prod, prods);
        syms = add_new_sym(prod->head, syms);
        Symbols *body = prod->body;
        while (body) {
            syms = add_new_sym(body->sym, syms);
            body = body->next;
        }
    }
    va_end(ap);

    Grammar *g = malloc_or_die(sizeof(Grammar));
    g->augstart = augstart();
    g->syms = syms;
    g->prods = prods;


    compute_first(g);
    compute_follow(g);

    return g;
}
