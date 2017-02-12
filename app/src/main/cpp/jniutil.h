//
// Created by tangzhongliang on 7/15/2016.
//

#ifndef JNIUTIL_H
#define JNIUTIL_H

#include <jni.h>
#include <stdio.h>
#include <stdlib.h>
#include <android/log.h>
#include <android/bitmap.h>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <fstream>
#include <iostream>

#define TAG    "jni-tag"
#define LOGD(...)  __android_log_print(ANDROID_LOG_INFO,TAG,__VA_ARGS__)
//#define LOGD(...)  write(__VA_ARGS__)
#define LOGI(...)  __android_log_print(ANDROID_LOG_INFO,TAG,__VA_ARGS__)
#define LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,TAG,__VA_ARGS__)

using namespace cv;
using namespace std;
struct ArrayStruct {
    int len;
    void *data;
};

//void write(char *__format, ...) {
//
//    char *ret;
//    va_list params;
//    va_start(params, __format);
//    int cnt = vsprintf(ret, __format, params);
//    va_end(params);
////    FILE* f = fopen("/mnt/hdd/log.txt","r");
////    fstream f("/mnt/hdd/log.txt",ios::app);
////    if (f){
////    fprintf(f,"%s --------- %d",ret,cnt);
////    fputc('\n',f);
////    fflush(f);
////    fclose(f);
////    }
//
//}

char *jstring2string(JNIEnv *env, jstring jstr) {
    char *rtn = NULL;
    jclass clsstring = env->FindClass("java/lang/String");
    jstring strencode = env->NewStringUTF("utf-8");
    jmethodID mid = env->GetMethodID(clsstring, "getBytes", "(Ljava/lang/String;)[B");
    jbyteArray barr = (jbyteArray) env->CallObjectMethod(jstr, mid, strencode);
    jsize alen = env->GetArrayLength(barr);
    jbyte *ba = env->GetByteArrayElements(barr, JNI_FALSE);
    if (alen > 0) {
        rtn = (char *) malloc(alen + 1);
        memcpy(rtn, ba, alen);
        rtn[alen] = 0;
    }
    env->ReleaseByteArrayElements(barr, ba, 0);
    return rtn;
}

//char* to jstring
jstring s2Jstring(JNIEnv *env, const char *pat) {
    jclass strClass = env->FindClass("Ljava/lang/String;");
    jmethodID ctorID = env->GetMethodID(strClass, "<init>", "([BLjava/lang/String;)V");
    jbyteArray bytes = env->NewByteArray(strlen(pat));
    env->SetByteArrayRegion(bytes, 0, strlen(pat), (jbyte *) pat);
    jstring encoding = env->NewStringUTF("utf-8");
    return (jstring) env->NewObject(strClass, ctorID, bytes, encoding);
}

//Mat getMatFromBytes(char* data,int width,int height){
//IplImage img= cvCreateImageHeader(cvSize(width,height),255,4);
//  cvSetData(&img,data,width*4);
//  cv::Mat mat(&img,0);
//  return mat;
//}
uint8_t *jbyteArray2charArray(JNIEnv *env, jbyteArray bytearray) {
    jbyte *bytes = env->GetByteArrayElements(bytearray, 0);
    return (uint8_t *) bytes;
}

jbyteArray charArray2jbyteArray(JNIEnv *env, char *s, int len) {
    jbyteArray rtnbytes = env->NewByteArray(len);
    env->SetByteArrayRegion(rtnbytes, 0, len, (jbyte *) s);
    return rtnbytes;
}

jintArray intArray2jintArray(JNIEnv *env, int *s, int len) {
    jintArray rtnbytes = env->NewIntArray(len);
    env->SetIntArrayRegion(rtnbytes, 0, (jint) len, (jint *) s);
    return rtnbytes;
}

void CopyCharFromInt(unsigned char *data, int *pixels, int len) {

    for (int i = 0; i < len; i++) {
        data[i * 4] = pixels[i] >> 24;
        data[i * 4 + 1] = pixels[i] >> 16;
        data[i * 4 + 2] = pixels[i] >> 8;
        data[i * 4 + 3] = pixels[i];
    }
}

void CopyIntFromChar(int *pixels, unsigned char *data, int len) {

    for (int i = 0; i < len; i++) {
        pixels[i] =
                data[i * 4] << 24 | data[i * 4 + 1] << 16 | data[i * 4 + 2] << 8 | data[i * 4 + 3];
    }
}

IplImage *covertIplImage(uint8_t *bytes, int width, int height, int channel) {
    IplImage *srcImg = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, channel);
    cvSetData(srcImg, bytes, width * channel);
    return srcImg;
}

Mat covertMat(uint8_t *bytes, int width, int height) {
    Mat mat(Size(width, height), CV_8UC4, bytes);
    return mat;
}

IplImage Mat2IplImage(Mat mat) {
    IplImage imgIpl = (IplImage) (mat);
    return imgIpl;
}

Mat IplImage2Mat(IplImage *img) {
    int channels = img->nChannels;
    Mat mat;
    if (channels == 3)
        mat = Mat(Size(img->width, img->height), CV_8UC3, (uint8_t *) img->imageData);
    else
        mat = Mat(Size(img->width, img->height), CV_8UC4, (uint8_t *) img->imageData);
    return mat;
}

