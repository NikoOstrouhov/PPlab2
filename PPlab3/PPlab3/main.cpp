#include <iostream>
#include <fstream>
#include <ctime>
#include <cmath>
#include <memory>
#include <vector>
#include <tuple>
#include <string>
#include "ThreadPool.h"

const int COUNT_OF_NUMBERS = 76442;
struct args
{
    std::vector<std::tuple<int, int, int>> completed_jobs;
    std::vector<float> angles;
    std::vector<float> cosines;
    std::mutex jobs_mutex;
    int start;
    int end;
};

void job(std::vector<float>& angles, std::vector<float>& cosines, int start, int end, std::vector<std::tuple<int, int, int>>& completed_jobs, std::mutex& jobs_mutex);
void initArray(std::vector<float>& arr, int n);
void waitForCompletion(ThreadPool& pool, std::vector<std::tuple<int, int, int>>& completed_jobs, std::mutex& jobs_mutex);
float randomFloat();
int randomInt(int a, int b);
float randomFloat(int a, int b);

int main()
{
    setlocale(LC_ALL, "ru");
    std::string thread_work_info = "ThreadInfo.txt";
    std::ofstream os1(thread_work_info);
    
    std::string cosined_angles = "End.txt";
    std::ofstream os2(cosined_angles);

    ThreadPool pool;

    std::cout << "Потоки начали вычислять косинусы " << COUNT_OF_NUMBERS << " углов\n";

    int thread_count = pool.getTreadsCount();

    std::vector<float> angles;
    initArray(angles, COUNT_OF_NUMBERS);
    std::vector<float> cosines;
    initArray(cosines, COUNT_OF_NUMBERS);

    std::vector<std::tuple<int, int, int>> completed_jobs;
    std::mutex jobs_mutex; 

    if (thread_count != 0)
    {
        int end, start;
        int step = end = COUNT_OF_NUMBERS / thread_count;
        int n = COUNT_OF_NUMBERS;
        int i = 0;
        while (end < n - 1) 
        {
            start = i * step + i;
            end = (start + step > n) ? COUNT_OF_NUMBERS - 1: start + step;
            pool.fillQueueJobs(std::bind(job, std::ref(angles),std::ref(cosines), start, end, std::ref(completed_jobs), std::ref(jobs_mutex)));
            i++;
        }
    }

    waitForCompletion(pool, completed_jobs, jobs_mutex);

    for (auto i : completed_jobs)
    {
        os1 << "Thread " << std::get<0>(i) << " completed job from index " << std::get<1>(i) << " to index " << std::get<2>(i) << std::endl;
    }
    for (int i = 0; i < COUNT_OF_NUMBERS; i++)
    {
        os2 << "Cos(" << angles[i] << ") = " << cosines[i] << std::endl;
    }
    std::cout << "\nПотоки закончили работу и информация об их действиях записана в файл " << thread_work_info << std::endl;
    std::cout << "Косинусы углов записаны в файл " << cosined_angles << std::endl;

    return 0;
}

void initArray(std::vector<float>& arr, int n)
{
    srand((unsigned int)time(NULL));
    for (int i = 0; i < n; i++)
    {
        arr.push_back(randomFloat(10, 360));
    }
}

void waitForCompletion(ThreadPool& pool, std::vector<std::tuple<int, int, int>>& completed_jobs, std::mutex& jobs_mutex)
{
    int completed_jobs_size;
    do
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        {
            std::lock_guard<std::mutex> lg(jobs_mutex);
            completed_jobs_size = (int)completed_jobs.size();
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    } while (pool.getJobsInProgress() != completed_jobs_size);
}

void job(std::vector<float>& angles, std::vector<float>& cosines, int start, int end, std::vector<std::tuple<int, int, int>>& completed_jobs, std::mutex& jobs_mutex)
{
    std::hash<std::thread::id> hasher;
    int thread_id = (int)hasher(std::this_thread::get_id());

    if (!angles.empty() && !cosines.empty())
    {
        for (int i = start; i <= end; i++)
        {
            cosines[i] = cos(angles[i]);
        }

        {
            std::lock_guard<std::mutex> lg(jobs_mutex);
            completed_jobs.push_back(std::make_tuple(std::abs(thread_id % 10000), start, end));
        }
    }
}

float randomFloat()
{
    return (float)(rand()) / (float)(RAND_MAX);
}

int randomInt(int a, int b)
{
    if (a > b)
        return randomInt(b, a);
    if (a == b)
        return a;
    return a + (rand() % (b - a));
}

float randomFloat(int a, int b)
{
    if (a > b)
        return randomFloat(b, a);
    if (a == b)
        return (float)a;

    return (float)randomInt(a, b) + randomFloat();
}
