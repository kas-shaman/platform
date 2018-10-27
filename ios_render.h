#pragma once

#import <GLKit/GLKit.h>

namespace platform {
    class IOSRender : public RenderingDeviceInterface {
    public:
        IOSRender(const std::shared_ptr<PlatformInterface> &platform);
        ~IOSRender() override;

        void updateCameraTransform(const float (&camPos)[3], const float(&camDir)[3], const float(&camVP)[16]) override;
        
        std::unique_ptr<Shader> createShader(
            const char *shadersrc,
            const std::initializer_list<ShaderInput> &vertex,
            const std::initializer_list<ShaderInput> &instance,
            const void *prmnt
        ) override;
        
        std::unique_ptr<Texture2D> createTexture(
            Texture2D::Format format,
            std::uint32_t width,
            std::uint32_t height,
            const std::initializer_list<const std::uint8_t *> &mipsData
        ) override;
        
        std::unique_ptr<StructuredData> createData(const void *data, std::uint32_t count, std::uint32_t stride) override;
        
        void applyShader(const std::unique_ptr<Shader> &shader, const void *constants) override;
        void applyTextures(const std::initializer_list<const Texture2D *> &textures) override;
        
        void drawGeometry(std::uint32_t vertexCount, Topology topology) override;
        
        void prepareFrame() override;
        void presentFrame(float dt) override;
        
        void getFrameBufferData(std::uint8_t *imgFrame) override;

    private:
        struct FrameData {
            float renderTargetBounds[4];
            float viewProjMatrix[16];
            float cameraPosition[4];
            float cameraDirection[4];
        }
        _frameData;
        
        std::shared_ptr<PlatformInterface> _platform;
        
        GLuint _shaderFrameDataBuffer;
        GLuint _shaderConstStreamBuffer;
        std::size_t _shaderConstStreamOffset;
    };

    std::shared_ptr<RenderingDeviceInterface> getRenderingDeviceInstance(const std::shared_ptr<PlatformInterface> &platform);
}
