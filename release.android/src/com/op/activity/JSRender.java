package com.op.activity;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

import android.opengl.GLSurfaceView;

public class JSRender implements GLSurfaceView.Renderer {

    @Override
    public native void onDrawFrame(GL10 arg0);

    @Override
    public native void onSurfaceChanged(GL10 arg0, int w, int h);

    @Override
    public native void onSurfaceCreated(GL10 arg0, EGLConfig config);
}
