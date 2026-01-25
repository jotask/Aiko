#pragma once

#include <vector>

#include <aiko_types.h>

namespace aiko::naiko
{

    struct CompilerOptions
    {
        enum class EmitterKind{ CPP, LLVM };
        std::vector<std::string> inputFiles;
        string outputFile;
        EmitterKind emitterKind = EmitterKind::CPP;
    };

    void printCompilerUsage();
    void checkMandatoryCompilerOptions(const CompilerOptions& opts);
    CompilerOptions parseArgs(int argc, char** argv);

}