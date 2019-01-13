
#include "interfaces.h"
#include "ios_platform.h"

#include <mutex>
#include <chrono>
#include <fstream>
#include <unordered_map>
#include <pthread.h>

#import <UIKit/UIKit.h>
#import <GLKit/GLKit.h>

namespace {
    struct KeyboardEventHandler {
        std::function<void(const platform::KeyboardEventArgs &)> down;
        std::function<void(const platform::KeyboardEventArgs &)> up;
    };

    struct InputEventHandler {
        std::function<void(const char (&utf8char)[4])> input;
        std::function<void()> backspace;
    };

    struct MouseEventHandler {
        std::function<void(const platform::MouseEventArgs &)> press;
        std::function<void(const platform::MouseEventArgs &)> move;
        std::function<void(const platform::MouseEventArgs &)> release;
    };

    struct TouchEventHandler {
        std::function<void(const platform::TouchEventArgs &)> start;
        std::function<void(const platform::TouchEventArgs &)> move;
        std::function<void(const platform::TouchEventArgs &)> release;
    };

    struct GamepadEventHandler {
        std::function<void(const platform::GamepadEventArgs &)> buttonPress;
        std::function<void(const platform::GamepadEventArgs &)> buttonRelease;
    };
    
    platform::EventHandlersToken _lastTokenGen = reinterpret_cast<platform::EventHandlersToken>(0x1);
    
    std::unordered_map<platform::EventHandlersToken, KeyboardEventHandler> _keyboardEventHandlers;
    std::unordered_map<platform::EventHandlersToken, InputEventHandler> _inputEventHandlers;
    std::unordered_map<platform::EventHandlersToken, MouseEventHandler> _mouseEventHandlers;
    std::unordered_map<platform::EventHandlersToken, TouchEventHandler> _touchEventHandlers;
    std::unordered_map<platform::EventHandlersToken, GamepadEventHandler> _gamepadEventHandlers;
    
    std::shared_ptr<platform::IOSPlatform> _platform;
    void *_glContext;
    
    void writeLog(const char *type, const char *fmt, va_list args) {
        static std::mutex _logGuard;
        std::lock_guard<std::mutex> guard(_logGuard);
        
        char buffer[32];
        std::time_t t = std::time(nullptr);
        std::strftime(buffer, sizeof(buffer), "%T", std::gmtime(&t));
        
        std::printf("[%s %s] ", buffer, type);
        std::vprintf(fmt, args);
        std::printf("\n");
    }
}

//--------------------------------------------------------------------------------------------------------------------------------
// Creepy IOS Stuff

@interface AppDelegate : UIResponder <UIApplicationDelegate>
@property (strong, nonatomic) UIWindow *window;
@end

@interface RootViewController : GLKViewController<UIKeyInput>
@property BOOL inputEnabled;
@end

RootViewController *controller; // global

@implementation AppDelegate
- (instancetype)init {
    if (self = [super init]) {
        self.window = [[UIWindow alloc] init];
    }
    
    return self;
}
- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    if (application.applicationState != UIApplicationStateBackground) {
        controller = [[RootViewController alloc] init];
        self.window.rootViewController = controller;
        self.window.backgroundColor = [UIColor groupTableViewBackgroundColor];
        [self.window makeKeyAndVisible];
    }
    return YES;
}
@end

@implementation RootViewController
{
    std::chrono::time_point<std::chrono::high_resolution_clock> _lastTime;
}

- (void)viewDidLoad {
    [super viewDidLoad];
    
    GLKView *view = (GLKView *)self.view;

    if (_glContext) {
        view.context = (__bridge_transfer EAGLContext *)(_glContext);
    }

    view.drawableDepthFormat = GLKViewDrawableDepthFormat24;
    view.drawableColorFormat = GLKViewDrawableColorFormatRGBA8888;
    view.drawableStencilFormat = GLKViewDrawableStencilFormatNone;
    view.clearsContextBeforeDrawing = NO;
    view.enableSetNeedsDisplay = NO;
    view.multipleTouchEnabled = YES;
    view.transform = CGAffineTransformMakeRotation(M_PI_2);
    
    self.inputEnabled = NO;
    self.preferredFramesPerSecond = 60;
    
    _lastTime = std::chrono::high_resolution_clock::now();
}
- (BOOL)hasText { return NO; }
- (BOOL)canBecomeFirstResponder { return self.inputEnabled; }
- (void)insertText:(NSString *)text {
    
}
- (void)deleteBackward {
    
}
- (BOOL)shouldAutorotate { return NO; }
- (BOOL)prefersStatusBarHidden { return YES; }
- (void)update {
    auto now = std::chrono::high_resolution_clock::now();
    auto dms = double(std::chrono::duration_cast<std::chrono::microseconds>(now - _lastTime).count()) / 1000.0f;

    if (_platform != nullptr && _platform->updateHandler) {
        _platform->updateHandler(dms);
    }
    
    _lastTime = now;
}

