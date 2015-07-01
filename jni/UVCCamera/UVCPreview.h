/*
 * UVCCamera
 * library and sample to access to UVC web camera on non-rooted Android device
 *
 * Copyright (c) 2014 saki t_saki@serenegiant.com
 *
 * File name: UVCPreview.h
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 * All files in the folder are under this Apache License, Version 2.0.
 * Files in the jni/libjpeg, jni/libusb and jin/libuvc folder may have a different license, see the respective files.
*/

#ifndef UVCPREVIEW_H_
#define UVCPREVIEW_H_

#include "libUVCCamera.h"
#include <pthread.h>
#include <android/native_window.h>
#include "objectarray.h"

#pragma interface

#define DEFAULT_PREVIEW_WIDTH 320
#define DEFAULT_PREVIEW_HEIGHT 240
#define DEFAULT_PREVIEW_FPS 30
#define DEFAULT_PREVIEW_MODE 0

typedef uvc_error_t (*convFunc_t)(uvc_frame_t *in, uvc_frame_t *out);

#define PIXEL_FORMAT_RAW 0		// same as PIXEL_FORMAT_YUV
#define PIXEL_FORMAT_YUV 1
#define PIXEL_FORMAT_RGB565 2
#define PIXEL_FORMAT_RGBX 3
#define PIXEL_FORMAT_YUV20SP 4
#define PIXEL_FORMAT_NV21 5		// YVU420SemiPlanar

/* vuemate2 defined */
#define PROBED_DSMS

typedef struct _vuemate_information {
	int sleepDuration;
	int sleepStatus;
	int currentStatus;
	int sleepingTime;
}VUEMATE_DATA;

// for callback to Java object
typedef struct {
#ifdef PROBED_DSMS
	jmethodID onDsms;
#endif	
	jmethodID onFrame;

} Fields_iframecallback;

class UVCPreview {
private:
	uvc_device_handle_t *mDeviceHandle;
	ANativeWindow *mPreviewWindow;
	volatile bool mIsRunning;
	int requestWidth, requestHeight, requestMode, requestFps;
	int frameWidth, frameHeight;
	int frameMode;
	size_t frameBytes;
	pthread_t preview_thread;
	pthread_mutex_t preview_mutex;
	pthread_cond_t preview_sync;
	ObjectArray<uvc_frame_t *> previewFrames;
	int previewFormat;
	size_t previewBytes;
//
	volatile bool mIsCapturing;
	ANativeWindow *mCaptureWindow;
	pthread_t capture_thread;
	pthread_mutex_t capture_mutex;
	pthread_cond_t capture_sync;
#ifdef PROBED_DSMS
	volatile bool mIsDsms;
	pthread_t dsms_thread;
	pthread_mutex_t dsms_mutex;
	pthread_cond_t dsms_sync;
	VUEMATE_DATA *dsmsQueu;
#endif
	uvc_frame_t *captureQueu;			// keep latest frame
	jobject mFrameCallbackObj;
	convFunc_t mFrameCallbackFunc;
	Fields_iframecallback iframecallback_fields;
	int mPixelFormat;
	size_t callbackPixelBytes;
//
	void clearDisplay();
	static void uvc_preview_frame_callback(uvc_frame_t *frame, void *vptr_args);
	void addPreviewFrame(uvc_frame_t *frame);
	uvc_frame_t *waitPreviewFrame();
	void clearPreviewFrame();
	static void *preview_thread_func(void *vptr_args);
	int prepare_preview(uvc_stream_ctrl_t *ctrl);
	void do_preview(uvc_stream_ctrl_t *ctrl);
	uvc_frame_t *draw_preview_one(uvc_frame_t *frame, ANativeWindow **window, convFunc_t func, int pixelBytes);
//
	void addCaptureFrame(uvc_frame_t *frame);
	uvc_frame_t *waitCaptureFrame();
	void clearCaptureFrame();
	static void *capture_thread_func(void *vptr_args);
	void do_capture(JNIEnv *env);
	void do_capture_surface(JNIEnv *env);
	void do_capture_idle_loop(JNIEnv *env);
	void do_capture_callback(JNIEnv *env, uvc_frame_t *frame);
#ifdef PROBED_DSMS
	void addDsmsFrame(VUEMATE_DATA *frame);
	VUEMATE_DATA *waitDsmsFrame();
	static void *dsms_thread_func(void *vptr_args);
	void do_dsms(JNIEnv *env);
	void do_dsms_callback(JNIEnv *env, VUEMATE_DATA *frame);
#endif
	void callbackPixelFormatChanged();
public:
	UVCPreview(uvc_device_handle_t *devh);
	~UVCPreview();

	inline const bool isRunning() const;
	int setPreviewSize(int width, int height, int mode);
	int setPreviewDisplay(ANativeWindow *preview_window);
	int setFrameCallback(JNIEnv *env, jobject frame_callback_obj);
	int startPreview();
	int stopPreview();
	inline const bool isCapturing() const;
#ifdef PROBED_DSMS
	inline const bool isDsmsDoing() const;
#endif
	int setCaptureDisplay(ANativeWindow *capture_window);
};

#endif /* UVCPREVIEW_H_ */
