#include <thread>

#include "ThreadPool.hpp"

int test(int i)
{
                std::cout << "hello " << i << '\n'<< std::flush;
                std::this_thread::sleep_for(std::chrono::microseconds(i));//seconds(1));
                std::cout << "world " << i << '\n' <<std::flush;
                return i*i;
}

int main()
{

    int thread_num = std::thread::hardware_concurrency();
    if (!thread_num)
    {
        thread_num = 2;
    }
    std::cout<< " start "<< thread_num << "threads"<<std::endl;

    ThreadPool pool(thread_num);
    std::vector< std::future<int> > results;

    for(int i = 0; i < 80; ++i) {
        results.emplace_back(
            pool.enqueue([i] {
                return test(i);
            })
        );
    }

    for(auto && result: results)
        std::cout << result.get() << '\n';
    std::cout << std::flush;

    return 0;
}
      
