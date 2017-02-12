//
// Created by tangzhongliang on 8/4/2016.
//

#ifndef FACE_HANDLE_H
#define FACE_HANDLE_H

#include <jni.h>
#include <stdio.h>
#include <stdlib.h>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <vector>
#include <opencv2/imgproc/imgproc.hpp>
#include <fstream>
#include "opencv2/highgui/highgui.hpp"
#include <android/bitmap.h>
#include "cv.h"
#include "highgui.h"
#include <string.h>

#ifndef JNIUTIL_H

#include <jniutil.h>

#endif
#ifdef __cplusplus
extern "C" {
#endif
using namespace std;
using namespace cv;
//return rect and release img
static CvMemStorage *storage = 0;
static CvHaarClassifierCascade *cascade = 0;


const char *cascade_name = "/mnt/sdcard/haarcascade_frontalface_alt2.xml";
void detect_and_draw(IplImage *img) {
    cascade = (CvHaarClassifierCascade *) cvLoad(cascade_name, 0, 0, 0);
    if (!cascade) {
        fprintf(stderr, "ERROR: Could not load classifier cascade\n");
        return;
    }
    storage = cvCreateMemStorage(0);
    double scale = 1.2;
    static CvScalar colors[] = {
            CvScalar{0, 0, 255}, CvScalar{0, 128, 255}, CvScalar{0, 255, 255}, CvScalar{0, 255, 0},
            CvScalar{255, 128, 0}, CvScalar{255, 255, 0}, CvScalar{255, 0, 0}, CvScalar{255, 0, 255}
    };//Just some pretty colors to draw with

    //Image Preparation
    //
    IplImage *gray = cvCreateImage(cvSize(img->width, img->height), 8, 1);
    IplImage *small_img = cvCreateImage(
            cvSize(cvRound(img->width / scale), cvRound(img->height / scale)), 8, 1);
    cvCvtColor(img, gray, CV_BGR2GRAY);
    cvResize(gray, small_img, CV_INTER_LINEAR);

    cvEqualizeHist(small_img, small_img); //直方图均衡

    //Detect objects if any
    //
    cvClearMemStorage(storage);
    double t = (double) cvGetTickCount();
    CvSeq *objects = cvHaarDetectObjects(small_img,
                                         cascade,
                                         storage,
                                         1.1,
                                         2,
                                         0/*CV_HAAR_DO_CANNY_PRUNING*/,
                                         cvSize(30, 30));
    t = (double) cvGetTickCount() - t;
    printf("detection time = %gms\n", t / ((double) cvGetTickFrequency() * 1000.));
    int count = objects ? objects->total : 0;
//    LOGD( "detection time = %gms,detect count=%d", t/((double)cvGetTickFrequency()*1000.) ,count);
    //Loop through found objects and draw boxes around them
    for (int i = 0; i < count; ++i) {
        CvRect *r = (CvRect *) cvGetSeqElem(objects, i);
        cvRectangle(img, cvPoint(r->x * scale, r->y * scale),
                    cvPoint((r->x + r->width) * scale, (r->y + r->height) * scale), colors[i % 8]);
    }
    for (int i = 0; i < (objects ? objects->total : 0); i++) {
        CvRect *r = (CvRect *) cvGetSeqElem(objects, i);
        CvPoint center;
        int radius;
        center.x = cvRound((r->x + r->width * 0.5) * scale);
        center.y = cvRound((r->y + r->height * 0.5) * scale);
        radius = cvRound((r->width + r->height) * 0.25 * scale);
        cvCircle(img, center, radius, colors[i % 8], 3, 8, 0);
    }
    cvReleaseImage(&gray);
    cvReleaseImage(&small_img);

}
vector<int> detect_rect(IplImage *img) {

    vector<int> result;
    cascade = (CvHaarClassifierCascade *) cvLoad(cascade_name, 0, 0, 0);
    if (!cascade) {
        fprintf(stderr, "ERROR: Could not load classifier cascade\n");
        return result;
    }
    storage = cvCreateMemStorage(0);
    double scale = 1.2;
    // static CvScalar colors[] = {
    //     {{0,0,255}},{{0,128,255}},{{0,255,255}},{{0,255,0}},
    //     {{255,128,0}},{{255,255,0}},{{255,0,0}},{{255,0,255}}
    // };//Just some pretty colors to draw with

    //Image Preparation
    //
    IplImage *gray = cvCreateImage(cvSize(img->width, img->height), 8, 1);
    IplImage *small_img = cvCreateImage(
            cvSize(cvRound(img->width / scale), cvRound(img->height / scale)), 8, 1);
    cvCvtColor(img, gray, CV_BGR2GRAY);
    cvResize(gray, small_img, CV_INTER_LINEAR);

    cvEqualizeHist(small_img, small_img); //直方图均衡

    //Detect objects if any
    //
    cvClearMemStorage(storage);
    double t = (double) cvGetTickCount();
    CvSeq *objects = cvHaarDetectObjects(small_img,
                                         cascade,
                                         storage,
                                         1.1,
                                         2,
                                         0/*CV_HAAR_DO_CANNY_PRUNING*/,
                                         cvSize(30, 30));
    t = (double) cvGetTickCount() - t;
    printf("detection time = %gms\n", t / ((double) cvGetTickFrequency() * 1000.));
    int count = objects ? objects->total : 0;
    for (int i = 0; i < count; ++i) {
        CvRect *r = (CvRect *) cvGetSeqElem(objects, i);
        result.push_back(cvRound(r->x * scale));
        result.push_back(cvRound(r->y * scale));
        result.push_back(cvRound(r->width * scale));
        result.push_back(cvRound(r->height * scale));
        LOGD("%f,%f,%f,%f", r->x * scale, r->y * scale, r->width * scale, r->height * scale);
    }
    //Loop through found objects and draw boxes around them
    // for(int i=0;i<count;++i)
    // {
    //     CvRect* r=(CvRect*)cvGetSeqElem(objects,i);
    //     cvRectangle(img, cvPoint(r->x*scale,r->y*scale), cvPoint((r->x+r->width)*scale,(r->y+r->height)*scale), colors[i%8]);
    // }
    for (int i = 0; i < (objects ? objects->total : 0); i++) {
        CvRect *r = (CvRect *) cvGetSeqElem(objects, i);
        CvPoint center;
        int radius;
        center.x = cvRound((r->x + r->width * 0.5) * scale);
        center.y = cvRound((r->y + r->height * 0.5) * scale);
        radius = cvRound((r->width + r->height) * 0.25 * scale);
        cvCircle(img, center, radius, cvScalar(0, 0, 255), 3, 8, 0);
    }
    if (objects) {
        cvSaveImage("/mnt/sdcard/result.jpg", img);
        LOGD("save result");
    }
    cvReleaseImage(&gray);
    cvReleaseImage(&small_img);

//   cvShowImage("img",img);
    return result;
}
#ifdef __cplusplus
}
#endif

#endif //IMAGE_TRANSLATE_FACEHANDLE_H
