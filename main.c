#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_COMMANDLENGHT 22
#define MAX_FIRSTCOMMANDLENGHT 6
#define INFINITY 999

int numberOfVertices;
int lunghezzaClassifica;

typedef struct edge_node{
    int edgeWeight;
    int destinationVertex;
    struct edge_node* next;
}EdgeNode;

typedef struct{
    EdgeNode** adjListArray;
}Graph;

EdgeNode* newEdgeNode(int dest, int weight){
    EdgeNode* newNode = malloc(sizeof (EdgeNode));
    newNode->destinationVertex = dest;
    newNode->edgeWeight = weight;
    newNode->next = NULL;
    return newNode;
};

Graph* createGraph(){
    Graph* graph = malloc(sizeof (Graph));
    graph->adjListArray = malloc(sizeof (EdgeNode*));
    for(int i = 0; i < numberOfVertices; i++){ //TODO useless??
        graph->adjListArray[i]=NULL;
    }

    return graph;
}

void addEdge(Graph* graph, int start, int dest, int weight){
    //inserimento in testa alla lista se il peso è maggiore di zero
    if(weight>0){
        EdgeNode* newNode = newEdgeNode(dest,weight);
        newNode->next = graph->adjListArray[start];
        graph->adjListArray[start] = newNode;
    }
}

// start of MinHeap Structure---------------------------------------
typedef struct {
    int vertexIndex;
    int distance;
}MinHeapNode;

typedef struct{
    int size;
    MinHeapNode **array;
    int* positionArray; //keep track of the Index of the vertices in the array min heap structure
    //ex at index 1 of position arrayContains the index of vertex 1 in array
}MinHeap;

MinHeapNode* newMinHeapNode(int vertexId, int dist){
    MinHeapNode* newMinHeapNode = malloc(sizeof (MinHeapNode));
    newMinHeapNode->distance = dist;
    newMinHeapNode->vertexIndex = vertexId;
    return newMinHeapNode;
}

MinHeap* createMinHeap(){
    MinHeap* minHeap = malloc(sizeof (MinHeap));
    minHeap->size=0;
    minHeap->positionArray= malloc(sizeof (numberOfVertices* sizeof (MinHeapNode*)));
    return minHeap;
}

void swapMinHeapNode(MinHeapNode** a, MinHeapNode** b){
    MinHeapNode * temp= *a;
    *a = *b;
    *b = temp;
}

void swapPositions(MinHeap* minHeap, int childIndex, int parentIndex){
    minHeap->positionArray[minHeap->array[childIndex]->vertexIndex] = parentIndex; // minHeap->positionArray[parentIndex]
    minHeap->positionArray[minHeap->array[parentIndex]->vertexIndex] = childIndex;

}

void minHeapify(MinHeap* minHeap, int index)
{
    int min = index;
    int left = (2*index)+1;
    int right = (2*index)+2;
    //se lindice sx è fuori dalla heap size e la sua distanza è minore di quella del padre allora diventa essa smallest
    if (left < minHeap->size && minHeap->array[left]->distance < minHeap->array[min]->distance )
        min = left;
    //identico ma a dx NB lordine di controllo prima sx e poi dx è importante
    if (right < minHeap->size && minHeap->array[right]->distance < minHeap->array[min]->distance )
        min = right;

    //se smallest è cambiato vuol dire che bisogna ruotaqre qualcosa--> esiste un figlio con valore di distanza minore del padre --> swap x avere min heap
    if (min != index)
    {
        // Swap positions--> the node with less distance is going up the heap --> array[min] take the position of array[index]
        swapPositions(minHeap,min,index);

        // Swap nodes in heap Array
        swapMinHeapNode(&minHeap->array[min], &minHeap->array[index]);

        minHeapify(minHeap, min);
    }
}

MinHeapNode* extractMin(MinHeap* minHeap){
    if(minHeap->size<1){ //se non esiste ritorno NUll
        return NULL;
    }

    MinHeapNode* min = minHeap->array[0]; //estraggo la radice
    minHeap->array[0] = minHeap->array[minHeap->size-1]; //la radice diventa lultimo elemento

    swapPositions(minHeap,0,minHeap->size-1);
    minHeap->size--;
    minHeapify(minHeap,0);

    return min;
}

void decreaseDistance(MinHeap* minHeap, int vertexIndex, int dist){
    int arrayHeapIndex = minHeap->positionArray[vertexIndex];
    minHeap->array[arrayHeapIndex]->distance = dist; //aggiorno la distanza nell array heaP

    int parentIndex = (arrayHeapIndex-1)/2;

    while (arrayHeapIndex!=0 && minHeap->array[arrayHeapIndex]->distance < minHeap->array[parentIndex]->distance){
        //sposto in alto i nodi con distanza minore

        swapPositions(minHeap,arrayHeapIndex,parentIndex); //aggiorno posizione nodi scambiati
        swapMinHeapNode(&minHeap->array[arrayHeapIndex],&minHeap->array[parentIndex]); //scambio nodi

        arrayHeapIndex = parentIndex; //mi muovo verso lalto
    }
}



int main() {
    //linux "/home/zano/Desktop/PROJECT21_API/inputfile.txt"
    //windows "C:\\Users\\Luca\\Desktop\\progettoApiNascosto\\inputfile.txt"
    FILE *fp = fopen("C:\\Users\\Luca\\Desktop\\progettoApiNascosto\\inputfile.txt", "r"); // read only

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
            numberOfVertices = (int) strtol(numberContainer, NULL, 10); //converte da array a int
            memset(numberContainer, 0, MAX_FIRSTCOMMANDLENGHT);
            j = 0;
            i++;
        }
    }
    lunghezzaClassifica = (int) strtol(numberContainer, NULL, 10);
    memset(firstcommand, 0, MAX_FIRSTCOMMANDLENGHT);        //TODO USLESS
    memset(numberContainer, 0, MAX_FIRSTCOMMANDLENGHT);

    //lettura comandi  2 casi possibili
    //1.AggiungiGrafo--> leggi Matrice --> riempi lista di adiacenza
    //2 Calcola cammini minimi e aggiungi risultato alla MaxHeap;
    //2.Topk--> stampa classifica
    int graphIndex=-1;
    int numeroCamminiMinimi; //variabile che mi conterra i cammini minimi

    while (fgets(inputContainer, MAX_COMMANDLENGHT, fp) != NULL) { //fino a che si puo leggere

        if (strcmp(inputContainer, "AggiungiGrafo\n") == 0) {
            graphIndex++;
            Graph* graph= createGraph(); //creo il grafo
            memset(inputContainer, 0, MAX_COMMANDLENGHT); //svuoto inputContainer

            //Se il comando è di aggiungi grafo -->leggi la matrice nxn
            for (i = 0; i < numberOfVertices; i++) {
                fgets(inputContainer, MAX_COMMANDLENGHT, fp);    //leggo riga matrice
                strtok(inputContainer, "\n");   //elimino il\n dalla riga letta

                //separo i numeri della riga
                char *edgeWeightToken = strtok(inputContainer, " ");
                j = 0; //indice delle colonne

//              walk through other number of the Line
                while (edgeWeightToken != NULL) {
                    //printf(" %s\n", edgeWeightToken );
                    int edgeWeight = (int) strtol(edgeWeightToken, NULL, 10);//converto il token a int
                    addEdge(graph,i, j++,edgeWeight); //aggiungo edge al grafo
                    edgeWeightToken = strtok(NULL, " "); //vado al next token
                }
            }



        } else if (strcmp(inputContainer, "TopK\n") == 0) {
            //stampa k grafi con cammini Minimi Minori
        }
    }
    return 0;
}