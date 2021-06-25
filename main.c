#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#define MAX_FIRSTCOMMANDLENGHT 13
#define INF 9999999


int numberOfVertices;
int lunghezzaClassifica;

//-------------------------------------------------------------------

// start of BinaryMinHeap with fibonacci Structure---------------------------------------
typedef struct fib_Node {
    int index; //index of the Node
    int distance; //distance of node index from node zero
    int degree; //number of child of node
    int mark; //true if node has lost child
    struct fib_Node *parent;
    struct fib_Node *child;
    struct fib_Node *left;
    struct fib_Node *right;
} FibNode;

typedef struct{
    int size;
    FibNode** staticPointers;
    FibNode *min; //puntatore al minimo
}FibHeap;

FibHeap* create_Fib_Heap() {
    FibHeap* H = malloc(sizeof(FibHeap));
    H->staticPointers = malloc(numberOfVertices * sizeof (FibNode*)); //keep pointers to verteces needed to speed up decrease Key operation,No need to search for Index value
    H->size = 0;
    H->min = NULL;
    return H;
}

//create a Node and return a Pointer to it
FibNode* createNewFibNode(int index,int key){
    FibNode* newFibNode= malloc(sizeof(FibNode));
    newFibNode->index = index;
    newFibNode->degree = 0;
    newFibNode->parent = NULL;
    newFibNode->child = NULL;
    newFibNode->mark = 0;
    newFibNode->distance = key;
    newFibNode->left = newFibNode;
    newFibNode->right = newFibNode;
    return newFibNode;
}

//inserisce nodeToinsert nella root list a sx di H->min
//no precondition on node toInsert Pointers
void insertIntoRootList(FibHeap* H, FibNode* nodeToInsert){
    //before: |_|-->|hMin|-->| |-->| |
    //after:  | |-->|nodeToInsert| --> |Hmin|
    if(H->min == NULL){ //se non esiste rootList
        H->min = nodeToInsert;
        nodeToInsert->left = nodeToInsert;
        nodeToInsert->right = nodeToInsert;
    }else{ //inserisco a sx di H->min
        H->min->left->right= nodeToInsert; //lelem a sx di Hmin ora punta al nuovo nodo da inserire
        nodeToInsert->right= H->min; //collego i puntatori inserimento a sx di h-min
        nodeToInsert->left= H->min->left;
        H->min->left = nodeToInsert;
    }
    nodeToInsert->parent = NULL; //essendo in rootList non ha padre per forza
}

// Inserting nodes
//alloca e crea un nodo e lo inserisce nella Heap
void fib_Heap_insert(FibHeap *H, FibNode* newNode){
    if (H->min == NULL){ //caso heap vuota
        H->min = newNode;
    }else{
        insertIntoRootList(H, newNode);
        if (newNode->distance < H->min->distance){ //update min
            H->min = newNode;
        }
    }
    H->size++;
}


//viene rimosso dalla rootList, y diventa figlio di x, x aumenta di grado e y viene unmarked o lo rimane
//y diventa x chld solo se è lunico figlio di x se no si mette alla dx di x child
void fib_Heap_Link(FibNode* y, FibNode* x){
    //remove y from root list of H
    //collego i vicini di y nella rootList
    y->left->right = y->right;
    y->right->left = y->left;
    //byPassedBySiblings(y);

    y->parent = x; //make y child of x

    if(x->child == NULL){ //caso x non abbia figli
        x->child=y; //--> y diventa il suo unico figlio
        y->left = y;
        y->right = y;
    }else{ //caso in cui x abbia gia altri figli
        FibNode* temp = x->child->right;
        x->child->right = y;
        y->left = x->child;
        temp->left = y;
        y->right = temp;
    }
    x->degree++;
    y->mark = 0;
}

