#include "naiko_compiler_options.h"

#include <logger/logger.h>

#include "compiler_helper.h"

#include <filesystem>

namespace aiko::naiko
{
    void printCompilerUsage()
    {
        logger::Log::info("NAIKO compiler");
        logger::Log::info("PoC basic compiler.");
        logger::Log::info("%sOptions", makeIndent(1).data());
        logger::Log::info("%s[Args] : All files to be compiled", makeIndent(2).data());
        logger::Log::info("%s-o <file> : Output file", makeIndent(2).data());
        logger::Log::info("%s--emit-* : Chose emit version, currently only supported \"--emit-cpp\", selected by default", makeIndent(2).data());
        logger::Log::info("%s--help : Print usage", makeIndent(2).data());
    }

    void checkMandatoryCompilerOptions(const CompilerOptions& opts)
    {

        if (opts.inputFiles.size() == 0)
        {
            logger::Log::error("Expects files to be compiled");
            std::exit(EXIT_FAILURE);
        }

        if (opts.inputFiles.size() > 1)
        {
            logger::Log::error("Unsupported featured, currently we only can compile one file");
            std::exit(EXIT_FAILURE);
        }

        for (const auto& file : opts.inputFiles)
        {
            if (std::filesystem::exists(file.c_str()) == false)
            {
                logger::Log::error("Input file dont' exist : [%s]", file.c_str());
                std::exit(EXIT_FAILURE);
            }
        }

        if (opts.outputFile.empty() == true)
        {
            logger::Log::error("Expects -o <name> for the output file");
            std::exit(EXIT_FAILURE);
        }
    }

    CompilerOptions parseArgs(int argc, char** argv)
    {
        const string naikoCompilerPath = string(argv[0]);
        CompilerOptions opts;

        for (int i = 1; i < argc; ++i)
        {
            string arg = string(argv[i]);

            if (arg == "--help")
            {
                printCompilerUsage();
                std::exit(EXIT_SUCCESS);
            }

            if (arg == "-o")
            {
                if (i  + 1 >= argc)
                {
                    logger::Log::error("-o expects an output file");
                    std::exit(EXIT_FAILURE);
                }

                if (argv[i+1][0] == '-')
                {
                    logger::Log::error("-o expects an output file");
                    std::exit(EXIT_FAILURE);
                }


                const auto path = std::filesystem::path(argv[++i]);
                opts.outputFile = std::filesystem::absolute(path).string();
                continue;
            }

            if (arg.starts_with("--emit-"))
            {
                if (arg == "--emit-cpp")
                {
                    opts.emitterKind = CompilerOptions::EmitterKind::CPP;
                }
                else
                {
                    logger::Log::info("Unknown emitter kind, Defaulting to CPP");
                }
                continue;
            }

            if (arg.empty() == false && arg.starts_with("-") == false)
            {
                const auto path = std::filesystem::path(arg);
                opts.inputFiles.emplace_back(std::filesystem::absolute(path).string());
                continue;
            }

            logger::Log::error("Unknow options: %s", arg.c_str());
            std::exit(EXIT_FAILURE);

        }

        checkMandatoryCompilerOptions(opts);

        return opts;
    }
}
