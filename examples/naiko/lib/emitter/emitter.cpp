#include "emitter.h"

#include <fstream>

namespace aiko::naiko
{
    Emitter::Emitter(const string file)
        : m_file(file)
    {

    }

    void Emitter::append(string text)
    {
        m_code += text;
    }

    void Emitter::newLine(string line)
    {
        m_code += line;
        m_code += "\n";
    }

    void Emitter::save()
    {
        std::ofstream file(m_file);
        file << m_code;
    }

    void Emitter::clear()
    {
        m_code.clear();
    }
}