//NB at the biginning of consolidate min is not really the min it has been moved a dx
void consolidate(FibHeap* H){
    int degree = (int)(log(H->size)/log(2))+2;
    FibNode* ArrayOfPointer[degree];
    int i,d;
    FibNode *x,*y;

    for(i=0; i < degree; ++i){
        ArrayOfPointer[i]=NULL;
    }

    FibNode* currentRootNode = H->min;
    FibNode* lastNode = currentRootNode->left;

    while(1){
        if(currentRootNode==lastNode)
            break;
        x=currentRootNode;
        d=currentRootNode->degree;
        currentRootNode = currentRootNode->right;
        while (ArrayOfPointer[d] != NULL && d<=degree){

            y = ArrayOfPointer[d];
            if(x->distance > y->distance){
                FibNode *temp = x;
                x = y;
                y = temp;
            } //ora rootNodeCurrent punta al nodo con valore minore
            fib_Heap_Link(y,x);
            ArrayOfPointer[d] = NULL;
            d++;
        }
        ArrayOfPointer[d] = x;
    }

    d = currentRootNode->degree;
    x = currentRootNode;
    while (ArrayOfPointer[d] != NULL) {
        // another node with the same degree as x
        y = ArrayOfPointer[d];
        if(x->distance > y->distance) {
            FibNode *t = x;
            x = y;
            y = t;
        }
        fib_Heap_Link(y,x);
        ArrayOfPointer[d] = NULL;
        d++;
    }

    ArrayOfPointer[d]=x;

    //-----------------------ora ho larray che punta agli alberi rimasti
    H->min = NULL;
    //loop to search for the new min
    for (i = 0; i < degree; ++i) {
        if(H->min == NULL && ArrayOfPointer[i] != NULL){
            H->min = ArrayOfPointer[i];
        }else if(ArrayOfPointer[i] != NULL && H->min!=NULL) {
            if(ArrayOfPointer[i]->distance < H->min->distance)
                H->min = ArrayOfPointer[i];
        }
    }
}


//concatenate the children into rootList
//children are passed already with father = NULL;
void insertChildrenIntoRootList(FibNode *min, FibNode *firstChild){
    FibNode *temp;

    temp = min->right;
    firstChild->left->right = temp;
    temp->left=firstChild->left;
    firstChild->left=min;
    min->right=firstChild;
}

// Extract min checked many times seems legit
FibNode* fib_Extract_Min(FibHeap *H) {
    FibNode *nodeToExtract = H->min;

    if (nodeToExtract != NULL) {
        if(nodeToExtract->child != NULL) {
            for (int i = 0; i < nodeToExtract->degree; ++i) { //il node child rimane invarito a fine ciclo torna dove ha iniziato
                nodeToExtract->child->parent = NULL;
                nodeToExtract->child = nodeToExtract->child->right;
            }

            insertChildrenIntoRootList(H->min, nodeToExtract->child); //unisco i figli alla rootList

            nodeToExtract->child = NULL; //ora il nodo da estrarre non ha piu figli
            nodeToExtract->degree = 0;
        }
        //remove nodetoExtract from the rootList
        //byPassedBySiblings(nodeToExtract);
        nodeToExtract->left->right =nodeToExtract->right;
        nodeToExtract->right->left = nodeToExtract->left;

        //caso un solo elemento era presente --> ora la rootList è vuota
        if (nodeToExtract == nodeToExtract->right) {
            H->min = NULL;
        } else {
            H->min = nodeToExtract->right; //sposto min a dx e chiamo consolidazione NB ora H->min non è effettivamente il minimo
            consolidate(H);
        }
        H->size--;
    }
    return nodeToExtract;
}


void Cut(FibHeap* H, FibNode *nodeToBeCut, FibNode *parentNode)
{
    if (parentNode->degree == 1) //se il nodo da tagliare è lunico figlio
        parentNode->child = NULL; //se è lunico figlio allora ora il padre non ha piu figli

    if (nodeToBeCut==parentNode->child) //se il nodo da tagliare era puntato dal padre allora ora punta al figlio a dx
        parentNode->child = nodeToBeCut->right; //ora il padre punta a quello a dx

    parentNode->degree--;

    //preparo condizioni per eliminazione di nodeToBeCut
    //byPassedBySiblings(nodeToBeCut);
    nodeToBeCut->left->right = nodeToBeCut->right;
    nodeToBeCut->right->left = nodeToBeCut->left;

    //il nodo tagliato ora punta a se stesso
    nodeToBeCut->right = nodeToBeCut;
    nodeToBeCut->left = nodeToBeCut;

    insertIntoRootList(H, nodeToBeCut); //NB this take care of nodeToBeCut--> parent = NULL;

    nodeToBeCut->mark = 0;

}

