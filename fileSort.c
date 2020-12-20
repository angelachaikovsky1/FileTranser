#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>
/*
    long story short:
    1. Read in files
    2. Int or String files?
    3. sort quick/ins
    4. print in ascending
    5. clear mem
    I ALSO NEED TO INCLUDE THE CORRECT INCLUDE STATEMENTS!
*/
//if we cant have globals then ill figure out a way around this.
// (worst case, length will be attached as the last node or soemthing)
int length = 0;
char type = 'n'; //for none will change after
int sizeOfLongestStr = 0;

struct nodeData{
    char *string;
    int strLen;
    int integer;
    struct nodeData * next;
};
void qsInts(int * number,int start,int end, int (*comparator)(void*, void*));
void qsStrings(char **strings,int start,int end, int (*comparator)(void*, void*));
int insertionSort( void* toSort, int (*comparator)(void*, void*));
int quickSort( void* toSort, int (*comparator)(void*, void*));
int comparator(void * a, void * b);
int simpleStrCmp(char * a, char * b);
void printFinal(struct nodeData * list, char t);
void freeList(struct nodeData * list);


int main(int argc, char ** argv)
{
    //argv[0] is irrelevent
    //argv[1] must be -q or -i
    //argv[2] must be file-to-be-sorted name

    char* fileName = NULL;
    char* sortType = NULL;
//printf("hello\n");
    if (argc < 3){
        printf("Fatal Error: expected two arguments, had %d \n", (argc-1));
        return 0;
    }else{
        //make sure input is proper
        //load in type of sort
        if(simpleStrCmp(argv[1], "-q") == 0){
            sortType = argv[1];
        }else if(simpleStrCmp(argv[1], "-i") == 0){
            sortType = argv[1];
        }else{
            //improperly formatted, fatal error
            printf("Fatal Error: has occurred \"%s\" is not a valid sort flag\n", argv[1]);
            return 0;
        }
        //dont forget to also set the third arg to filename
        // we will check for errors in the file later
        fileName = argv[2];
    }

    int fd = open(fileName, O_RDONLY);

    if(fd < 0){
        //or it doesnt exist??
        printf("Fatal Error: file \"%s\" does not exit\n", argv[2]);
        return 0;
    }

//printf("bye\n");
    //TODO:
    //char array buff
    //64 or BIGGER CUZ FRAN COULD TROLL US
    struct nodeData * head;
    struct nodeData * ptr;
    char * buffer = (char*)malloc(100*sizeof(char));
    int bufferSize = 0;
    int currentMax = 100;

    int bytes;

    while(1){
        //read in data
        //ignore white space
        //stop reading when you hit a comma
        //IMPLIMENT NONBLOCKING CRAP
        //keep buffer size for when transfering data (to know how much to transfer)
        //set bufferSize = 0 before relooping
        bytes = read(fd, (buffer + bufferSize), 1);



        //can read ever return -1? yes... if error... bleh
        if(bytes == 0){
            //make last structure then break
            if(bufferSize != 0){
                buffer[bufferSize] = '\0';
                if(head == NULL){
                    head = (struct nodeData *)malloc(sizeof(struct nodeData));
                    ptr = head;
                    length++;
                }else{
                    ptr->next = (struct nodeData *)malloc(sizeof(struct nodeData));
                    ptr = ptr->next;
                    length++;
                }
                if(buffer[bufferSize - 1] >= '0' && buffer[bufferSize - 1] <= '9'){
                    //should work for negative numbers too
                    //is int
                    type = 'i';
                    int toSave = atoi((const char *)(buffer));
                    ptr->integer = toSave;
                }else{
                    type = 's';
                    //can we use strcpy? if not do manually...
                    ptr->string = (char *)malloc(bufferSize*sizeof(char));
                    strcpy(ptr->string, buffer);
                    ptr->strLen = (bufferSize + 1);
                    if((bufferSize + 1) > sizeOfLongestStr){
                        sizeOfLongestStr = (bufferSize + 1);
                    }
                }
            }
            break;
        }



        if((buffer[bufferSize] != ',') && (isspace(buffer[bufferSize]) == 0)){
            bufferSize++;
        }else if(buffer[bufferSize] == ','){
            //create new instance

            if(head == NULL){
                ptr = (struct nodeData *)malloc(sizeof(struct nodeData));
                head = ptr;
                length++;
            }else{
                ptr->next = (struct nodeData *)malloc(sizeof(struct nodeData));
                ptr = ptr->next;
                length++;
            }

            if(bufferSize == 0){
                //means empty string
                if(type == 'i'){
                  ptr->integer = 0;
                }else{
                  //if not known or is string do this
                  ptr->string = (char *)malloc(bufferSize*sizeof(char));
                  ptr->string[0] = '\0';
                  ptr->integer = 0;
                  ptr->strLen = (bufferSize + 1);
                  if((bufferSize + 1) > sizeOfLongestStr){
                        sizeOfLongestStr = (bufferSize + 1);
                  }
                }
            }else{
                buffer[bufferSize] = '\0';
                //something is cookin in there !
                if (type != 'n'){
                    //dont set the type over and over again
                     if(type == 'i'){
                        int toSave = atoi((const char *)(buffer));
                        ptr->integer = toSave;
                    }else{
                        ptr->string = (char *)malloc(bufferSize*sizeof(char));
                        strcpy(ptr->string, buffer);
                        ptr->strLen = (bufferSize + 1);
                        if((bufferSize + 1) > sizeOfLongestStr){
                            sizeOfLongestStr = (bufferSize + 1);
                        }
                    }
                }else{
                    if(buffer[bufferSize - 1] >= '0' && buffer[bufferSize - 1] <= '9'){
                        //should work for negative numbers too
                        //is int
                        type = 'i';
                        int toSave = atoi((const char *)(buffer));
                        ptr->integer = toSave;
                    }else{
                        type = 's';
                        //can we use strcpy? if not do manually...
                        ptr->string = (char *)malloc(bufferSize*sizeof(char));
                        strcpy(ptr->string, buffer);
                        ptr->strLen = (bufferSize + 1);
                        if((bufferSize + 1) > sizeOfLongestStr){
                            sizeOfLongestStr = (bufferSize + 1);
                        }
                    }
                }
            }
        bufferSize = 0;
        }

         if(bufferSize == currentMax){
            //make more space
            char * newBuff = (char*)malloc(currentMax*2*sizeof(char));
            memcpy(newBuff, buffer, bufferSize);
            free(buffer);
            buffer = newBuff;

            currentMax = bufferSize *2;
        }
    }
    //free all memory alloced
    int (*cmp)(void *, void *) = comparator;

    if(argv[1][1] == 'q'){
        quickSort(head, cmp);
    }else{
        insertionSort(head, cmp);
    }



    free(buffer);
    printFinal(head, type);

    struct nodeData * list = head;

    if(type == 's'){
        if(list != NULL){
            struct nodeData * curr = list;
            while(curr != NULL){
                list = curr->next;
                free(curr->string);
                curr = list;
            }
        }
    }




    freeList(head);
    close(fd);
    //printf("%d", sizeOfLongestStr);
    return 0;
}



