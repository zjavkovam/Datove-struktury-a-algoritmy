#include<stdio.h>
#include<stdlib.h>
#include <time.h>

#include "AVL.h"
#include "R-B.h"
#include "OA.h"
#include "Chain.h"

void test_AVL(long int max){

    long int *numbers, num;
    numbers = malloc(max*sizeof(long int));
    struct node *root = NULL;

    clock_t begin = clock();
    for(int i=0; i<max; i++){
        num = abs((rand() * rand())) % max;
        numbers[i] = num;
        root = insert(root, num);
    }

    printf("AVL TREE\n");
    clock_t end = clock();
    double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
    printf("Insert: %.3f\n",time_spent);

    clock_t b2 = clock();
    for(int i=0; i<max; i++){
        search(root,numbers[i]);
    }
    clock_t e2 = clock();
    double time_spent2 = (double)(e2 - b2) / CLOCKS_PER_SEC;
    printf("Find: %.3f\n",time_spent2);

}


void test_RB(long int max){

    long int *numbers, num;
    numbers = malloc(max*sizeof(long int));
    struct node_RB *root = NULL;


    clock_t begin = clock();
    for(int i=0; i<max; i++){
        num = (rand() * rand()) % max;
        numbers[i] = num;
        insert_RB(&root, num);
    }

    printf("RED-BLACK TREE\n");
    clock_t end = clock();
    double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
    printf("Insert: %.3f\n",time_spent);


    clock_t b2 = clock();
    for(int i=0; i<max; i++){
        search_RB(root,numbers[i]);
    }
    clock_t e2 = clock();
    double time_spent2 = (double)(e2 - b2) / CLOCKS_PER_SEC;
    printf("Find: %.3f",time_spent2);

}

void test_OA(char **array, int *size, int *full, int max){
    char *s;
    char ** str;
    str = (char **) malloc(max * sizeof(char*));
    int str_len = 5;

    clock_t begin = clock();
    for(int i=0; i<max; i++){
        str[i] = (char *) malloc(str_len * sizeof(char));
        s = randomString(str_len);
        strcpy(str[i],s);
        array = insert_OA(array, str[i] , size, full);
        if(find(array,s,*size)){
            printf("%s sa nepodarilo vložiť", s);
        }
    }

    printf("OPEN ADRESSING HASH TABLE\n");
    clock_t end = clock();
    double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
    printf("Insert: %.3f\n",time_spent);



    clock_t b2 = clock();
    for(int i=0; i<max; i++){
        find(array,str[i],max);
    }
    clock_t e2 = clock();
    double time_spent2 = (double)(e2 - b2) / CLOCKS_PER_SEC;
    printf("Find: %.3f",time_spent2);
}


void test_CH(int max){
    HashTable *hashtable = initCH();
    char *s;
    char ** str;
    str = (char **) malloc(max * sizeof(char*));
    int str_len = 20;

    clock_t begin = clock();
    for(int i=0; i<max; i++){
        str[i] = (char *) malloc(str_len * sizeof(char));
        s = randomString(str_len);
        strcpy(str[i],s);
        set(hashtable,s);
        if(get(hashtable,s)){
            printf("chyba");
        }
    }

    printf("CHAINING\n");
    clock_t end = clock();
    double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
    printf("Insert: %.3f\n",time_spent);
    clock_t b2 = clock();
    for(int i=0; i<max; i++){
        get(hashtable,str[i]);
    }
    clock_t e2 = clock();
    double time_spent2 = (double)(e2 - b2) / CLOCKS_PER_SEC;
    printf("Find: %.3f",time_spent2);
}


int main() {
    srand(time(0));
    test_AVL(100000);
    printf("\n\n");
    test_RB(100000);

    printf("\n\n");
    int size = 2000001;
    int full = 0;

    //inicializácia poľa
    char **array = init(size);
    test_OA(array, &size, &full, 100000);


    printf("\n\n");
    test_CH(100000);
    return 0;
}
