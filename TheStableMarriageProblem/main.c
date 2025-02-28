#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#define N 4 // Number of men and women

// Function prototypes
void man(int rank);
void woman(int rank);

// Global preference arrays
int menPreferences[N][N] = {
    {0, 1, 2, 3},
    {1, 0, 3, 2},
    {2, 3, 0, 1},
    {3, 2, 1, 0}};

int womenPreferences[N][N] = {
    {0, 1, 2, 3},
    {1, 0, 3, 2},
    {2, 3, 0, 1},
    {3, 2, 1, 0}};

int main(int argc, char *argv[])
{
    int rank, size;

    // Initialize the MPI environment
    MPI_Init(&argc, &argv);
    // Get the rank of the process
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    // Get the number of processes
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Check if the number of processes is correct
    if (size != 2 * N)
    {
        if (rank == 0)
        {
            printf("This program requires %d processes.\n", 2 * N);
        }
        MPI_Finalize();
        return -1;
    }

    // Assign roles to processes: first N processes are men, the rest are women
    if (rank < N)
    {
        man(rank);
    }
    else
    {
        woman(rank - N);
    }

    // Finalize the MPI environment
    MPI_Finalize();
    return 0;
}

void man(int rank)
{
    // Iterate through the man's preferences and propose to each woman in order
    for (int i = 0; i < N; i++)
    {
        int womanRank = menPreferences[rank][i];
        // Send proposal to the woman
        MPI_Send(&rank, 1, MPI_INT, womanRank + N, 0, MPI_COMM_WORLD);
        printf("Man %d is proposing to Woman %d.\n", rank, womanRank);
    }
    // TODO Keep track of which women have been proposed to using the proposals array

    
}

void woman(int rank)
{
    printf("Woman %d is listening.\n", rank);
    int currentPartner = -1; // No partner initially
    int proposal;
    MPI_Status status;

    // Loop to receive proposals
    for (int i = 0; i < N; i++)
    {
        // Receive proposal from a man
        MPI_Recv(&proposal, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
        int manRank = proposal;

        // Compare incoming proposal with current partner (if any)
        if (currentPartner == -1 || womenPreferences[rank][manRank] < womenPreferences[rank][currentPartner])
        {
            // Accept the proposal
            currentPartner = manRank;
            printf("Woman %d accepted proposal from Man %d.\n", rank, manRank);
        }
        else
        {
            // Reject the proposal
            printf("Woman %d rejected proposal from Man %d.\n", rank, manRank);
        }
    }
}