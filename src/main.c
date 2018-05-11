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

int number_of_processes,
    current_processID,
    nextNode,
    previousNode,
    lastNode,
    sizeOfSubmatrix;

int vector[2];
int matrix[16];
int * tmp = NULL;
int * submatrix = NULL;


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
*                                   displayMatrix()
*
*
* Description : display a matrix
* Arguments : array of integers (elements of the matrix) and integer (size)
* Return : void
********************************************************************************
*/

void displayMatrix(int * matrix, int size)
{
  printf("Matrix: |");
  for (int i = 0 ; i < size ; i++) {
    if (i != size - 1) {
      printf(" %d,", matrix[i]);
    } else {
      printf(" %d |\n", matrix[i]);
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
  sizeOfSubmatrix = 16 / number_of_processes;
  submatrix = malloc(sizeOfSubmatrix * sizeof(int));
  tmp = malloc(sizeOfSubmatrix * sizeof(int));
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
*                                   scatter()
*
*
* Description : scatter the data to all the nodes
* Arguments : the data and its size + the reception data and its size
* Return : void
********************************************************************************
*/

void scatter(int * dataSource, int size_of_the_dataSource, int * dataDestination, int size_of_the_dataDestination)
{

  if (current_processID == ROOT_PROCESS_ID) {
    for (int i = size_of_the_dataDestination ; i < size_of_the_dataSource ; i++) {
      MPI_Send(&dataSource[i], size_of_the_dataDestination, MPI_INT, nextNode, 0, MPI_COMM_WORLD);
    }
    submatrix = &dataSource[current_processID];
  } else {
    MPI_Recv(dataDestination, size_of_the_dataDestination, MPI_INT, previousNode, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    for (int i = 0 ; i < (ROOT_PROCESS_ID - 1 - current_processID + number_of_processes) % number_of_processes ; i++) {
      MPI_Send(dataDestination, size_of_the_dataDestination, MPI_INT, nextNode, 0, MPI_COMM_WORLD);
      MPI_Recv(dataDestination, size_of_the_dataDestination, MPI_INT, previousNode, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }
    //submatrix = tmp;
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
    // Initialize the vector
    vector[0] = 5;
    vector[1] = 12;
    // Initialize the matrix
    matrix[0] = 1;
    matrix[1] = 10;
    matrix[2] = 2;
    matrix[3] = 20;

    matrix[4] = 3;
    matrix[5] = 30;
    matrix[6] = 4;
    matrix[7] = 40;
    matrix[8] = 5;

    matrix[9] = 50;
    matrix[10] = 6;
    matrix[11] = 60;
    matrix[12] = 7;
    matrix[13] = 70;
    matrix[14] = 8;
    matrix[15] = 80;
    displayMatrix(matrix, 16);
  }

  //broadcast(vector, 2);
  scatter(matrix, 16, submatrix, sizeOfSubmatrix);
  // displayVector(vector, 2);

  displayMatrix(submatrix, sizeOfSubmatrix);
  // printf("Taille de chaque sous-matrices = %d\n", sizeOfSubmatrix);
  // printf("ProcessID: %d | nextNode = %d - previousNode = %d - lastNode = %d\n", current_processID, nextNode, previousNode, lastNode);

  MPI_Finalize();

  return 0;
}
