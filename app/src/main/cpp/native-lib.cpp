#include <jni.h>
#include <string>
#include "imagehandle.h"
#include "stitching_detailed.h"
#include "cutcard.h"

using namespace std;
using namespace cv;
using namespace cv::detail;
#define JNIFUNCTION_NATIVE(sig) Java_jp_co_ricoh_advop_supersizescan_MainActivity_##sig

void recover(const char *path1, const char *path2, const char *path);

#ifdef __cplusplus

void recover(const char *path1, const char *path2, const char *path) {
    Mat srcImg1, srcImg2, resultImg, img_merge, outImg_left, outImg_right, temp;
    //read mat
    srcImg1 = imread(path1);
    srcImg2 = imread(path2);
    resultImg = imread(path);
    int len = resultImg.cols * 3;
    int repeatLen = srcImg1.cols + srcImg2.cols - len;
    flip(srcImg2, srcImg2, -1);
    temp = srcImg2(Rect(repeatLen, 0, srcImg2.cols - repeatLen, srcImg2.rows));
    //compute repeat(rows1+rows2 - result.rows*scale)

    //pin jie
    Size size(len, MAX(srcImg1.rows, srcImg2.rows));
    img_merge.create(size, srcImg1.type());
    img_merge = Scalar::all(0);

    outImg_left = img_merge(Rect(0, 0, srcImg1.cols, srcImg1.rows));
    outImg_right = img_merge(Rect(srcImg1.cols, 0, len - srcImg1.cols, srcImg1.rows));

    srcImg1.copyTo(outImg_left);
    temp.copyTo(outImg_right);
    imwrite(path, img_merge);
    srcImg1.release();
    srcImg2.release();
    resultImg.release();
    img_merge.release();
}

extern "C" {
#endif
JNIEXPORT jint JNICALL JNIFUNCTION_NATIVE(combine2(JNIEnv * env, jobject
                                                  obj, jstring
                                                  first, jstring
                                                  second, jstring
                                                  resultPath, jint
                                                  jdegree)) {
    int degree = (int) jdegree;
    string result_temp = "/mnt/hdd/result_temp.jpg";
    string result_name_left = "/mnt/hdd/result_left.jpg";
    string result_name_right = "/mnt/hdd/result_right.jpg";
    string result_name_middle = "/mnt/hdd/result_middle.jpg";
    char *part1 = "/mnt/hdd/superscanjpeg_part1.jpg";
    char *part2 = "/mnt/hdd/superscanjpeg_part2.jpg";

    const char *strPath1 = env->GetStringUTFChars(first, 0);
    const char *strPath2 = env->GetStringUTFChars(second, 0);
    const char *strResultPath = env->GetStringUTFChars(resultPath, 0);
//    cutCard(strPath1,strPath1,0);
//    cutCard(strPath2,strPath2,0);
    Mat cutImg, srcImg, srcImg2, tempImg;
    tempImg = imread(strPath1);
    resize(tempImg, srcImg, Size(tempImg.cols / 3, tempImg.rows / 3), cv::INTER_AREA);
    rotateImage(srcImg, degree);
    imwrite(part1, srcImg);
    tempImg = imread(strPath2);
    resize(tempImg, srcImg2, Size(tempImg.cols / 3, tempImg.rows / 3), cv::INTER_AREA);
    rotateImage(srcImg2, degree);
    flip(srcImg2, srcImg2, -1);
    imwrite(part2, srcImg2);
    tempImg.release();
    srcImg.release();
    srcImg2.release();

    vector<String> src;
    src.push_back(String(part1));
    src.push_back(String(part2));
    int ret = chuli(src, strResultPath);
    remove(part1);
    remove(part2);
//    recover(strPath1, strPath2, strResultPath);

    return ret;
}
JNIEXPORT jstring JNICALL JNIFUNCTION_NATIVE(stringFromJNI(JNIEnv * env, jobject
                                                     obj)) {
    LOGD("Hello from C++%d", 2);
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}

//JNIEXPORT jboolean JNICALL JNIFUNCTION_NATIVE(scale(JNIEnv * env, jobject
//                                                      obj, jstring
//                                                      file, jint
//                                                      scale, jint
//                                                      rotation)) {
//    LOGD("scale");
//    const char *cFile = env->GetStringUTFChars(file, 0);
//    Mat src = imread(cFile);
//    Mat dst;
//    resize(src, dst, cv::Size(src.cols / scale, src.rows / scale));
//    dst = rotateImage(dst, rotation);
//    LOGD("scale rotateImage succecss");
//    int ret = imwrite(cFile, dst);
//    src.release();
//    dst.release();
//    return ret;
//}

JNIEXPORT jboolean JNICALL JNIFUNCTION_NATIVE(correctRect(JNIEnv * env, jobject
                                                      obj, jstring
                                                      file, jstring
                                                      dstFile, jint
                                                      rotation)) {
    const char *cFile = env->GetStringUTFChars(file, 0);
    LOGD("correctRect");
    int ret = cutCard(cFile, env->GetStringUTFChars(dstFile, 0), rotation);
    LOGD("correctRect result%d", ret);
    return ret == 0;
}
#ifdef __cplusplus
}
#endif
