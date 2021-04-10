#include "herd.h"
#include <stdio.h>
#include <stdlib.h>

int main(void) {
    Symbol *e_ = epsilon();
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
    Production *p5 = production(E, 2, e_, d);
    Production *p6 = production(F, 1, d);
    Production *p7 = production(F, 1, e_);
    Grammar *g = grammar(S, 7, p1, p2, p3, p4, p5, p6, p7);
    printf("first(S)=");
    print_syms(S->first);
    printf("\n");
    printf("follow(S)=");
    print_syms(S->follow);
    printf("\n");
    printf("first(E)=");
    print_syms(E->first);
    printf("\n");
    printf("follow(E)=");
    print_syms(E->follow);
    printf("\n");
    printf("first(F)=");
    print_syms(F->first);
    printf("\n");
    printf("follow(F)=");
    print_syms(F->follow);
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
    free_symbol(a);
    free_symbol(b);
    free_symbol(c);
    free_symbol(d);
    free_symbol(S);
    free_symbol(E);
    free_symbol(F);
    free_symbol(e_);
    free_production(p1);
    free_production(p2);
    free_production(p3);
    free_production(p4);
    free_production(p5);
    free_production(p6);
    free_production(p7);
    free_grammar(g);

}
