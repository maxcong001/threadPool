/*
 * Copyright (c) 2016-20017 Max Cong <savagecm@qq.com>
 * this code can be found at https://github.com/maxcong001/threadPool
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include <iostream>
#include <vector>
#include <chrono>
#include <map>
#include <thread>
#include <string>
#include <tuple>
#include "MThreadPool.hpp"
#include <log4cplus/logger.h>
//#include <log4cplus/consoleappender.h>
#include <log4cplus/fileappender.h>
#include <log4cplus/layout.h>
//#include <log4cplus/ndc.h>
//#include <log4cplus/mdc.h>
#include <log4cplus/helpers/loglog.h>
#include <log4cplus/thread/threads.h>
//#include <log4cplus/helpers/sleep.h>
#include <log4cplus/loggingmacros.h>

using namespace std;
using namespace log4cplus;
using namespace log4cplus::helpers;
Logger logger = Logger::getInstance(LOG4CPLUS_TEXT("Max:"));

using namespace std;
typedef tuple<string, int> M_TUPLE;
typedef std::function<M_TUPLE()> M_FUNCTION;
vector<std::function<tuple<string, int>()>> M_VECTOR;
typedef std::lock_guard<std::recursive_mutex> M_GUARD;
typedef std::unique_lock<std::recursive_mutex> M_UNIQUE;
std::recursive_mutex func_mutex;

enum RET_CODE
{
    M_SUCCESS = 0,
    M_FAIL,
    M_MAX
};

M_TUPLE M_put()
{
    std::string M_func(__func__);
    // fucntion body
    LOG4CPLUS_DEBUG(logger, "hello!");
    std::this_thread::sleep_for(std::chrono::microseconds(100)); //seconds(1));
    LOG4CPLUS_DEBUG(logger, "world!");
    return std::make_tuple(M_func, M_SUCCESS);
}
void M_LOG()
{
}

int main()
{
    log4cplus::initialize();
    try
    {
        SharedObjectPtr<Appender> append_1(new FileAppender("Test.log"));
        append_1->setName(LOG4CPLUS_TEXT("First"));

        log4cplus::tstring pattern = LOG4CPLUS_TEXT("[%d{%m/%d/%y %H:%M:%S,%Q}] %c %-5p - %m [%l]%n");
        //  std::tstring pattern = LOG4CPLUS_TEXT("%d{%c} [%t] %-5p [%.15c{3}] %%%x%% - %m [%l]%n");
        append_1->setLayout(std::auto_ptr<Layout>(new PatternLayout(pattern)));
        Logger::getRoot().addAppender(append_1);
        logger.setLogLevel(DEBUG_LOG_LEVEL);
    }
    catch (...)
    {
        Logger::getRoot().log(FATAL_LOG_LEVEL, LOG4CPLUS_TEXT("Exception occured..."));
    }
    LOG4CPLUS_DEBUG(logger, "set logger done!"
                                << "\nhello log4cplus\n");
    int thread_num = std::thread::hardware_concurrency();
    if (!thread_num)
    {
        thread_num = 1;
    }
    M_VECTOR.push_back(M_put);
    M_VECTOR.push_back(M_put);
    M_VECTOR.push_back(M_put);
    M_VECTOR.push_back(M_put);
    std::cout << " start " << thread_num << "threads" << std::endl;
    ThreadPool pool(thread_num);
    std::vector<std::future<M_TUPLE>> results;
    M_FUNCTION tmp;
    while (!M_VECTOR.empty())
    {
        {
            M_GUARD lock1(func_mutex);
            tmp = M_VECTOR.back();
        }
        results.emplace_back(
            pool.enqueue([=] {
                return tmp();
            }));
        {
            M_GUARD lock1(func_mutex);
            M_VECTOR.pop_back();
        }
    }
    for (auto &&result : results)
    {
        std::string tmp_str;
        int tmp_bool;
        tie(tmp_str, tmp_bool) = result.get();
        cout << "string is " << tmp_str << "bool is " << tmp_bool << endl;
    }
    std::cout << std::flush;
    return 0;
}
