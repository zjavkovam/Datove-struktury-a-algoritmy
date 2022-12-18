#import <stdio.h>
#import <stdlib.h>
#include <time.h>
#include <string.h>

#define first_free()     (*((int *) (memory)))
#define allock_head(p)   (*((int*) p))
#define allock_foot(p)   (*((int*) (p+sizeof(int)+abs(allock_head(p)))))
#define free_head(p)     (*((int*) (p-sizeof(int))))
#define next_block(p)    (*(int*) (p+sizeof(int)+abs((*(int*)(p-sizeof(int))))))
#define prev_block(p)    (*(int*) (p-2*sizeof(int)))

typedef struct block {
    int head;
    int next;
    int prev;
} block;

void *memory;
int memory_check(void *ptr);
void remove_from_list(struct block *delete);


void *memory_alloc(unsigned int size){
    int new_offset;
    block *blk, *next, *prev, *new;
    void *free = memory + first_free();                     //nájdenie prvého voľného bloku v pamäti

    //Upravenie veľkosti pamäte na alokáciu na párne číslo
    if(size%2==1) size++;

    //Ak nie je  žiadny voľný blok, vráti 0
    if(first_free()==0){
        return 0;
    }

    while(1){
        blk = free;

        //ak je blok dostatočne veľký cyklus skončí
        if(blk->head >= size){
            break;
        }

        //Ak je na konci -> nie je žiadný vhodný blok pamäte, vráti 0
        if(blk->next == 0){
            return 0;
        }

        //program prejde na ďalší voľný blok
        free = memory + blk->next;
    }

    //nájdenie predchádzajúceho a nasledujúceho bloku, ktoré sa nachádzajú pri vybranom voľnom bloku
    next = memory + blk->next;
    prev = memory + blk->prev;

    //ak treba blok rozdeliť a vložíť nový
    if(blk->head-size >= sizeof(block)+sizeof(int)){

        //vytvorí sa nový voľný blok
        new_offset = (int) (size + 2*(sizeof(int)));                                   //vypočíta sa offset novýho bloku
        new = free + new_offset;                                                       //vytvorenie nového voľného bloku
        new->next = blk->next;                                                         //offset ostane rovnaký ako pôvodného voľného bloku
        new->prev = blk->prev;
        new->head = blk->head-new_offset;                                              //hlavička nového voľného bloku
        *((int*) (free + new_offset + new->head + sizeof(int))) = new->head;           //pätička nového voľného bloku

        //ak sa blok nenachádza na začiatku alebo na konci, upraví sa offset v predchádzajúcom a nasledujúcom bloku
        if(blk->next!=0)
            next->prev += new_offset;
        if(blk->prev!=0)
            prev->next += new_offset;

        //ak je na začiatku upraví sa offset ku prvému voľnému bloku
        if(next == prev) {
            first_free() += size + 2*sizeof(int);
        }

        //spraví sa  vždy - nastaví sa hlavička a päta
        allock_head(free) = (int) size*(-1);
        allock_foot(free) = (int) size*(-1);
    }

    //ak sa  alokuje celý blok
    else{

        //blok sa vymaže zo zoznamu;
        remove_from_list(blk);

        //nastaví sa hlavička a päta
        allock_head(free) = blk->head * (-1);
        (*((int*)(free+sizeof(int)+abs(blk->head)))) = blk->head;
    }
    return (free + sizeof(int));
}

void remove_from_list(struct block *delete){
    block *next, *prev;
    if(delete->next == 0 && delete->prev==0){
        first_free() = 0;
    }
    else if(delete->prev == 0){
        next = memory + delete->next;
        first_free() = delete->next;
        next->prev = 0;
    }
    else if(delete->next==0){
        prev = memory + delete->prev;
        prev->next = 0;
    }
    else{
        next = memory + delete->next;
        prev = memory + delete->prev;
        next->prev = delete->prev;
        prev->next = delete->next;
    }
}

