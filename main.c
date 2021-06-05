#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAXNUMBEROFVERTICES 30
#define MAX_FIRSTCOMMANDLENGHT 6

struct Vertex;
typedef struct edge_node{
    //int vertexIndex;
    struct edge_node* nextEdge;
    struct Vertex* VertexPointer;
}Edge;

typedef struct vertex_node{
    int vertexIndex;
    //struct vertex* nextVertex; //prossimo vertice nella lista dei vertici
    struct Edge* firstEdge; //primo edge nella tab di adiacenza
}Vertex;

int main() {

    FILE *fp = fopen("/home/zano/Desktop/PFAPI21_Zanotto_10583439/inputfile.txt", "r"); // read only

    // test for files not existing.
    if (fp == NULL) {
        perror(fp);
        exit(-1);
    }


    //ARRAY di char che conterrano il primo comando
    int numberOfVerteces;
    int lunghezzaClassifica;
    char numberContainer[MAX_FIRSTCOMMANDLENGHT];
    char firstcommand[10];

    memset(numberContainer,0,MAX_FIRSTCOMMANDLENGHT);            //svuoto il numberContainer

    //lettura primo comando
    if (fgets(firstcommand, MAX_FIRSTCOMMANDLENGHT, fp) == NULL) {
            return -1;
    }//es. "11,2"

        int i = 0; //indice firstCommand
        int j = 0; //indice numberContainer

        while (firstcommand[i] != '\n') {
            //inserisco i numeri in n
            numberContainer[j++] = firstcommand[i++];

            if (firstcommand[i] == ',') {
                numberOfVerteces = (int) strtol(numberContainer, NULL, 10); //converte da array a int
                memset(numberContainer, 0, MAX_FIRSTCOMMANDLENGHT);
                j = 0;
                i++;
            }
        }
        lunghezzaClassifica = (int) strtol(numberContainer, NULL, 10);
        memset(firstcommand, 0, MAX_FIRSTCOMMANDLENGHT);        //TODO USLESS
        memset(numberContainer, 0, MAX_FIRSTCOMMANDLENGHT);
}
