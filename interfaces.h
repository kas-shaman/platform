#pragma once

#include <cstdint>
#include <memory>
#include <vector>
#include <functional>

namespace native {
    enum class PlatformKeyboardKey {
        A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
        LEFT, RIGHT, UP, DOWN,
        SPACE, ENTER, BACKSPACE, TAB
    };

    struct PlatformKeyboardEventArgs {
        std::uint32_t key;
    };

    struct PlatformMouseEventArgs {
        mutable float coordinateX;
        mutable float coordinateY;
        bool  isLeftButtonPressed;
        bool  isRightButtonPressed;
    };

    using PlatformCallbackToken = struct {} *;

    // Interface provides low-level core methods
    //
    class PlatformInterface {
    public:
        static std::shared_ptr<PlatformInterface> makeInstance();
        virtual ~PlatformInterface() {}

    public:
        virtual void logInfo(const char *fmt, ...) = 0;
        virtual void logWarning(const char *fmt, ...) = 0;
        virtual void logError(const char *fmt, ...) = 0;

        // Forming vector of file pathes
        // @dirPath  - target directory
        // @return   - vector of pathes. Example: "data/map1"
        //
        virtual std::vector<std::string> formFileList(const char *dirPath) const = 0;

        // Load file to memory
        // @filePath - file path. Example: "data/map1/test.png"
        // @return   - true if file successfully loaded
        //
        virtual bool loadFile(const char *filePath, std::unique_ptr<unsigned char []> &data, std::size_t &size) const = 0;

        // Getting native screen size in pixels
        //
        virtual float getNativeScreenWidth() const = 0;
        virtual float getNativeScreenHeight() const = 0;

        // Connecting render with native window
        // @context  - platform-dependent handle (ID3D11Device * for windows, EAGLContext * for ios, etc)
        // @return   - platform-dependent result (IDXGISwapChain * for windows)
        //
        virtual void *setNativeRenderingContext(void *context) = 0;

        virtual void showCursor() = 0;
        virtual void hideCursor() = 0;

        virtual PlatformCallbackToken addKeyboardCallbacks(
            std::function<void(const PlatformKeyboardEventArgs &)> &&down,
            std::function<void(const PlatformKeyboardEventArgs &)> &&up
        ) = 0;

        // Set callbacks for mouse/pointer movement
        // When @move called coordinateX/coordinateY can be replaced with user's value (Platform will set new pointer coordinates)
        //
        virtual PlatformCallbackToken addMouseCallbacks(
            std::function<void(const PlatformMouseEventArgs &)> &&press,
            std::function<void(const PlatformMouseEventArgs &)> &&move,
            std::function<void(const PlatformMouseEventArgs &)> &&release
        ) = 0;

        virtual void removeCallbacks(PlatformCallbackToken& token) = 0;

        // Start platform update cycle
        // This method blocks execution until application exit
        //
        virtual void run(std::function<void(float)> &&updateAndDraw) = 0;
    };

    struct RenderShaderInput {
        enum class Format {
            VERTEX_ID = 0,
            HALF2, HALF4,
            FLOAT1, FLOAT2, FLOAT3, FLOAT4,
            SHORT2, SHORT4,
            SHORT2_NRM, SHORT4_NRM,
            BYTE4,
            BYTE4_NRM,
            INTEGER1, INTEGER2, INTEGER3, INTEGER4,
        };

        const char *name;
        Format format;
        bool perInstance = false;
        std::size_t arraySize = 1;
    };

    struct RenderShader {
        struct Data;
        struct Data *_data;

        RenderShader();
        RenderShader(Data *);
        ~RenderShader();

        RenderShader(RenderShader &&);
        RenderShader& operator =(RenderShader &&);

        RenderShader(const RenderShader &) = delete;
        RenderShader& operator =(const RenderShader &) = delete;
    };

    struct RenderTexture {
        struct Data;
        struct Data *_data;

        enum class Format {
            RGBA8UN = 0,    // rgba 1 byte per channel normalized to [0..1]
            R8UN = 1,       // 1 byte grayscale normalized to [0..1]. In shader .r component is used
        };

        RenderTexture();
        RenderTexture(Data *);
        ~RenderTexture();

        RenderTexture(RenderTexture &&);
        RenderTexture& operator =(RenderTexture &&);

        RenderTexture(const RenderTexture &) = delete;
        RenderTexture& operator =(const RenderTexture &) = delete;

