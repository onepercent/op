//
//  Application.cpp
//  v8
//
//  Created by jie on 13-8-4.
//  Copyright (c) 2013年 jie. All rights reserved.
//
#include "global.h"
#include "Java_com_op_activity_JSActivity.h"
#include "Java_com_op_activity_JSRender.h"
#include "Java_com_op_activity_JSSurfaceView.h"
#include <jni.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include "utils/AssetUtil.h"
#include "app/Application.h"

Application* app = NULL;

// ==========================
// Life Cycle
// ==========================
JNIEXPORT void JNICALL Java_com_op_activity_JSActivity_initWithAsset
(JNIEnv * env, jobject instance, jobject assetManager) {
    AssetUtil::mgr = AAssetManager_fromJava(env, assetManager);
}

JNIEXPORT void JNICALL Java_com_op_activity_JSActivity_jsCreate
(JNIEnv * env, jclass activityClass) {
    LOGI("Java_com_op_activity_JSActivity_jsCreate 01");
    app = new Application();
    LOGI("Java_com_op_activity_JSActivity_jsCreate 02");
}

JNIEXPORT void JNICALL Java_com_op_activity_JSActivity_jsDestory
(JNIEnv * env, jclass activityClass) {
    LOGI("Java_com_op_activity_JSActivity_jsDestory");
    app->destroy();
    delete app;
    app = 0;
}

JNIEXPORT void JNICALL Java_com_op_activity_JSActivity_evalScript
(JNIEnv * env, jclass activityClass, jstring script) {
}

// ==========================
// View status change
// ==========================
JNIEXPORT void JNICALL Java_com_op_activity_JSSurfaceView_jsPause
  (JNIEnv *, jclass) {
    LOGI("Java_com_op_activity_JSSurfaceView_jsPause");
	app->pause();
}

JNIEXPORT void JNICALL Java_com_op_activity_JSSurfaceView_jsResume
  (JNIEnv *, jclass) {
	app->resume();
}

// ==========================
// Events
// ==========================
JNIEXPORT void JNICALL Java_com_op_activity_JSSurfaceView_jsTouchClick
  (JNIEnv *, jclass, jint button, jint state, jint x, jint y) {
	app->appendMouseTouch(button, state, x, y);
}

JNIEXPORT void JNICALL Java_com_op_activity_JSSurfaceView_jsTouchMove
  (JNIEnv *, jclass, jint x, jint y) {
	app->appendMouseMove(x, y);
}

JNIEXPORT void JNICALL Java_com_op_activity_JSSurfaceView_jsKeyBackPress
  (JNIEnv *, jclass) {
	LOGE("jsKeyBackPress empth implement");
}

JNIEXPORT void JNICALL Java_com_op_activity_JSSurfaceView_jsKeyMenuPress
  (JNIEnv *, jclass) {
	//app->appendKeyPress();
	LOGE("jsKeyMenuPress empth implement");
}

// ==========================
// Render
// ==========================
JNIEXPORT void JNICALL Java_com_op_activity_JSRender_onSurfaceCreated
  (JNIEnv * env, jobject instance) {
    app->init();
	app->onSurfaceCreated(800, 480);
}

JNIEXPORT void JNICALL Java_com_op_activity_JSRender_onDrawFrame
  (JNIEnv * env, jobject instance, jobject) {
	app->onDrawFrame();
}

JNIEXPORT void JNICALL Java_com_op_activity_JSRender_onSurfaceChanged
  (JNIEnv * env, jobject instance, jobject, jint width, jint height) {
	app->onSurfaceChanged(width, height);
}
