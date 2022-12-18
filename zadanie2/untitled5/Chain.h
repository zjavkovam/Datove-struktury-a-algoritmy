//
// Created by Monika Zjavková on 05/04/2021.
//

#ifndef UNTITLED5_CHAIN_H
#define UNTITLED5_CHAIN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#define HASHSIZE 1000000

typedef struct HashNode
{
    char *value;
    struct HashNode *next;
} HashNode;

typedef struct HashTable
{
    HashNode *slots[HASHSIZE];
} HashTable;


void set(HashTable *hashtable, char *value)
{
    int key = hash(value);
    HashNode *node = (HashNode *)malloc(sizeof(HashNode));
    node->value = value;
    node->next = NULL;

    int slot = abs(key % HASHSIZE);
    HashNode *slot_node = hashtable->slots[slot];
    if (slot_node == NULL)
    {
        hashtable->slots[slot] = node;
    }
    else
    {
        while (slot_node->next != NULL && !(strcmp(value, slot_node->value)))
        {
            slot_node = slot_node->next;
        }

        if(strcmp(value, slot_node->value)) {
            slot_node->next = node;
        }
    }
}


int get(HashTable *hashtable, char *value)
{
    int key = hash(value);
    int slot = abs(key%HASHSIZE);
    HashNode *node = hashtable->slots[slot];

    while (node != NULL && strcmp(node->value, value))
    {
        node = node->next;
    }

    if (node == NULL)
    {
        return 1;
    }
    return 0;
}


HashTable *initCH()
{
    HashTable *hashtable = (HashTable *)malloc(sizeof(HashTable));

    for (int i = 0; i < HASHSIZE; i++)
    {
        hashtable->slots[i] = NULL;
    }
    return hashtable;
}


/*
int main()
{
    srand(time(0));
    int max = 1000000;
    test_insert(max);
    return 0;
};
*/

#endif //UNTITLED5_CHAIN_H