jobject Mat2Bitmap(JNIEnv *env, Mat img) {
    //
    //creating a new bitmap to put the pixels into it - using Bitmap Bitmap.createBitmap (int width, int height, Bitmap.Config config) :
    //
    int width = img.cols;
    int height = img.rows;
    jclass bitmapCls = env->FindClass("android/graphics/Bitmap");
    LOGD("creating new bitmap...width=%d,height=%d", width, height);
    jmethodID createBitmapFunction = env->GetStaticMethodID(bitmapCls, "createBitmap",
                                                            "(IILandroid/graphics/Bitmap$Config;)Landroid/graphics/Bitmap;");
    jstring configName = env->NewStringUTF("ARGB_8888");
    jclass bitmapConfigClass = env->FindClass("android/graphics/Bitmap$Config");
    jmethodID valueOfBitmapConfigFunction = env->GetStaticMethodID(bitmapConfigClass, "valueOf",
                                                                   "(Ljava/lang/String;)Landroid/graphics/Bitmap$Config;");
    jobject bitmapConfig = env->CallStaticObjectMethod(bitmapConfigClass,
                                                       valueOfBitmapConfigFunction, configName);
    jobject newBitmap = env->CallStaticObjectMethod(bitmapCls, createBitmapFunction, width, height,
                                                    bitmapConfig);
    //
    // putting the pixels into the new bitmap:
    //
    int ret;
    void *bitmapPixels;
    if ((ret = AndroidBitmap_lockPixels(env, newBitmap, &bitmapPixels)) < 0) {
        LOGE("AndroidBitmap_lockPixels() failed ! error=%d", ret);
        return NULL;
    }
    uint8_t *newBitmapPixels = (uint8_t *) bitmapPixels;
    //memcpy(newBitmapPixels, img->imageData, sizeof(uint32_t) * img->width*img->height);
    Mat rgba;
    cvtColor(img, rgba, CV_BGRA2RGBA);
    memcpy(newBitmapPixels, rgba.data, sizeof(uint32_t) * height * width);
    AndroidBitmap_unlockPixels(env, newBitmap);
    return newBitmap;
}

Mat Bitmap2Mat(JNIEnv *env, jobject bitmap) {
    Mat mat;
    //
    //getting bitmap info:
    //
    LOGD("reading bitmap info...");
    AndroidBitmapInfo info;
    int ret;
    if ((ret = AndroidBitmap_getInfo(env, bitmap, &info)) < 0) {
        LOGE("AndroidBitmap_getInfo() failed ! error=%d", ret);
        return mat;
    }
    LOGD("width:%d height:%d stride:%d", info.width, info.height, info.stride);
    if (info.format != ANDROID_BITMAP_FORMAT_RGBA_8888) {
        LOGE("Bitmap format is not RGBA_8888!");
        return mat;
    }
    //
    //read pixels of bitmap into native memory :
    //
    LOGD("reading bitmap pixels...");
    void *bitmapPixels;
    if ((ret = AndroidBitmap_lockPixels(env, bitmap, &bitmapPixels)) < 0) {
        LOGE("AndroidBitmap_lockPixels() failed ! error=%d", ret);
        return mat;
    }
    Mat test(info.height, info.width, CV_8UC4, bitmapPixels);
    Mat bgra;
    cvtColor(test, bgra, CV_RGBA2BGRA);
    AndroidBitmap_unlockPixels(env, bitmap);
    //IplImage* img = covertIPlImg(src,info.width,info.height);
    return bgra;
}

void setMatSize(Mat &mat, int rows, int cols) {
    mat.rows = rows;
    mat.cols = cols;
}

IplImage *YUV420_To_IplImage_Opencv(unsigned char *pYUV420, int width, int height) {
    if (!pYUV420) {
        return NULL;
    }

    IplImage *yuvimage, *rgbimg, *yimg, *uimg, *vimg, *uuimg, *vvimg;

    int nWidth = width;
    int nHeight = height;
    rgbimg = cvCreateImage(cvSize(nWidth, nHeight), IPL_DEPTH_8U, 3);
    yuvimage = cvCreateImage(cvSize(nWidth, nHeight), IPL_DEPTH_8U, 3);

    yimg = cvCreateImageHeader(cvSize(nWidth, nHeight), IPL_DEPTH_8U, 1);
    uimg = cvCreateImageHeader(cvSize(nWidth / 2, nHeight / 2), IPL_DEPTH_8U, 1);
    vimg = cvCreateImageHeader(cvSize(nWidth / 2, nHeight / 2), IPL_DEPTH_8U, 1);

    uuimg = cvCreateImage(cvSize(nWidth, nHeight), IPL_DEPTH_8U, 1);
    vvimg = cvCreateImage(cvSize(nWidth, nHeight), IPL_DEPTH_8U, 1);

    cvSetData(yimg, pYUV420, nWidth);
    cvSetData(uimg, pYUV420 + nWidth * nHeight, nWidth / 2);
    cvSetData(vimg, pYUV420 + long(nWidth * nHeight * 1.25), nWidth / 2);
    cvResize(uimg, uuimg, CV_INTER_LINEAR);
    cvResize(vimg, vvimg, CV_INTER_LINEAR);

    cvMerge(yimg, uuimg, vvimg, NULL, yuvimage);
    cvCvtColor(yuvimage, rgbimg, CV_YCrCb2BGR);

    cvReleaseImage(&uuimg);
    cvReleaseImage(&vvimg);
    cvReleaseImageHeader(&yimg);
    cvReleaseImageHeader(&uimg);
    cvReleaseImageHeader(&vimg);

    cvReleaseImage(&yuvimage);

    if (!rgbimg) {
        return NULL;
    }

    return rgbimg;
}

//LOGD("########## i = %d", i);
#endif //JNIUTIL_H
