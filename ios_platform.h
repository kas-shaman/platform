#pragma once

namespace platform {
    class IOSPlatform : public PlatformInterface {
    public:
        IOSPlatform();
        ~IOSPlatform() override;

        void logInfo(const char *fmt, ...) override;
        void logWarning(const char *fmt, ...) override;
        void logError(const char *fmt, ...) override;

        std::vector<std::string> formFileList(const char *dirPath) override;
        
        bool loadFile(const char *filePath, std::unique_ptr<unsigned char []> &data, std::size_t &size) override;

        float getNativeScreenWidth() const override;
        float getNativeScreenHeight() const override;

        void *setNativeRenderingContext(void *context) override;

        void showCursor() override;
        void hideCursor() override;
        
        void showKeyboard() override;
        void hideKeyboard() override;
        
        EventHandlersToken addKeyboardEventHandlers(
            std::function<void(const KeyboardEventArgs &)> &&down,
            std::function<void(const KeyboardEventArgs &)> &&up
        ) override;

        EventHandlersToken addInputEventHandlers(
            std::function<void(const char (&utf8char)[4])> &&input,
            std::function<void()> &&backspace
        ) override;

        EventHandlersToken addMouseEventHandlers(
            std::function<void(const MouseEventArgs &)> &&press,
            std::function<void(const MouseEventArgs &)> &&move,
            std::function<void(const MouseEventArgs &)> &&release
        ) override;

        EventHandlersToken addTouchEventHandlers(
            std::function<void(const TouchEventArgs &)> &&start,
            std::function<void(const TouchEventArgs &)> &&move,
            std::function<void(const TouchEventArgs &)> &&release
        ) override;

        EventHandlersToken addGamepadEventHandlers(
            std::function<void(const GamepadEventArgs &)> &&buttonPress,
            std::function<void(const GamepadEventArgs &)> &&buttonRelease
        ) override;

        void run(
            std::function<void(float)> &&updateAndDraw
        ) override;
        
        void removeEventHandlers(EventHandlersToken token) override;
        void exit() override;
        
    public:
        std::function<void(float)> updateAndDrawHandler;
    
    private:
        float _nativeScreenWidth;
        float _nativeScreenHeight;
    };
    
    std::shared_ptr<PlatformInterface> getPlatformInstance();
}