int memory_free(void *valid_ptr){
    //skontroluje sa či je blok alokovaný
    if(!memory_check(valid_ptr)){
        return 1;
    }

    block *freed, *next, *merge_next, *merge_prev, *temp;
    int freed_offset = (int) (valid_ptr-memory-sizeof(int));                                        //nový offset pre práve uvoľnený blok
    freed = valid_ptr-sizeof(int);                                                          //vytvorí sa nový voľný blok

    //Uvoľnenie alokovaného bloku
    freed->head = free_head(valid_ptr) * (-1);                                          //hlavička sa nastaví na uvoľnený blok
    *((int *) (valid_ptr + freed->head)) = freed->head;                                 //pätička nového uvoľneného bloku

    //ak zoznam blokov nie je voľný
    if (first_free() != 0) {
        next = memory + first_free();
        freed->next = first_free();                     //pôvodný prvý voľný sa nastaví ako ďalší pre nový voľný blok
        next->prev = freed_offset;                      //nový voľný blok sa nastaví ako predchádzajúci pre pôvodný
    }
    freed->prev = 0;
    first_free() = freed_offset;

    //**********************************************************************

    //SPÁJANIE VOĽNÝCH BLOKOV
    //Ak je zľava voľný blok
    if(valid_ptr!=memory+2*sizeof(int) && prev_block(valid_ptr)>0  &&  next_block(valid_ptr)<0){

        merge_prev = valid_ptr-prev_block(valid_ptr)-3*sizeof(int);                         //nájdenie predchádzajúceho voľného
        remove_from_list(merge_prev);

        merge_prev->head += (int) (freed->head + 2*sizeof(int));                            //nastavenie novej hlavičky
        *((int*)(valid_ptr+free_head(valid_ptr))) = merge_prev->head;                       //nastavenie novej pätičky
        first_free() = (int) (freed_offset - 2*sizeof(int)-prev_block(valid_ptr));          //nový offset pre prvý voľný

       if(memory+freed->next != merge_prev){            //smerník na ďalší
           merge_prev->next = freed->next;              //ak je v zozname ďalší merge_prev, smerníky sa nemenia
       }

       //upravenie offsetu ďalšiemu voľnému bloku v pamäti
       if(merge_prev->next != 0) {
           next = memory + merge_prev->next;
           next->prev = first_free();
       }

       merge_prev->prev=0;                              //offset späť je 0-> je na začiatku
    }

    //ak je sprava voľný blok
    else if((prev_block(valid_ptr)<0 || valid_ptr==memory+2*sizeof(int)) && next_block(valid_ptr)>0){

        merge_next = valid_ptr + freed->head+sizeof(int);                                       //nájdenie voľného bloku za uvoľneným

        remove_from_list(merge_next);                                               //odstráni sa zo zoznamu voľných blokov
        freed->head += (int) (merge_next->head+2*sizeof(int));                      //hlavička nového bloku
        *((int*)(valid_ptr+freed->head)) = freed->head;                             //nová pätička
        first_free() = freed_offset;                                                //presunutie bloku na začiatok zoznamu

        //smerníky ostávajú pre freed rovnaké, keďže sa nový blok len k nemu napojí a zvýši veľkosť
    }


    //ak je voľný blok z oboch strán
    else if(valid_ptr!=memory+2*sizeof(int) && prev_block(valid_ptr) > 0 && next_block(valid_ptr) > 0){

        merge_next = valid_ptr + freed->head + sizeof(int);                                          //ďalší voľný v pamäti
        merge_prev = valid_ptr-prev_block(valid_ptr)-3*sizeof(int);                                  //predchádzajúci voľný v pamäti
        remove_from_list(merge_next);
        remove_from_list(merge_prev);

        merge_prev->head = (int) (merge_prev->head + merge_next->head + freed->head + 4*(sizeof(int)));       //hlavička nového bloku
        *((int*)(valid_ptr+merge_next->head+freed->head+2*sizeof(int))) = merge_prev->head;                    //pätička nového bloku

        first_free() = (int) (freed_offset - 2*sizeof(int)-prev_block(valid_ptr));                            //nový offset

        //nastavenie pre offset na ďalší voľný blok podľa toho či v zozname je druhý pamäťovo ďalší/predchádzajúci
        temp = freed;
        while(memory+temp->next == merge_next || memory+temp->next == merge_prev){
            temp = memory + temp->next;
        }
        merge_prev->next = temp->next;

        if(merge_prev->next != 0) {
            next = memory + merge_prev->next;
            next->prev = first_free();
        }

        //upravím offset ďalšiemu voľnému bloku v pamäti
        if(merge_prev->next != 0) {
            next = memory + merge_prev->next;
            next->prev = first_free();
        }
        merge_prev->prev=0;                              //offset späť je 0-> je na začiatku

    }
    return 0;
}

