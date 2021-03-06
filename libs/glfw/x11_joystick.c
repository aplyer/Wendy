//========================================================================
// GLFW - An OpenGL library
// Platform:    X11
// API version: 3.0
// WWW:         http://www.glfw.org/
//------------------------------------------------------------------------
// Copyright (c) 2002-2006 Marcus Geelnard
// Copyright (c) 2006-2010 Camilla Berglund <elmindreda@elmindreda.org>
//
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would
//    be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such, and must not
//    be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source
//    distribution.
//
//========================================================================

#include "internal.h"

#ifdef _GLFW_USE_LINUX_JOYSTICKS
#include <linux/joystick.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <regex.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#endif // _GLFW_USE_LINUX_JOYSTICKS


//========================================================================
// Attempt to open the specified joystick device
//========================================================================

static int openJoystickDevice(int joy, const char* path)
{
#ifdef _GLFW_USE_LINUX_JOYSTICKS
    char numAxes, numButtons;
    int fd, version;

    fd = open(path, O_RDONLY | O_NONBLOCK);
    if (fd == -1)
        return GL_FALSE;

    _glfwLibrary.X11.joystick[joy].fd = fd;

    // Verify that the joystick driver version is at least 1.0
    ioctl(fd, JSIOCGVERSION, &version);
    if (version < 0x010000)
    {
        // It's an old 0.x interface (we don't support it)
        close(fd);
        return GL_FALSE;
    }

    ioctl(fd, JSIOCGAXES, &numAxes);
    _glfwLibrary.X11.joystick[joy].numAxes = (int) numAxes;

    ioctl(fd, JSIOCGBUTTONS, &numButtons);
    _glfwLibrary.X11.joystick[joy].numButtons = (int) numButtons;

    _glfwLibrary.X11.joystick[joy].axis =
        (float*) malloc(sizeof(float) * numAxes);
    if (_glfwLibrary.X11.joystick[joy].axis == NULL)
    {
        close(fd);

        _glfwSetError(GLFW_OUT_OF_MEMORY, NULL);
        return GL_FALSE;
    }

    _glfwLibrary.X11.joystick[joy].button =
        (unsigned char*) malloc(sizeof(char) * numButtons);
    if (_glfwLibrary.X11.joystick[joy].button == NULL)
    {
        free(_glfwLibrary.X11.joystick[joy].axis);
        close(fd);

        _glfwSetError(GLFW_OUT_OF_MEMORY, NULL);
        return GL_FALSE;
    }

    _glfwLibrary.X11.joystick[joy].present = GL_TRUE;
#endif // _GLFW_USE_LINUX_JOYSTICKS

    return GL_TRUE;
}


//========================================================================
// Polls for and processes events for all present joysticks
//========================================================================

static void pollJoystickEvents(void)
{
#ifdef _GLFW_USE_LINUX_JOYSTICKS
    int i;
    ssize_t result;
    struct js_event e;

    for (i = 0;  i <= GLFW_JOYSTICK_LAST;  i++)
    {
        if (!_glfwLibrary.X11.joystick[i].present)
            continue;

        // Read all queued events (non-blocking)
        for (;;)
        {
            errno = 0;
            result = read(_glfwLibrary.X11.joystick[i].fd, &e, sizeof(e));

            if (errno == ENODEV)
                _glfwLibrary.X11.joystick[i].present = GL_FALSE;

            if (result == -1)
                break;

            // We don't care if it's an init event or not
            e.type &= ~JS_EVENT_INIT;

            switch (e.type)
            {
                case JS_EVENT_AXIS:
                    _glfwLibrary.X11.joystick[i].axis[e.number] =
                        (float) e.value / 32767.0f;

                    // We need to change the sign for the Y axes, so that
                    // positive = up/forward, according to the GLFW spec.
                    if (e.number & 1)
                    {
                        _glfwLibrary.X11.joystick[i].axis[e.number] =
                            -_glfwLibrary.X11.joystick[i].axis[e.number];
                    }

                    break;

                case JS_EVENT_BUTTON:
                    _glfwLibrary.X11.joystick[i].button[e.number] =
                        e.value ? GLFW_PRESS : GLFW_RELEASE;
                    break;

                default:
                    break;
            }
        }
    }
#endif // _GLFW_USE_LINUX_JOYSTICKS
}


