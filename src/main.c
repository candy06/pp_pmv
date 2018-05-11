/*
********************************************************************************
*                           Polytech'Nice Sophia
*
*
* Filename : main.c
* Description : implementation of a distributed matrix vector product
* Programmer : L.ROSE
********************************************************************************
*/

/*
********************************************************************************
*                             INCLUDES FILES
********************************************************************************
*/

# include <stdio.h>
# include <stdlib.h>
# include <mpi.h>


/*
********************************************************************************
*                             CONSTANTS & MACRO
********************************************************************************
*/

# define ROOT_PROCESS_ID 0

/*
********************************************************************************
*                             GLOBAL VARIABLES
********************************************************************************
*/

int number_of_processes, current_processID, nextNode, previousNode, lastNode;
int vector[2];


/*
********************************************************************************
*                                   displayVector()
*
*
* Description : display a vector
* Arguments : array of integers (elements of the vector) and integer (size)
* Return : void
********************************************************************************
*/

void displayVector(int * vector, int size)
{
  printf("Vecteur: (");
  for (int i = 0 ; i < size ; i++) {
    if (i != size - 1) {
      printf(" %d,", vector[i]);
    } else {
      printf(" %d )\n", vector[i]);
    }
  }
}


/*
********************************************************************************
*                                   init()
*
*
* Description : set the variables we'll need for the algorithm
* Arguments : none
* Return : void
********************************************************************************
*/

void init()
{
  nextNode = (current_processID + 1) % number_of_processes;
  previousNode = (current_processID - 1 + number_of_processes) % number_of_processes;
  lastNode = (ROOT_PROCESS_ID + number_of_processes - 1) % number_of_processes;
}


/*
********************************************************************************
*                                   broadcast()
*
*
* Description : broadcast the data to all the nodes
* Arguments : the data and its size
* Return : void
********************************************************************************
*/

void broadcast(int * data, int size_of_the_data)
{
  if (current_processID == ROOT_PROCESS_ID) {
    MPI_Send(data, size_of_the_data, MPI_INT, nextNode, 0, MPI_COMM_WORLD);
  } else if (current_processID == lastNode) {
    MPI_Recv(data, size_of_the_data, MPI_INT, previousNode, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
  } else {
    MPI_Recv(data, size_of_the_data, MPI_INT, previousNode, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    MPI_Send(data, size_of_the_data, MPI_INT, nextNode, 0, MPI_COMM_WORLD);
  }
}


/*
********************************************************************************
*                                   main()
*
*
* Description : the classic main method of a C program
* Arguments : argc (number of agruments) and argv (list of arguments)
* Return : a code to inform if everything went well or not
********************************************************************************
*/

int main(int argc, char const *argv[])
{

  MPI_Init(NULL, NULL);

  MPI_Comm_size(MPI_COMM_WORLD, &number_of_processes);
  MPI_Comm_rank(MPI_COMM_WORLD, &current_processID);

  init();

  if (current_processID == ROOT_PROCESS_ID) {
    vector[0] = 5;
    vector[1] = 12;
  }

  broadcast(vector, 2);

  displayVector(vector, 2);
  // printf("ProcessID: %d | nextNode = %d - previousNode = %d - lastNode = %d\n", current_processID, nextNode, previousNode, lastNode);

  MPI_Finalize();

  return 0;
}
