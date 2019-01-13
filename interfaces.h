#pragma once

// TODO: graphic state control
// TODO: sound
// TODO: joystick
// TODO: tests

#include <cstdint>
#include <memory>
#include <vector>
#include <string>
#include <functional>

namespace platform {
    struct Base {
    protected:
        Base() = default;
        ~Base() = default;
    
    private:
        Base(Base &&) = delete;
        Base(const Base &) = delete;
        Base &operator =(Base &&) = delete;
        Base &operator =(const Base &) = delete;
    };
    
    enum class KeyboardKey {
        A = 0, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
        NUM0, NUM1, NUM2, NUM3, NUM4, NUM5, NUM6, NUM7, NUM8, NUM9,
        LEFT, RIGHT, UP, DOWN,
        SPACE, ENTER, TAB,
        F1, F2, F3, F4, F5, F6, F7, F8, F9, F10,
        _count
    };
    
    struct KeyboardEventArgs {
        KeyboardKey key;
        bool alt;
        bool shift;
        bool ctrl;
    };
    
    struct MouseEventArgs {
        mutable float coordinateX;
        mutable float coordinateY;
        bool  isLeftButtonPressed;
        bool  isRightButtonPressed;
    };
    
    struct TouchEventArgs {
        float coordinateX;
        float coordinateY;
        std::size_t touchID;
    };
    
    struct GamepadEventArgs {
    
    };
    
    using EventHandlersToken = struct {} *;
    
    // Interface provides low-level core methods
    //
    class PlatformInterface : public Base {
    public:
        PlatformInterface() = default;
        virtual ~PlatformInterface() {}
        
    public:
        // Thread-safe logging
        virtual void logInfo(const char *fmt, ...) = 0;
        virtual void logWarning(const char *fmt, ...) = 0;
        virtual void logError(const char *fmt, ...) = 0;
        
        // Forms std::vector of file paths in @dirPath
        // @dirPath  - target directory. Example: "data/map1"
        // @return   - vector of paths
        //
        virtual std::vector<std::string> formFileList(const char *dirPath) = 0;
        
        // Loads file to memory
        // @filePath - file path. Example: "data/map1/test.png"
        // @return   - true if file successfully loaded. Items returned by formFileList should be successfully loaded.
        //
        virtual bool loadFile(const char *filePath, std::unique_ptr<uint8_t[]> &data, std::size_t &size) = 0;
        
        // Returns native screen size in pixels
        //
        virtual float getNativeScreenWidth() const = 0;
        virtual float getNativeScreenHeight() const = 0;
        
        // Connecting render with native window. Used by RenderingDevice. Must be called before run()
        // @context  - platform-dependent handle (ID3D11Device * for windows, EAGLContext * for ios, etc)
        // @return   - platform-dependent result (IDXGISwapChain * for windows)
        //
        virtual void *setNativeRenderingContext(void *context) = 0;
        
        // Show/hide PC mouse pointer
        virtual void showCursor() = 0;
        virtual void hideCursor() = 0;
        
        // Show/hide keyboard on mobile devices
        virtual void showKeyboard() = 0;
        virtual void hideKeyboard() = 0;
        
        // Set handlers for PC keyboard
        // @return nullptr if is not supported
        //
        virtual EventHandlersToken addKeyboardEventHandlers(
            std::function<void(const KeyboardEventArgs &)> &&down,
            std::function<void(const KeyboardEventArgs &)> &&up
        ) = 0;
        
        // Set handlers for User's input (physical or virtual keyboard)
        // @return nullptr if is not supported
        //
        virtual EventHandlersToken addInputEventHandlers(
            std::function<void(const char (&utf8char)[4])> &&input,
            std::function<void()> &&backspace
        ) = 0;
        
        // Set handlers for PC mouse
        // coordinateX/coordinateY of MouseEventArgs struct can be replaced with user's value (Platform will set new pointer coordinates)
        // @return nullptr if is not supported
        //
        virtual EventHandlersToken addMouseEventHandlers(
            std::function<void(const MouseEventArgs &)> &&press,
            std::function<void(const MouseEventArgs &)> &&move,
            std::function<void(const MouseEventArgs &)> &&release
        ) = 0;
        
