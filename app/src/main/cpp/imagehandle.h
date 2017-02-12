//
// Created by tangzhongliang on 7/15/2016.
//
#ifndef IMAGE_HANDLE_H
#define IMAGE_HANDLE_H

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
#include "jniutil.h"
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif
using namespace std;
using namespace cv;
int rotateImage(Mat &, int);
void rotateImageFile(const char *pathString, char* dstString, int degree) {

    Mat src = imread(pathString);
    rotateImage(src, degree);
    imwrite(dstString, src);
    src.release();
}
int rotateImage(Mat &src, int degree) {

    if (degree == 180) {
        flip(src, src, 0);// 转置
        flip(src, src, 1);// Y轴翻转
//        setMatSize(src, src.rows, src.cols);
    }
    if (degree == 90) {
        transpose(src, src);// 转置
        flip(src, src, 0);// Y轴翻转
//        setMatSize(src, src.cols, src.rows);
    } else if (degree == -90 || degree == 270) {
        transpose(src, src);// 转置
        flip(src, src, 1);// Y轴翻转
//        setMatSize(src, src.cols, src.rows);
    }
    return 0;
}
void imageCombine(char *targetFile, char *logoFile, int xOffset, int yOffset) {
    cv::Mat image = cv::imread(targetFile);
    cv::Mat logo = cv::imread(logoFile);
    cv::Mat imageROI;
    imageROI = image(cv::Rect(xOffset, yOffset, logo.cols, logo.rows));
    logo.copyTo(imageROI);
    //cv::namedWindow("result");
    //cv::imshow("result",image);
    imwrite(targetFile, image);
    //cv::waitKey();
}
Mat imageRoi(Mat mat, int left, int top, int width, int height) {
    Rect rect(left, top, width, height);
    Mat matRoi(mat, rect);
    matRoi.cols = width;
    matRoi.rows = height;
    //imwrite("/mnt/sdcard/temp.jpg",matRoi);
    return matRoi;
}

IplImage *cvimageROI(IplImage *res, int left, int top, int right, int bottom) {
    IplImage *dst;
    CvRect rect(left, top, right, bottom);
    dst = cvCreateImage(cvGetSize(res), res->depth, res->nChannels);;
    cvSetImageROI(res, rect);
    //提取ROI
    cvCopy(res, dst, NULL);
    cvResetImageROI(res);
    return dst;
}

#ifdef __cplusplus
}
#endif
#endif