void cascading_Cut(FibHeap* H, FibNode* node){
    FibNode* parentNode = node->parent;
    if(parentNode != NULL){//fino a che non sono arrivato in rootList
        if(node->mark == 0){
            node->mark = 1;
        }else{
            Cut(H,node,parentNode);
            cascading_Cut(H,parentNode);
        }
    }
}

void fib_Heap_DecreaseKey(FibHeap* H, FibNode* nodeToDecrease, int newDistance){
    nodeToDecrease->distance = newDistance;
    FibNode* parentNode = nodeToDecrease->parent;

    if(parentNode == NULL){ //im in the root list //TODO nb this if is usless but can speed up time if node to be removed its in rootList skippa il resto della funzione
        if(newDistance < H->min->distance){ //se il nuovo valore è minore del min faccio update del min e finisco
            H->min = nodeToDecrease;
            return;
        }
    }else if(nodeToDecrease->distance < parentNode->distance){ //caso in cui non sono nella root e la heap proprety non è violata
        Cut(H,nodeToDecrease,parentNode);
        cascading_Cut(H,parentNode);
    }
    //update the min if the new value is the smallest ergo è stato aggiunto in rootòost
    if(nodeToDecrease->distance < H->min->distance)//se il nuovo valore è minore del min faccio update del min e finisco
        H->min = nodeToDecrease;
}
//_---------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------
//la fibHeap contiene i nodi che posso visitare di frontiera
int fib_uniform_Cost_Search(int graph[numberOfVertices][numberOfVertices], FibHeap *fibHeapPtr,int* explored) {
    int sommaCamminiMinimi=0;
    //explored = contenitore per i nodi che abbbiamo gia visitato
    memset(explored,0,sizeof(int)*(numberOfVertices));

    FibNode* nodeZero = createNewFibNode(0, 0); //creo nuovo nodo con dist infinito
    fib_Heap_insert(fibHeapPtr,nodeZero);
    //FIbHEap Q contiene i  vicini da poter visitare
    //keep track of explored verteces, 0 if is unexplored 1 if explored temporary 2 if extracted-> made permanent
    //explored[0] = 2; //node zero is already extracted-> made permanent


    while (fibHeapPtr->size != 0){ //fino a che la heap non è vuota
        int u;
        int v = 0;
        FibNode *minDistanceNode = fib_Extract_Min(fibHeapPtr);
        //aggiorna somma CamminiMinimi quando estrai un nodo perchè è qua che diventa permanente*/
        sommaCamminiMinimi += minDistanceNode->distance; //verranno inseriti solo nodi raggiungibili quindi non ho bisogno di controlli

        u = minDistanceNode->index; //index del nodo estratto
        explored[u]=2; //setto il nodo estratto a explored

        int edgeWeight;

        while (v < numberOfVertices-1) {
            edgeWeight = graph[u][++v];
            //leggo la sua adj list e aggiungo alla Heap tutti i suoi vicini non gia visitati con la loro distanza giusta
            if (explored[v] != 2 && edgeWeight > 0 && u != v ){ //se il nodo che posso raggiungere non è ancora stato estratto,o auto anello o non esiste edge
                if(explored[v]==0){ //se il nodo non è presente nella heap->lo creo e lo inserisco gia con la distanza giusta
                    FibNode* newFrontierNode = createNewFibNode(v,minDistanceNode->distance + edgeWeight); //creo nodo del vicino
                    fibHeapPtr->staticPointers[v]=newFrontierNode;
                    fib_Heap_insert(fibHeapPtr,newFrontierNode); // lo aggiungo alla heap
                    explored[v]=1;  //lo segno come esplorato
                }else if(fibHeapPtr->staticPointers[v]->distance > minDistanceNode->distance+ edgeWeight){ //se il nodo è gia presente nella Heap explored[v])=1;--> e ha valore maggiore di quello esistente decreaseKey
                    fib_Heap_DecreaseKey(fibHeapPtr,fibHeapPtr->staticPointers[v],minDistanceNode->distance+ edgeWeight);
                }
            }
        }
        free(minDistanceNode); //delete the node extracted from minHeap
    }
    return sommaCamminiMinimi;
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
    minHeap->array = malloc((numberOfVertices+1) * sizeof (MinHeapNode*));
    return minHeap;
}