        // Set handlers for touch
        // @return nullptr if is not supported
        //
        virtual EventHandlersToken addTouchEventHandlers(
            std::function<void(const TouchEventArgs &)> &&start,
            std::function<void(const TouchEventArgs &)> &&move,
            std::function<void(const TouchEventArgs &)> &&finish
        ) = 0;
        
        // Set handlers for gamepad
        // @return nullptr if is not supported
        //
        virtual EventHandlersToken addGamepadEventHandlers(
            std::function<void(const GamepadEventArgs &)> &&buttonPress,
            std::function<void(const GamepadEventArgs &)> &&buttonRelease
        ) = 0;
        
        // Start platform update cycle
        // This method blocks execution until application exit
        // Depending on the app state, 'draw' may or may not be called
        //
        virtual void run(
            std::function<void(float)> &&update,
            std::function<void()> &&draw
        ) = 0;
        
        // Remove handlers of any type
        //
        virtual void removeEventHandlers(EventHandlersToken token) = 0;
        
        // Breaks platform update cycle
        //
        virtual void exit() = 0;
    };
    
    // Interface provides Audio control methods
    //
    class AudioDeviceInterface : public Base {
    public:
        AudioDeviceInterface() = default;
        virtual ~AudioDeviceInterface() {}
        
    public:
        
    };

    // Topology of vertex data
    //
    enum class Topology {
        LINES = 0,
        LINESTRIP,
        TRIANGLES,
        TRIANGLESTRIP,
        _count
    };
    
    // Field description for Vertex Shader input struct
    //
    struct ShaderInput {
        enum class Format {
            VERTEX_ID = 0,
            HALF2, HALF4,
            FLOAT1, FLOAT2, FLOAT3, FLOAT4,
            SHORT2, SHORT4,
            SHORT2_NRM, SHORT4_NRM,
            BYTE4,
            BYTE4_NRM,
            INT1, INT2, INT3, INT4,
            _count
        };
        
        const char *name;
        Format format;
    };
    
    class Shader : public Base {
    public:
        Shader() {}
    };
    
    class Texture2D : public Base {
    public:
        enum class Format {
            RGBA8UN = 0,    // rgba 1 byte per channel normalized to [0..1]
            R8UN = 1,       // 1 byte grayscale normalized to [0..1]. In shader .r component is used
            _count
        };
        
        Texture2D() {}
        
        std::uint32_t getWidth() const;
        std::uint32_t getHeight() const;
        std::uint32_t getMipCount() const;
        Texture2D::Format getFormat() const;
    };
    
    class StructuredData : public Base {
    public:
        StructuredData() {}
        
        std::uint32_t getCount() const;
        std::uint32_t getStride() const;
    };
    
    // Interface provides 3D-visualization methods
    //
    class RenderingDeviceInterface {
    public:
        RenderingDeviceInterface() = default;
        virtual ~RenderingDeviceInterface() {}
        
    public:
        virtual void updateCameraTransform(const float (&camPos)[3], const float(&camDir)[3], const float(&camVP)[16]) = 0;
        
