#include "herd.h"
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void print_sym(Symbol *sym) {
    switch (sym->type) {
        case EPSILON_SYM:
            printf("_");
            break;
        case EOF_SYM:
            printf("$");
            break;
        case AUG_START_SYM:
            printf("^");
            break;
        default:
            printf("%s", sym->name);
    }
}

void print_syms(Symbols *syms) {
    while (syms) {
        print_sym(syms->sym);
        printf(" ");
        syms = syms->next;
    }
}

bool sym_eq(Symbol *sym1, Symbol *sym2) {
    if (sym1 == sym2) {
        return true;
    } else {
        SymbolType t1 = sym1->type;
        SymbolType t2 = sym2->type;
        if ((t1 == EPSILON_SYM && t2 == EPSILON_SYM)
                || (t1 == EOF_SYM && t2 == EOF_SYM)
                || (t1 == AUG_START_SYM && t2 == AUG_START_SYM)) {
            return true;
        } else if ((t1 == TERMINAL_SYM && t2 == TERMINAL_SYM)
                || (t1 == NON_TERMINAL_SYM && t2 == NON_TERMINAL_SYM)) {
            return strcmp(sym1->name, sym2->name) == 0;
        }
    }
    return false;
}

Symbols *add_sym(Symbol *src, Symbols *dst) {
    Symbols *new_head = malloc_or_die(sizeof(Symbols));
    new_head->sym = src;
    new_head->next = dst;
    return new_head;
}

Symbols *add_new_sym(Symbol *src, Symbols *dst, bool *altered) {
    Symbols *head = dst;
    while (head) {
        if (sym_eq(head->sym, src)) {
            return dst;
        }
        head = head->next;
    }

    *altered = true;

    Symbols *new_head = malloc_or_die(sizeof(Symbols));
    new_head->sym = src;
    new_head->next = dst;
    return new_head;
}

Symbols *add_all_new_syms(Symbols *src, Symbols *dst, bool *altered) {
    while (src) {
        // Do not copy epsilons into list (useful for computing first and follow)
        if (src->sym->type != EPSILON_SYM) {
            dst = add_new_sym(src->sym, dst, altered);
        }
        src = src->next;
    }
    return dst;
}

bool contains_epsilon(Symbols *syms) {
    while (syms) {
        if (syms->sym->type == EPSILON_SYM) {
            return true;
        }
        syms = syms->next;
    }
    return false;
}

Productions *add_prod(Production *prod, Productions *prods) {
    Productions *new_head = malloc_or_die(sizeof(Productions));
    new_head->prod = prod;
    new_head->next = prods;
    return new_head;
}

void compute_first(Grammar *g) {
    Symbols *syms = g->syms;
    while (syms) {
        Symbol *sym = syms->sym;
        if (sym->type == TERMINAL_SYM) {
            sym->first = add_sym(sym, sym->first);
        }
        syms = syms->next;
    }
    bool altered;
    do {
        altered = false;
        Productions *prods = g->prods;
        while (prods) {
            Production *prod = prods->prod;
            Symbol *head = prod->head;
            Symbol **body = prod->body;
            uint32_t body_size = prod->body_size;
            uint32_t i;
            for (i = 0; i < body_size; i++) {
                Symbol *sym = body[i];
                Symbols *first = sym->first;
                head->first = add_all_new_syms(first, head->first, &altered);
                if (!contains_epsilon(first)) {
                    break;
                }
            }
            if (i == body_size) { // Epsilon present in first set of every symbol in production
                head->first = add_new_sym(g->epsilon, head->first, &altered);
            }
            prods = prods->next;
        }
    } while (altered);
}

void compute_follow(Grammar *g) {
    g->start->follow = add_sym(g->eof, g->start->follow);
    bool altered;
    do {
        altered = false;
        Productions *prods = g->prods;
        while (prods) {
            Production *prod = prods->prod;
            Symbol *head = prod->head;
            Symbol **body = prod->body;
            uint32_t body_size = prod->body_size;
            for (uint32_t i = 0; i < body_size-1; i++) {
                Symbol *sym = body[i];
                SymbolType t = sym->type;
                Symbol *next = body[i+1];
                Symbols *next_first = next->first;
                if (t == NON_TERMINAL_SYM) {
                    sym->follow = add_all_new_syms(next_first, sym->follow, &altered);
                    if (contains_epsilon(next_first)) {
                        sym->follow = add_all_new_syms(head->follow, sym->follow, &altered);
                    }
                }
            }
            Symbol *last = body[body_size-1];
            if (last->type == NON_TERMINAL_SYM) {
                last->follow = add_all_new_syms(head->follow, last->follow, &altered);
            }
            prods = prods->next;
        }
    } while (altered);
}

Symbol *symbol(SymbolType type, char *name) {
    Symbol *sym = malloc_or_die(sizeof(Symbol));
    sym->type = type;
    sym->name = name;
    sym->first = NULL;
    sym->follow = NULL;
    return sym;
}

void free_symbol(Symbol *sym) {
    Symbols *first = sym->first;
    while (first) {
        Symbols *next = first->next;
        free(first);
        first = next;
    }
    Symbols *follow = sym->follow;
    while (follow) {
        Symbols *next = follow->next;
        free(follow);
        follow = next;
    }
    free(sym);
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

Symbol *epsilon(void) {
    Symbol *sym = symbol(EPSILON_SYM, NULL);
    sym->first = add_sym(sym, sym->first);
    return sym;
}

Production *production(Symbol *head, uint32_t body_size, ...) {
    Symbol **body = malloc_or_die(body_size * sizeof(Symbol *));
    va_list ap;
    va_start(ap, body_size);
    for (uint32_t i = 0; i < body_size; i++) {
        body[i] = va_arg(ap, Symbol *);
    }
    va_end(ap);
    Production *prod = malloc_or_die(sizeof(Production));
    prod->head = head;
    prod->body_size = body_size;
    prod->body = body;
    return prod;
}

void free_production(Production *prod) {
    free(prod->body);
    free(prod);
}

Grammar *grammar(Symbol *start, uint32_t prod_count, ...) {
    Productions *prods = NULL;
    Symbols *syms = NULL;
    va_list ap;
    va_start(ap, prod_count);
    for (uint32_t i = 0; i < prod_count; i++) {
        Production *prod = va_arg(ap, Production *);
        prods = add_prod(prod, prods);
        bool altered = false;
        syms = add_new_sym(prod->head, syms, &altered);
        Symbol **body = prod->body;
        uint32_t body_size = prod->body_size;
        for (uint32_t j = 0; j < body_size; j++) {
            Symbol *sym = body[j];
            syms = add_new_sym(sym, syms, &altered);
        }
    }
    va_end(ap);

    Grammar *g = malloc_or_die(sizeof(Grammar));
    g->eof = eof();
    g->epsilon = epsilon();
    g->augstart = augstart();
    g->augstart_prod = production(g->augstart, 1, start);
    prods = add_prod(g->augstart_prod, prods);
    g->start = start;
    g->syms = syms;
    g->prods = prods;

    compute_first(g);
    compute_follow(g);

    return g;
}

void free_grammar(Grammar *g) {
    free_symbol(g->eof);
    free_symbol(g->epsilon);
    free_symbol(g->augstart);
    free_production(g->augstart_prod);
    Productions *prods = g->prods;
    while (prods) {
        Productions *next = prods->next;
        free(prods);
        prods = next;
    }
    Symbols *syms = g->syms;
    while (syms) {
        Symbols *next = syms->next;
        free(syms);
        syms = next;
    }
    free(g);
}
