#include "online_test_manager.h"

#include <string>
#include <streambuf>
#include <iostream>
#include <filesystem>
#include <string>
#include <fstream>
#include <streambuf>
#include <algorithm>
#include <random>
#include <mutex>
#include <thread>

#include <core/log.h>

#include <json/json.h>

namespace nes::test::online
{

    void TestManager::init()
    {

        const std::string GLOBAL_PATH = "C:/Users/Jose/Documents/Aiko/examples/nes6502/assets/tests/";

        std::mutex m;

        auto parse = [&](std::string str)
        {
            Json::Reader reader;
            Json::Value root;
            reader.parse(str, root);

            for (Json::Value::ArrayIndex i = 0; i != root.size(); i++)
            {
                Json::Value tmp = root[i];
                Json::FastWriter fastWriter;
                std::string output = fastWriter.write(tmp);
                std::lock_guard<std::mutex> lockGuard(m);
                tests.push_back(OnlinesTest(output));
            }
        };

#if true

        auto readFile = [&](std::filesystem::path path)
        {
            std::ifstream t(path);
            std::string str((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
            t.close();
            parse(str);
        };

        std::vector<std::thread> threads;

        for (const auto& entry : std::filesystem::directory_iterator(GLOBAL_PATH))
        {
            if (std::filesystem::is_regular_file(entry.path()))
            {
                threads.emplace_back(std::thread(readFile, entry.path()));
            }
        }

        int loaded = 0;

        for (auto& th : threads)
        {
            th.join();
        }

        int finished = 0;

#else 
        std::string path = GLOBAL_PATH + "0a.json";
        std::ifstream t(path);
        std::string str((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
        parse(str);
#endif

        // std::shuffle(tests.begin(), tests.end(), std::default_random_engine{});

    }

    void TestManager::run()
    {

        static bool first = true;
        if (first == true)
        {
            first = false;
            init();
        }

        for (OnlinesTest& t : tests)
        {
            aiko::Log::info("RUN ", t.name);
            t.run();
            aiko::Log::info("   Test passed ", t.name );
        
        }

        int a = 0;

    }

}
