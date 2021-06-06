#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_COMMANDLENGHT 22
#define MAX_FIRSTCOMMANDLENGHT 6
#define INFINITY 999
#define TEMP 0
#define PERM 1
#define NIL 1

int numberOfVerteces;
int lunghezzaClassifica;
//struct Vertex;
//struct Edge;

typedef struct edge_node{
    int edgeWeight;
    struct edge_node* nextEdge;
    struct Vertex *destinationVertex;
}Edge;

typedef struct vertex_node{
    int vertexIndex;
    int status;
    //struct vertex* nextVertex; //prossimo vertice nell ARRAY dei vertici
    struct edge_node* firstEdge; //primo edge nella tab di adiacenza
}Vertex;



void InitializeVerteces(Vertex *vertices, int numberOfVerteces){
    for(int i=0; i<numberOfVerteces; i++){
        vertices[i].vertexIndex=i;
        vertices[i].firstEdge = NULL;
    }
}

//TODO aggiuntaVertici di adjaceza
//prende in ingresso lindirizzo del vertice di partenza e quello di arrivo
//inserisce l'elemento nella lista di adjacenza contenetne il peso per raggiungerlo
// è un inserimento in testa ad una lista.
void addEdge(Vertex *vertices, int edgeWeight, int startIndex, int destinationIndex){
    Edge *newEdge = malloc(sizeof (Edge)); //il nuovo edge da inserire

    newEdge->destinationVertex = &vertices[destinationIndex];
    newEdge->edgeWeight = edgeWeight;
    newEdge->nextEdge = vertices[startIndex].firstEdge;

    vertices[startIndex].firstEdge = newEdge;
}

void dijkstra(Vertex* vertices, int* predecessorIndex, int* distance){
    int current;

    for(int i=0; i<numberOfVerteces; i++){
        distance[i]=INFINITY;
        //status[i]=TEMP;
        predecessorIndex[i]= NIL;
    }

    distance[0]=0;

    while (1){ //continua fino a che min_temp non riesce a trovare altri nodi
        current = min_temp(vertices,distance);//cerca il nodo adj con minor distanza possibile in statto temporaneo restituisce il suo indice
        if(current==NIL){
            return;
        }
        //TODO remeber to make status[current]=PERM;

    }
}

//returns the temporary vertex with minim value of distance, returns NIL if no temporary vertex is left or
//all temporary vertices left have pathlenght infinity
int min_temp(Vertex* vertices,int* distance){
    int i;
    int min = INFINITY;
    int k = NIL;
    for(i=0; i<numberOfVerteces;i++){
        if(vertices[i].status==TEMP && distance[i]<min){
            min=distance[i];
            k=i;
        }
    }
    return k;
}


int main() {

    FILE *fp = fopen("/home/zano/Desktop/PFAPI21_Zanotto_10583439/inputfile.txt", "r"); // read only

    // test for files not existing.
    if (fp == NULL) {
        perror(fp);
        exit(-1);
    }


    //ARRAY di char che conterrano il primo comando
    char numberContainer[MAX_FIRSTCOMMANDLENGHT];
    char firstcommand[10];
    char inputContainer[22];

    memset(numberContainer, 0, MAX_FIRSTCOMMANDLENGHT);            //svuoto il numberContainer

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

    //inizializzazione ARRAY dei vertici
    Vertex vertices[numberOfVerteces];
    InitializeVerteces(vertices, numberOfVerteces);

    //lettura comandi  2 casi possibili
    //1.AggiungiGrafo--> leggi Matrice --> riempi lista di adiacenza
    //2.Topk--> stampa classifica

    while (fgets(inputContainer, MAX_COMMANDLENGHT, fp) != NULL) { //fino a che si puo leggere

        if (strcmp(inputContainer, "AggiungiGrafo\n") == 0) {
            memset(inputContainer, 0, MAX_COMMANDLENGHT); //svuoto inputContainer
//            j=0;
            //Se il comando è di aggiungi grafo -->leggi la matrice nxn
            for (i = 0; i < numberOfVerteces; i++) {
                fgets(inputContainer, MAX_COMMANDLENGHT, fp);    //leggo riga matrice
                strtok(inputContainer, "\n");   //elimino il\n
                //separo i numeri della riga
                char *edgeWeightToken = strtok(inputContainer, " ");
                j = 0; //indice delle colonne

//              walk through other number of the Line
                while (edgeWeightToken != NULL) {
                    //printf(" %s\n", edgeWeightToken );
                    int edgeWeight = (int) strtol(edgeWeightToken, NULL, 10);//converto il token a int
                    addEdge(vertices, edgeWeight, i, j++);
                    edgeWeightToken = strtok(NULL, " ");
                }
            }
            int predecessor[numberOfVerteces]; //array che contiene l'ID del hop precedente al nodo i
            int distance[numberOfVerteces]; //array che contiene la distanza del nodo i dal nodo zero
            dijkstra(vertices,predecessor,distance);
            //TODO comando che chiama djsktra e inserisce numero cammini minimi e id del grafo in una min heap
        } else if (strcmp(inputContainer, "TopK") == 0) {
            //TODO TOPK command
            //TODO fa semplicemente un estrazione dalla min heap tante volte qualta è la lunghezza della classifica
        }
    }
    return 0;
}