//preferably build these two sorts using linked lists of the data
//dont forget that "comparator" is just a name of the function ptr
int insertionSort( void* toSort, int (*comparator)(void*, void*)){
   struct nodeData * head = (struct nodeData *)(toSort);
    struct nodeData *ptr;
    ptr= head;

    if(type == 'i'){
    int * numarray=malloc((length+1)*sizeof(int));

    //copy integer data to array
    int i=0;

    while(i<=length+1&&ptr!=NULL){
        numarray[i] = ptr->integer;
        ptr=ptr->next;
        i++;
    }

    //sort
    int a;
    int key=0;
    int b=0;
    for(a=1;a<length;a++){
        key=numarray[a];
        b=a-1;
        //printf("%d%d\n", numarray[b-1], key);
        while(b>=0 &&comparator(&numarray[b],&key)>0){
           //printf("%d return val\n", comparator(&numarray[b-1],&key));
            numarray[b+1]=numarray[b];
            b=b-1;
        }
        numarray[b+1]=key;
    }

    //copy data back to list
    struct nodeData * newPtr;
    int count=0;
    newPtr= head;
        while(count<=length&&newPtr!=NULL){
            newPtr->integer=numarray[count];
            newPtr=newPtr->next;
            count++;
        }
        free(numarray);
    }else{
    //copy string data to array
        int i = 0;
        char ** stringArray = (char**)malloc((length+1)*sizeof(char*));
        for(i = 0; i < length; i++){
            stringArray[i] = (char *)malloc(sizeOfLongestStr * sizeof(char));
            strcpy(stringArray[i], ptr->string);
           // printf("%s\n", ptr->string);
            ptr = ptr->next;
        }
        int a;
        char * key;
        int b=0;
        for(a=1;a<length;a++){
            key=stringArray[a];
            b=a-1;
            while(b >= 0 && comparator(stringArray[b],key)>0){
                //positive means string a comes after string b
                //negative is a before b
                stringArray[b+1]=stringArray[b];
                b=b-1;
            }
            stringArray[b+1]=key;
        }
        //copy data back to nodes
        struct nodeData * newPtr;
        int count=0;
        newPtr= head;
        while(count<length&&newPtr!=NULL){
            strcpy(newPtr->string,stringArray[count]);
            newPtr=newPtr->next;
            count++;
        }
        int x = 0;
        for (x = 0; x < (length + 1); x++){
            free(stringArray[x]);
        }
        free(stringArray);
    }
    return 0;
}