void swapMinHeapNode(MinHeapNode** a, MinHeapNode** b){
    MinHeapNode *temp= *a;
    *a = *b;
    *b = temp;
}


void minHeapify(MinHeap* minHeap, int index)
{
    int minPos = index; //contains the index of the minimum element in the array heap
    int left = (2*index);
    int right = (2*index)+1;
    //se lindice sx è fuori dalla heap size e la sua distanza è minore di quella del padre allora diventa essa smallest
    if (left <= minHeap->size && minHeap->array[left]->distance < minHeap->array[minPos]->distance)
        minPos = left;
    //identico ma a dx NB lordine di controllo prima sx e poi dx è importante
    if (right <= minHeap->size && minHeap->array[right]->distance < minHeap->array[minPos]->distance)
        minPos = right;

    //se smallest è cambiato vuol dire che bisogna swappare qualcosa--> esiste un figlio con valore di distanza minore del padre --> swap x avere minPos heap
    if (minPos != index)
    {
        // Swap positions--> the node with less distance is going up the heap --> array[minPos] take the position of array[index]
        //swapPositions(minHeap, minPos, index);
        minHeap->positionArray[minHeap->array[minPos]->vertexIndex] = index; // minHeap->positionArray[parentIndex]
        minHeap->positionArray[minHeap->array[index]->vertexIndex] = minPos;
        swapMinHeapNode(&minHeap->array[minPos], &minHeap->array[index]);
        minHeapify(minHeap, minPos);
    }
}

//extract the root swap it with last element and call heapify, return a pointer to it
//if heap is empty return NULL
MinHeapNode* extractMin(MinHeap* minHeap){
    if(minHeap->size==0){ //se non esiste ritorno NUll
        return NULL;
    }

    //swapPositions(minHeap,1,minHeap->size);
    minHeap->positionArray[minHeap->array[1]->vertexIndex] = minHeap->size; // minHeap->positionArray[parentIndex]
    minHeap->positionArray[minHeap->array[minHeap->size]->vertexIndex] = 1;
    MinHeapNode* min = minHeap->array[1]; //estraggo la radice
    minHeap->array[1] = minHeap->array[minHeap->size]; //la radice diventa lultimo elemento

    minHeap->size--;
    minHeapify(minHeap,1);

    return min;
}

void decreaseDistance(MinHeap* minHeap, int vertexIndex, int dist){

    int arrayHeapIndex = minHeap->positionArray[vertexIndex];
    minHeap->array[arrayHeapIndex]->distance = dist; //aggiorno la distanza nell array heaP

    while (arrayHeapIndex != 1 && minHeap->array[arrayHeapIndex]->distance < minHeap->array[(arrayHeapIndex)/2]->distance){
        //sposto in alto i nodi con distanza minore
        //swapPositions(minHeap,arrayHeapIndex,(arrayHeapIndex)/2); //aggiorno posizione nodi scambiati
        minHeap->positionArray[minHeap->array[arrayHeapIndex]->vertexIndex] = arrayHeapIndex/2; // minHeap->positionArray[parentIndex]
        minHeap->positionArray[minHeap->array[(arrayHeapIndex)/2]->vertexIndex] = arrayHeapIndex;
        MinHeapNode *temp= minHeap->array[arrayHeapIndex];
        minHeap->array[arrayHeapIndex] = minHeap->array[(arrayHeapIndex)/2];
        minHeap->array[(arrayHeapIndex)/2] = temp;
        //swapMinHeapNode(&minHeap->array[arrayHeapIndex],&minHeap->array[(arrayHeapIndex)/2]); //scambio nodi

        arrayHeapIndex = (arrayHeapIndex)/2; //mi muovo verso lalto
    }
}

