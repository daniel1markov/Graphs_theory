
/*Made by : Daniel Markov & Adi Levy */

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define num_of_Vertices 1000
#define num_of_graphs 500


// Queue for the BFS algorithm
typedef struct queue {
    int items[num_of_Vertices];
    int front;
    int rear;
}queue;

// list of connected edges
typedef struct list {
    int vertex;
    struct list* next;
}list;

// Graph 

typedef struct Graph {
    int Vertices;
    struct list** Array_of_Lists;
    int* visited;
}Graph;

Graph* build_random_graph(int, float);
void add_edges(Graph*, int, int);
list* createlist(int);
struct queue* createQueue();
void enqueue(queue*, int);
int dequeue(queue*);
int isEmpty(queue*);
void bfs(Graph*, int, int*);
int Is_isolated(Graph*);
int connectivity(Graph*);
void free_graph(Graph*);
int diameter(Graph*);
void threshold_1_3();
void threshold_2();


void main()
{
    srand(time(0));
    threshold_1_3();
    threshold_2();
}


void threshold_2() // Checkes the probability to get diameter of 2 for 500 graphs for each probability given to build a graph
{
    FILE* fpt;
    int i, j;
    float diam[10] = { 0 }, diameter_1 = 0;
    float p[10] = { 0.1,0.107,0.11,0.113,0.116,0.12,0.125,0.128,0.13,0.135 }; // probabilities to build the graph, in this case our Threshold is 0.1175 so we chose 5 P under the Threshold and 5 P below
    Graph* GG;

    for (i = 0; i < 10 ;i++) // runs for each probability
    {
        if (i)
        {
            printf("diam = %lf\n\n", diameter_1 / num_of_graphs);
            diam[i - 1] = diameter_1 / num_of_graphs;
        }
            diameter_1 = 0;
        for (j = 0; j < num_of_graphs; j++) // for each probability we build 500 graphs
        {
            GG = build_random_graph(num_of_Vertices, p[i]);

            if (diameter(GG) == 2) // if the diameter equals 2 (thats what we are looking for) add 1 to the total sum and int the end we will normalize it.
            {
                diameter_1 += 1;
            }

            free_graph(GG);

        }
    }
    printf("diam = %lf\n\n", diameter_1 / num_of_graphs);
    diam[i - 1] = diameter_1 / num_of_graphs;

    fpt = fopen("Diameter.csv", "w+"); // writing into csv file the data we collected for each P.
    for (i = 0;i < 10;i++)
    {
        fprintf(fpt, "%lf,", p[i]);
    }
    fprintf(fpt, "\n");
    for (i = 0;i < 10;i++)
    {
        fprintf(fpt, "%lf,", diam[i]);
    }
   
}

void threshold_1_3() //Checkes the probability to get (Connected graph and isolated vertex) for 500 graphs for each probability given to build a graph
{
    int x, j, i;
    float  iso = 0, connected = 0;
    float isolated[10] = { 0 }, connect[10] = { 0 };
    float p[10] = { 0.004,0.0057,0.006,0.0063,0.0067,0.0073,0.0078,0.0083,0.0087,0.015 }; // probabilities to build the graph, in this case our Threshold is 0.0069 so we chose 5 P under the Threshold and 5 P below
    Graph* GG;
    FILE* fpt;

    for (i = 0; i < 10 ;i++) // runs for each probability
    {
        if (i)
        {
            printf("isolated = %lf\n", iso / num_of_graphs);
            printf("connected= %lf\n", connected / num_of_graphs);
            isolated[i - 1] = iso / num_of_graphs;
            connect[i - 1] = connected / num_of_graphs;
        }
        iso = 0;
        connected = 0;

        for (j = 0; j < num_of_graphs; j++) // for each probability we build 500 graphs
        {
            GG = build_random_graph(num_of_Vertices, p[i]);
            x = Is_isolated(GG); // if the vertex is isolated we will sum it and normalize it in the end.
            if (x)
            {
                iso += 1;
                connected += 0;
            }
            else
            {
                connected += connectivity(GG); // if the graph is connected we will sum it and normalize it in the end.
                iso += 0;
            }
            free_graph(GG);
        }
    }
 

    printf("isolated = %lf\n", iso / num_of_graphs);
    printf("connected= %lf\n", connected / num_of_graphs);
    isolated[i - 1] = iso / num_of_graphs;
    connect[i - 1] = connected / num_of_graphs;

    fpt = fopen("Isolated.csv", "w+"); // writing into csv file the data we collected for each P.
    for (i = 0;i < 10;i++)
    {
        fprintf(fpt, "%lf,", p[i]);
    }
    fprintf(fpt, "\n");
    for (i = 0;i < 10;i++)
    {
        fprintf(fpt, "%lf,", isolated[i]);
    }
    
    fpt = fopen("Connected.csv", "w+"); // writing into csv file the data we collected for each P.

    for (i = 0;i < 10;i++)
    {
        fprintf(fpt, "%lf,", p[i]);
    }
    fprintf(fpt, "\n");
    for (i = 0;i < 10;i++)
    {
        fprintf(fpt, "%lf,", connect[i]);
    }
    
}

