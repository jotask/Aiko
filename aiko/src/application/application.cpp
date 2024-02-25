#include "application/application.h"

namespace aiko
{

    Application::Application()
        : m_aiko( std::make_unique<Aiko>( this ) )
    {
        m_aiko->run();
    }

}
