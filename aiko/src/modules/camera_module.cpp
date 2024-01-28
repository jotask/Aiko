#include "camera_module.h"

namespace aiko
{
    
    void CameraModule::init()
    {
        m_camera.position = { 30.0f, 20.0f, 30.0f };          // Camera position
        m_camera.target = { 0.0f, 0.0f, 0.0f };               // Camera looking at point
        m_camera.up = { 0.0f, 1.0f, 0.0f };                   // Camera up vector (rotation towards target)
        m_camera.fovy = 70.0f;                                // Camera field-of-view Y
        m_camera.projection = CAMERA_PERSPECTIVE;             // Camera projection type

    }

}