int memory_check(void *ptr){
    if(ptr == NULL || ptr<=memory){
        return 0;
    }

    //funkcia prechádza všetky alokované bloky, kým nenájde zhodu
    void *head = memory+sizeof(int);
    while (head + sizeof(int) != ptr){

        //ak je v hlavičke -1, znamená to, že funkcia je na konci a nenašla zhodu
        if( (*((int *) (head))) == -1){
            return 0;
        }

        head = head + abs(*((int*)head)) + 2*sizeof(int);
    }

    //kontrola, či hlavička je záporná -> alokovaná
    if( *((int*)head) < 0){
        return 1;
    }
    return 0;
}

void memory_init(void *ptr, unsigned int size){
    //Upravenie veľkosti pamäte na párne číslo
    if(size%2) size--;

    //naplnenie pamäte 1
    for(int i=0; i<size; i++){
        *((char*)(ptr+i)) = 1;
    }

    memory = ptr;
    block *blk = memory + sizeof(int);                            //vytovrenie voľného bloku v pamäti
    blk->head = (int) (size - 4 * (sizeof(int)));                 //veľkosť voľného bloku
    blk->next = 0;
    blk->prev = 0;

    *((int*)(memory+blk->head+3*sizeof(int))) = -1;              //označenie konca pamäte
    *((int*)(memory+blk->head+2*sizeof(int))) = blk->head;       //nastavenie pätičky voľného bloku
    first_free() = sizeof(int);                                  //offset ku prvému voľnému bloku
}

void maloc_test(char *region,int min, int max, int min_block, int max_block, char random){
    char* pointer;
    int size, sum=0, my_malloc=0, malloc_count=0, sum_count=0;
    int memory_size = (rand() % (max - min + 1)) + min;;
    size=(rand() % (max_block - min_block + 1)) + min_block;
    memory_init(region, size);


    while(1){
        //ak sa testuje alokovanie náhodných veľkostí blokov
        if(random == 'r'){
            while(sum+memory_size > size && size - sum <= min) {
                memory_size = (rand() % (max - min + 1)) + min;
            }
        }

        //ak sa alokuje, pripočíta sa aktuálna veľkosť bloku
        pointer = (char *) memory_alloc(memory_size);
        if (memory_check(pointer)) {
            my_malloc += memory_size;
            memset(pointer, 0, memory_size);
            malloc_count++;
        }
        else if(pointer!=NULL){
            printf("ERROR\n");
        }

        sum += memory_size;
        sum_count++;

        //test pokračuje, kým voľná pamäť pri ideálnom alokovaní nie je menšia ako najmenší možný blok
        if (size - sum <= min) {
            break;
        }
    }

    //vypočítanie percento úspešnosti
    float blocks = ((float)malloc_count / (float) sum_count) * 100;
    if(random!='r') {
        printf("Memory size: %d bytes: allocated %.2f%% blocks, size of block:%d\n", size, blocks, memory_size);
    }
    else{
        printf("Memory size: %d bytes: allocated %.2f%% blocks, size of block: %d - %d bytes\n", size, blocks, min,max);
    }
}