- (void)glkView:(GLKView *)view drawInRect:(CGRect)rect {
    if (_platform != nullptr && _platform->drawHandler) {
        _platform->drawHandler();
    }
}

- (void)touchesBegan:(NSSet<UITouch *> *)touches withEvent:(nullable UIEvent *)event {
    for (UITouch *item in touches) {
        platform::TouchEventArgs args;
        args.touchID = reinterpret_cast<std::size_t>(item);
        args.coordinateX = [item locationInView:nil].y;
        args.coordinateY = [item locationInView:nil].x;

        for (auto &index : _touchEventHandlers) {
            if (index.second.start) {
                index.second.start(args);
            }
        }
    }
}

- (void)touchesMoved:(NSSet<UITouch *> *)touches withEvent:(nullable UIEvent *)event {
    for (UITouch *item in touches) {
        platform::TouchEventArgs args;
        args.touchID = reinterpret_cast<std::size_t>(item);
        args.coordinateX = [item locationInView:nil].y;
        args.coordinateY = [item locationInView:nil].x;

        for (auto &index : _touchEventHandlers) {
            if (index.second.move) {
                index.second.move(args);
            }
        }
    }
}

- (void)touchesEnded:(NSSet<UITouch *> *)touches withEvent:(nullable UIEvent *)event {
    for (UITouch *item in touches) {
        platform::TouchEventArgs args;
        args.touchID = reinterpret_cast<std::size_t>(item);
        args.coordinateX = [item locationInView:nil].y;
        args.coordinateY = [item locationInView:nil].x;

        for (auto &index : _touchEventHandlers) {
            if (index.second.release) {
                index.second.release(args);
            }
        }
    }
}

- (void)touchesCancelled:(NSSet<UITouch *> *)touches withEvent:(nullable UIEvent *)event {
    for (UITouch *item in touches) {
        platform::TouchEventArgs args;
        args.touchID = reinterpret_cast<std::size_t>(item);
        args.coordinateX = [item locationInView:nil].y;
        args.coordinateY = [item locationInView:nil].x;

        for (auto &index : _touchEventHandlers) {
            if (index.second.release) {
                index.second.release(args);
            }
        }
    }
}

@end

//--------------------------------------------------------------------------------------------------------------------------------

namespace platform {
    IOSPlatform::IOSPlatform() {
        CGRect bounds = [[UIScreen mainScreen] bounds];
        double scale = [[UIScreen mainScreen] scale];
        
        _nativeScreenWidth = float(bounds.size.height * scale);
        _nativeScreenHeight = float(bounds.size.width * scale);
        
        logInfo("[Platform] Platform: OK");
    }
    IOSPlatform::~IOSPlatform() {}
    
    void IOSPlatform::logInfo(const char *fmt, ...) {
        va_list args;
        va_start(args, fmt);
        writeLog("Info", fmt, args);
        va_end(args);
    }
    
    void IOSPlatform::logWarning(const char *fmt, ...) {
        va_list args;
        va_start(args, fmt);
        writeLog("Warning", fmt, args);
        va_end(args);
    }
    
    void IOSPlatform::logError(const char *fmt, ...) {
        va_list args;
        va_start(args, fmt);
        writeLog("Error", fmt, args);
        va_end(args);
    }
    
    std::vector<std::string> IOSPlatform::formFileList(const char *dirPath) {
        std::vector<std::string> result;
        
        @autoreleasepool {
            NSString *basedir = [[[NSBundle mainBundle] resourcePath] stringByAppendingPathComponent:@(dirPath)];
            NSArray  *list = [[NSFileManager defaultManager] contentsOfDirectoryAtPath:basedir error:nil];
    
            auto &resultref = result;
            
            [list enumerateObjectsUsingBlock:^(NSString *entry, NSUInteger idx, BOOL *stop) {
                resultref.emplace_back(std::string([entry fileSystemRepresentation], [entry length]));
            }];
        }
    
        return result;
    }
    
