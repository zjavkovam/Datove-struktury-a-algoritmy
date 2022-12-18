//
// Created by Monika Zjavková on 05/04/2021.
//

#ifndef UNTITLED5_AVL_H
#define UNTITLED5_AVL_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>


struct node{
    long int data;
    long int height;
    struct node *left;
    struct node *right;
};

struct node* new_node(long int data){
    struct node *new = (struct node*)malloc(sizeof(struct node));
    new->data = data;
    new->height = 1;
    new->left = NULL;
    new->right = NULL;
    return new;
}


//Nastavenie novej výšky
void update_height(struct node *node){
    long int left=0, right=0;

    //bude sa s ňou počítať iba v prípade, že nie je uzol NULL
    if(node->left != NULL){
        left = node->left->height;
    }
    if(node->right != NULL){
        right = node->right->height;
    }

    //Vyberie sa maximum
    if(left > right){
        node->height = left+1;
    }
    else{
        node->height = right+1;
    }
}

struct node *right(struct node *node){
    struct node *a = node->left;
    struct node *b = a->right;

    //rotácia doprava
    a->right = node;
    node->left = b;

    //nastavenie nových výšok
    update_height(node);
    update_height(a);
    return a;
}


struct node *left(struct node *node){
    struct node *a = node->right;
    struct node *b = a->left;

    //rotácia doľava
    a->left = node;
    node->right = b;

    //nastavenie novej výšky
    update_height(node);
    update_height(a);
    return a;
}

struct node *balance(struct node *node, long int data){
    long int balance=0;

    //vypočítanie koeficientu vyváženia
    //výška ľavého - výška pravého podstromu
    if(node->left != NULL){
        balance += node->left->height;
    }
    if(node->right != NULL){
        balance -= node->right->height;
    }


    //Určenie rotácie podľa koeficientu vyváženia
    if(balance > 1){
        //rotácia 2x vľavo
        if(data < node->left->data){
            return right(node);
        }
        else{
            //rotácia 2x vpravo
            node->left = left(node->left);
            return right(node);
        }
    }

    else if(balance < -1){
        //rotácia 2x vpravo
        if(data > node->right->data){
            return left(node);
        }
        else{
            //rotácia vpravo-> vľavo
            node->right = right(node->right);
            return left(node);
        }
    }

    else{
        //nezmenený uzol
        return node;
    }
}

struct node* insert(struct node *node, long int data){

    //pridanie ako do nevyváženého stromu
    if(node == NULL) {
        return new_node(data);
    }
    else if(data < node->data) {
        node->left = insert(node->left, data);
    }
    else if (data > node->data) {
        node->right = insert(node->right, data);
    }
    else {
        return node;
    }

    //nastavenie novej výšky a vyváženie
    update_height(node);
    node = balance(node,data);
    return node;
}


int search(struct node *root, long int data){
    struct node *now = root;
    while(now->data!=data){
        if(now->data>data){
            now=now->left;
        }
        else{
            now = now->right;
        }
        if(now==NULL){
            return 1;
        }
    }
    return 0;
}

void test(long int max){

    long int *numbers, num;
    numbers = malloc(max*sizeof(long int));
    struct node *root = NULL;

    clock_t begin = clock();
    for(int i=0; i<max; i++){
        num = (rand() * rand()) % max;
        numbers[i] = num;
        root = insert(root, num);
    }

    clock_t end = clock();
    double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
    printf("%.3f\n",time_spent);

    clock_t b2 = clock();
    for(int i=0; i<max; i++){
        search(root,numbers[i]);
    }
    clock_t e2 = clock();
    double time_spent2 = (double)(e2 - b2) / CLOCKS_PER_SEC;
    printf("%.3f",time_spent2);

}

/*

int main() {
    srand(time(0));
    test(10000);
    return 0;
}

*/

#endif //UNTITLED5_AVL_H