void free_tester(char *region,int min, int max, int min_block, int max_block, char side){
    int my_malloc=0, count_malloc = 0, freed = 0, index;
    int size = (rand() % (max_block - min_block + 1)) + min_block;
    int memory_size;
    char* pointer;
    char* pointers[size/8];
    memory_init(region, size);

    //pôvodná hlavička voľného bloku
    int beg_head = *((int*)(memory + sizeof(int))), end_head;


    //alokovanie náhodne veľkých blokov pamäte
    while(1){
        memory_size = (rand() % (max - min + 1)) + min;
        pointer = (char *) memory_alloc(memory_size);
        if(memory_check(pointer)){
            my_malloc += memory_size;
            pointers[count_malloc] = pointer;
            count_malloc++;
        }
        else{
            break;
        }
    }

    //pri uvoľňovaní od začiatku pamäte - testovanie spájania zľava
    if(side == 'l') {
        for (int i = 0; i < count_malloc; i++) {
            memory_free(pointers[i]);
        }
    }

    //pri uvoľňovaní od konca pamäte - testovanie sprava
    if(side == 'r'){
        for (int i = count_malloc-1; i >= 0; i--) {
            memory_free(pointers[i]);
        }
    }

    //náhodné uvoľňovanie pamäte
    if(side == 'b'){
        while(freed != count_malloc){
            index = (rand() % ((count_malloc-1) - 0 + 1)) + 0;
            if(memory_check(pointers[index])){
                memory_free(pointers[index]);
                freed++;
            }
        }
    }
    //ak sa pôvodná hlavička rovná hlavičke na konci, pamäť bola uvoľnená úspešne
    end_head = *((int*)(memory + sizeof(int)));
    if(beg_head != end_head){
        printf("ERROR");
    }
    else{
        printf("Memory of size %d was freed successfully.\n",size);
    }

}

int main(){
    srand(time(0));
    char region[110000];
    char* pointer[13000];

    //TESTOVANIE ALOKOVANIA PAMÄTI
    //prideľovanie rovnakých blokov malej veľkosti - 8 až 24 bajtov, pri použití malých celkových blokov - 50,100 a 200 bajtov
    maloc_test(region,8,24,50,50, 'x');
    maloc_test(region,8,24,100,100, 'x');
    maloc_test(region,8,24,200,200, 'x');
    printf("\n");

    //prideľovanie nerovnakých blokov  veľkosti - 8 až 24 bajtov, pri použití malých celkových blokov - 50,100 a 200 bajtov
    maloc_test(region,8,24,50,50, 'r');
    maloc_test(region,8,24,100,100, 'r');
    maloc_test(region,8,24,200,200, 'r');
    printf("\n");

    //prideľovanie nerovnakých blokov väčšej veľkosti - 500 až 5000 bajtov, pri použití väčších celkových blokov - aspoň 1000
    maloc_test(region,500,5000,10000,10000, 'r');
    maloc_test(region,500,5000,10000,50000, 'r');
    maloc_test(region,500,5000,10000,100000, 'r');
    printf("\n");

    //prideľovanie nerovnakých blokov malých aj veľkých- 8 až 50000 bajtov, pri použití väčších celkových blokov - aspoň 1000
    maloc_test(region,8,5000,10000,30000, 'r');
    maloc_test(region,8,10000,20000,50000, 'r');
    maloc_test(region,8,50000,40000,100000, 'r');
    printf("\n");


    //TESTOVANIE UVOĽŇOVANIA PAMÄTI
    //testovanie uvoľňovania pamäti pri uvoľňovaní od začiatku pamäti - rovnakých blokov aj náhodne veľkých blokoch
    free_tester(region,16,16,100,200, 'l');
    free_tester(region,8,24,100,200, 'l');
    free_tester(region,16,16,5000,50000, 'l');
    free_tester(region,8,24,5000,50000, 'l');
    printf("\n");

    //testovanie uvoľňovania pamäti pri uvoľňovaní od konca pamäti - rovnakých blokov aj náhodne veľkých blokoch
    free_tester(region,16,16,100,200, 'r');
    free_tester(region,8,24,100,200, 'r');
    free_tester(region,16,16,5000,50000, 'r');
    free_tester(region,8,24,5000,50000, 'r');
    printf("\n");

    //testovanie uvoľňovania pamäti pri uvoľňovaní pamäti v náhodnom poradí - rovnakých blokov aj náhodne veľkých blokoch
    free_tester(region,16,16,100,200, 'b');
    free_tester(region,8,24,100,200, 'b');
    free_tester(region,16,16,5000,50000, 'b');
    free_tester(region,8,24,5000,50000, 'b');

    return 0;
}

