/* tietze
 * doodad-1 rotating
 * doodad-3
 * heawood low-resolution
 * petersen
 * tesseract
 * */

#include <iostream>
#include <cmath>
#include <ctime>
#include <vector>
#include <string>
#include <cstdlib>
#include "SimpleGraph.h"

using std::cin;
using std::cos;
using std::cout;
using std::endl;
using std::make_pair;
using std::pair;
using std::sin;
using std::string;
using std::vector;

void Welcome();
void Init();
bool IfContinue();
void Edge_repel(int a);
void Calculate_repel(int a, int b);
void Calculate_attract(int a, int b);
void Modify();
void Work();
void Clear();

const double Pi = 3.14159265358979323;
const double k_repel = 3e-3;
const double k_attract = 3e-3;
int n, m;
double OperationTime;
string Filename;
time_t StartTime;
SimpleGraph graph;
vector<pair<double, double>> velocity;

// Main method
int main()
{
    Welcome();
    Work();
    return 0;
}

/* Prints a message to the console welcoming the user and
 * describing the program. */
void Welcome()
{
    cout << "Welcome to CS106L GraphViz!" << endl;
    cout << "This program uses a force-directed graph layout algorithm" << endl;
    cout << "to render sleek, snazzy pictures of various graphs." << endl;
    cout << endl;
}

// Initilaize the graph by reading the nodes and the edges
void Init()
{
    int a, b;
    Edge tmp_edge;
    Node tmp_node;
    srand(time(0));

    cout << "please input a filename" << endl;
    cin >> Filename;
    cout << "how long do you wang to run this program" << endl;
    cout << "please input a integrated number represents the seconds" << endl;
    OperationTime = 10;
    cin >> OperationTime;
    freopen(Filename.c_str(), "r", stdin);

    cin >> n;
    while (cin >> a >> b)
    {
        m++;
        tmp_edge.start = a;
        tmp_edge.end = b;
        graph.edges.push_back(tmp_edge);
    }
    for (int i = 0; i < n; i++)
    {
        tmp_node.x = cos(2.0 * Pi * (double)i / (double)n);
        tmp_node.y = sin(2.0 * Pi * (double)i / (double)n);
        graph.nodes.push_back(tmp_node);
        velocity.push_back(make_pair(0, 0));
    }
}

/* add random pertubations to the node
 * the pertubations also decrese with operating
 * time going by */
double random_pert(double Force)
{
    double ElapsedTime = difftime(time(NULL), StartTime);
    int tmp = rand() % 10; // to control the maximum of the pertubations
    return 2 * Force * (double)tmp / 100.0 * (1.0 - ElapsedTime / OperationTime) * (tmp & 1 ? 1 : -1);
}

/* add penalities for crossing edge
 * by assuming there is a node at the center
 * of each edge which produce force to other nodes */
void Edge_repel(int a)
{
    int n1, n2; // n1 n2 are the two ends of an edge
    double x0, x1, y0, y1, dis, tmp_force, theta;
    n1 = graph.edges[a].start;
    n2 = graph.edges[a].end;
    x0 = 0.5 * (graph.nodes[n1].x + graph.nodes[n2].x); // x0 represents the pos_x of the middle of the edge
    y0 = 0.5 * (graph.nodes[n1].y + graph.nodes[n2].y);
    for (int i = 0; i < n; i++)
    {
        // add this conditon to better performe on clique and grid
        if ((3 * m < n * n) && (i == n1 || i == n2))
            continue;
        x1 = graph.nodes[i].x;
        y1 = graph.nodes[i].y;
        dis = (sqrt((x0 - x1) * (x0 - x1) + (y0 - y1) * (y0 - y1)));
        dis = std::max(dis, 0.005);
        tmp_force = 3 * k_repel / dis;
        theta = atan2(y1 - y0, x1 - x0);
        velocity[i].first += tmp_force * cos(theta);
        velocity[i].second += tmp_force * sin(theta);
    }
}

// judge whether to stop
bool IfContinue()
{
    double ElapsedTime = difftime(time(NULL), StartTime);
    return ElapsedTime < OperationTime;
}

// calculate repel force between two nodes
void Calculate_repel(int a, int b)
{
    double x0, y0, x1, y1, dis, tmp_force, theta;
    x0 = graph.nodes[a].x;
    y0 = graph.nodes[a].y;
    x1 = graph.nodes[b].x;
    y1 = graph.nodes[b].y;
    dis = (sqrt((x0 - x1) * (x0 - x1) + (y0 - y1) * (y0 - y1)));
    tmp_force = k_repel / dis;
    theta = atan2(y1 - y0, x1 - x0);
    velocity[a].first -= tmp_force * cos(theta) + random_pert(tmp_force);
    velocity[a].second -= tmp_force * sin(theta) + random_pert(tmp_force);
    velocity[b].first += tmp_force * cos(theta) + random_pert(tmp_force);
    velocity[b].second += tmp_force * sin(theta) + random_pert(tmp_force);
}

// calculate attract force between two nodes
void Calculate_attract(int a, int b)
{
    double x0, y0, x1, y1, dis, tmp_force, theta;
    x0 = graph.nodes[a].x;
    y0 = graph.nodes[a].y;
    x1 = graph.nodes[b].x;
    y1 = graph.nodes[b].y;
    dis = ((x0 - x1) * (x0 - x1) + (y0 - y1) * (y0 - y1));
    tmp_force = k_attract * dis;
    theta = atan2(y1 - y0, x1 - x0);
    velocity[a].first += tmp_force * cos(theta) + random_pert(tmp_force);
    velocity[a].second += tmp_force * sin(theta) + random_pert(tmp_force);
    velocity[b].first -= tmp_force * cos(theta) + random_pert(tmp_force);
    velocity[b].second -= tmp_force * sin(theta) + random_pert(tmp_force);
}

/* calculate the repel forces between nodes
 * and the attract forcces from edges */
void Modify()
{
    // calculate the repel forces between nodes
    for (int i = 0; i < n; i++)
        for (int j = i + 1; j < n; j++)
            Calculate_repel(i, j);

    // calculate the attract forces between edges
    for (int i = 0; i < m; i++)
        Calculate_attract(graph.edges[i].start, graph.edges[i].end);

    /*calculate the force produced by edges
     * to avoid crssing edges */
    for (int i = 0; i < m; i++)
        Edge_repel(i);

    for (int i = 0; i < n; i++)
    {
        graph.nodes[i].x += velocity[i].first;
        graph.nodes[i].y += velocity[i].second;
        velocity[i].first = velocity[i].first * 0.001;
        velocity[i].second = velocity[i].second * 0.001;
    }
}

void Clear()
{
    velocity.clear();
    graph.edges.clear();
    graph.nodes.clear();
    n = 0;
    m = 0;
}

/* loop:
 * decide wheteher to end
 * calculate the forces and the position of nodes
 * draw the graph */
void Work()
{
    Init();
    InitGraphVisualizer(graph);
    StartTime = time(NULL);
    while (IfContinue())
    {
        Modify();
        DrawGraph(graph);
    }
    Clear();
}
