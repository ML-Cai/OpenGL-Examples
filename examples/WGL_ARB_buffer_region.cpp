//========================================================================
// OpenGL triangle example
// Copyright (c) Camilla Löwy <elmindreda@glfw.org>
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
//! [code]

#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>
#define GLFW_INCLUDE_NONE
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include <GL/gl.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <Windows.h>
#include <iostream>
#include <vector>
#pragma comment(lib, "opengl32.lib")

// Function pointer for WGL_ARB_buffer_region
#define WGL_BACK_COLOR_BUFFER_BIT_ARB           0x00000002

typedef HANDLE(WINAPI* PFNWGLCREATEBUFFERREGIONARBPROC)(HDC hDC, int iLayerPlane, UINT uType);
typedef VOID(WINAPI* PFNWGLDELETEBUFFERREGIONARBPROC)(HANDLE hRegion);
typedef BOOL(WINAPI* PFNWGLSAVEBUFFERREGIONARBPROC)(HANDLE hRegion, int x, int y, int width, int height);
typedef BOOL(WINAPI* PFNWGLRESTOREBUFFERREGIONARBPROC)(HANDLE hRegion, int x, int y, int width, int height, int xSrc, int ySrc);

PFNWGLCREATEBUFFERREGIONARBPROC wglCreateBufferRegionARB = NULL;
PFNWGLDELETEBUFFERREGIONARBPROC wglDeleteBufferRegionARB = NULL;
PFNWGLSAVEBUFFERREGIONARBPROC wglSaveBufferRegionARB = NULL;
PFNWGLRESTOREBUFFERREGIONARBPROC wglRestoreBufferRegionARB = NULL;

static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

// Function to save framebuffer as an image
void saveFrameBufferToImage(int width, int height, const std::string &file_name) {
    // Allocate buffer to hold pixel data
    std::vector<unsigned char> pixels(3 * width * height);

    // Read the pixels from the framebuffer
    glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, pixels.data());

    // Write the pixel data to an image file
    stbi_write_png(file_name.c_str(), width, height, 3, pixels.data(), width * 3);
}

bool loadWGLExtensions() {
    wglCreateBufferRegionARB = (PFNWGLCREATEBUFFERREGIONARBPROC)wglGetProcAddress("wglCreateBufferRegionARB");
    wglDeleteBufferRegionARB = (PFNWGLDELETEBUFFERREGIONARBPROC)wglGetProcAddress("wglDeleteBufferRegionARB");
    wglSaveBufferRegionARB = (PFNWGLSAVEBUFFERREGIONARBPROC)wglGetProcAddress("wglSaveBufferRegionARB");
    wglRestoreBufferRegionARB = (PFNWGLRESTOREBUFFERREGIONARBPROC)wglGetProcAddress("wglRestoreBufferRegionARB");

    if (!wglCreateBufferRegionARB || !wglDeleteBufferRegionARB || !wglSaveBufferRegionARB || !wglRestoreBufferRegionARB) {
        return false;
    }
    return true;
}

GLfloat vertices[] = {
    -1.0f, -1.0f, 0.0f,
    1.0f, -1.0f, 0.0f,
    0.0f,  1.0f, 0.0f
};

GLfloat colors[] = {
    1.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 1.0f
};

void drawTriangle() {
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);

    glVertexPointer(3, GL_FLOAT, 0, vertices);
    glColorPointer(3, GL_FLOAT, 0, colors);
    glDrawArrays(GL_TRIANGLES, 0, 3);
}

int main(void)
{
    glfwSetErrorCallback(error_callback);

    if (!glfwInit())
        exit(EXIT_FAILURE);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL Triangle", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(window);
    gladLoadGL(glfwGetProcAddress);
    glfwSwapInterval(1);
    if (!loadWGLExtensions()) {
        std::cerr << "Failed to load WGL extensions" << std::endl;
        return 1;
    }

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);


    HWND hwnd = glfwGetWin32Window(window);
    HDC hdc = GetDC(hwnd); // Get the HDC from the GLFW window
    HANDLE bufferRegion = wglCreateBufferRegionARB(hdc, 0, WGL_BACK_COLOR_BUFFER_BIT_ARB);
    if (!bufferRegion) {
        exit(1);
    }

    while (!glfwWindowShouldClose(window))
    {
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT);


        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        drawTriangle();
        glFinish();
        wglSaveBufferRegionARB(bufferRegion, 0, 0, 800, 600);
        saveFrameBufferToImage(width, height, "1.save_800x600.png");

        wglRestoreBufferRegionARB(bufferRegion, 0, 0, 400, 300, 0, 0);
        saveFrameBufferToImage(width, height, "2.restore_x-0_y-0_W-400_H-300_xRrc-0_ySrc-0.png");

        wglRestoreBufferRegionARB(bufferRegion, 100, 100, 400, 300, 0, 0);
        saveFrameBufferToImage(width, height, "3.restore_x-100_y-100_W-400_H-300_xRrc-0_ySrc-0.png");

        wglRestoreBufferRegionARB(bufferRegion, 0, 0, 400, 300, 100, 100);
        saveFrameBufferToImage(width, height, "4.restore_x-0_y-0_W-400_H-300_xRrc-100_ySrc-100.png");

        wglRestoreBufferRegionARB(bufferRegion, 100, 100, 400, 300, 100, 100);
        saveFrameBufferToImage(width, height, "5.restore_x-100_y-100_W-400_H-300_xRrc-100_ySrc-100.png");

        glfwSwapBuffers(window);
        glfwPollEvents();
        break;
    }

    glfwDestroyWindow(window);

    glfwTerminate();
    exit(EXIT_SUCCESS);
}

//! [code]