        // Create shader from source text
        // @vertex    - input layout for vertex shader (all such variables have 'vertex_' prefix)
        // @instance  - input layout for vertex shader (all such variables have 'instance_' prefix)
        // @prmnt     - pointer to data for the block of permanent constants (Can be nullptr in case of )
        // @shadersrc - generic shader source text. Example:
        // s--------------------------------------
        //     prmnt {                          - block of permanent constants. Can be omitted if unused.
        //         constName0 : float4          -
        //     }
        //     const {                          - block of per-apply constants. Can be omitted if unused.
        //         constName1 : float4          -
        //         constNames[16] : float4      - spaces in/before array braces are not permitted
        //     }
        //     inter {                          - vertex output/fragment input. Can be omitted if unused.
        //         varName4 : float4            - vertex shader also has float4 'out_position' variable
        //     }
        //     vssrc {                          - assume that input = {{"position", ShaderInput::Format::FLOAT4}, {"color", ShaderInput::Format::BYTE4_NRM}};
        //         inter.varName4 = vertex_color;
        //         out_position = _mul(_viewProjMatrix, float4(vertex_position, 1.0));
        //     }
        //     fssrc {                          - fragment shader also has float4 'out_color' variable
        //         out_color = input.varName4;
        //     }
        // s--------------------------------------
        // Types:
        //     matrix4, matrix3, float, float2, float3, float4, int, int2, int3, int4, uint, uint2, uint3, uint4
        //
        // Per frame global constants:
        //     _renderTargetBounds : float2 - render target size in pixels
        //     _viewProjMatrix     : matrix - view * projection matrix
        //     _cameraPosition     : float4 - camera position (w = 1)
        //     _cameraDirection    : float4 - normalized camera direction (w = 0)
        //
        // Textures:
        //     _textures[8] - array of 8 texture slots. Example: float4 color = _tex2d(_textures[0], float2(0, 0));
        //
        // Global functions:
        //     _transform(v, m), _sign(s), _dot(v, v), _sin(v), _cos(v), _norm(v), _tex2d(t, v)
        //
        virtual std::shared_ptr<Shader> createShader(
            const char *shadersrc,
            const std::initializer_list<ShaderInput> &vertex,
            const std::initializer_list<ShaderInput> &instance = {},
            const void *prmnt = nullptr
        ) = 0;
        
        // Create texture from binary data
        // @w and @h    - width and height of the 0th mip layer
        // @imgMipsData - array of pointers. Each [i] pointer represents binary data for i'th mip and cannot be nullptr
        //
        virtual std::shared_ptr<Texture2D> createTexture(
            Texture2D::Format format,
            std::uint32_t width,
            std::uint32_t height,
            const std::initializer_list<const std::uint8_t *> &mipsData = {}
        ) = 0;
        
        // Create geometry
        // @data        - pointer to data (array of structures)
        // @count       - count of structures in array
        // @stride      - size of struture
        // @return      - handle
        //
        virtual std::shared_ptr<StructuredData> createData(const void *data, std::uint32_t count, std::uint32_t stride) = 0;
        
        // TODO: render states
        
        // Apply shader
        // @shader      - shader object.
        // @constants   - pointer to data for 'const' block. Can be nullptr (constants will not be set)
        //
        virtual void applyShader(const std::shared_ptr<Shader> &shader, const void *constants = nullptr) = 0;
        
        // Apply textures. textures[i] can be nullptr (texture will not be set)
        //
        virtual void applyTextures(const std::initializer_list<const Texture2D *> &textures) = 0;
        
        // Draw vertexes without geometry
        //
        virtual void drawGeometry(std::uint32_t vertexCount, Topology topology = Topology::TRIANGLES) = 0;

        // Draw vertexes from StructuredData
        // @vertexData and @instanceData has layout set by current shader. Both can be nullptr
        //
        virtual void drawGeometry(
            const std::shared_ptr<StructuredData> &vertexData,
            const std::shared_ptr<StructuredData> &instanceData,
            std::uint32_t vertexCount,
            std::uint32_t instanceCount,
            Topology topology = Topology::TRIANGLES
        ) = 0;

        // TODO: draw indexed geometry
        
        virtual void prepareFrame() = 0;
        virtual void presentFrame(float dt) = 0;
        
        // Get last rendered frame as a bitmap in memory
        // @imgFrame - array of size = PlatformInterface::getNativeScreenWidth * PlatformInterface::getNativeScreenHeight * 4
        //
        virtual void getFrameBufferData(std::uint8_t *imgFrame) = 0;
    };
    
    std::shared_ptr<PlatformInterface> getPlatformInstance();
    std::shared_ptr<AudioDeviceInterface> getAudioDeviceInstance(const std::shared_ptr<PlatformInterface> &platform);
    std::shared_ptr<RenderingDeviceInterface> getRenderingDeviceInstance(const std::shared_ptr<PlatformInterface> &platform);
}

