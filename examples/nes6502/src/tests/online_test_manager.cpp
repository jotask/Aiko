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

#include <core/log.h>

#include <json/json.h>

namespace nes::test::online
{
    void TestManager::init()
    {

        // #define LOAD_ALL_FILES

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
                tests.push_back(OnlinesTest(output));
            }
        };

#ifdef LOAD_ALL_FILES
        std::string path = "C:/Users/j.iznardo/Documents/Aiko/examples/nes6502/assets/tests";
        for (const auto& entry : fs::directory_iterator(path))
        {
            if (fs::is_regular_file(entry.path()))
            {
                std::ifstream t(entry.path());
                std::string str((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
                parse(str);
                // FIXME Only do one for now
                return;
            }
        }
#else 
        std::string path = "C:/Users/j.iznardo/Documents/Aiko/examples/nes6502/assets/tests/_tests.json";
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
    }

}
