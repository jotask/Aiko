#pragma once

#include "aiko_types.h"
#include "modules/base_module.h"
#include "types/textures.h"
#include "models/shader.h"
#include "events/event.hpp"
#include "types/color.h"
#include "types/textures.h"

namespace aiko
{

    class CameraModule;
    class Camera;
    class DisplayModule;
    class Mesh;
    class Shader;
    class Transform;

    class RenderModule : public BaseModule
    {
    
    public:

        RenderModule();
        virtual ~RenderModule();

    protected:

        virtual void connect(ModuleConnector*);
    
        virtual void preInit() override;
        virtual void init() override;
        virtual void postInit() override;
    
        void virtual preUpdate() override;
        void virtual update() override;
        void virtual postUpdate() override;
    
        virtual void preRender() override;
        virtual void render() override;
        virtual void postRender() override;
    
        virtual void beginFrame() override;
        virtual void endFrame() override;
    
    public:

        void initMesh(Mesh*);
        void refreshMesh(Mesh*);
        void renderMesh(Camera*, Transform*, Mesh*, Shader*);

        void initShader(Shader*, const char*, const char*);
        void refreshShader(Mesh*);

        CameraModule* getCameraModule() { return m_cameraModule; }
        ivec2 getDisplaySize();

        texture::RenderTexture2D* getRenderTexture();

        void clearBackground(Color);
        void beginMode2D(Camera*);
        void endMode2D();
        void beginMode3D(Camera*);
        void endMode3D();
        void beginTextureMode(texture::RenderTexture2D& target);
        void endTextureMode(void);
        void beginShaderMode(aiko::ShaderData* shader);
        void endShaderMode(void);
        void beginBlendMode(BlendMode);
        void endBlendMode(void);

        /*
        // Texture
        void drawTexture(texture::Texture texture, int posX, int posY, Color tint);
        void drawTextureV(texture::Texture texture, vec2 position, Color tint);
        void drawTextureEx(texture::Texture texture, vec2 position, float rotation, float scale, Color tint);
        void drawTextureRec(texture::Texture texture, Rectangle source, vec2 position, Color tint);
        void drawTexturePro(texture::Texture texture, Rectangle source, Rectangle dest, vec2 origin, float rotation, Color tint);

        // 2d Context
        void drawPixel(int x, int y, Color);
        void drawPixel(vec2 pos, Color);
        void drawLine(int startPosX, int startPosY, int endPosX, int endPosY, Color color);
        void drawLine(vec2 startPos, vec2 endPos, Color color);
        void drawCircle(int centerX, int centerY, float radius, Color color);
        void drawCircle(vec2 center, float radius, Color color);
        void drawCircleLines(int centerX, int centerY, float radius, Color color);
        void drawCircleLines(vec2 center, float radius, Color color);
        void drawEllipse(int centerX, int centerY, float radiusH, float radiusV, Color color);
        void drawEllipseLines(int centerX, int centerY, float radiusH, float radiusV, Color color);
        void drawRing(vec2 center, float innerRadius, float outerRadius, float startAngle, float endAngle, int segments, Color color);
        void drawRingLines(vec2 center, float innerRadius, float outerRadius, float startAngle, float endAngle, int segments, Color color);
        void drawRectangle(int posX, int posY, int width, int height, Color color);
        void drawRectangle(vec2 position, vec2 size, Color color);
        void drawRectangleRec(Rectangle rec, Color color);
        void drawRectangleLines(int posX, int posY, int width, int height, Color color);
        void drawRectangleLinesEx(Rectangle rec, float lineThick, Color color);
        void drawRectangleRounded(Rectangle rec, float roundness, int segments, Color color);
        void drawRectangleRoundedLines(Rectangle rec, float roundness, int segments, float lineThick, Color color);
        void drawTriangle(vec2 v1, vec2 v2, vec2 v3, Color color);
        void drawTriangleLines(vec2 v1, vec2 v2, vec2 v3, Color color);
        void drawPoly(vec2 center, int sides, float radius, float rotation, Color color);
        void drawPolyLines(vec2 center, int sides, float radius, float rotation, Color color);
        void drawPolyLinesEx(vec2 center, int sides, float radius, float rotation, float lineThick, Color color);
        */

        // Shader
        aiko::ShaderData loadShaderData(const char*, const char*);
        void unloadShader( aiko::ShaderData& );
        int getShaderLocation(aiko::ShaderData&, const char* uniformName );
        void setShaderUniformValue(aiko::ShaderData&, int, const void*, aiko::ShaderUniformDataType);
        void setShaderUniformValueV(aiko::ShaderData&, int, const void*, aiko::ShaderUniformDataType, int);

    private:

        bool m_scale;

        CameraModule* m_cameraModule;
        DisplayModule* m_displayModule;

        texture::RenderTexture2D m_renderTexture2D;

        void onWindowResize(Event&);
    
    };

}