void min_Bin_Heap_insert(MinHeap * minHeap, int key, int gIndex){
    MinHeapNode * newNode;
    minHeap->size++;
    newNode = newMinHeapNode(gIndex,key); //creo nuovo nodo (metto camminiMinimi a -INF durante la creazione e setto gIndex)
    minHeap->array[minHeap->size] = newNode; //lo inserisco alla fine (lo faccio puntare all ultimno posto
    minHeap->positionArray[gIndex] = minHeap->size;
    decreaseDistance(minHeap, gIndex , key); //setto il valore dei cammini minimi a key con increse key cosi chiama heapify
}

int bin_Uniform_Cost_Search(int graph[numberOfVertices][numberOfVertices], MinHeap *binHeapPtr, int* explored) {
    int sommaCamminiMinimi=0;
    //explored = contenitore per i nodi che abbbiamo gia visitato
    memset(explored,0,sizeof(int)*(numberOfVertices));

    //MinHeapNode * nodeZero = newMinHeapNode(0, 0); //creo nuovo nodo con dist infinito
    min_Bin_Heap_insert(binHeapPtr,0,0); //NB si occupa gia lei di allocare il nuovo nodo
    //FIbHEap Q contiene i  vicini da poter visitare
    //keep track of explored verteces, 0 if is unexplored 1 if explored temporary 2 if extracted-> made permanent


    while (binHeapPtr->size != 0){ //fino a che la heap non è vuota
        int u;
        int v=0;
        MinHeapNode *minDistanceNode = extractMin(binHeapPtr);
        //aggiorna somma CamminiMinimi quando estrai un nodo perchè è qua che diventa permanente*/
        sommaCamminiMinimi += minDistanceNode->distance; //verranno inseriti solo nodi raggiungibili quindi non ho bisogno di controlli

        u = minDistanceNode->vertexIndex; //index del nodo estratto
        explored[u]=2; //setto il nodo estratto a explored
        int edgeWeight;

        while (v < numberOfVertices-1){ //leggo tutta la riga di u ovvero tutti gli edge che escono da u
            edgeWeight = graph[u][++v]; //peso della freccia da u a v faccio subito ++ passo subito a 1 perchè la prima colonna è inutile
            if (explored[v] != 2 && u != v && edgeWeight > 0){ //se il nodo che posso raggiungere non è ancora stato estratto, se non sono nel caso di autoAnello o edge inesistente
                if(explored[v]==0){ //se il nodo non è presente nella heap->lo creo e lo inserisco gia con la distanza giusta
                    min_Bin_Heap_insert(binHeapPtr,minDistanceNode->distance + edgeWeight,v);
                    explored[v]=1;  //lo segno come esplorato
                }else if(binHeapPtr->array[binHeapPtr->positionArray[v]]->distance > minDistanceNode->distance + edgeWeight){ //se il nodo è gia presente nella Heap explored[v])=1;--> e ha valore maggiore di quello esistente decreaseKey
                    decreaseDistance(binHeapPtr,v, minDistanceNode->distance + edgeWeight);
                }
            }
        }
        free(minDistanceNode); //delete the node extracted from minHeap
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
    NewMaxHeapNode->camminiMinimi = INF*-1;
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


void swapMaxH(GraphHeapNode **a, GraphHeapNode **b) {
    GraphHeapNode* t;
    t = *a;
    *a = *b;
    *b = t;
}


void increase_key(MaxHeap* maxHeap, int index, int key) {
    maxHeap->array[index]->camminiMinimi = key;

    while((index>1) && (maxHeap->array[index/2]->camminiMinimi <= maxHeap->array[index]->camminiMinimi)) {
        GraphHeapNode* t;
        t = maxHeap->array[index];
        maxHeap->array[index] = maxHeap->array[index/2];
        maxHeap->array[index/2] = t;
        //swapMaxH(&maxHeap->array[index], &maxHeap->array[index/2]); //è un UP_heapify
        index = index/2;  //move to parent
    }
}

//é un down Heapify
void maxHeapify(MaxHeap* maxHeap, int index){
    int left_child = index*2;
    int right_child = index*2+1;

    int maxPos;
    if(left_child > lunghezzaClassifica) //caso in cui non abbia figli
        return;

    if(left_child <= maxHeap->size && maxHeap->array[left_child]->camminiMinimi >= maxHeap->array[index]->camminiMinimi){ //TODO controllo child <= size è ridondante? NO non lo è
        maxPos = left_child;
    } else{
        maxPos=index;
    }

    if(right_child <= maxHeap->size && maxHeap->array[right_child]->camminiMinimi > maxHeap->array[maxPos]->camminiMinimi){ //TODO occhio ho cambiato >= in >
        maxPos = right_child;
    }

    if(maxPos != index){
        swapMaxH(&maxHeap->array[index], &maxHeap->array[maxPos]);
        maxHeapify(maxHeap, maxPos);
    }
}

void insert(MaxHeap* maxHeap, int key, int gIndex){
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
            newNode = newMaxHeapNode(gIndex);
            free(maxHeap->array[1]); //elimino la root
            maxHeap->array[1]=newNode; //aggiorno la root
            maxHeap->array[1]->camminiMinimi = key; //inserisco la distanza cosi perche con increase/decreaseKey chiamere heapifyUp che non funzia devo andare down ora
            maxHeapify(maxHeap, 1); //top_down
        }
    }
}


