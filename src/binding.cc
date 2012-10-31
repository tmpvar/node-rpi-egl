#include <node.h>
#include <v8.h>

#include <GLES2/gl2.h>
#include <EGL/egl.h>
#include <bcm_host.h>

using namespace v8;

EGLContext context;
EGLDisplay display;
EGLSurface surface;
EGLNativeWindowType window;

int egl_createContext() {

  EGLint numConfigs;
  EGLint majorVersion;
  EGLint minorVersion;
  EGLConfig config;
  EGLint contextAttribs[] = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE, EGL_NONE };
  EGLint attribList[] =
  {
       EGL_RED_SIZE,       5,
       EGL_GREEN_SIZE,     6,
       EGL_BLUE_SIZE,      5,
       EGL_ALPHA_SIZE,     EGL_DONT_CARE,
       EGL_DEPTH_SIZE,     EGL_DONT_CARE,
       EGL_STENCIL_SIZE,   EGL_DONT_CARE,
       EGL_SAMPLE_BUFFERS, 1,
       EGL_NONE
  };

 
  display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
  if (display == EGL_NO_DISPLAY) {
    return 0;
  }

  // Initialize EGL
  if (!eglInitialize(display, &majorVersion, &minorVersion)) {
    return 0;
  }

  // Get configs
  if (!eglGetConfigs(display, NULL, 0, &numConfigs)) {
    return 0;
  }

  // Choose config
  if (!eglChooseConfig(display, attribList, &config, 1, &numConfigs)) {
    return 0;
  }

  // Create a surface
  surface = eglCreateWindowSurface(display, config, window, NULL);
  if ( surface == EGL_NO_SURFACE ) {
    return 0;
  }

  // Create a GL context
  context = eglCreateContext(display, config, EGL_NO_CONTEXT, contextAttribs );
  if ( context == EGL_NO_CONTEXT ) {
    return 0;
  }   
   
  // Make the context current
  if ( !eglMakeCurrent(display, surface, surface, context) ) {
    return 0;
  }

  return 1;
}

Handle<Value> CreateEGLWindow(const Arguments& args) {
  HandleScope scope;
  
  static EGL_DISPMANX_WINDOW_T nativewindow;

  // Create a surface that fills the entire screen
  uint32_t width, height;
  if (graphics_get_display_size(0, &width, &height) < 0) {
    ThrowException(Exception::TypeError(String::New("Could not get display size")));
    return scope.Close(Undefined());
  }

  VC_RECT_T src, dest;
  dest.x = dest.y = src.x = src.y = 0;
  src.width = dest.width = width;
  src.width = dest.height = height;

  DISPMANX_DISPLAY_HANDLE_T dispman_display = vc_dispmanx_display_open(0);
  DISPMANX_UPDATE_HANDLE_T dispman_update = vc_dispmanx_update_start(0);
         
  DISPMANX_ELEMENT_HANDLE_T dispman_element = vc_dispmanx_element_add(
    dispman_update,           // update
    dispman_display,          // display
    0,                        // layer
    &dest,                    // destination rect
    0,                        // resource handle
    &src,                     // source rect  
    DISPMANX_PROTECTION_NONE, // protection
    0,                        // alpha
    0,                        // clamp
    (DISPMANX_TRANSFORM_T)0   // transform
  );
      
  nativewindow.element = dispman_element;
  nativewindow.width = width;
  nativewindow.height = height;
  vc_dispmanx_update_submit_sync( dispman_update );
  
  window = &nativewindow;
  return scope.Close(Undefined());
}

Handle<Value> CreateEGLContext(const Arguments& args) {
  HandleScope scope;

  egl_createContext();
  
  return scope.Close(Undefined());
}

//
// Display the backbuffer
//
Handle<Value> SwapEGLBuffers(const Arguments& args) {
  HandleScope scope;

  eglSwapBuffers(display, surface);

  return scope.Close(Undefined());
}

void init(Handle<Object> target) {
  NODE_SET_METHOD(target, "createContext", CreateEGLContext);
  NODE_SET_METHOD(target, "createWindow", CreateEGLWindow);
  NODE_SET_METHOD(target, "swapBuffers", SwapEGLBuffers);

  bcm_host_init();
}

NODE_MODULE(binding, init);

