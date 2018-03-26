// Laba4_PP_C++.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"

#include <mpi.h>
#include <iostream>
#include <math.h>
#include <ctime>
#include <stdlib.h>

//function sleep
#include <Windows.h>

using namespace std;

int main(int argc, char **argv)
{
	//russian output
	setlocale(LC_ALL, "rus");

	//procID, count of all processes
	int rank, size;

	MPI_Init(&argc, &argv);
	MPI_Comm comm = MPI_COMM_WORLD;
	MPI_Status status;

	//return count of processors in group(comm) in variable "size"
	MPI_Comm_size(comm, &size);

	//return number of calling processor in group(comm) in variable "rank"
	MPI_Comm_rank(comm, &rank);

	//size of array
	int N = 0;

	//pointer on array
	int *A = nullptr;

	//pointer on array
	int *newA;

	//array of requests from processes
	MPI_Request *requests = nullptr;

	//range of rand number
	int R;

	//magic number
	int M = 0;

	//equal piece for every process
	int piece = 0;

	//storage for count of iterations
	int counterIter = 0;

	//message tag
	int beginTag = 111;

	//message tag
	int stopTag = 222;

	//message tag
	int returnATag = 333;
	
	//message tag
	int recieveATag = 444;

	//running only on 0 processor
	if (rank == 0)
	{
		//size
		N = 12;

		//range of rand number
		R = 10;

		//magic number
		M = 5;

		//allocate memory for array A and them array requests
		A = new int[N];
		requests = new MPI_Request[N];

		//for really random numbers
		srand(time(NULL));

		//fill array randomly by range [1, R]
		cout << "A = {";
		for (size_t i = 0; i < N - 1; i++)
		{
			A[i] = rand() % R + 1;
			cout << A[i] << ", ";
		}
		cout << (A[N-1] = rand() % R + 1) << "};" << endl;

		//equal piece for every process
		piece = trunc(N / size);

		//delay request send count of iterations to 0-process when find M any process
		MPI_Send_init(&counterIter, sizeof(counterIter), MPI_INT, 0, stopTag, comm, &requests[rank]);

		//0-process send value of "M", N and piece all processes
		MPI_Bcast(&M, sizeof(M), MPI_INT, 0, comm);
		MPI_Bcast(&N, sizeof(N), MPI_INT, 0, comm);
		MPI_Bcast(&piece, sizeof(piece), MPI_INT, 0, comm);

		for (size_t r = 1; r < size; r++)
			for (size_t i = 0; i < N; i++)
				MPI_Send(&A[i], 1, MPI_INT, r, recieveATag, comm);

		//output begin work of process
		cout << "Start process " << rank << " with indices of A [" << rank * piece << ", " << rank * piece + piece - 1 << "], M = " << M << ", N = " << N << ", piece = " << piece << endl;

		cout << "Process " << rank << " " << A[0] << endl;

		//process calculate your own piece of array
		for (size_t i = rank * piece; i < rank * piece + piece; i++)
		{
			//calculate
			if (A[i] != M)
			{
				A[i] = sin(A[i] - M) * exp(A[i]) - log10(cos(A[i]));

				//update count of iteration for every process
				counterIter++;

				//if already last element of array "A"
				if (i == rank * piece + piece - 1)
				{
					//if not found value "M" in array A, then send changing part of array					
				}
			}
			else
			{
				//all process send count of iterations
				//MPI_Startall(size, requests);

				cout << "Process " << rank << " find M" << endl;
				break;
			}
		}

		//wait
		//MPI_Recv(&counterIter, sizeof(counterIter), MPI_INT, , stopTag, comm, &status);

		cout << "End wait" << endl;

		/*while (MPI_Wait || )
		{
			
		}*/

		//if ()
		//{
		//	//wait stopTag
		//	for (size_t i = 1; i < size; i++)
		//	{
		//		MPI_Recv(&counterIter, sizeof(counterIter), MPI_INT, i, stopTag, comm, &status);

		//		//output all iterations of processes
		//		cout << "Process " << i << "" << " execute " << counterIter << " iterations" << endl;
		//	}
		//}
		//else
		//{
		//	//array for build in one place
		//	newA = A;

		//	//build array A
		//	for (size_t i = 1; i < size; i++)
		//	{
		//		MPI_Recv(&A, sizeof(A), MPI_INT, i, returnATag, comm, &status);

		//		//every process give her parts of array A
		//		for (size_t i = rank * piece; i < rank * piece + piece; i++)
		//			newA[i] = A[i];
		//	}

		//	//output newA
		//	for (size_t i = 0; i < N; i++)
		//		cout << newA[i] << " ";
		//}
	}
	else if (rank != size - 1)
	{
		cout << "PrePre-start process " << rank << endl;

		//delay request send count of iterations to 0-process when find M any process
		//MPI_Send_init(&counterIter, sizeof(counterIter), MPI_INT, 0, stopTag, comm, &requests[rank]);

		cout << "Pre-start process " << rank << endl;

		//0-process send value of "M", N and piece all processes
		MPI_Bcast(&M, sizeof(M), MPI_INT, 0, comm);
		MPI_Bcast(&N, sizeof(N), MPI_INT, 0, comm);
		MPI_Bcast(&piece, sizeof(piece), MPI_INT, 0, comm);

		//output begin work of process
		cout << "Start process " << rank << " with indices of A [" << rank * piece << ", " << rank * piece + piece - 1 << "], M = " << M << ", N = " << N << ", piece = " << piece << endl;

		//create local array A, because process not see dynamic array created in 0-process
		int* localA = new int[N];
		for (size_t i = 0; i < N; i++)
			MPI_Recv(&localA[i], 1, MPI_INT, 0, recieveATag, comm, &status);

		//every process calculate your own piece of array
		for (size_t i = rank * piece; i < rank * piece + piece; i++)
		{
			//calculate
			if (localA[i] != M)
			{
				localA[i] = sin(localA[i] - M) * exp(localA[i]) - log10(cos(localA[i]));

				//update count of iteration for every process
				counterIter++;

				//if already last element of array "A"
				if (i == rank * piece + piece - 1)
				{
					//if not found value "M" in array A, then send changing part of array
					MPI_Send(&localA, N, MPI_INT, 0, returnATag, comm);
				}
			}
			else
			{
				//all process send count of iterations
				//MPI_Startall(size, requests);

				cout << "Process " << rank << " find M" << endl;

				//because MPI_StartAll work w/o lock
				break;
			}
		}		
	}
	else
	{
		cout << "PrePre-start process " << rank << endl;

		//delay request send count of iterations to 0-process when find M any process
		//MPI_Send_init(&counterIter, sizeof(counterIter), MPI_INT, 0, stopTag, comm, &requests[rank]);

		cout << "Pre-start process " << rank << endl;

		//0-process send value of "M", N and piece all processes
		MPI_Bcast(&M, sizeof(M), MPI_INT, 0, comm);
		MPI_Bcast(&N, sizeof(N), MPI_INT, 0, comm);
		MPI_Bcast(&piece, sizeof(piece), MPI_INT, 0, comm);

		//output begin work of process
		cout << "Start process " << rank << " with indices of A [" << rank * piece << ", " << N - 1 << "], M = " << M << ", N = " << N << ", piece = " << piece << endl;

		//create local array A, because process not see dynamic array created in 0-process
		int* localA = new int[N];
		for (size_t i = 0; i < N; i++)
			MPI_Recv(&localA[i], 1, MPI_INT, 0, recieveATag, comm, &status);

		//last process calculate rest
		for (size_t i = rank * piece; i < N; i++)
		{
			//calculate
			if (localA[i] != M)
			{
				localA[i] = sin(localA[i] - M) * exp(localA[i]) - log10(cos(localA[i]));

				//update count of iteration for every process
				counterIter++;

				//if already last element of array "A"
				if (i == rank * piece + piece - 1)
				{
					//if not found value "M" in array A, then send changing part of array
					MPI_Send(&localA, N, MPI_INT, 0, returnATag, comm);
				}				
			}
			else
			{
				//all process send count of iterations
				//MPI_Startall(size, requests);

				cout << "Process " << rank << " find M" << endl;

				//because MPI_StartAll work w/o lock
				break;
			}
			
		}
	}
	
	MPI_Finalize();
	return 0;
}

//terminate all processes in communicator
//MPI_Abort(comm, 404);

//stop and send count of iterations with delay requests
//MPI_Send_init(&counterIter, sizeof(counterIter), MPI_INT, 0, stopTag, comm, &request);

//launch all requests
//MPI_Startall(1, requests);

//delete communicator
//MPI_Comm_free(comm);

//delete structures of data associated with this request (request = MPI_Request_null, all operations associated with this request will be stopped)
//MPI_Request_free(&request);

//int* localA = new int[N];
//MPI_Bcast(&A, 1, MPI_INT, 0, comm);
