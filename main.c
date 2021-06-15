#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_COMMANDLENGHT 2400
#define MAX_FIRSTCOMMANDLENGHT 15
#define INFINITY 9999999

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
    EdgeNode* newNode = malloc(sizeof(EdgeNode));
    newNode->destinationVertex = dest;
    newNode->edgeWeight = weight;
    newNode->next = NULL;
    return newNode;
}

Graph* createGraph(){
    Graph* graph = malloc(sizeof(Graph));
    graph->adjListArray = malloc(numberOfVertices * sizeof (EdgeNode*));
    return graph;
}

void addEdge(Graph* graph, int start, int dest, int weight){
    //inserimento in testa alla lista se il peso è maggiore di zero e non è un autoanello o una freccia di ritorno a zero
    if(weight>0 && start!=dest && dest != 0){
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
    MinHeapNode **array; //dynamic array which create a heap of minHeapNodes
    int* positionArray; //keep track of the Index of the vertices in the array min heap structure
    //ex at index 1 of position arrayContains the index of vertex 1 in array
}MinHeap;

MinHeapNode* newMinHeapNode(int vertexId, int dist){
    MinHeapNode* newMinHeapNode = malloc(sizeof(MinHeapNode));
    newMinHeapNode->distance = dist;
    newMinHeapNode->vertexIndex = vertexId;
    return newMinHeapNode;
}

MinHeap* createMinHeap(){
    MinHeap* minHeap = malloc(sizeof (MinHeap));
    minHeap->size=0;
    minHeap->positionArray= malloc(numberOfVertices* sizeof(int));
    minHeap->array = malloc(numberOfVertices * sizeof (MinHeapNode*));
    return minHeap;
}

void swapMinHeapNode(MinHeapNode** a, MinHeapNode** b){
    MinHeapNode *temp= *a;
    *a = *b;
    *b = temp;
}

//swap value in the position array
void swapPositions(MinHeap* minHeap, int childIndex, int parentIndex){
    minHeap->positionArray[minHeap->array[childIndex]->vertexIndex] = parentIndex; // minHeap->positionArray[parentIndex]
    minHeap->positionArray[minHeap->array[parentIndex]->vertexIndex] = childIndex;
}

void minHeapify(MinHeap* minHeap, int index)
{
    int min = index; //containse the index of the minimum element in the array heap
    int left = (2*index)+1;
    int right = (2*index)+2;
    //se lindice sx è fuori dalla heap size e la sua distanza è minore di quella del padre allora diventa essa smallest
    if (left < minHeap->size && minHeap->array[left]->distance < minHeap->array[min]->distance)
        min = left;
    //identico ma a dx NB lordine di controllo prima sx e poi dx è importante
    if (right < minHeap->size && minHeap->array[right]->distance < minHeap->array[min]->distance)
        min = right;

    //se smallest è cambiato vuol dire che bisogna swappare qualcosa--> esiste un figlio con valore di distanza minore del padre --> swap x avere min heap
    if (min != index)
    {
        // Swap positions--> the node with less distance is going up the heap --> array[min] take the position of array[index]
        swapPositions(minHeap,min,index);
        swapMinHeapNode(&minHeap->array[min], &minHeap->array[index]);
        minHeapify(minHeap, min);
    }
}

//extract the root swap it with last element and call heapify, return a pointer to it
//if heap is empty return NULL
//a
MinHeapNode* extractMin(MinHeap* minHeap){
    if(minHeap->size==0){ //se non esiste ritorno NUll
        return NULL;
    }

    swapPositions(minHeap,0,minHeap->size-1);
    MinHeapNode* min = minHeap->array[0]; //estraggo la radice
    minHeap->array[0] = minHeap->array[minHeap->size-1]; //la radice diventa lultimo elemento

    minHeap->size--;
    minHeapify(minHeap,0);

    return min;
}

void decreaseDistance(MinHeap* minHeap, int vertexIndex, int dist){

    int arrayHeapIndex = minHeap->positionArray[vertexIndex];
    minHeap->array[arrayHeapIndex]->distance = dist; //aggiorno la distanza nell array heaP

    //int parentIndex = (arrayHeapIndex-1)/2;

    while (arrayHeapIndex!=0 && minHeap->array[arrayHeapIndex]->distance < minHeap->array[(arrayHeapIndex-1)/2]->distance){
        //sposto in alto i nodi con distanza minore
        swapPositions(minHeap,arrayHeapIndex,(arrayHeapIndex-1)/2); //aggiorno posizione nodi scambiati
        swapMinHeapNode(&minHeap->array[arrayHeapIndex],&minHeap->array[(arrayHeapIndex-1)/2]); //scambio nodi

        arrayHeapIndex = (arrayHeapIndex-1)/2; //mi muovo verso lalto
    }
}


int dijkstra(Graph* graph) {

    MinHeap *minHeap = createMinHeap();

    for (int i = 0; i < numberOfVertices; i++) {
        minHeap->array[i] = newMinHeapNode(i, INFINITY);
        minHeap->positionArray[i] = i;
    }

    minHeap->array[0]->distance = 0;
    int sommaCamminiMinimi = 0;
    minHeap->size = numberOfVertices;

    while (minHeap->size != 0) { //fino a che la heap non è vuota
        MinHeapNode *minDistanceNode = extractMin(minHeap);
        /*if(minDistanceNode==NULL)
            return 0;
        //aggiorna somma CamminiMinimi quando estrai un nodo perchè è qua che diventa permanente*/
        if (minDistanceNode->distance != INFINITY ) {
            sommaCamminiMinimi += minDistanceNode->distance;
        }

        int u = minDistanceNode->vertexIndex;

        EdgeNode *temp = graph->adjListArray[u];
        while (temp != NULL) {
            int v = temp->destinationVertex;
            if (minHeap->positionArray[v] < minHeap->size && minHeap->array[minHeap->positionArray[v]]->distance > temp->edgeWeight + minDistanceNode->distance) {
                decreaseDistance(minHeap, v,minDistanceNode->distance + temp->edgeWeight);
            }
            temp = temp->next;
        }
    }
    return sommaCamminiMinimi;
}

//max_heap contenente id grafi e n cammini Minimi---------------------------------------

typedef struct graph_heap_Node
{
    int gIndex;
    int camminiMinimi;

}GraphHeapNode;


typedef struct max_heap{
    GraphHeapNode **array;
    int size;
}MaxHeap;

GraphHeapNode* newMaxHeapNode(int graphIndex){
    GraphHeapNode* NewMaxHeapNode =(GraphHeapNode*) malloc(sizeof(GraphHeapNode));
    NewMaxHeapNode->gIndex = graphIndex;
    NewMaxHeapNode->camminiMinimi = -1*INFINITY;
    return NewMaxHeapNode;
}

//inizializza e restituisce un puntatore a MaxHeap
MaxHeap* createMaxHeap(){
    MaxHeap* maxHeap = malloc(sizeof (MaxHeap));
    maxHeap->size=0;
    //maxHeap->lunghezzaClassifica=lunghezzaClassifica;
    maxHeap->array = malloc((lunghezzaClassifica+1) * sizeof(GraphHeapNode*)); //NB +1 xk la heap parte da 1
    //maxHeap->array[0]=NULL; //TODO occhio a questo magari crea problemi in futuro o è useless??
    return maxHeap;
}


void swapMaxH(GraphHeapNode **a, GraphHeapNode **b ) {
    GraphHeapNode* t;
    t = *a;
    *a = *b;
    *b = t;
}


void increase_key(MaxHeap* maxHeap, int index, int key) {
    maxHeap->array[index]->camminiMinimi = key;
    //if(key == maxHeap->array[1]->camminiMinimi)

    while((index>1) && (maxHeap->array[index/2]->camminiMinimi <= maxHeap->array[index]->camminiMinimi)) {
        swapMaxH(&maxHeap->array[index], &maxHeap->array[index/2]); //è un UP_heapify
        index = index/2;  //move to parent
    }
}

//é un down Heapify
void maxHeapify(MaxHeap* maxHeap, int index){
    int left_child = index*2;
    int right_child = index*2+1;

    int largest;
    if(left_child > lunghezzaClassifica) //caso in cui non abbia figli
        return;

    if(left_child <= maxHeap->size && maxHeap->array[left_child]->camminiMinimi >= maxHeap->array[index]->camminiMinimi){ //TODO controllo child <= size è ridondante? NO non lo è
        largest = left_child;
    } else{
        largest=index;
    }

    if(right_child <= maxHeap->size && maxHeap->array[right_child]->camminiMinimi > maxHeap->array[largest]->camminiMinimi){ //TODO occhio ho cambiato >= in >
        largest = right_child;
    }

    if(largest!=index){
        swapMaxH(&maxHeap->array[index], &maxHeap->array[largest]);
        maxHeapify(maxHeap, largest);
    }
}

void insert (MaxHeap* maxHeap, int key, int gIndex){
    GraphHeapNode* newNode;
    if(maxHeap->size < lunghezzaClassifica){
        maxHeap->size++;
        newNode = newMaxHeapNode(gIndex); //creo nuovo nodo (metto camminiMinimi a -INF durante la creazione e setto gIndex)
        maxHeap->array[maxHeap->size] = newNode; //lo inserisco alla fine (lo faccio puntare all ultimno posto
        increase_key(maxHeap,  maxHeap->size , key); //setto il valore dei cammini minimi a key con increse key cosi chiama heapify
    }else{
        if(key < maxHeap->array[1]->camminiMinimi){ //se il valore che voglio inserire è minore della root allora diventa root e la root vecchia viene eliminata
            //poi chiamo heapify cosi facendo in root avro sempre il massimo valore dei 5 che verra scambiato con quello entrante in caso sia minore inoltre la heap e riorganizzata
            //cosi facendo non devo salvare piu di lunghezzaclassifica elementi-->heapify è piu veloce e occupo meno mem
            //NB LA STAMPA FINALE DI TOPK pero dovra avvenire dal basso verso lalto perchè i valori piu piccoli stanno alle foglie
            // è possibile?? alla peggio una bella Heap Sort o quicksort(BEST OPTION) e stampa reverse
            newNode = newMaxHeapNode(gIndex);
            free(maxHeap->array[1]); //elimino la root
            maxHeap->array[1]=newNode; //aggiorno la root
            maxHeap->array[1]->camminiMinimi = key; //inserisco la distanza cosi perche con increase/decreaseKey chiamere heapifyUp che non funzia devo andare down ora
            maxHeapify(maxHeap, 1); //top_down
        }
    }
}

int main() {
    FILE *fp = fopen("/home/zano/Desktop/PFAPI21_Zanotto_10583439/open_tests/input_5", "r"); // read only

    // test for files not existing.
    if (fp == NULL) {
        perror(fp);
        exit(-1);
    }

    //ARRAY di char che conterrano il primo comando
    char numberContainer[MAX_FIRSTCOMMANDLENGHT];
    char firstcommand[MAX_FIRSTCOMMANDLENGHT];
    char inputContainer[MAX_COMMANDLENGHT];

    memset(numberContainer, 0, MAX_FIRSTCOMMANDLENGHT);    //svuoto il numberContainer //TODO usless??

    //lettura primo comando
    if (fgets(firstcommand, MAX_FIRSTCOMMANDLENGHT, fp) == NULL) {
        return -1;
    }//es. "11,2"

    int i = 0; //indice firstCommand
    int j = 0; //indice numberContainer

    while (firstcommand[i] != '\n') {
        //inserisco i numeri in n
        numberContainer[j++] = firstcommand[i++];

        if (firstcommand[i] == ' ') {
            numberOfVertices = (int) strtol(numberContainer, NULL, 10); //converte da array a int
            memset(numberContainer, 0, MAX_FIRSTCOMMANDLENGHT);
            j = 0;
            i++;
        }
    }
    lunghezzaClassifica = (int) strtol(numberContainer, NULL, 10);
    //memset(firstcommand, 0, MAX_FIRSTCOMMANDLENGHT);        //TODO USLESS
    //memset(numberContainer, 0, MAX_FIRSTCOMMANDLENGHT);  //TODO useless??

    //lettura comandi  2 casi possibili
    //1.AggiungiGrafo--> leggi Matrice --> riempi lista di adiacenza
    //2 Calcola cammini minimi e aggiungi risultato alla MaxHeap;
    //2.Topk--> stampa classifica
    int graphIndex=-1;
    int numeroCamminiMinimi; //TODO devo fare un assegnamwnto??

    MaxHeap* maxHeapPtr = createMaxHeap(); //creo la max Heap che conterra lindice del grafo e il proprio numero dei cammini minimi NB il primo elemento è ad index 1

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
                char *edgeWeightToken = strtok(inputContainer, ",");
                j = 0; //indice delle colonne

//              walk through other number of the Line
                while (edgeWeightToken != NULL) {
                    //printf(" %s\n", edgeWeightToken );
                    int edgeWeight = (int) strtol(edgeWeightToken, NULL, 10);
                    //(int) strtol(edgeWeightToken, NULL, 10);//converto il token a int
                    addEdge(graph, i, j++, edgeWeight); //aggiungo edge al grafo
                    edgeWeightToken = strtok(NULL, ","); //vado al next token
                }
            }

            numeroCamminiMinimi = dijkstra(graph); //una volta letta la matrice e riempito il grafo calcolo i cammini Minimi
            insert(maxHeapPtr, numeroCamminiMinimi, graphIndex); //inserisco il risultato nella maxHeap che contiene gli indici dei k grafi con camm minimi minori

        } else if (strcmp(inputContainer, "TopK\n") == 0) {
            //--------------------- riordinamento useless //TODO togli prima di mettere sul server
/*            for (i = 1; i < lunghezzaClassifica+1; i++){
                for(j=1;j<lunghezzaClassifica+1;j++){
                    if(maxHeapPtr->array[i]->camminiMinimi > maxHeapPtr->array[j]->camminiMinimi){
                        swapMaxH(&maxHeapPtr->array[i],&maxHeapPtr->array[j]);
                    }
                }
            }*/
            //-------------------------------
            for (i = 1; i < lunghezzaClassifica+1; i++) { //NB parte da 1 perchè il primo posto della maxHeap è vuoto
                if(i <= maxHeapPtr->size) //TODO < o <=
                    printf("%d ",maxHeapPtr->array[i]->gIndex);
                else{
                    printf("\n");
                    break;
                }
            }
        }
    }
    return 0;
}