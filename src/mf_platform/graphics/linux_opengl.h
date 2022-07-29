#define GL_GLEXT_PROTOTYPES
#include <GL/glx.h>
#include <GL/glext.h>

typedef GLXContext (*glXCreateContextAttribsARBProc)(Display*, GLXFBConfig, GLXContext, int, const int*);

void opengl_end(void *userdata) {
    mf::platform::Platform *p = (mf::platform::Platform *) userdata;
    glXSwapBuffers(p->os.display, p->os.x11_window);
}

void opengl_init(mf::platform::Platform &p) {
    static int visualAttribs[] = {
      GLX_X_RENDERABLE    , True,
      GLX_DRAWABLE_TYPE   , GLX_WINDOW_BIT,
      GLX_RENDER_TYPE     , GLX_RGBA_BIT,
      GLX_X_VISUAL_TYPE   , GLX_TRUE_COLOR,
      GLX_RED_SIZE        , 8,
      GLX_GREEN_SIZE      , 8,
      GLX_BLUE_SIZE       , 8,
      GLX_ALPHA_SIZE      , 8,
      GLX_DEPTH_SIZE      , 24,
      GLX_STENCIL_SIZE    , 8,
      GLX_DOUBLEBUFFER    , True,
      //GLX_SAMPLE_BUFFERS  , 1,
      //GLX_SAMPLES         , 4,
      None
    };
    i32 fbCount = 0;
    GLXFBConfig *fbc = glXChooseFBConfig(p.os.display,
                                         p.os.screen,
                                         visualAttribs,
                                         &fbCount);

    MF_Assert(fbc);
    i32 fbIndex = -1;
    //i32 bestSampleBuffers = 0;
    i32 bestSamples = 0;

    for (i32 i = 0; i < fbCount; ++i)
    {
        XVisualInfo *vi = glXGetVisualFromFBConfig(p.os.display, fbc[i]);
        i32 sampleBuffers = 0;
        i32 samples = 0;
        glXGetFBConfigAttrib(p.os.display, fbc[i], GLX_SAMPLE_BUFFERS, &sampleBuffers);
        glXGetFBConfigAttrib(p.os.display, fbc[i], GLX_SAMPLES, &samples);
        if (sampleBuffers && samples > bestSamples)
        {
            fbIndex = i;
            bestSamples = samples;
        }
        XFree(vi);
    }
    MF_Assert(fbIndex >= 0);
    
    int attribs[] = {
        GLX_CONTEXT_MAJOR_VERSION_ARB, 3,
        GLX_CONTEXT_MINOR_VERSION_ARB, 0,
        None
    };
    
    p.os.vi = glXGetVisualFromFBConfig(p.os.display, fbc[fbIndex]);
    p.os.visual = p.os.vi->visual;
    p.os.colormap = XCreateColormap(p.os.display,
                                    p.os.root,
                                    p.os.vi->visual,
                                    AllocNone);

    p.os.graphicHandle = malloc(sizeof(GLXContext));


    glXCreateContextAttribsARBProc createContext = (glXCreateContextAttribsARBProc)glXGetProcAddress((const GLubyte*)"glXCreateContextAttribsARB");
    MF_Assert(createContext);
    //*((GLXContext *) x11->graphicHandle) = glXCreateContext(x11->display, x11->vi, 0, GL_TRUE);

    //*((GLXContext *) x11->graphicHandle) = glXCreateContextAttribsARBProc(x11->display, fbc[fbIndex], 0, True, attribs);
    GLXContext context = createContext(p.os.display, fbc[fbIndex], 0, True, attribs);
    //GLXContext glxContext = glXCreateContext(x11->display, x11->vi, NULL, GL_TRUE);
    bool success = glXMakeCurrent(p.os.display, p.os.x11_window, context);
    MF_Assert(success);

    *((GLXContext *) p.os.graphicHandle) = context;

    i32 major = 0;
    i32 minor = 0;
    glXQueryVersion(p.os.display, &major, &minor);

    p.custom_end = opengl_end;
}
