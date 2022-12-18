//
// Created by Monika Zjavková on 05/04/2021.
//

#ifndef UNTITLED5_OA_H
#define UNTITLED5_OA_H


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

char **resize(char **array, int *size, int *full);


int hash(char string[]){
    int hash=0;
    int letter=0;
    int len = strlen(string);                                       //dĺžka slova

    //vypočítanie hash hodnoty pre slovo
    for(int i = 0; i<len; i++){

        //výpočet hashu pre jedno písmeno - hodnota sa líši od pozície, na ktorej sa nachádza v slove
        letter = (int)string[i] * pow(33,i);
        hash += letter;                                             //pripočítanie písmena už k hotovému slovu
    }

    return hash;
}


char **init(int size){
    char **array;
    array = (char **) malloc(size * sizeof(char*));
    for(int i = 0; i<size; i++){
        array[i]=NULL;
    }
    return array;
}

char **insert_OA(char **array, char string[], int *size, int *full){
    int h_string, index, new_index;
    int len = strlen(string);


    //V prípade, že je už napoly plné pole, zväčší sa, aby sa predišlo kolíziam
    if(*full > (*size)/2){
        array = resize(array, size,full);
    }

    h_string = hash(string);                                                     //vypočítanie hash hodnoty pre dané slovo
    index = abs(h_string % (*size));                                            //vypočítanie indexu z hash hodnoty

    //Ak sa v poli na danom mieste nič nenachádza, vloží sa tam reťazec
    if(array[index] == NULL){
        array[index] = (char *) malloc(len * sizeof(char));                 //alokovanie pamäte pre daný blok
        strcpy(array[index], string);                                           //vloženie slova
        (*full)++;                                                             //zväčší sa počet umiestnených reťazcov
        return array;
    }

        //V prípade, že už sa tam nachádza rovnaký reťazec, aký chceme vložiť, nič sa nezmení
    else if(!(strcmp(array[index], string))){
        return array;
    }

        //hľadanie najbližšej voľnej pozície
    else{
        new_index = index+1;                                                      //nový index, kde budeme ukladať reťazec
        while(new_index != index){                                               //cyklus skončí, ak sa reťazec vložil, alebo sa dostaneme na začiatok

            if(array[new_index] == NULL){                                        //ak sa na novom mieste nič, nenachádza, vloží sa
                array[new_index] = (char *) malloc(len * sizeof(char));
                strcpy(array[new_index],string);
                (*full)++;
                return array;
            }

            else if(!(strcmp(array[new_index],string))){                        //ak je tam rovnaký reťazec, nestane sa nič
                return array;
            }

            new_index = (new_index+1) % *size;                                                        //ak tam bola iná hodnota, posunieme sa o pozíciu doprava
        }
    }
}


char  **resize(char **array, int *size, int *full){
    int new_size = (*size*2)+1;                                                //vypočítanie novej veľkosti
    char **new_array;                                                          //vytvorenie nového poľa
    new_array = (char **) malloc(new_size * sizeof(char*));
    for(int i = 0; i<new_size; i++){
        new_array[i] = NULL;                                                     //nastavenie všetkých smerníkov na NULL
    }

    (*full)=0;
    for(int i=0; i<*size; i++){                                                 //Prehashovanie starej tabuľky a vloženie do novej
        if(array[i] != 0){
            insert_OA(new_array, array[i], &new_size,full);
        }
    }

    *size = new_size;                                                          //nastavenie novej veľkosti
    return new_array;                                                         //vratí nové pole
}

int find(char **array, char string[], int size){
    int  h_string = hash(string);                                                     //vypočítanie hash hodnoty pre dané slovo
    int index = abs(h_string % (size));                                      //vypočítanie indexu
    int new_index = index+1;


    if(array[index] == NULL){                                              //Aka sa tam nič nenachádza vráti chybu
        return 1;
    }

    else if(! (strcmp(array[index],string)) ){                             //skontroluje sa, či je na tom mieste
        return 0;
    }

    else{                                                                  //ak nie, prehľadá sa celé pole
        while(new_index!=index){                                           //ak došiel na prázdne miesto, nenachádza sa v poli
            if(array[new_index] == NULL){
                return 1;
            }
            if(! (strcmp(array[new_index],string)) ){                      //kontrola, či našlo hľadaný prvok
                return 0;
            }

            new_index = (new_index+1) % size;                                                   //posunutie o prvok doprava
        }
    }
    return 1;
}


void print(char **array, int size){
    for(int i=0; i<size; i++){
        printf("%d %s\n",i,array[i]);
    }
}

const char alphabet[] = "abcdefghijklmnopqrstuvwxyz";
int intN(int n) {
    return rand() % n;
}

char *randomString(int len) {
    char *s = malloc((len + 1) * sizeof(char));
    for (int i = 0; i < len; i++) {
        s[i] = alphabet[intN(strlen(alphabet))];
    }
    s[len] = '\0';
    return s;
}



/*
int main() {
    srand(time(0));
    int size = 2000001;
    int full = 0;

    //inicializácia poľa
    char **array = init(size);
    test_insert(array, &size, &full, 1000000);
    return 0;
}
 */


#endif //UNTITLED5_OA_H
