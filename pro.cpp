#include <stdio.h> 
#include <fstream>
#include <string>
#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <map>
#include <algorithm>

#include <mpi.h>
using namespace std;

#define TAG 42
#define parent_node(edge) (edge / 4)
#define node_l_edge(node) (node * 4)
#define node_r_edge(node) (node * 4) + 2
#define reverse_edge(edge) (edge % 2 == 0 ? edge + 1 : edge -1)
#define going_left(edge) (edge % 4 < 2 ? 1 : 0)
#define child_node_l(node) (node * 2) + 1
#define child_node_r(node) (node * 2) + 2
#define going_down(edge) (edge % 2 == 0)

int main(int argc, char **argv) {
    MPI_Init(&argc, &argv);
    int edge;
    MPI_Comm_rank(MPI_COMM_WORLD, &edge);  //id procesu
    int input_len =  string(argv[1]).length();
    int edge_len = 2*(input_len - 1);

    if (edge == 0 && input_len < 4) 
    {
        cout << argv[1] << endl;
    }
    else 
    {
        int parent_node = parent_node(edge);
        int child_node_L = child_node_l(parent_node);
        int child_node_R = child_node_r(parent_node);
        int reverse_edge = reverse_edge(edge);
        int going_left = going_left(edge);

        int etours[edge_len];
        int weights[edge_len];
        int suffix_sums[edge_len];
        int order[edge_len];

        // ETOUR COUNTING
        if (going_down(edge)) // ide dole
        {
            if (going_left(edge)) // ide dolava
            {
                int next_left_node = child_node_l(child_node_L);
                if (next_left_node >= input_len) // uz nie je node -vratis sa reversom
                {
                    int etour = reverse_edge(edge);
                    MPI_Allgather(&etour, 1, MPI_INT, &(etours), 1, MPI_INT, MPI_COMM_WORLD);
                }
                else // este je node, zanor sa
                {
                    int etour = child_node_L * 4; 
                    MPI_Allgather(&etour, 1, MPI_INT, &(etours), 1, MPI_INT, MPI_COMM_WORLD);
                    
                }
            }
            else // ide doprava
            {
                int next_left_node = child_node_l(child_node_R);
                if (next_left_node > input_len) // uz nie je node -vratis sa reversom
                {
                    int etour = reverse_edge(edge);
                    MPI_Allgather(&etour, 1, MPI_INT, &(etours), 1, MPI_INT, MPI_COMM_WORLD);
                }
                else // este je node, zanor sa
                {
                    int etour = child_node_R * 4; 
                    MPI_Allgather(&etour, 1, MPI_INT, &(etours), 1, MPI_INT, MPI_COMM_WORLD);
                }
            }
        }
        else // ide hore
        {
            if (going_left(edge)) // idem zlava nahor
            {
                if (child_node_R < input_len) // este pozri praveho
                {
                    int etour = edge + 1;
                    MPI_Allgather(&etour, 1, MPI_INT, &(etours), 1, MPI_INT, MPI_COMM_WORLD);
                }
                else //  nie je pravy, vrat sa hore
                {
                    int etour = edge / 4 - 1 + parent_node;
                    MPI_Allgather(&etour, 1, MPI_INT, &(etours), 1,MPI_INT, MPI_COMM_WORLD);
                }
            }
            else //idem sprava nahor
            {
                if (edge == 3) // posledny
                {
                    int etour = edge;
                    MPI_Allgather(&etour, 1, MPI_INT, &(etours), 1, MPI_INT, MPI_COMM_WORLD);
                }
                else
                {
                    int etour = edge / 4 - 1 + parent_node;
                    MPI_Allgather(&etour, 1, MPI_INT, &(etours), 1, MPI_INT, MPI_COMM_WORLD);
                }
            }
        }

        // SUFFIX SUM
        int weight = edge % 2 == 0 ? 1 : 0;
        MPI_Allgather(&weight, 1, MPI_INT, &(weights), 1, MPI_INT, MPI_COMM_WORLD);
        
        for (int k = 0; k <= 100; k ++)
        {
            weights[edge] = weights[edge] + weights[etours[edge]];
            etours[edge] = etours[etours[edge]];
        }

        MPI_Allgather(&weights[edge], 1, MPI_INT, &suffix_sums, 1, MPI_INT, MPI_COMM_WORLD);
        
        // posli iba dopredne hrany
        if (edge % 2 == 0)
        {
            int order = input_len - weights[edge];
            MPI_Send(&order, 1, MPI_INT, 3, 0, MPI_COMM_WORLD);
        }

        if (edge == 3) // trojka prijme vsetky a syncne
        {
            int node_ids[input_len];
            node_ids[0] = 0;
            for (int i = 0; i < input_len-1; i ++)
            {
                int rank = 2*i;
                MPI_Recv(&node_ids[i+1], 1, MPI_INT, rank, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            }

            vector<pair<int, char>> pairs;
            for (int i = 0; i < input_len; i++)
            {
                pairs.push_back(make_pair(node_ids[i], argv[1][i]));

            }
            sort(pairs.begin(), pairs.end());
            for (const auto &item : pairs)
                cout << item.second;
            cout << endl;
        }
    }

    MPI_Finalize();
    return 0;
}
