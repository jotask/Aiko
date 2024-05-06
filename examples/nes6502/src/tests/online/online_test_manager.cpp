#include "online_test_manager.h"

#include <string>
#include <streambuf>
#include <iostream>
#include <filesystem>
#include <string>
#include <fstream>
#include <streambuf>

#include <json/json.h>

namespace fs = std::filesystem;

namespace test::online
{
    TestManager::TestManager()
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
    }

    void TestManager::run()
    {
        for (OnlinesTest& t : tests)
        {
            t.run();
        }
    }

}
