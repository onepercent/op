//
//  main.cpp
//  jstest
//
//  Created by jie on 13-7-22.
//  Copyright (c) 2013年 jie. All rights reserved.
//
#include <string>

#include "app/Application.h"
#include "classes/file.h"
#include "utils/AssetUtil.h"
#include "modules/CCImage.h"
#include "modules/CCTexture2D.h"
#include <GLUT/GLUT.h>
#include <OpenGL/gl.h>
//#include "TestGc.h"
#include "core/RingBuffer.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/swizzle.hpp>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <iostream>

using namespace v8;

static void printGLString(const char *name, GLenum s) {
	const char *v = (const char *) glGetString(s);
	LOGI("GL %s = %s\n", name, v);
}

static bool checkGlError(const char* op) {
	for (GLint error = glGetError(); error; error = glGetError()) {
		LOGI("after %s() glError (0x%x)\n", op, error);
        return false;
	}
    return true;
}

std::string dir(std::string path, std::string subpath) {
    std::string::size_type index = path.find_last_of('/');
    if (index != std::string::npos){
        return path.substr(0, index).append(subpath);
    } else {
        return subpath;
    }
}

std::string source_root = dir(__FILE__, "/assets/").c_str();

void testVersion() {
    const GLubyte *renderer = glGetString( GL_RENDERER );
    const GLubyte *vendor = glGetString( GL_VENDOR );
    const GLubyte *version = glGetString( GL_VERSION );
    const GLubyte *glslVersion = glGetString(GL_SHADING_LANGUAGE_VERSION);
    
    //    GLint major, minor;
    //    glGetIntegerv(GL_VERSION, &major);
    //    glGetIntegerv(GL_VERSION, &minor);
    //    glGetIntegerv(GL_MAJOR_VERSION, &major);
    //    glGetIntegerv(GL_MINOR_VERSION, &minor);
    printf("GL Vendor : %s\n", vendor);
    printf("GL Renderer : %s\n", renderer);
    printf("GL Version (string) : %s\n", version);
    //    printf("GL Version (integer) : %d.%d\n", major, minor);
    printf("GLSL Version : %s\n", glslVersion);
}

void testImageLoad() {
    JSFile* file = new JSFile();
    AssetUtil::load(file, "images/pngnow.png");
    node::CCImage* img = new node::CCImage();
    img->initWithImageData((void*)file->chars(), file->size());
    delete file;
    
    node::CCTexture2D* t2d = new node::CCTexture2D();
    t2d->initWithImage(img);
    
    LOGI("width:%d, height:%d", img->getWidth(), img->getHeight());
}

int powOf2(int num) {
    num--;
    int offset = 1;
    while ((num & (num + 1)) != 0) {
        num |= num >> offset;
        offset = offset << 1;
    }
    return num + 1;
}

Application* app = NULL;
void onDrawFrame() {
    //app->gc();
    app->onDrawFrame();
    if(!checkGlError("onDrawFrame==>")) {
        return;
    }
    glutPostRedisplay();
    glutSwapBuffers();
}
void onSurfaceChanged(int w, int h) {
    app->onSurfaceChanged(w, h);
}
void onMouseClick(int button, int state, int x, int y) {
    //    LOGI("onTouch %d %d %d %d", button, state, x, y);
    app->appendMouseTouch(button, state, x, y);
}
void onMouseMove(int x, int y) {
    //    LOGI("onMove %d %d", x, y);
    app->appendMouseMove(x, y);
}
void onKeyPress(unsigned char key, int x, int y) {
    //    LOGI("onTouch %d %d %d", key, x, y);
    app->appendKeyPress(key, x, y);
}
void start(int argc, char ** argv, int width, int height) {
	glutInit(&argc, argv);
    glutInitWindowSize(width, height);
	glutCreateWindow("Xcode Glut Demo");

    testImageLoad();
    testVersion();

    app = new Application();
    app->init();
    app->resume();
    app->onSurfaceCreated(width, height);
    app->onSurfaceChanged(width, height);

	glutDisplayFunc(onDrawFrame);
    glutReshapeFunc(onSurfaceChanged);
    glutMouseFunc(onMouseClick);
    glutMotionFunc(onMouseMove);
    glutKeyboardFunc(onKeyPress);
	glutMainLoop();

    app->destroy();
    delete app;
}
int main(int argc, char ** argv)
{
    start(argc, argv, 800, 480);
    return 0;
}
