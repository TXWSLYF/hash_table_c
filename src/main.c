// main.c
#include <stdio.h>
#include "hash_table.h"

int main()
{
    ht_hash_table *ht = ht_new();
    ht_insert(ht, "name", "alex");
    ht_insert(ht, "age", "32");
    ht_insert(ht, "age", "34");
    ht_insert(ht, "name", "alex1");

    printf("%s\n", ht_search(ht, "name"));
    printf("%s\n", ht_search(ht, "age"));
}