    bool IOSPlatform::loadFile(const char *filePath, std::unique_ptr<uint8_t[]> &data, std::size_t &size) {
        std::string fullPath;
        
        @autoreleasepool {
            fullPath = [[[NSBundle mainBundle] resourcePath] cStringUsingEncoding:NSUTF8StringEncoding];
            fullPath += "/";
            fullPath += filePath;
        }
        
        std::ifstream stream (fullPath, std::ios::binary | std::ios::ate);

        if (stream.is_open()) {
            stream.clear();
            size = stream.tellg();
            stream.seekg(0);
            data = std::make_unique<uint8_t[]>(size);
            stream.read(reinterpret_cast<char *>(data.get()), size);
            return stream.gcount() == size;
        }
        else {
            logError("[Platform] File %s is not found", filePath);
        }
        
        return false;
    }
    
    float IOSPlatform::getNativeScreenWidth() const {
        return _nativeScreenWidth;
    }
    
    float IOSPlatform::getNativeScreenHeight() const {
        return _nativeScreenHeight;
    }
    
    void *IOSPlatform::setNativeRenderingContext(void *context) {
        _glContext = context;
        return nullptr;
    }
    
    void IOSPlatform::showCursor() {}
    void IOSPlatform::hideCursor() {}
    
    void IOSPlatform::showKeyboard() {
        @autoreleasepool {
            controller.inputEnabled = YES;
            [controller becomeFirstResponder];
        }
    }
    
    void IOSPlatform::hideKeyboard() {
        @autoreleasepool {
            [controller resignFirstResponder];
            controller.inputEnabled = NO;
        }
    }
    
    EventHandlersToken IOSPlatform::addKeyboardEventHandlers(
        std::function<void(const KeyboardEventArgs &)> &&down,
        std::function<void(const KeyboardEventArgs &)> &&up
    ) {
        _keyboardEventHandlers.emplace(++_lastTokenGen, KeyboardEventHandler{ std::move(down), std::move(up) });
        return _lastTokenGen;
    }
    
    EventHandlersToken IOSPlatform::addInputEventHandlers(
        std::function<void(const char (&utf8char)[4])> &&input,
        std::function<void()> &&backspace
    ) {
        _inputEventHandlers.emplace(++_lastTokenGen, InputEventHandler{ std::move(input), std::move(backspace) });
        return _lastTokenGen;
    }
    
    EventHandlersToken IOSPlatform::addMouseEventHandlers(
        std::function<void(const MouseEventArgs &)> &&press,
        std::function<void(const MouseEventArgs &)> &&move,
        std::function<void(const MouseEventArgs &)> &&release
    ) {
        _mouseEventHandlers.emplace(++_lastTokenGen, MouseEventHandler{ std::move(press), std::move(move), std::move(release) });
        return _lastTokenGen;
    }
    
    EventHandlersToken IOSPlatform::addTouchEventHandlers(
        std::function<void(const TouchEventArgs &)> &&start,
        std::function<void(const TouchEventArgs &)> &&move,
        std::function<void(const TouchEventArgs &)> &&release
    ) {
        _touchEventHandlers.emplace(++_lastTokenGen, TouchEventHandler{ std::move(start), std::move(move), std::move(release) });
        return _lastTokenGen;
    }
    
    EventHandlersToken IOSPlatform::addGamepadEventHandlers(
        std::function<void(const GamepadEventArgs &)> &&buttonPress,
        std::function<void(const GamepadEventArgs &)> &&buttonRelease
    ) {
        _gamepadEventHandlers.emplace(++_lastTokenGen, GamepadEventHandler{ std::move(buttonPress), std::move(buttonRelease) });
        return _lastTokenGen;
    }
    
    void IOSPlatform::removeEventHandlers(EventHandlersToken token) {}
    void IOSPlatform::run(
        std::function<void(float)> &&update,
        std::function<void()> &&draw
    ) {
        updateHandler = update;
        drawHandler = draw;
     
        @autoreleasepool {
            char *argv[] = {0};
            UIApplicationMain(0, argv, nil, NSStringFromClass([AppDelegate class]));
        }
    }
    
    void IOSPlatform::exit() {}
    
    std::shared_ptr<PlatformInterface> getPlatformInstance() {
        if (_platform == nullptr) {
            _platform = std::make_shared<platform::IOSPlatform>();
        }
        
        return _platform;
    }
}
