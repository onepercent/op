/*
 * Copyright (C) 2007 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.op.activity;

import android.annotation.SuppressLint;
import android.app.Activity;
import android.app.ActivityManager;
import android.content.Context;
import android.content.pm.ConfigurationInfo;
import android.content.res.AssetManager;
import android.opengl.GLSurfaceView;
import android.os.Build;
import android.os.Bundle;
import android.view.ViewGroup;
import android.widget.FrameLayout;

public class JSActivity extends Activity {

    static {
        System.loadLibrary("v8");
        System.loadLibrary("glm");
        System.loadLibrary("freetype_gl");
        System.loadLibrary("game");
    }

    private AssetManager  mgr;

    private GLSurfaceView mView;

    private boolean hasGLES20() {
        ActivityManager am = (ActivityManager) getSystemService(Context.ACTIVITY_SERVICE);
        ConfigurationInfo info = am.getDeviceConfigurationInfo();
        return info.reqGlEsVersion >= 0x20000;
    }

    public GLSurfaceView onCreateView() {
        ViewGroup.LayoutParams framelayout_params = new ViewGroup.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.MATCH_PARENT);
        FrameLayout framelayout = new FrameLayout(this);
        framelayout.setLayoutParams(framelayout_params);

        GLSurfaceView view = new JSSurfaceView(this);
        //GLSurfaceView view = new GLSurfaceView(this);
        //view.setRenderer(new JSRender());
        framelayout.addView(view);

        setContentView(framelayout);
        return view;
    }

    @Override
    protected void onCreate(Bundle icicle) {
        super.onCreate(icicle);

        System.out.println("JSActivity.onCreate():" + hasGLES20());

        // prepare tools
        mgr = getAssets();
        initWithAsset(mgr);

        jsCreate();
        mView = onCreateView();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        jsDestory();
        System.out.println("JSActivity.onDestroy=============()");
    }

    @Override
    protected void onPause() {
        super.onPause();
        mView.onPause();
    }

    @Override
    protected void onResume() {
        super.onResume();
        mView.onResume();
    }

    /**
     * 使用 asset 来初始化上下文
     * 
     * @param assetManager
     */
    public native void initWithAsset(AssetManager assetManager);

    public static native void jsCreate();

    public static native void jsDestory();

    public static native void evalScript(String script);
}