void qsInts(int * number, int start, int end, int (*comparator)(void*, void*)){
    int i;
    int j;
    int pivot;
    int temp;

    if(start<end){
    pivot=start;
    i=start;
    j=end;

    while(i<j){
            //if return positive then a after b
         while(comparator(&number[i], &number[pivot]) <= 0 &&i<end)
            i++;
         while(comparator(&number[j], &number[pivot]) > 0)
            j--;
         if(i<j){
            temp=number[i];
            number[i]=number[j];
            number[j]=temp;
         }
      }

      temp=number[pivot];
      number[pivot]=number[j];
      number[j]=temp;
      qsInts(number,start,j-1, comparator);
      qsInts(number,j+1, end, comparator);

    }
}

void qsStrings(char **strings,int start,int end, int (*comparator)(void*, void*)){
    int i;
    int j;
    int pivot;
    char * temp = (char *)malloc(sizeOfLongestStr*sizeof(char));

    if(start<end){
      pivot=start;
      i=start;
      j=end;

      while(i<j){
            //if return positive then a after b
         while(comparator(strings[i], strings[pivot]) <= 0 &&i<end)
            i++;
         while(comparator(strings[j], strings[pivot]) > 0)
            j--;
         if(i<j){

            strcpy(temp,strings[i]);
            strcpy(strings[i],strings[j]);
            strcpy(strings[j],temp);
         }
      }


      strcpy(temp,strings[pivot]);
      strcpy(strings[pivot],strings[j]);
      strcpy(strings[j],temp);
      qsStrings(strings,start,j-1, comparator);
      qsStrings(strings,j+1,end, comparator);

    }

    free(temp);

}

int quickSort( void* toSort, int (*comparator)(void*, void*)){
    //cast void * toSort to our struct
    //hope this is how u do it!

    struct nodeData * head = (struct nodeData *)(toSort);
    struct nodeData *ptr;
    ptr= head;

    if(type == 'i'){
    //copy integer data to array
        int * numarray=malloc((length+1)*sizeof(int));
        int i=0;

        while(i<=length+1&&ptr!=NULL){
            numarray[i] = ptr->integer;
            ptr=ptr->next;
            i++;
        }
        qsInts(numarray, 0, length-1, comparator);
        struct nodeData * newPtr;
        int count=0;
        newPtr= head;
        while(count<=length&&newPtr!=NULL){
            newPtr->integer=numarray[count];
            newPtr=newPtr->next;
            count++;
        }
        free(numarray);
    }else{
        int i = 0;
        char ** stringArray = (char**)malloc((length + 1)*sizeof(char*));
        for(i = 0; i < length; i++){
            stringArray[i] = (char *)malloc(sizeOfLongestStr * sizeof(char));
            strcpy(stringArray[i], ptr->string);
           // printf("%s\n", ptr->string);
            ptr = ptr->next;
        }
        qsStrings(stringArray, 0, length-1, comparator);
        struct nodeData * newPtr;
        int count=0;
        newPtr= head;
        while(count<length&&newPtr!=NULL){
            strcpy(newPtr->string,stringArray[count]);
            newPtr=newPtr->next;
            count++;
        }
        int x = 0;
        for (x = 0; x < (length + 1); x++){
            free(stringArray[x]);
        }
        free(stringArray);
    }
    return 0;
}

//frannys official comparator function
//these void * will most likely be linked list heads
// if we can keep global variables then we can have a marker for if char type or int type
int comparator(void * a, void * b){



    if(type=='i'){
      int* c =a;
    int *d =b;
        if(*c>*d){
            return 1;
        }else if(*c <*d){
            return -1;
        }else{
            return 0;
        }
    }else{
       return simpleStrCmp((char*)a, (char*)b);

    }

}

//I dont know if we can use this but we can change/remove later
int simpleStrCmp(char * a, char * b){
    int i = 0;

    while (1){
        if(a[i] == '\0' && b[i] == '\0'){
            return 0;
        }else if (a[i] == b[i]){
            i++;
            continue;
        }else if(a[i] == '\0' && b[i] != '\0'){
            return -1;
        }else if(b[i] == '\0' && a[i] != '\0'){
              return 1;
        }else{
            //positive means string a comes after string b
            //negative is a before b
            return (int)(a[i] - b[i]);
        }
    }

    return 0;
}

//maybe dont make it void star once we know what it is?
void printFinal(struct nodeData * list, char t){
    //given the type printf %d or %s
    if(list == NULL){
        return;
    }

    struct nodeData * ptr = list;
    //int or char
    do{
        if(type == 'i'){
            printf("%d\n", ptr->integer);
        }else{
            printf("%s\n", ptr->string);
        }
        ptr = ptr->next;

    }while(ptr != NULL);


}

//also remove void * once we agree on datatype
void freeList(struct nodeData * list){
    if(list != NULL){
        struct nodeData * ptr = list;
        while(ptr != NULL){
            list = ptr->next;
            //free(ptr->string);
            free(ptr);
            ptr = list;
        }
    }
}