//////////////////////////////////////////////////////////////////////////
//////                       GLFW internal API                      //////
//////////////////////////////////////////////////////////////////////////

//========================================================================
// Initialize joystick interface
//========================================================================

int _glfwInitJoysticks(void)
{
#ifdef _GLFW_USE_LINUX_JOYSTICKS
    int i, joy = 0;
    regex_t regex;
    DIR* dir;
    const char* dirs[] =
    {
        "/dev/input",
        "/dev"
    };

    if (regcomp(&regex, "^js[0-9]\\+$", 0) != 0)
    {
        _glfwSetError(GLFW_PLATFORM_ERROR, "X11: Failed to compile regex");
        return GL_FALSE;
    }

    for (i = 0;  i < sizeof(dirs) / sizeof(dirs[0]);  i++)
    {
        struct dirent* entry;

        dir = opendir(dirs[i]);
        if (!dir)
            continue;

        while ((entry = readdir(dir)))
        {
            char path[20];
            regmatch_t match;

            if (regexec(&regex, entry->d_name, 1, &match, 0) != 0)
                continue;

            snprintf(path, sizeof(path), "%s/%s", dirs[i], entry->d_name);
            if (openJoystickDevice(joy, path))
                joy++;
        }

        closedir(dir);
    }

    regfree(&regex);
#endif // _GLFW_USE_LINUX_JOYSTICKS

    return GL_TRUE;
}


//========================================================================
// Close all opened joystick handles
//========================================================================

void _glfwTerminateJoysticks(void)
{
#ifdef _GLFW_USE_LINUX_JOYSTICKS
    int i;

    for (i = 0;  i <= GLFW_JOYSTICK_LAST;  i++)
    {
        if (_glfwLibrary.X11.joystick[i].present)
        {
            close(_glfwLibrary.X11.joystick[i].fd);
            free(_glfwLibrary.X11.joystick[i].axis);
            free(_glfwLibrary.X11.joystick[i].button);

            _glfwLibrary.X11.joystick[i].present = GL_FALSE;
        }
    }
#endif // _GLFW_USE_LINUX_JOYSTICKS
}


//////////////////////////////////////////////////////////////////////////
//////                       GLFW platform API                      //////
//////////////////////////////////////////////////////////////////////////

//========================================================================
// Determine joystick capabilities
//========================================================================

int _glfwPlatformGetJoystickParam(int joy, int param)
{
    pollJoystickEvents();

    if (!_glfwLibrary.X11.joystick[joy].present)
        return 0;

    switch (param)
    {
        case GLFW_PRESENT:
            return GL_TRUE;

        case GLFW_AXES:
            return _glfwLibrary.X11.joystick[joy].numAxes;

        case GLFW_BUTTONS:
            return _glfwLibrary.X11.joystick[joy].numButtons;

        default:
            _glfwSetError(GLFW_INVALID_ENUM, NULL);
    }

    return 0;
}


//========================================================================
// Get joystick axis positions
//========================================================================

int _glfwPlatformGetJoystickAxes(int joy, float* axes, int numAxes)
{
    int i;

    pollJoystickEvents();

    if (!_glfwLibrary.X11.joystick[joy].present)
        return 0;

    if (_glfwLibrary.X11.joystick[joy].numAxes < numAxes)
        numAxes = _glfwLibrary.X11.joystick[joy].numAxes;

    for (i = 0;  i < numAxes;  i++)
        axes[i] = _glfwLibrary.X11.joystick[joy].axis[i];

    return numAxes;
}


//========================================================================
// Get joystick button states
//========================================================================

int _glfwPlatformGetJoystickButtons(int joy, unsigned char* buttons,
                                    int numButtons)
{
    int i;

    pollJoystickEvents();

    if (!_glfwLibrary.X11.joystick[joy].present)
        return 0;

    if (_glfwLibrary.X11.joystick[joy].numButtons < numButtons)
        numButtons = _glfwLibrary.X11.joystick[joy].numButtons;

    for (i = 0;  i < numButtons;  i++)
        buttons[i] = _glfwLibrary.X11.joystick[joy].button[i];

    return numButtons;
}

