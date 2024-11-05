#include <iostream>
#include <ctime>
#include <memory>
#include "ThreadPool.h"

const int COUNT_OF_NUMBERS = 100;

void job(int* arr, int n, int start, int end);
int* initArray(int n);

int main()
{
	ThreadPool pool;
	int thread_count = pool.getTreadsCount();
	int* arr = initArray(COUNT_OF_NUMBERS);
	int start_b = 0, end_b = COUNT_OF_NUMBERS / thread_count, n = COUNT_OF_NUMBERS;
	int temp_b = end_b;
	while(end_b < n)
	{
		pool.fillQueueJobs(std::bind(job, arr, n, start_b, end_b));
		start_b = end_b + 1;
		end_b += temp_b;
	}
	end_b = n - 1;
	pool.fillQueueJobs(std::bind(job, arr, n, start_b, end_b));
	//start
	delete[]arr;
}

void job(int *arr, int n, int start, int end)
{
	for (int i = start; i <= end; i++)
	{
		arr[i] = sqrt(arr[i]);
		arr[i] *= arr[i];
	}
}

int* initArray(int n)
{
	int* arr = new int[n];
	srand(time(NULL));
	for (int i = 0; i < n; i++)
	{
		arr[i] = rand() % 100;
	}
	return arr;
}