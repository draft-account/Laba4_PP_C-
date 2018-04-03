// Laba4_PP_C++.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"

#include <mpi.h>
#include <iostream>
#include <math.h>
#include <ctime>
#include <stdlib.h>

//math library
#include <math.h>

//for really random
#include <ctime>

//function sleep
#include <Windows.h>

using namespace std;

int main(int argc, char **argv)
{
	int rank, size;
	MPI_Init(&argc, &argv);
	MPI_Comm comm = MPI_COMM_WORLD;
	MPI_Comm_size(comm, &size);
	MPI_Comm_rank(comm, &rank);
	MPI_Status status;
	MPI_Request* sendReq = new MPI_Request[size];
	MPI_Request recvReq;

	//flag
	int flag = 0;

	//tag
	int delayReqTag = 111;

	//the size of the array
	int N;

	//the range of numbers
	int R;

	//the magic number
	int M;

	//for 0-process a pointer to an array of random values
	long* A = nullptr;

	//equal pieces
	int piece;

	//for stop runnning all processes (1 - yes, 0 - no)
	int IsStop = 0;

	if (rank == 0)
	{
		N = 20;
		M = 5;
		R = 10;

		//allocate size of the array
		A = new long[N];

		srand(time(0));

		//generate random values in the range [1, R]
		for (size_t i = 0; i < N; i++)
		{
			A[i] = rand() % R + 1;
			cout << A[i] << " ";
		}
		cout << endl;
	}

	//0-process send all processes "N" and magic number
	MPI_Bcast(&N, 1, MPI_INT, 0, comm);
	MPI_Bcast(&M, 1, MPI_INT, 0, comm);

	//initialize size of pieces
	piece = N / size;

	//for recieve piece of array A
	long* B = new long[piece];

	//is shared between all nodes evenly
	MPI_Scatter(A, piece, MPI_INT, B, piece, MPI_INT, 0, comm);

	//synchronization all processes
	MPI_Barrier(comm);

	//initialize delayed request (send command all processes)
	for (size_t i = 0; i < size; i++)
		MPI_Send_init(&IsStop, 1, MPI_INT, i, delayReqTag, comm, &sendReq[i]);

	//all process not stop running
	MPI_Irecv(&IsStop, 1, MPI_INT, MPI_ANY_SOURCE, delayReqTag, comm, &recvReq);

	int counterIter = 0;
	for (size_t i = 0; i < piece; i++)
	{
		MPI_Test(&recvReq, &flag, &status);

		cout << "Flag " << flag << " pr" << rank << endl;

		//if not recieved stop command
		if (flag == 0)
		{
			if (B[i] != M)
			{
				B[i] = sin(B[i] - M) * exp(B[i]) - log10(cos(B[i]));

				//count of completed iterations
				counterIter++;
			}
			else
			{
				//stop programm (all processes needs to be stopped)
				IsStop = 1;
				cout << "\nFind pr" << rank << "\n" << endl;

				//start delayed request
				MPI_Startall(size, sendReq);
			}
		}

		//if someone send to rank-process command stop
		if (IsStop == 1)
			break;
	}

	//synchronization all processes
	MPI_Barrier(comm);

	if (IsStop == 1)
	{
		//for gather all "countIter" in 0-process
		int* AllCountIter = new int[size];

		//gather piece of completed "countIter" in one place "AllCounterIter" (in 0-process)
		MPI_Gather(&counterIter, 1, MPI_INT, AllCountIter, 1, MPI_INT, 0, comm);

		//output "counterIter" from all processes
		if (rank == 0)
		{
			cout << "Output 'counterIter' from all processes" << endl;
			for (size_t i = 0; i < size; i++)
				cout << AllCountIter[i] << " ";
			cout << endl;
		}
	}
	else
	{
		//gather piece of "B" in one place "A" (in 0-process)
		MPI_Gather(B, piece, MPI_INT, A, piece, MPI_INT, 0, comm);

		//output new A
		if (rank == 0)
		{
			cout << "Output new A" << endl;
			for (size_t i = 0; i < N; i++)
				cout << A[i] << " ";
			cout << endl;
		}
	}

	MPI_Finalize();
	return 0;
}