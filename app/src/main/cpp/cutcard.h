//
// Created by tangzhongliang on 8/4/2016.
//

#ifndef IMAGE_TRANSLATE_CUTCARD_H
#define IMAGE_TRANSLATE_CUTCARD_H

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

using namespace std;
using namespace cv;
int sufix = 0;
//return rect and release img
int cutCard(string srcPath, string dstPath, int degree) {
    Mat src = imread(srcPath, -1);
    //rotate
    LOGD("cutCard src size %d %d", src.cols, src.rows);
    Mat rotateDst;
    if (degree == 180) {
        flip(src, src, 0);// 转置
        flip(src, rotateDst, 1);// Y轴翻转
    }

    Mat rotateDst2;
    if (degree == 90) {
        transpose(src, rotateDst);// 转置
        flip(rotateDst, rotateDst, 0);// Y轴翻转
    } else if (degree == -90) {
        transpose(src, rotateDst);// 转置
        flip(rotateDst, rotateDst, 1);// Y轴翻转
    } else {
        rotateDst = src;
    }
    Mat graySrc;
    //cut card
    cvtColor(rotateDst, graySrc, CV_BGR2GRAY);
    threshold(graySrc, graySrc, 240, 255, CV_THRESH_BINARY);
//    imshow("src",src);
    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;
    findContours(graySrc, contours, hierarchy,
                 CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);
    Mat dst = Mat::ones(rotateDst.rows, rotateDst.cols, CV_8UC3);
    int idx = 0;
//        for( ; idx >= 0; idx = hierarchy[idx][0] )
//        {
//            vector<Point> tmp = contours[idx];
//            Scalar color( rand()&255, rand()&255, rand()&255 );
//            drawContours( dst, contours, idx, color, 1, 8, hierarchy );
//        }
//        imshow("",dst);
// 寻找最大连通域
    double maxArea = 0, secondArea = 0;
    vector<cv::Point> maxContour, secondContour;
    LOGD("contour size is %d", static_cast<int>(contours.size()));
    for (size_t i = 0; i < contours.size(); i++) {
        double area = cv::contourArea(contours[i]);
        if (area > maxArea && area > 100 * 100) {
            secondArea = maxArea;
            maxArea = area;
            secondContour = maxContour;
            maxContour = contours[i];
        } else if (area > secondArea && area > 100 * 100) {
            secondArea = area;
            secondContour = contours[i];
        }
    }
    for (int i = 0; i < maxContour.size(); i++)
        circle(graySrc, maxContour[i], 2, Scalar(255, 255, 0));
//    char * temp;
//    sprintf(temp,"/mnt/hdd/cut_%d",sufix);
//    sufix++;
    imwrite("/mnt/hdd/cut.jpg",graySrc);
    // 将轮廓转为矩形框

    LOGD("cutcard area %f %f", maxArea, secondArea);
    if (secondArea == 0 && maxArea == 0) {
        LOGD("do not detach area");
        return -1;
    }
    RotatedRect rRect;
//    int rate = src.cols * src.rows / secondArea;
    if (secondArea > src.cols * src.rows * 15 / 20) {
        rRect = minAreaRect(secondContour);
    } else {
        rRect = minAreaRect(maxContour);
    }
    ellipse(dst, rRect, Scalar(255, 255, 0));
    int dstw, dsth;
    Point2f vertices[4];
    Point2f verdst[4];
    Point2f temp[4];
    rRect.points(vertices);
    //set point co-ordinates by yourself
    int maxY = -1;
    int minY = INT_MAX;
    int maxX = -1;
    int minX = INT_MAX;

    for (int i = 0; i <= 3; i++) {
        if (vertices[i].x > maxX) {
            maxX = vertices[i].x;
            temp[3] = Point2f(vertices[i].x, vertices[i].y);
        }

        if (vertices[i].x < minX) {
            minX = vertices[i].x;
            temp[1] = Point2f(vertices[i].x, vertices[i].y);
        }

        if (vertices[i].y > maxY) {
            maxY = vertices[i].y;
            temp[2] = Point2f(vertices[i].x, vertices[i].y);
        }

        if (vertices[i].y < minY) {
            minY = vertices[i].y;
            temp[0] = Point2f(vertices[i].x, vertices[i].y);
        }
    }

    Mat Idst;
    cout << "rRect.angle:" << rRect.angle << endl;
    for (int i = 0; i < 4; i++) {
        vertices[i] = temp[i];
    }
    dsth = rRect.size.height;
    dstw = rRect.size.width;
    if ((int(rRect.angle) - rRect.angle) == 0 && (int) rRect.angle % 90 == 0) {
        vertices[0].x = minX;
        vertices[0].y = minY;
        vertices[1].x = minX;
        vertices[1].y = maxY;
        vertices[2].x = maxX;
        vertices[2].y = maxY;
        vertices[3].x = maxX;
        vertices[3].y = minY;
    }
    if (rRect.angle < 0 && rRect.angle > -45) {
        dsth = rRect.size.height;
        dstw = rRect.size.width;
        verdst[0] = Point2f(dstw, 0);
        verdst[1] = Point2f(0, 0);
        verdst[2] = Point2f(0, dsth);
        verdst[3] = Point2f(dstw, dsth);
        Idst = Mat(dsth, dstw, CV_8UC1);
    } else if (rRect.angle > 0 && rRect.angle < 45) {
        verdst[0] = Point2f(0, 0);
        verdst[1] = Point2f(0, dsth);
        verdst[2] = Point2f(dstw, dsth);
        verdst[3] = Point2f(dstw, 0);
        Idst = Mat(dsth, dstw, CV_8UC1);
    } else if (rRect.angle == 0) {
        verdst[0] = Point2f(0, 0);
        verdst[1] = Point2f(0, dsth);
        verdst[2] = Point2f(dstw, dsth);
        verdst[3] = Point2f(dstw, 0);
        Idst = Mat(dsth, dstw, CV_8UC1);
    } else {
        verdst[0] = Point2f(0, 0);
        verdst[1] = Point2f(0, dstw);
        verdst[2] = Point2f(dsth, dstw);
        verdst[3] = Point2f(dsth, 0);
        Idst = Mat(dstw, dsth, CV_8UC1);
    }
    Mat warpMatrix = getPerspectiveTransform(vertices, verdst);
    warpPerspective(rotateDst, Idst, warpMatrix, Idst.size());
//    imshow("Idst",Idst);
    imwrite(dstPath, Idst);
    src.release();
    return 0;
}

#endif //IMAGE_TRANSLATE_CUTCARD_H
