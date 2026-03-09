#include "assets_manager_module.h"

namespace aiko
{

    AssetsManagerModule::AssetsManagerModule(Aiko* aiko)
       : BaseModule(aiko)
    {
    }

    void AssetsManagerModule::init()
    {
    }

    void AssetsManagerModule::update()
    {
    }

    void AssetsManagerModule::beginFrame()
    {
    }

    void AssetsManagerModule::endFrame()
    {
    }

    void AssetsManagerModule::dispose()
    {
        m_manager.clear();
    }

}
