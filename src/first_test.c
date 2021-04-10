#include "herd.h"
#include <stdio.h>

int main(void) {
    Symbol *a = terminal("a");
    Symbol *b = terminal("b");
    Symbol *c = terminal("c");
    Symbol *d = terminal("d");
    Symbol *S = nonterminal("S");
    Symbol *E = nonterminal("E");
    Symbol *F = nonterminal("F");
    Production *p1 = production(S, 2, a, S);
    Production *p2 = production(S, 1, E);
    Production *p3 = production(E, 2, F, b);
    Production *p4 = production(E, 2, S, c);
    Production *p5 = production(E, 2, epsilon(), d);
    Production *p6 = production(F, 1, d);
    Production *p7 = production(F, 1, epsilon());
    Grammar *g = grammar(S, 7, p1, p2, p3, p4, p5, p6, p7);
    printf("first(S)=");
    print_syms(S->first);
    printf("\n");
    printf("first(E)=");
    print_syms(E->first);
    printf("\n");
    printf("first(F)=");
    print_syms(F->first);
    printf("\n");
    printf("first(a)=");
    print_syms(a->first);
    printf("\n");
    printf("first(b)=");
    print_syms(b->first);
    printf("\n");
    printf("first(c)=");
    print_syms(c->first);
    printf("\n");
}
