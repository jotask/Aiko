#pragma once

namespace aiko
{

    class UIRaycastTarget
    {
    public:

        virtual ~UIRaycastTarget() = default;

        void setRaycastTarget(bool raycastTarget) { m_raycastTarget = raycastTarget; }
        bool isRaycastTarget() const { return m_raycastTarget; }

    private:

        bool m_raycastTarget = true;
    };

}
