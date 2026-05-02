#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "strtab.h"

param *working_list_head = NULL;
param *working_list_end = NULL;
table_node *current_scope = NULL;

// internal helpers

//duplicate string with malloc
static char *dup_str(const char *s) {
    if (!s) return NULL;
    char *copy = (char *)malloc(strlen(s) + 1);
    if (!copy ){
        fprintf(stderr, "Out of memory duplicating string\n");
        exit(1);
    }
    strcpy(copy, s);
    return copy;
}

static unsigned long hash_id(const char *id, const char *scope) {
    unsigned long hash = 5381;
    int c;

    if (scope) {
        const char *p = scope;
        while ((c = *p++) != 0) {
            hash = ((hash << 5) + hash) + c;
        }
    }

    if (id) {
        const char *p = id;
        while ((c = *p++) != 0){
            hash = ((hash << 5) + hash) + c;
        }
    }
    return hash % MAXIDS;
}

// allocate and zero a new table node
static table_node *alloc_table_node(table_node *parent) {
    table_node *node = (table_node *)malloc(sizeof(table_node));
    if (!node) {
        fprintf(stderr, "Out of memory allocating table_node\n");
        exit(1);
    }
    for (int i = 0; i < MAXIDS; i++) {
        node -> strTable[i] = NULL;
    }
    node->numChildren = 0;
    node->parent = parent;
    node->first_child = NULL;
    node->last_child = NULL;
    node->next = NULL;

    return node;
}

// look for id in current scope using the entrys stored scope string
static int lookup_in_current_scope(char *id){
    if (!current_scope) return -1;

    for (int i = 0; i < MAXIDS; i++) {
        if (current_scope->strTable[i] != NULL && strcmp(current_scope->strTable[i]->id, id) == 0) {
            return i;
        }
    }
    return -1;
}

// public API from strtab.h

// inserts a symbol into the current symbol table tree
int ST_insert(char *id, int data_type, int symbol_type, int *scope) {
    // create global scope
    if (current_scope == NULL){
        current_scope = alloc_table_node(NULL);
    }
    
    char *scope_str = (char *)scope;
    if(scope_str == NULL) {
        scope_str = "";
    }

    unsigned long idx = hash_id(id, scope_str);
    unsigned long start = idx;

    while (1) {
        if(current_scope->strTable[idx] == NULL) {
            symEntry *entry = (symEntry *)malloc(sizeof(symEntry));
            if (!entry) {
                fprintf(stderr, "Out of memory allocating symEntry\n");
                exit(1);
            }

            entry->id = dup_str(id);
            entry->scope = dup_str(scope_str);
            entry->data_type = data_type;
            entry->symbol_type = symbol_type;
            entry->size = 0;
            entry->params = NULL;

            current_scope->strTable[idx] = entry;
            return (int)idx;
        }

        idx = (idx + 1) % MAXIDS;
        if (idx == start) {
            return -1;
        }
    }
}

//look up symbol from current scope upward through its parents
symEntry *ST_lookup(char *id){
    table_node *node = current_scope;
    while (node != NULL) {
        for (int i = 0; i < MAXIDS; i++) {
            if (node->strTable[i] != NULL && strcmp(node->strTable[i]->id, id) == 0) {
                return node->strTable[i];
            }
        }
        node = node->parent;
    }
    return NULL;
}

symEntry *ST_lookup_current(char *id) {
    if (current_scope == NULL){
        return NULL;
    }
    for (int i = 0; i < MAXIDS; i++) {
        if (current_scope->strTable[i] != NULL && strcmp(current_scope->strTable[i]->id, id) == 0){
            return current_scope->strTable[i];
        }
    }
    return NULL;
}

// Create a param node and appends to working list
void add_param(int data_type, int symbol_type){
    param *p = (param *)malloc(sizeof(param));
    if (!p) {
        fprintf(stderr, "Out of memory allocating param\n");
        exit(1);
    }
    p->data_type = data_type;
    p->symbol_type=symbol_type;
    p->next = NULL;

    if(working_list_head == NULL) {
        working_list_head = p;
        working_list_end = p;
    } else {
        working_list_end->next = p;
        working_list_end = p;
    }
}

//attach parameter list to function entry at index i in the parent scope
void connect_params(int i, int num_params){
    if (!current_scope || !current_scope->parent){
        return;
    }

    if(i < 0 || i >= MAXIDS) return;

    table_node *func_scope_node = current_scope->parent;
    symEntry *entry = func_scope_node->strTable[i];

    if (!entry) return;

    entry->params = working_list_head;
    entry->size = num_params;

    //reset working list
    working_list_head = NULL;
    working_list_end = NULL;
}

// Create a new scope called at function body on start
void new_scope() {
    if (current_scope == NULL){
        current_scope = alloc_table_node(NULL);
        return;
    }
    table_node *child = alloc_table_node(current_scope);

    if(current_scope->first_child == NULL){
        current_scope->first_child= child;
        current_scope->last_child = child;
    } else {
        current_scope->last_child->next = child;
        current_scope->last_child = child;
    }
    current_scope->numChildren += 1;
    current_scope = child;
}

// moves towards the root of the symbol table tree
void up_scope() {
    if (current_scope && current_scope->parent) {
        current_scope = current_scope->parent;
    }
}

char *get_symbol_id(int idx) {
    table_node *node = current_scope;
    while (node != NULL) {
        if (idx >= 0 && idx < MAXIDS && node->strTable[idx] != NULL)
            return node->strTable[idx]->id;
        node = node->parent;
    }
    return "";
}

static void print_scope(table_node *node, int level) {
    if (node == NULL) return;
    for (int i = 0; i < MAXIDS; i++) {
        if (node->strTable[i] != NULL) {
            symEntry *e = node->strTable[i];
            printf("id=%s scope=%s data_type=%d symbol_type=%d size=%d\n",
                   e->id, e->scope, e->data_type, e->symbol_type, e->size);
        }
    }
    table_node *child = node->first_child;
    while (child != NULL) {
        print_scope(child, level + 1);
        child = child->next;
    }
}

void print_sym_tab(void) {
    table_node *root = current_scope;
    while (root && root->parent) root = root->parent;
    print_scope(root, 0);
}