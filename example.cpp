#include <iostream>
#include <vector>
#include <chrono>
#include <map>
#include <thread>
#include <string>
#include <tuple>
#include "ThreadPool.hpp"

using namespace std;
typedef tuple<string, int> M_TUPLE;
typedef std::function<M_TUPLE()> M_FUNCTION;
vector< std::function<tuple<string, int>()> > M_VECTOR;
typedef std::lock_guard<std::recursive_mutex> M_GUARD;
typedef std::unique_lock<std::recursive_mutex> M_UNIQUE;
std::recursive_mutex func_mutex;

enum RET_CODE{
    M_SUCCESS = 0,
    M_FAIL,
    M_MAX
};

M_TUPLE M_put()
{
    std::string M_func(__func__);
    
    // fucntion body
    std::cout << "hello " << '\n'<< std::flush;
    std::this_thread::sleep_for(std::chrono::microseconds(100));//seconds(1));
    std::cout << "world "  << '\n' <<std::flush;

    return std::make_tuple(M_func, M_SUCCESS);
}

int main()
{
    int thread_num = std::thread::hardware_concurrency();
    if (!thread_num)
    {
        thread_num = 2;
    }
    // now Jsaon's code does not support multi-client. So set the thread pool to one thread
    thread_num = 1;
    M_VECTOR.push_back(M_put);
    M_VECTOR.push_back(M_put);
    M_VECTOR.push_back(M_put);
    M_VECTOR.push_back(M_put);

    std::cout<< " start "<< thread_num << "threads"<<std::endl;

    ThreadPool pool(thread_num);
    std::vector< std::future<M_TUPLE> > results;
    M_FUNCTION tmp;

    while(!M_VECTOR.empty()) 
    {
        {
            M_GUARD lock1(func_mutex);
            tmp = M_VECTOR.back();
        }


        results.emplace_back(
        pool.enqueue([&] {
            return tmp();
            })
        );
        {
            M_GUARD lock1(func_mutex);
            M_VECTOR.pop_back();
        }

    }


    for(auto && result: results)
    {
        std::string tmp_str;
        int tmp_bool;
        tie(tmp_str, tmp_bool) = result.get();
        cout << "string is "<< tmp_str<< "bool is "<<tmp_bool<<endl;
    }

    std::cout << std::flush;

    return 0;
}