        std::uint32_t getWidth() const;
        std::uint32_t getHeight() const;
        std::uint32_t getMipCount() const;
    };

    struct RenderGeometry {
        struct Data;
        struct Data *_data;

        RenderGeometry();
        RenderGeometry(Data *);
        ~RenderGeometry();

        RenderGeometry(RenderGeometry &&);
        RenderGeometry& operator =(RenderGeometry &&);

        RenderGeometry(const RenderGeometry &) = delete;
        RenderGeometry& operator =(const RenderGeometry &) = delete;

        std::uint32_t getCount() const;
        std::uint32_t getStride() const;
    };

    // Interface provides 3D-visualization methods
    //
    class RenderInterface {
    public:
        static std::shared_ptr<RenderInterface> makeInstance(const std::shared_ptr<PlatformInterface> &platform);
        virtual ~RenderInterface() {}

    public:
        virtual void updateCameraTransform(const float (&camPos)[3], const float(&camDir)[3], const float(&camVP)[16]) = 0;

        // Create shader from source text
        // @input       - input layout for vertex shader (access through 'input' variable)
        // @shadersrc   - generic shader source text. Example:
        // s--------------------------------------
        //     const {                          - [0] block of constants
        //         constName0 : float4            - 
        //     }
        //     const {                          - [1] block of constants
        //         constNames[16] : float4      - spaces in/before array braces are not permitted
        //     }
        //     inter {                          - vertex output is same as fragment input
        //         varName4 : float4            - vertex output also have float4 'position' variable
        //     }
        //     vssrc {
        //         output.varName4 = input.varName + varName0;
        //         output.position = _mul(_VP, float4(input.varName3, 1.0));
        //     }
        //     fssrc {                          - fragment output have float4 'color' variable
        //         output.color = input.varName4;
        //     }
        // s--------------------------------------
        // Types:
        //     matrix, float1, float2, float3, float4, int1, int2, int3, int4
        //
        // Per frame global constants:
        //     _VP      - view * projection matrix
        //     _CamPos  - camera position
        //     _CamDir  - normalized camera direction
        //
        // Global functions:
        //     _mul(v|m, v), _sign(s), _dot(v, v), _norm(v), _tex2D(v)
        //
        virtual RenderShader createShader(const std::initializer_list<RenderShaderInput> &input, const char *shadersrc, const char *name) = 0;

        // Create texture from binary data
        // @imgMipsBinaryData - array of pointers. Each [i] pointer represents binary data for i'th mip and cannot be nullptr
        //
        virtual RenderTexture createTexture(RenderTexture::Format format, std::uint32_t width, std::uint32_t height, std::uint32_t mipCount, const std::uint8_t *const *imgMipsBinaryData = nullptr) = 0;

        // Create geometry
        // @data        - pointer to data (array of structures)
        // @count       - count of structures in array
        // @stride      - size of struture
        // @return      - handle
        //
        virtual RenderGeometry createGeometry(const void *data, std::uint32_t count, std::uint32_t stride) = 0;

        // Apply shader
        // @shader      - shader handle
        // @constants   - pointers to data for 'const' blocks. constData[i] can be nullptr (constants will not be updated, but set)
        //
        virtual void applyShader(const RenderShader &shader, const std::initializer_list<const void *> &constants = {}) = 0;

        // Update data of shader constants. Shader will not be set.
        // @shader      - shader handle
        // @constants   - pointers to data for 'const' blocks. constData[i] can be nullptr (constants will not be updated, but set)
        //
        virtual void applyShaderConstants(const RenderShader &shader, const std::initializer_list<const void *> &constants = {}) = 0;

        // Apply textures. textures[i] can be nullptr (texture will not be set)
        //
        virtual void applyTextures(const std::initializer_list<const RenderTexture *> &textures) = 0;

        // Draw geometry
        // @geometry - list of geometry (one geometry per slot)
        //
        virtual void drawGeometry(const std::initializer_list<const RenderGeometry *> &geometry, std::uint32_t vertexCount, std::uint32_t instanceCount) = 0;
        //virtual void debugDrawLine(const math::vector3f &position, const math::vector3f &to, const math::color &color) = 0;
        //virtual void debugDrawCircle(const math::vector3f &position, float radius, const math::color &color) = 0;

        virtual void prepareFrame() = 0;
        virtual void presentFrame(float dt) = 0;
    };

    
}