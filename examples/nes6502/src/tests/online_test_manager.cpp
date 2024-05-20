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

    void TestManager::run()
    {

        const std::string GLOBAL_PATH = "C:/Users/Jose/Documents/Aiko/examples/nes6502/assets/tests/";

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

        auto readFile = [&](std::filesystem::path path)
            {
                std::ifstream t(path);
                std::string str((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
                t.close();
                parse(str);
            };

        auto runByteOpCode = [&](Byte opCode)
        {
            tests.clear();

            auto fin = GLOBAL_PATH + toString(opCode).substr(2, 2) + ".json";
            readFile(fin);

            for (OnlinesTest& t : tests)
            {
                aiko::Log::info("RUN ", t.name);
                t.run();
                aiko::Log::info("   Test passed ", t.name);
            }

            int a = 0;
        };

        for (Byte i = 0x2a ; i <= 0xff ; i++)
        {
            runByteOpCode(i);
        }

        int a = 0;

    }

}
