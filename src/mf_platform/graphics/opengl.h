#pragma once

void opengl_init(mf::platform::Platform &p);

#if defined(MF_OS_WINDOWS)
#include <mf_platform/graphics/windows_opengl.h>
#else
#error MF_OS_NOT_SUPPORTED
#endif

