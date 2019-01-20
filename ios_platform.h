#pragma once

namespace platform {
    class IOSPlatform : public Platform {
    public:
        IOSPlatform();
        ~IOSPlatform();

        std::vector<std::string> formFileList(const char *dirPath);
        bool loadFile(const char *filePath, std::unique_ptr<unsigned char []> &data, std::size_t &size);

        float getNativeScreenWidth() const;
        float getNativeScreenHeight() const;

        void *setNativeRenderingContext(void *context);

        void showCursor();
        void hideCursor();
        void showKeyboard();
        void hideKeyboard();
        
        EventHandlersToken addKeyboardEventHandlers(
            std::function<void(const KeyboardEventArgs &)> &&down,
            std::function<void(const KeyboardEventArgs &)> &&up
        );

        EventHandlersToken addInputEventHandlers(
            std::function<void(const char (&utf8char)[4])> &&input,
            std::function<void()> &&backspace
        );

        EventHandlersToken addMouseEventHandlers(
            std::function<void(const MouseEventArgs &)> &&press,
            std::function<void(const MouseEventArgs &)> &&move,
            std::function<void(const MouseEventArgs &)> &&release
        );

        EventHandlersToken addTouchEventHandlers(
            std::function<void(const TouchEventArgs &)> &&start,
            std::function<void(const TouchEventArgs &)> &&move,
            std::function<void(const TouchEventArgs &)> &&release
        );

        EventHandlersToken addGamepadEventHandlers(
            std::function<void(const GamepadEventArgs &)> &&buttonPress,
            std::function<void(const GamepadEventArgs &)> &&buttonRelease
        );

        void run(std::function<void(float)> &&updateAndDraw);
        void removeEventHandlers(EventHandlersToken token);
        void exit();
        
    public:
        std::function<void(float)> updateAndDrawHandler;
    
    private:
        float _nativeScreenWidth;
        float _nativeScreenHeight;
    };
    
    std::vector<std::string> Platform::formFileList(const char *dirPath) {
        return static_cast<IOSPlatform *>(this)->formFileList(dirPath);
    }

    bool Platform::loadFile(const char *filePath, std::unique_ptr<unsigned char []> &data, std::size_t &size) {
        return static_cast<IOSPlatform *>(this)->loadFile(filePath, data, size);
    }

    float Platform::getNativeScreenWidth() const {
        return static_cast<const IOSPlatform *>(this)->getNativeScreenWidth();
    }
    
    float Platform::getNativeScreenHeight() const {
        return static_cast<const IOSPlatform *>(this)->getNativeScreenHeight();
    }

    void *Platform::setNativeRenderingContext(void *context) {
        return static_cast<IOSPlatform *>(this)->setNativeRenderingContext(context);
    }

    void Platform::showCursor() {
        static_cast<IOSPlatform *>(this)->showCursor();
    }
    
    void Platform::hideCursor() {
        static_cast<IOSPlatform *>(this)->hideCursor();
    }
    
    void Platform::showKeyboard() {
        static_cast<IOSPlatform *>(this)->showKeyboard();
    }
    
    void Platform::hideKeyboard() {
        static_cast<IOSPlatform *>(this)->hideKeyboard();
    }

    EventHandlersToken Platform::addKeyboardEventHandlers(
        std::function<void(const KeyboardEventArgs &)> &&down,
        std::function<void(const KeyboardEventArgs &)> &&up
    )
    {
        return static_cast<IOSPlatform *>(this)->addKeyboardEventHandlers(std::move(down), std::move(up));
    }
    
    EventHandlersToken Platform::addInputEventHandlers(
        std::function<void(const char (&utf8char)[4])> &&input,
        std::function<void()> &&backspace
    )
    {
        return static_cast<IOSPlatform *>(this)->addInputEventHandlers(std::move(input), std::move(backspace));
    }

    EventHandlersToken Platform::addMouseEventHandlers(
        std::function<void(const MouseEventArgs &)> &&press,
        std::function<void(const MouseEventArgs &)> &&move,
        std::function<void(const MouseEventArgs &)> &&release
    )
    {
        return static_cast<IOSPlatform *>(this)->addMouseEventHandlers(std::move(press), std::move(move), std::move(release));
    }

    EventHandlersToken Platform::addTouchEventHandlers(
        std::function<void(const TouchEventArgs &)> &&start,
        std::function<void(const TouchEventArgs &)> &&move,
        std::function<void(const TouchEventArgs &)> &&release
    )
    {
        return static_cast<IOSPlatform *>(this)->addTouchEventHandlers(std::move(start), std::move(move), std::move(release));
    }

    EventHandlersToken Platform::addGamepadEventHandlers(
        std::function<void(const GamepadEventArgs &)> &&buttonPress,
        std::function<void(const GamepadEventArgs &)> &&buttonRelease
    )
    {
        return static_cast<IOSPlatform *>(this)->addGamepadEventHandlers(std::move(buttonPress), std::move(buttonRelease));
    }

    void Platform::run(std::function<void(float)> &&updateAndDraw) {
        static_cast<IOSPlatform *>(this)->run(std::move(updateAndDraw));
    }
    
    void Platform::removeEventHandlers(EventHandlersToken token) {
        static_cast<IOSPlatform *>(this)->removeEventHandlers(token);
    }
    
    void Platform::exit() {
        static_cast<IOSPlatform *>(this)->exit();
    }
}