int diameter(Graph* graph) // finds the diameter of the graph
{
    int diam = 0, i ,temps;

    if (!(connectivity(graph))) // if the graph is not connected tha diameter is infinity.
    {
        return INT_MAX;
    }
   
    free(graph->visited);
    graph->visited = (int*)calloc(graph->Vertices, sizeof(int)); // for each run in the bfs we need our visited arr to be empty.
    
    for (i = 0; i < graph->Vertices;i++)  // runs in each vertex in the graph to find the diameter
    {
       bfs(graph, i, &temps); // running bfs and sending "temps" by reference to store the biggest diam he find in the BFS
        if (diam < temps)
        {
            diam = temps;
        }
        free(graph->visited);
        graph->visited = (int*)calloc(graph->Vertices, sizeof(int)); // for each run in the bfs we need our visited arr to be empty.
        temps = 0;
    }

    return diam; // after running on all the vertices we return the biggest diameter we found.
}

// BFS algorithm

void bfs(Graph* graph, int startVertex, int* max) 
{
    queue* q = createQueue();
    int distance[1000] = { 0 };
    int local_max = 0;
    graph->visited[startVertex] = 1;
    enqueue(q, startVertex);

    while (!isEmpty(q)) // if its empty we done.
    {
        int V = dequeue(q);
        list* temp = graph->Array_of_Lists[V];

        while (temp) {
            int other_V = temp->vertex;

            if (graph->visited[other_V] == 0) {  // going to each vertex in the queue and looking to his path, first of all visited = 1, after that adding the distance.
                graph->visited[other_V] = 1;
                enqueue(q, other_V);
                distance[other_V] = distance[V] + 1;
                if (distance[other_V] > local_max)
                {

                    local_max = distance[other_V]; //finding the biggest distance (for the diameter)

                }
            }
            temp = temp->next; // moving to his neighbors
        }
    }
    *max = local_max;
    free(q); 
}


int connectivity(Graph* graph) // checks if the graph is connected
{
    int i, gg;
    bfs(graph, 1,&gg);
    for (i = 0; i < graph->Vertices; i++) // running on each vertex[visited] and if there's one vertex thats not visited, the graph is not connected.
    {
        if (!(graph->visited[i]))
        {
            return 0;
        }
    }
    return 1;
}

// Creating a list

list* createlist(int v) {
    list* List = malloc(sizeof(list));
    List->vertex = v;
    List->next = NULL;
    return List;
}

// Creating a graph
Graph* build_random_graph(int vertices, float p)
{
    int i, j;
    Graph* graph = malloc(sizeof(Graph));
    graph->Vertices = vertices; // setting the num_of_verices in the graph struct

    graph->Array_of_Lists = malloc(vertices * sizeof(list*)); // creating an array of lists.
    graph->visited = calloc(vertices ,sizeof(int));

    for (i = 0; i < vertices; i++)
    {
        graph->Array_of_Lists[i] = NULL; // defined by null and only when theres an edge between that vertex to other one it will change.
    }

    for (i = 0; i < num_of_Vertices;i++) //build the random graph.
    {
        for (j = i + 1;j < num_of_Vertices;j++)
        {
            if (((float)rand() / (RAND_MAX)) <= p) // j= i+1 beacuse we cant make an edge between the vertex with him self.
            {
                add_edges(graph, i, j);

            }
        }
    }
    return graph;
}

void add_edges(Graph* graph, int u, int v)
{
    // Add edge from src to dest
    list* List = createlist(v);  
    List->next = graph->Array_of_Lists[u]; // creating new list and "adding" an edge u ---> v
    graph->Array_of_Lists[u] = List;

    // Add edge from dest to src
    List = createlist(u);
    List->next = graph->Array_of_Lists[v]; // creating new list and "adding" the same edge in the opposite way v------> u, because in undirceted graph u---> v == v----> u.
    graph->Array_of_Lists[v] = List;
}

int Is_isolated(Graph* graph)
{
    int k;
    for (k = 0;k < graph->Vertices;k++)
    {
        if (graph->Array_of_Lists[k] == NULL) // beacuse we defined all the vertices as NULL we can find that by O(v)
        {
            return 1;
        }
    }
    return 0;
}


// Create a queue
queue* createQueue()
{
    queue* q = malloc(sizeof(queue));
    q->front = -1;
    q->rear = -1;
    return q;
}

// Check if the queue is empty
int isEmpty(queue* q)
{
    if (q->rear == -1)
        return 1;
    else
        return 0;
}


// Adding value into queue
void enqueue(queue* q, int value)
{
    if (q->rear == num_of_Vertices - 1)
        printf("\nQueue is Full!!");
    else {
        if (q->front == -1)
            q->front = 0;
        q->rear++;
        q->items[q->rear] = value;
    }
}

// Removing value from queue

int dequeue(queue* q)
{
    int item;
    if (isEmpty(q)) {
        // printf("Queue is empty");
        item = -1;
    }
    else {
        item = q->items[q->front];
        q->front++;
        if (q->front > q->rear) {
            //  printf("Resetting queue ");
            q->front = q->rear = -1;
        }
    }
    return item;
}

// free_graph

void free_graph(Graph* graph)
{
    int i;
    struct list* tmp;
    for (i = 0; i < num_of_Vertices; i++)
    {
        while (graph->Array_of_Lists[i] != NULL)
        {
            tmp = graph->Array_of_Lists[i];
            graph->Array_of_Lists[i] = graph->Array_of_Lists[i]->next;
            free(tmp);
        }
    }

    free(graph->visited);
    free(graph);
}