int main() {
//    FILE *fp = fopen("/home/zano/Desktop/PFAPI21_Zanotto_10583439/open_tests/input_4", "r"); // read only
//
//    // test for files not existing.
//    if (fp == NULL) {
//        perror(fp);
//        exit(-1);
//    }

    //ARRAY di char che conterrano il primo comando
    char firstcommand[MAX_FIRSTCOMMANDLENGHT];

    int i,j;

    //lettura primo comando
    if (fgets(firstcommand, MAX_FIRSTCOMMANDLENGHT, stdin) == NULL) {
        return -1;
    }//es. "11,2"
    char* string,*end;

    string = firstcommand;

    numberOfVertices =(int) strtol(string,&end,10);
    lunghezzaClassifica =(int) strtol(end,NULL,10);

    //todo change value of k in maxCommL = numOfVErt*k , maybe MAXFIRSTCOMMANDLENGHT--
    int maxCommandLenght = (numberOfVertices+1)*4+numberOfVertices; //Lunghezza del BUffer 5 è la mia stima ogni numero ha 399 numeri da leggere che sono numeri compresi tra le (0-6 cifre) ho stimato 5 perchè so che ci saranno molti zeri in media quindi ho stimato che i numeri siano di 5 cifre (stima larga)+ nvertici virgole
    char inputContainer[maxCommandLenght]; //Container per linput

    //lettura comandi  2 casi possibili
    //1.AggiungiGrafo--> leggi Matrice --> riempi lista di adiacenza
    //2 Calcola cammini minimi e aggiungi risultato alla MaxHeap;
    //2.Topk--> stampa classifica
    int graphIndex=-1;
    int numeroCamminiMinimi; //TODO devo fare un assegnamwnto??

    int graph[numberOfVertices][numberOfVertices];

    MaxHeap* maxHeapPtr = createMaxHeap(); //creo la max Heap che conterra lindice del grafo e il proprio numero dei cammini minimi NB il primo elemento è ad index 1
    //int* explored = malloc((numberOfVertices)*sizeof(int)); //support for the Uniform cost search
    int explored[numberOfVertices];

    if(numberOfVertices < 250){
        MinHeap* minHeapPtr = createMinHeap();
        while (fgets(inputContainer, maxCommandLenght,stdin) != NULL) { //fino a che si puo leggere

            if (strcmp(inputContainer, "AggiungiGrafo\n") == 0) {
                graphIndex++;
                memset(inputContainer, 0,maxCommandLenght); //svuoto inputContainer

                //Se il comando è di aggiungi grafo -->leggi la matrice nxn
                for (i = 0; i < numberOfVertices; ++i) {
                    if(fgets(inputContainer, maxCommandLenght, stdin) == NULL){
                        return 1;
                    };    //leggo riga matrice

                    j = 0; //indice delle colonne
                    string = inputContainer;

//              walk through other number of the Line and fill the matrix
                    while (1) {
                        int edgeWeight =(int) strtol(string, &end, 10);
                        if (string == end)
                            break;
                        graph[i][j]=edgeWeight;
                        string = end+1;
                        ++j;
                    }
                }
                numeroCamminiMinimi= bin_Uniform_Cost_Search(graph,minHeapPtr,explored);
                insert(maxHeapPtr, numeroCamminiMinimi, graphIndex); //inserisco il risultato nella maxHeap che contiene gli indici dei k grafi con camm minimi minori

            } else if (strcmp(inputContainer, "TopK\n") == 0) {
                for (i = 1; i < lunghezzaClassifica+1; ++i) { //NB parte da 1 perchè il primo posto della maxHeap è vuoto
                    if(i <= maxHeapPtr->size)
                        if(i==maxHeapPtr->size){
                            printf("%d",maxHeapPtr->array[i]->gIndex);
                        }else{
                            printf("%d ",maxHeapPtr->array[i]->gIndex);
                        }
                    else{
                        printf("\n");
                        break;
                    }
                }
            }
        }
//            free(minHeapPtr->array);
//            free(minHeapPtr->positionArray);
//            free(minHeapPtr);
    }else{
        FibHeap* fibHeapPtr = create_Fib_Heap();
        while (fgets(inputContainer, maxCommandLenght,stdin) != NULL) { //fino a che si puo leggere

            if (strcmp(inputContainer, "AggiungiGrafo\n") == 0) {
                graphIndex++;
                memset(inputContainer, 0,maxCommandLenght); //svuoto inputContainer

                //Se il comando è di aggiungi grafo -->leggi la matrice nxn
                for (i = 0; i < numberOfVertices; ++i){
                    if(fgets(inputContainer, maxCommandLenght,stdin) == NULL){
                        return 1;
                    };    //leggo riga matrice

                    j = 0; //indice delle colonne
                    string = inputContainer;

//              walk through other number of the Line
                    while (1){
                        int edgeWeight =(int) strtol(string, &end, 10);
                        if (string == end)
                            break;
                        graph[i][j]=edgeWeight;
                        string = end+1;
                        j++;
                    }
                }

                numeroCamminiMinimi = fib_uniform_Cost_Search(graph,fibHeapPtr,explored);
                insert(maxHeapPtr, numeroCamminiMinimi, graphIndex); //inserisco il risultato nella maxHeap che contiene gli indici dei k grafi con camm minimi minori

            } else if (strcmp(inputContainer, "TopK\n") == 0) {
                for (i = 1; i < lunghezzaClassifica+1; ++i) { //NB parte da 1 perchè il primo posto della maxHeap è vuoto
                    if(i <= maxHeapPtr->size)
                        if(i==maxHeapPtr->size){
                            printf("%d",maxHeapPtr->array[i]->gIndex);
                        }else{
                            printf("%d ",maxHeapPtr->array[i]->gIndex);
                        }
                    else{
                        printf("\n");
                        break;
                    }
                }
            }
        }
//        free(fibHeapPtr->staticPointers);
//        free(fibHeapPtr);
    }

    //TODO eliminazione maxHeap e Graph useless togli quando metti su server------------------------------------------

    // free(explored);
//    for(i=1; i<lunghezzaClassifica+1; i++){
//        free(maxHeapPtr->array[i]);
//    }
//    free(graph->adjListArray);
//    free(graph);
//    free(maxHeapPtr->array);
//    free(maxHeapPtr);
    return 0;
}

