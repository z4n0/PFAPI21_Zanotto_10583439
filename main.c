#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#define MAX_FIRSTCOMMANDLENGHT 13

int numberOfVertices;
int lunghezzaClassifica;

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
//y diventa x chld solo se ?? lunico figlio di x se no si mette alla dx di x child
void fib_Heap_Link(FibNode* y, FibNode* x){
    //remove y from root list of H
    //collego i vicini di y nella rootList
    y->left->right = y->right;
    y->right->left = y->left;

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


//add the children into rootList
//children are passed already with father = NULL;
void insertChildrenIntoRootList(FibNode *min, FibNode *firstChild){
    FibNode *temp;

    temp=min->right;
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

        //caso un solo elemento era presente --> ora la rootList ?? vuota
        if (nodeToExtract == nodeToExtract->right) {
            H->min = NULL;
        } else {
            H->min = nodeToExtract->right; //sposto min a dx e chiamo consolidazione NB ora H->min non ?? effettivamente il minimo
            consolidate(H);
        }
        H->size--;
    }
    return nodeToExtract;
}


void Cut(FibHeap* H, FibNode *nodeToBeCut, FibNode *parentNode)
{
    if (parentNode->degree == 1) //se il nodo da tagliare ?? lunico figlio
        parentNode->child = NULL; //se ?? lunico figlio allora ora il padre non ha piu figli

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

    if(parentNode == NULL){ //im in the root list
        if(newDistance < H->min->distance){ //se il nuovo valore ?? minore del min faccio update del min e finisco
            H->min = nodeToDecrease;
            return;
        }
    }else if(nodeToDecrease->distance < parentNode->distance){ //caso in cui non sono nella root e la heap proprety non ?? violata
        Cut(H,nodeToDecrease,parentNode);
        cascading_Cut(H,parentNode);
    }
    //update the min if the new value is the smallest ergo ?? stato aggiunto in root??ost
    if(nodeToDecrease->distance < H->min->distance)//se il nuovo valore ?? minore del min faccio update del min e finisco
        H->min = nodeToDecrease;
}
//_---------------------------------------------------------------------------------------------
//la fibHeap contiene i nodi che posso visitare di frontiera
int fib_uniform_Cost_Search(int graph[numberOfVertices][numberOfVertices], FibHeap *fibHeapPtr,int* explored) {
    int sommaCamminiMinimi=0;
    //explored = contenitore per i nodi che abbbiamo gia visitato
    memset(explored,0,sizeof(int)*(numberOfVertices));

    FibNode* nodeZero = createNewFibNode(0, 0); //creo nuovo nodo con dist infinito
    fib_Heap_insert(fibHeapPtr,nodeZero);
    //FIbHEap Q contiene i  vicini da poter visitare
    //explored keep track of explored verteces, 0 if is unexplored 1 if explored temporary 2 if extracted-> made permanent

    while (fibHeapPtr->size != 0){ //fino a che la heap non ?? vuota
        int u;
        int v = 0;
        FibNode *minDistanceNode = fib_Extract_Min(fibHeapPtr);
        //aggiorna somma CamminiMinimi quando estrai un nodo perch?? ?? qua che diventa permanente*/
        sommaCamminiMinimi += minDistanceNode->distance; //verranno inseriti solo nodi raggiungibili quindi non ho bisogno di controlli

        u = minDistanceNode->index; //index del nodo estratto
        explored[u]=2; //setto il nodo estratto a explored

        int edgeWeight;

        while (v < numberOfVertices-1) {
            edgeWeight = graph[u][++v];
            //leggo la sua adj list e aggiungo alla Heap tutti i suoi vicini non gia visitati con la loro distanza giusta
            if (explored[v] != 2 && edgeWeight > 0 && u != v ){ //se il nodo che posso raggiungere non ?? ancora stato estratto,o auto anello o non esiste edge
                if(explored[v]==0){ //se il nodo non ?? presente nella heap->lo creo e lo inserisco gia con la distanza giusta
                    FibNode* newFrontierNode = createNewFibNode(v,minDistanceNode->distance + edgeWeight); //creo nodo del vicino
                    fibHeapPtr->staticPointers[v]=newFrontierNode;
                    fib_Heap_insert(fibHeapPtr,newFrontierNode); // lo aggiungo alla heap
                    explored[v]=1;  //lo segno come esplorato
                }else if(fibHeapPtr->staticPointers[v]->distance > minDistanceNode->distance+ edgeWeight){ //se il nodo ?? gia presente nella Heap explored[v])=1;--> e ha valore maggiore di quello esistente decreaseKey
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

int minHeapSize=0;

void swapMinHeapNode(MinHeapNode* a, MinHeapNode* b){
    MinHeapNode temp= *a;
    *a = *b;
    *b = temp;
}

void minHeapify(MinHeapNode minHeap[numberOfVertices+1], int index, int positionArray[numberOfVertices])
{
    int minPos = index; //contains the index of the minimum element in the array heap
    int left = (2*index);
    int right = (2*index)+1;
    //se lindice sx ?? fuori dalla heap maxHeapSize e la sua distanza ?? minore di quella del padre allora diventa essa smallest
    if (left <= minHeapSize && minHeap[left].distance < minHeap[minPos].distance)
        minPos = left;
    //identico ma a dx NB lordine di controllo prima sx e poi dx ?? importante
    if (right <= minHeapSize && minHeap[right].distance < minHeap[minPos].distance)
        minPos = right;

    //se smallest ?? cambiato vuol dire che bisogna swappare qualcosa--> esiste un figlio con valore di distanza minore del padre --> swap x avere minPos heap
    if (minPos != index)
    {
        // Swap positions--> the node with less distance is going up the heap --> array[minPos] take the position of array[index]
        //swapPositions(minHeap, minPos, index);
        positionArray[minHeap[minPos].vertexIndex] = index; // minHeap->positionArray[parentIndex]
        positionArray[minHeap[index].vertexIndex] = minPos;

        swapMinHeapNode(&minHeap[minPos], &minHeap[index]);
        minHeapify(minHeap, minPos, positionArray);
    }
}

MinHeapNode extractMin(MinHeapNode minHeap[numberOfVertices+1], int positionArray[numberOfVertices]) {
        positionArray[minHeap[1].vertexIndex] = minHeapSize; // minHeap->positionArray[parentIndex]
        positionArray[minHeap[minHeapSize].vertexIndex] = 1;
        MinHeapNode min = minHeap[1]; //estraggo la radice
        minHeap[1] = minHeap[minHeapSize]; //la radice diventa lultimo elemento

        minHeapSize--;
        minHeapify(minHeap, 1,positionArray);
    return min;
}

void decreaseDistance(MinHeapNode minHeap[], int vertexIndex, int dist, int positionArray[numberOfVertices]){

    int arrayHeapIndex = positionArray[vertexIndex];
    minHeap[arrayHeapIndex].distance = dist; //aggiorno la distanza nell array heaP

    while (arrayHeapIndex != 1 && minHeap[arrayHeapIndex].distance < minHeap[(arrayHeapIndex)/2].distance){
        //sposto in alto i nodi con distanza minore
        positionArray[minHeap[arrayHeapIndex].vertexIndex] = arrayHeapIndex/2; // minHeap->positionArray[parentIndex]
        positionArray[minHeap[(arrayHeapIndex)/2].vertexIndex] = arrayHeapIndex;
        MinHeapNode temp= minHeap[arrayHeapIndex];
        minHeap[arrayHeapIndex] = minHeap[(arrayHeapIndex)/2];
        minHeap[(arrayHeapIndex)/2] = temp;

        arrayHeapIndex = (arrayHeapIndex)/2; //mi muovo verso lalto
    }
}

void min_Bin_Heap_insert(MinHeapNode minHeap[numberOfVertices+1], int key, int gIndex,int positionArray[numberOfVertices]){
    minHeapSize++;
    minHeap[minHeapSize].distance=key; //lo inserisco alla fine (lo faccio puntare all ultimno posto
    minHeap[minHeapSize].vertexIndex=gIndex;
    positionArray[gIndex] = minHeapSize;
    decreaseDistance(minHeap, gIndex , key, positionArray); //setto il valore dei cammini minimi a key con increse key cosi chiama heapify
}

int bin_Uniform_Cost_Search(int graph[numberOfVertices][numberOfVertices], MinHeapNode minHeap[numberOfVertices+1], int* explored, int positionArray[numberOfVertices]) {
    int sommaCamminiMinimi=0;
    //explored = contenitore per i nodi che abbbiamo gia visitato
    memset(explored,0,sizeof(int)*(numberOfVertices));

    //MinHeapNode * nodeZero = newMinHeapNode(0, 0); //creo nuovo nodo con dist infinito
    min_Bin_Heap_insert(minHeap,0,0,positionArray); //NB si occupa gia lei di allocare il nuovo nodo
    //FIbHEap Q contiene i  vicini da poter visitare
    //explored keep track of explored verteces, 0 if is unexplored 1 if explored temporary 2 if extracted-> made permanent


    while (minHeapSize != 0){ //fino a che la heap non ?? vuota
        int u;
        int v=0;
        MinHeapNode minDistanceNode = extractMin(minHeap,positionArray);
        //aggiorna somma CamminiMinimi quando estrai un nodo perch?? ?? qua che diventa permanente*/
        sommaCamminiMinimi += minDistanceNode.distance; //verranno inseriti solo nodi raggiungibili quindi non ho bisogno di controlli

        u = minDistanceNode.vertexIndex; //index del nodo estratto
        explored[u]=2; //setto il nodo estratto a explored
        int edgeWeight;

        while (v < numberOfVertices-1){ //leggo tutta la riga di u ovvero tutti gli edge che escono da u
            edgeWeight = graph[u][++v]; //peso della freccia da u a v faccio subito ++ passo subito a 1 perch?? la prima colonna ?? inutile
            if (explored[v] != 2 && u != v && edgeWeight > 0){ //se il nodo che posso raggiungere non ?? ancora stato estratto, se non sono nel caso di autoAnello o edge inesistente
                if(explored[v]==0){ //se il nodo non ?? presente nella heap->lo creo e lo inserisco gia con la distanza giusta
                    min_Bin_Heap_insert(minHeap,minDistanceNode.distance + edgeWeight,v,positionArray);
                    explored[v]=1;  //lo segno come esplorato
                }else if(minHeap[positionArray[v]].distance > minDistanceNode.distance + edgeWeight){ //se il nodo ?? gia presente nella Heap explored[v])=1;--> e ha valore maggiore di quello esistente decreaseKey
                    decreaseDistance(minHeap,v, minDistanceNode.distance + edgeWeight,positionArray);
                }
            }
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

int maxHeapSize=0;

void increase_key(GraphHeapNode* maxHeap, int index, int key) {
    maxHeap[index].camminiMinimi = key;

    while((index>1) && (maxHeap[index/2].camminiMinimi <= maxHeap[index].camminiMinimi)) {
        GraphHeapNode t = maxHeap[index];
        maxHeap[index] = maxHeap[index/2];
        maxHeap[index/2] = t;
        index = index/2;  //move to parent
    }
}

//?? un down Heapify
void maxHeapify(GraphHeapNode* maxHeap, int index){
    int left_child = index*2;
    int right_child = index*2+1;

    int maxPos;
    if(left_child > lunghezzaClassifica) //caso in cui non abbia figli
        return;

    if(left_child <= maxHeapSize && maxHeap[left_child].camminiMinimi >= maxHeap[index].camminiMinimi){
        maxPos = left_child;
    }else{
        maxPos=index;
    }

    if(right_child <= maxHeapSize && maxHeap[right_child].camminiMinimi > maxHeap[maxPos].camminiMinimi){
        maxPos = right_child;
    }

    if(maxPos != index){
        GraphHeapNode t = maxHeap[index];
        maxHeap[index] = maxHeap[maxPos];
        maxHeap[maxPos] = t;
        maxHeapify(maxHeap, maxPos);
    }
}

void insert(GraphHeapNode maxHeap[lunghezzaClassifica+1], int key, int gIndex){
    if(maxHeapSize < lunghezzaClassifica){
        maxHeapSize++;
        maxHeap[maxHeapSize].gIndex = gIndex;
        increase_key(maxHeap, maxHeapSize , key); //setto il valore dei cammini minimi a key con increse key cosi chiama heapify
    }else{
        if(key < maxHeap[1].camminiMinimi){ //se il valore che voglio inserire ?? minore della root allora diventa root e la root vecchia viene eliminata
            //poi chiamo heapify cosi facendo in root avro sempre il massimo valore dei 5 che verra scambiato con quello entrante in caso sia minore inoltre la heap e riorganizzata
            //cosi facendo non devo salvare piu di lunghezzaclassifica elementi-->heapify ?? piu veloce e occupo meno mem
            maxHeap[1].gIndex = gIndex;
            maxHeap[1].camminiMinimi = key; //inserisco la distanza cosi perche con increase/decreaseKey chiamere heapifyUp che non funzia devo andare down ora
            maxHeapify(maxHeap, 1); //top_down
        }
    }
}

int main() {
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

    int maxCommandLenght = (numberOfVertices+1)*4+numberOfVertices; //Lunghezza del BUffer 5 ?? la mia stima ogni numero ha 399 numeri da leggere che sono numeri compresi tra le (0-6 cifre) ho stimato 5 perch?? so che ci saranno molti zeri in media quindi ho stimato che i numeri siano di 5 cifre (stima larga)+ nvertici virgole
    char inputContainer[maxCommandLenght]; //Container per linput

    //lettura comandi  2 casi possibili
    //1.AggiungiGrafo--> leggi Matrice --> riempi lista di adiacenza
    //2 Calcola cammini minimi e aggiungi risultato alla MaxHeap;
    //2.Topk--> stampa classifica
    int graphIndex=-1;
    int numeroCamminiMinimi;

    int graph[numberOfVertices][numberOfVertices];
    GraphHeapNode maxHeapArray[lunghezzaClassifica+1];
    int explored[numberOfVertices];

    if(numberOfVertices < 350){
        MinHeapNode minHeapArray[numberOfVertices+1];
        int positionArray[numberOfVertices]; //containes at index k the index of minHeapArray where you find node k;

        while (fgets(inputContainer, maxCommandLenght,stdin) != NULL) { //fino a che si puo leggere

            if (strcmp(inputContainer, "AggiungiGrafo\n") == 0) {
                graphIndex++;
                memset(inputContainer, 0,maxCommandLenght); //svuoto inputContainer

                //Se il comando ?? di aggiungi grafo -->leggi la matrice nxn
                for (i = 0; i < numberOfVertices; ++i) {
                    if(fgets(inputContainer, maxCommandLenght,stdin) == NULL){
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
                numeroCamminiMinimi= bin_Uniform_Cost_Search(graph,minHeapArray,explored,positionArray);
                insert(maxHeapArray, numeroCamminiMinimi, graphIndex); //inserisco il risultato nella maxHeap che contiene gli indici dei k grafi con camm minimi minori

            } else if (strcmp(inputContainer, "TopK\n") == 0) {
                for (i = 1; i < lunghezzaClassifica+1; ++i) { //NB parte da 1 perch?? il primo posto della maxHeap ?? vuoto
                    if(i <= maxHeapSize)
                        if(i==maxHeapSize){
                            printf("%d",maxHeapArray[i].gIndex);
                        }else{
                            printf("%d ",maxHeapArray[i].gIndex);
                        }
                    else{
                        printf("\n");
                        break;
                    }
                }
            }
        }
    }else{
        FibHeap* fibHeapPtr = create_Fib_Heap();
        while (fgets(inputContainer, maxCommandLenght,stdin) != NULL) { //fino a che si puo leggere

            if (strcmp(inputContainer, "AggiungiGrafo\n") == 0) {
                graphIndex++;
                memset(inputContainer, 0,maxCommandLenght); //svuoto inputContainer
                //Se il comando ?? di aggiungi grafo -->leggi la matrice nxn
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
                insert(maxHeapArray, numeroCamminiMinimi, graphIndex); //inserisco il risultato nella maxHeap che contiene gli indici dei k grafi con camm minimi minori

            } else if (strcmp(inputContainer, "TopK\n") == 0) {
                for (i = 1; i < lunghezzaClassifica+1; ++i) { //NB parte da 1 perch?? il primo posto della maxHeap ?? vuoto
                    if(i <= maxHeapSize)
                        if(i==maxHeapSize){
                            printf("%d",maxHeapArray[i].gIndex);
                        }else{
                            printf("%d ",maxHeapArray[i].gIndex);
                        }
                    else{
                        printf("\n");
                        break;
                    }
                }
            }
        }
    }
    return 0;
}

