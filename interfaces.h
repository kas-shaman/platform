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
    enum class KeyboardKey {
        A = 0, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
        NUM0, NUM1, NUM2, NUM3, NUM4, NUM5, NUM6, NUM7, NUM8, NUM9,
        LEFT, RIGHT, UP, DOWN,
        SPACE, ENTER, TAB,
        F1, F2, F3, F4, F5, F6, F7, F8, F9, F10,
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
    class PlatformInterface {
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
        virtual std::vector<std::string> formFileList(const char *dirPath) const = 0;
        
        // Loads file to memory
        // @filePath - file path. Example: "data/map1/test.png"
        // @return   - true if file successfully loaded. Items returned by formFileList should be successfully loaded.
        //
        virtual bool loadFile(const char *filePath, std::unique_ptr<uint8_t[]> &data, std::size_t &size) const = 0;
        
        // Returns native screen size in pixels
        //
        virtual float getNativeScreenWidth() const = 0;
        virtual float getNativeScreenHeight() const = 0;
        
        // Connecting render with native window. Used by Platform.
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
        
    private:
        PlatformInterface(PlatformInterface &&) = delete;
        PlatformInterface(const PlatformInterface &) = delete;
        PlatformInterface &operator =(PlatformInterface &&) = delete;
        PlatformInterface &operator =(const PlatformInterface &) = delete;
    };
    
    // Interface provides Audio control methods
    //
    class AudioDeviceInterface {
    public:
        AudioDeviceInterface() = default;
        virtual ~AudioDeviceInterface() {}
        
    public:
        
    private:
        AudioDeviceInterface(AudioDeviceInterface &&) = delete;
        AudioDeviceInterface(const AudioDeviceInterface &) = delete;
        AudioDeviceInterface &operator =(AudioDeviceInterface &&) = delete;
        AudioDeviceInterface &operator =(const AudioDeviceInterface &) = delete;
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
            INTEGER1, INTEGER2, INTEGER3, INTEGER4,
        };
        
        const char *name;
        Format format;
        bool perInstance = false;
    };
    
    class Shader {
    public:
        Shader() = default;
        virtual ~Shader() {}
        
    protected:
        Shader(Shader &&) = delete;
        Shader(const Shader &) = delete;
        Shader& operator =(Shader &&) = delete;
        Shader& operator =(const Shader &) = delete;
    };
    
    class Texture2D {
    public:
        enum class Format {
            RGBA8UN = 0,    // rgba 1 byte per channel normalized to [0..1]
            R8UN = 1,       // 1 byte grayscale normalized to [0..1]. In shader .r component is used
        };
        
        Texture2D() = default;
        virtual ~Texture2D() {}
        
        virtual std::uint32_t getWidth() const = 0;
        virtual std::uint32_t getHeight() const = 0;
        virtual std::uint32_t getMipCount() const = 0;
        
    protected:
        Texture2D(Texture2D &&) = delete;
        Texture2D(const Texture2D &) = delete;
        Texture2D& operator =(Texture2D &&) = delete;
        Texture2D& operator =(const Texture2D &) = delete;
    };
    
    class Geometry {
    public:
        enum class Topology {
            LINES = 0,
            LINESTRIP,
            TRIANGLES,
            TRIANGLESTRIP,
        };
        
        Geometry() = default;
        virtual ~Geometry() {}
        
        virtual std::uint32_t getCount() const = 0;
        virtual std::uint32_t getStride() const = 0;
        
    protected:
        Geometry(Geometry &&) = delete;
        Geometry(const Geometry &) = delete;
        Geometry& operator =(Geometry &&) = delete;
        Geometry& operator =(const Geometry &) = delete;
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
        virtual Shader createShader(const std::initializer_list<ShaderInput> &input, const char *shadersrc, const char *name) = 0;
        
        // Create texture from binary data
        // @imgMipsBinaryData - array of pointers. Each [i] pointer represents binary data for i'th mip and cannot be nullptr
        //
        virtual Texture2D createTexture(Texture2D::Format format, std::uint32_t width, std::uint32_t height, std::uint32_t mipCount, const std::uint8_t *const *imgMipsBinaryData = nullptr) = 0;
        
        // Create geometry
        // @data        - pointer to data (array of structures)
        // @count       - count of structures in array
        // @stride      - size of struture
        // @return      - handle
        //
        virtual Geometry createGeometry(const void *data, std::uint32_t count, std::uint32_t stride) = 0;
        
        // Apply shader
        // @shader      - shader handle
        // @constants   - pointers to data for 'const' blocks. constData[i] can be nullptr (constants will not be updated, but set)
        //
        virtual void applyShader(const Shader &shader, const std::initializer_list<const void *> &constants = {}) = 0;
        
        // Update data of shader constants. Shader will not be set.
        // @shader      - shader handle
        // @constants   - pointers to data for 'const' blocks. constData[i] can be nullptr (constants will not be updated, but set)
        //
        virtual void applyShaderConstants(const Shader &shader, const std::initializer_list<const void *> &constants = {}) = 0;
        
        // Apply textures. textures[i] can be nullptr (texture will not be set)
        //
        virtual void applyTextures(const std::initializer_list<const Texture2D *> &textures) = 0;
        
        // Draw geometry
        // @geometry - geometry, can be nullptr
        //
        virtual void drawGeometry(const Geometry *geometry, std::uint32_t vertexCount, std::uint32_t instanceCount, Geometry::Topology topology = Geometry::Topology::TRIANGLES) = 0;
        
        virtual void prepareFrame() = 0;
        virtual void presentFrame(float dt) = 0;
        
        virtual void getLastFrameRendered() = 0;
        
    private:
        RenderingDeviceInterface(RenderingDeviceInterface &&) = delete;
        RenderingDeviceInterface(const RenderingDeviceInterface &) = delete;
        RenderingDeviceInterface &operator =(RenderingDeviceInterface &&) = delete;
        RenderingDeviceInterface &operator =(const RenderingDeviceInterface &) = delete;
    };
    
    std::shared_ptr<PlatformInterface> getPlatformInstance();
    std::shared_ptr<AudioDeviceInterface> getAudioDeviceInstance(const std::shared_ptr<PlatformInterface> &platform);
    std::shared_ptr<RenderingDeviceInterface> getRenderingDeviceInstance(const std::shared_ptr<PlatformInterface> &platform);
}

