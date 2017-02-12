#include <jni.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <iostream>
#include <fstream>
#include <string>
#include <android/log.h>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <vector>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <fstream>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/stitching/stitcher.hpp"


using namespace cv;
using namespace std;
using namespace cv::detail;

#define LOG_TAG   "stitch"
#define  LOGI(...) __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)


bool try_use_gpu = false;
vector<Mat> imgs;
string result_name = "/mnt/hdd/result_middle.jpg";
string result_name_left = "/mnt/hdd/result_left.jpg";
string result_name_right = "/mnt/hdd/result_right.jpg";
string result_name_left_all = "/mnt/hdd/result_left_all.jpg";
string result_name_right_all = "/mnt/hdd/result_right_all.jpg";
string result_name_cut_result = "/mnt/hdd/cut_result.jpg";
string result_file2_180 = "/mnt/hdd/file2_180.jpg";


extern "C" {

string   jstringTostring(JNIEnv*   env,   jstring   jstr)
{
  char*   rtn   =   NULL;
  jclass   clsstring   =   env->FindClass("java/lang/String");
  jstring   strencode   =   env->NewStringUTF("GB2312");
  jmethodID   mid   =   env->GetMethodID(clsstring,   "getBytes",   "(Ljava/lang/String;)[B");
  jbyteArray   barr=   (jbyteArray)env->CallObjectMethod(jstr,mid,strencode);
  jsize   alen   =   env->GetArrayLength(barr);
  jbyte*   ba   =   env->GetByteArrayElements(barr,JNI_FALSE);
  if(alen   >   0)
  {
   rtn   =   (char*)malloc(alen+1);         //new   char[alen+1];
   memcpy(rtn,ba,alen);
   rtn[alen]=0;
  }
  env->ReleaseByteArrayElements(barr,ba,0);
  string stemp(rtn);
  free(rtn);
  return   stemp;
}

//JNIEXPORT void JNICALL Java_jp_co_ricoh_cheetah_app_jni_Stitch_plus(JNIEnv* env, jobject obj, jstring path1, jstring path2);
//JNIEXPORT void JNICALL Java_jp_co_ricoh_cheetah_app_jni_Stitch_StitchOpencv(JNIEnv* env, jobject obj, jstring path1, jstring path2);

JNIEXPORT jint JNICALL Java_jp_co_ricoh_cheetah_app_jni_Stitch_StitchOpencvPlus(
		JNIEnv* env, jobject obj, jstring path1, jstring path2, jfloat scaling)
{
	String strPath1 = jstringTostring(env,path1);
	String strPath2 = jstringTostring(env,path2);
	float cutLength = 300*scaling;

	Mat cutImg, tmpImg, tmpImg2;
	tmpImg  = imread(strPath1);
	cutImg = tmpImg(Rect(tmpImg.cols-cutLength,0,cutLength,tmpImg.rows));
	imwrite("/mnt/hdd/part1.jpg",cutImg);

	tmpImg = imread(strPath2);
	tmpImg2 = tmpImg(Rect(tmpImg.cols-cutLength,0,cutLength,tmpImg.rows));
	flip(tmpImg2,cutImg,-1);
	imwrite("/mnt/hdd/part2.jpg",cutImg);


	Mat img=imread("/mnt/hdd/part1.jpg");
	imgs.push_back(img);
	img=imread("/mnt/hdd/part2.jpg");
	imgs.push_back(img);


	Mat result;
	Stitcher stitcher = Stitcher::createDefault(try_use_gpu);
	stitcher.stitch(imgs, result);

	LOGE("CYGwidth and height ---------------...%d %d",result.cols,result.rows);
    if(result.cols!=0 || result.rows!=0)
    {
    	imwrite(result_name, result);

        Mat mat1;
        Mat mat2;
        Mat mat3;
        mat1 = imread(strPath1);

        int y = result.rows - mat1.rows;   //阴影长度
        int x = cutLength-(result.cols - cutLength);   //重叠长度

        LOGE("CYG ---------------...%d %d %d",x,y,result.cols);
        mat3 = result(Rect((result.cols-x)/2,y,x,mat1.rows-y));
        imwrite(result_name_cut_result, mat3); //middle stitch result

        mat1 = imread(strPath1);
        mat2 = mat1(Rect(0,0,mat1.cols-result.cols/2,mat1.rows-y));
    	imwrite(result_name_left,mat2);

    	mat1  = imread(strPath2);
    	mat2 = mat1(Rect(0,0,mat1.cols-result.cols/2,mat1.rows-y));
    	flip(mat2,mat3,-1);
    	imwrite(result_name_right,mat3);
    	mat1.release();
    	mat2.release();
    	mat3.release();
    	LOGE("CYG End imwrite...");
    	return x;
    }
    return 0;

}
}









//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//    cv::setBreakOnError(true);
//	int num_images = 2;
//    double work_scale = 1, seam_scale = 1, compose_scale = 1;
//    bool is_work_scale_set = false, is_seam_scale_set = false, is_compose_scale_set = false;
//
//	LOGE("CYG Finding features...");
//
//#if ENABLE_LOG
//    int64 t = getTickCount();
//#endif
//
//    Ptr<FeaturesFinder> finder;
//    if (features_type == "surf")
//    {
////#if defined(HAVE_OPENCV_NONFREE) && defined(HAVE_OPENCV_GPU)
////        if (try_gpu && gpu::getCudaEnabledDeviceCount() > 0)
////            finder = new SurfFeaturesFinderGpu();
////        else
////#endif
//    	initModule_nonfree();
//    	finder = new SurfFeaturesFinder();
//
//    	LOGE("CYG 2222222222222...");
//
//    }
//    else if (features_type == "orb")
//    {
//        finder = new OrbFeaturesFinder(Size(3,1), 1500, 1.3f, 5);
//    }
//    else
//    {
//        cout << "Unknown 2D features type: '" << features_type << "'.\n";
//        return 0;
//    }
//
//    Mat full_img, img;
//    vector<ImageFeatures> features(num_images);
//    vector<Mat> images(num_images);
//    vector<Size> full_img_sizes(num_images);
//    double seam_work_aspect = 1;
//
//	LOGE("CYG 33333333333333...");
//
//	#pragma omp parallel for
//	for (int i = 0; i < num_images; ++i)
//    {
//		if (i == 0)
//		{
//			  Mat tmp  = imread(strPath1);
//				LOGE("CYG 44444444444...%d",tmp.cols);
//				LOGE("CYG 44444444444...%d",tmp.rows);
//
//			  full_img = tmp(Rect(tmp.cols-cutLength,0,cutLength,tmp.rows));
//			  imwrite("/mnt/hdd/part1.jpg",full_img);
//				LOGE("CYG 44444444444...");
//
//		}
//		else if (i == 1)
//		{
//			  Mat tmp = imread(strPath2);
//			  Mat tmp2 = tmp(Rect(tmp.cols-cutLength,0,cutLength,tmp.rows));
//			  flip(tmp2,full_img,-1);
//			  imwrite("/mnt/hdd/part2.jpg",full_img);
//
//		}
////		else if (i == 2)
////		{
////			full_img = imread(strPath3);
////		}else
////		{
////			full_img = imread(strPath4);
////		}
//
//        full_img_sizes[i] = full_img.size();
//
//        if (full_img.empty())
//        {
//            LOGE("CYG Can't open image ");
//            return 0;
//        }
//        if (work_megapix < 0)
//        {
//            img = full_img;
//            work_scale = 1;
//            is_work_scale_set = true;
//        }
//        else
//        {
//            if (!is_work_scale_set)
//            {
//                work_scale = min(1.0, sqrt(work_megapix * 1e6 / full_img.size().area()));
//                is_work_scale_set = true;
//            }
//            resize(full_img, img, Size(), work_scale, work_scale);
//        }
//        if (!is_seam_scale_set)
//        {
//            seam_scale = min(1.0, sqrt(seam_megapix * 1e6 / full_img.size().area()));
//            seam_work_aspect = seam_scale / work_scale;
//            is_seam_scale_set = true;
//        }
//
//        (*finder)(img, features[i]);
//        features[i].img_idx = i;
//        LOGLN("Features in image #" << i+1 << ": " << features[i].keypoints.size());
//
//        resize(full_img, img, Size(), seam_scale, seam_scale);
//        images[i] = img.clone();
//    }
//
////    finder->collectGarbage();
//    full_img.release();
//    img.release();
//
//    LOGLN("Finding features, time: " << ((getTickCount() - t) / getTickFrequency()) << " sec");
//
//    LOGE("CYG Pairwise matching");
//#if ENABLE_LOG
//    t = getTickCount();
//#endif
////    vector<MatchesInfo> pairwise_matches;
////    BestOf2NearestMatcher matcher(try_gpu, match_conf);
////    matcher(features, pairwise_matches);
////    matcher.collectGarbage();
//
//    vector<MatchesInfo> pairwise_matches;
//    BestOf2NearestMatcher matcher(try_gpu, match_conf);
//    Mat matchMask(features.size(),features.size(),CV_8U,Scalar(0));
//    for (int i = 0; i < num_images -1; ++i)
//    {
//    	matchMask.at<char>(i,i+1) =1;
//    }
//    matcher(features, pairwise_matches,matchMask);
//    matcher.collectGarbage();
//    LOGLN("Pairwise matching, time: " << ((getTickCount() - t) / getTickFrequency()) << " sec");
//
//    // Check if we should save matches graph
//    if (save_graph)
//    {
//        LOGE("CYG Saving matches graph...");
//        ofstream f(save_graph_to.c_str());
//        f << matchesGraphAsString(img_names, pairwise_matches, conf_thresh);
//    }
//
//    LOGE("features.size before() = %d",features.size());
//
//    LOGE("features [0] size = %d",features[0].keypoints.size());
//    LOGE("features [1] size = %d",features[1].keypoints.size());
//    LOGE("features [2] size = %d",features[2].keypoints.size());
//    LOGE("features [3] size = %d",features[3].keypoints.size());
//
//    // Leave only images we are sure are from the same panorama
//    vector<int> indices = leaveBiggestComponent(features, pairwise_matches, conf_thresh);
//
//    if (indices.size() != num_images)
//    {
//        LOGE("EXIT!!! indices.size() != num_images");
//    	pairwise_matches.clear();
//    	features.clear();
//    	full_img_sizes.clear();
//    	images.clear();
//    	return 0;
//    }
//
//    LOGE("features.size after() = %d",features.size());
//    LOGE("pairwise_matches.size() = %d",pairwise_matches.size());
//
//    vector<Mat> img_subset;
//    vector<string> img_names_subset;
//    vector<Size> full_img_sizes_subset;
//
//    LOGE("CYG Pairwise matching -- 2");
//
//    LOGE("indices.size() = %d",indices.size());
//
//    for (size_t i = 0; i < indices.size(); ++i)
//    {
//        //img_names_subset.push_back(img_names[indices[i]]);
//        img_subset.push_back(images[indices[i]]);
//        full_img_sizes_subset.push_back(full_img_sizes[indices[i]]);
//    }
//
//    images = img_subset;
//    //img_names = img_names_subset;
//    full_img_sizes = full_img_sizes_subset;
//
//    // Check if we still have enough images
//    //num_images = static_cast<int>(img_names.size());
//    //if (num_images < 2)
//    //{
//    //    LOGLN("Need more images");
//    //    return -1;
//    //}
//    LOGE("pairwise_matches [0] size = %d",pairwise_matches[0].matches.size());
//    LOGE("pairwise_matches [1] size = %d",pairwise_matches[1].matches.size());
//    LOGE("pairwise_matches [2] size = %d",pairwise_matches[2].matches.size());
//    LOGE("pairwise_matches [3] size = %d",pairwise_matches[3].matches.size());
//
////    if (pairwise_matches[0].matches.size() == 0 && pairwise_matches[1].matches.size() == 0) return;
//
//    HomographyBasedEstimator estimator;
//    vector<CameraParams> cameras;
//
//    LOGE("CYG Pairwise matching -- 3");
//
//    estimator(features, pairwise_matches, cameras);
//
//    LOGE("CYG Pairwise matching -- 4");
//
//    LOGE("cameras.size() = %d",cameras.size());
//
//
//    for (size_t i = 0; i < cameras.size(); ++i)
//    {
//        Mat R;
//        cameras[i].R.convertTo(R, CV_32F);
//        cameras[i].R = R;
//        LOGLN("Initial intrinsics #" << indices[i]+1 << ":\n" << cameras[i].K());
//    }
//
//    Ptr<detail::BundleAdjusterBase> adjuster;
//    if (ba_cost_func == "reproj") adjuster = new detail::BundleAdjusterReproj();
//    else if (ba_cost_func == "ray") adjuster = new detail::BundleAdjusterRay();
//    else
//    {
//        cout << "Unknown bundle adjustment cost function: '" << ba_cost_func << "'.\n";
//        return 0;
//    }
//    adjuster->setConfThresh(conf_thresh);
//    Mat_<uchar> refine_mask = Mat::zeros(3, 3, CV_8U);
//    if (ba_refine_mask[0] == 'x') refine_mask(0,0) = 1;
//    if (ba_refine_mask[1] == 'x') refine_mask(0,1) = 1;
//    if (ba_refine_mask[2] == 'x') refine_mask(0,2) = 1;
//    if (ba_refine_mask[3] == 'x') refine_mask(1,1) = 1;
//    if (ba_refine_mask[4] == 'x') refine_mask(1,2) = 1;
//    adjuster->setRefinementMask(refine_mask);
//    (*adjuster)(features, pairwise_matches, cameras);
//
//    // Find median focal length
//
//    vector<double> focals;
//    for (size_t i = 0; i < cameras.size(); ++i)
//    {
//        LOGLN("Camera #" << indices[i]+1 << ":\n" << cameras[i].K());
//        focals.push_back(cameras[i].focal);
//    }
//
//    sort(focals.begin(), focals.end());
//    float warped_image_scale;
//    if (focals.size() % 2 == 1)
//        warped_image_scale = static_cast<float>(focals[focals.size() / 2]);
//    else
//        warped_image_scale = static_cast<float>(focals[focals.size() / 2 - 1] + focals[focals.size() / 2]) * 0.5f;
//
//    if (do_wave_correct)
//    {
//        vector<Mat> rmats;
//        for (size_t i = 0; i < cameras.size(); ++i)
//            rmats.push_back(cameras[i].R);
//        waveCorrect(rmats, wave_correct);
//        for (size_t i = 0; i < cameras.size(); ++i)
//            cameras[i].R = rmats[i];
//    }
//
//    LOGE("CYG Warping images (auxiliary)... ");
//#if ENABLE_LOG
//    t = getTickCount();
//#endif
//
//    vector<Point> corners(num_images);
//    vector<Mat> masks_warped(num_images);
//    vector<Mat> images_warped(num_images);
//    vector<Size> sizes(num_images);
//    vector<Mat> masks(num_images);
//
//    // Preapre images masks
//    for (int i = 0; i < num_images; ++i)
//    {
//        masks[i].create(images[i].size(), CV_8U);
//        masks[i].setTo(Scalar::all(255));
//    }
//
//    // Warp images and their masks
//
//    Ptr<WarperCreator> warper_creator;
//#if defined(HAVE_OPENCV_GPU)
//    if (try_gpu && gpu::getCudaEnabledDeviceCount() > 0)
//    {
//        if (warp_type == "plane") warper_creator = new cv::PlaneWarperGpu();
//        else if (warp_type == "cylindrical") warper_creator = new cv::CylindricalWarperGpu();
//        else if (warp_type == "spherical") warper_creator = new cv::SphericalWarperGpu();
//    }
//    else
//#endif
//    {
//        if (warp_type == "plane") warper_creator = new cv::PlaneWarper();
//        else if (warp_type == "cylindrical") warper_creator = new cv::CylindricalWarper();
//        else if (warp_type == "spherical") warper_creator = new cv::SphericalWarper();
//        else if (warp_type == "fisheye") warper_creator = new cv::FisheyeWarper();
//        else if (warp_type == "stereographic") warper_creator = new cv::StereographicWarper();
//        else if (warp_type == "compressedPlaneA2B1") warper_creator = new cv::CompressedRectilinearWarper(2, 1);
//        else if (warp_type == "compressedPlaneA1.5B1") warper_creator = new cv::CompressedRectilinearWarper(1.5, 1);
//        else if (warp_type == "compressedPlanePortraitA2B1") warper_creator = new cv::CompressedRectilinearPortraitWarper(2, 1);
//        else if (warp_type == "compressedPlanePortraitA1.5B1") warper_creator = new cv::CompressedRectilinearPortraitWarper(1.5, 1);
//        else if (warp_type == "paniniA2B1") warper_creator = new cv::PaniniWarper(2, 1);
//        else if (warp_type == "paniniA1.5B1") warper_creator = new cv::PaniniWarper(1.5, 1);
//        else if (warp_type == "paniniPortraitA2B1") warper_creator = new cv::PaniniPortraitWarper(2, 1);
//        else if (warp_type == "paniniPortraitA1.5B1") warper_creator = new cv::PaniniPortraitWarper(1.5, 1);
//        else if (warp_type == "mercator") warper_creator = new cv::MercatorWarper();
//        else if (warp_type == "transverseMercator") warper_creator = new cv::TransverseMercatorWarper();
//    }
//
//    if (warper_creator.empty())
//    {
//        cout << "Can't create the following warper '" << warp_type << "'\n";
//        return 0;
//    }
//
//    Ptr<RotationWarper> warper = warper_creator->create(static_cast<float>(warped_image_scale * seam_work_aspect));
//
//    for (int i = 0; i < num_images; ++i)
//    {
//        Mat_<float> K;
//        cameras[i].K().convertTo(K, CV_32F);
//        float swa = (float)seam_work_aspect;
//        K(0,0) *= swa; K(0,2) *= swa;
//        K(1,1) *= swa; K(1,2) *= swa;
//
//        corners[i] = warper->warp(images[i], K, cameras[i].R, INTER_LINEAR, BORDER_REFLECT, images_warped[i]);
//        sizes[i] = images_warped[i].size();
//
//        warper->warp(masks[i], K, cameras[i].R, INTER_NEAREST, BORDER_CONSTANT, masks_warped[i]);
//    }
//
//    vector<Mat> images_warped_f(num_images);
//    for (int i = 0; i < num_images; ++i)
//        images_warped[i].convertTo(images_warped_f[i], CV_32F);
//
//    LOGLN("Warping images, time: " << ((getTickCount() - t) / getTickFrequency()) << " sec");
//
//    Ptr<ExposureCompensator> compensator = ExposureCompensator::createDefault(expos_comp_type);
//    compensator->feed(corners, images_warped, masks_warped);
//
//    Ptr<SeamFinder> seam_finder;
//    if (seam_find_type == "no")
//        seam_finder = new detail::NoSeamFinder();
//    else if (seam_find_type == "voronoi")
//        seam_finder = new detail::VoronoiSeamFinder();
//    else if (seam_find_type == "gc_color")
//    {
////#if defined(HAVE_OPENCV_GPU)
////        if (try_gpu && gpu::getCudaEnabledDeviceCount() > 0)
////            seam_finder = new detail::GraphCutSeamFinderGpu(GraphCutSeamFinderBase::COST_COLOR);
////        else
////#endif
//            seam_finder = new detail::GraphCutSeamFinder(GraphCutSeamFinderBase::COST_COLOR);
//    }
//    else if (seam_find_type == "gc_colorgrad")
//    {
////#if defined(HAVE_OPENCV_GPU)
////        if (try_gpu && gpu::getCudaEnabledDeviceCount() > 0)
////            seam_finder = new detail::GraphCutSeamFinderGpu(GraphCutSeamFinderBase::COST_COLOR_GRAD);
////        else
////#endif
//            seam_finder = new detail::GraphCutSeamFinder(GraphCutSeamFinderBase::COST_COLOR_GRAD);
//    }
//    else if (seam_find_type == "dp_color")
//        seam_finder = new detail::DpSeamFinder(DpSeamFinder::COLOR);
//    else if (seam_find_type == "dp_colorgrad")
//        seam_finder = new detail::DpSeamFinder(DpSeamFinder::COLOR_GRAD);
//    if (seam_finder.empty())
//    {
//        cout << "Can't create the following seam finder '" << seam_find_type << "'\n";
//        return 0;
//    }
//    seam_finder->find(images_warped_f, corners, masks_warped);
//
//    // Release unused memory
//    images.clear();
//    images_warped.clear();
//    images_warped_f.clear();
//    masks.clear();
//
//    LOGE("CYG Compositing...");
//#if ENABLE_LOG
//    t = getTickCount();
//#endif
//
//    Mat img_warped, img_warped_s;
//    Mat dilated_mask, seam_mask, mask, mask_warped;
//    Ptr<Blender> blender;
//    //double compose_seam_aspect = 1;
//    double compose_work_aspect = 1;
//
//	#pragma omp parallel for
//    for (int img_idx = 0; img_idx < cameras.size(); ++img_idx)
//    {
//        LOGLN("Compositing image #" << indices[img_idx]+1);
//
//        // Read image and resize it if necessary
//		if (img_idx == 0)
//		{
//			  Mat tmp  = imread(strPath1);
//			 full_img = tmp(Rect(tmp.cols-cutLength,0,cutLength,tmp.rows));
//		}
//		else if (img_idx == 1)
//		{
//			  Mat tmp = imread(strPath2);
//			  Mat tmp2 = tmp(Rect(tmp.cols-cutLength,0,cutLength,tmp.rows));
//			  flip(tmp2,full_img,-1);
//
//		}
////		else if (img_idx == 2)
////		{
////			full_img = imread(strPath3);
////		}else
////		{
////			full_img = imread(strPath4);
////		}
//
//        if (!is_compose_scale_set)
//        {
//            if (compose_megapix > 0)
//                compose_scale = min(1.0, sqrt(compose_megapix * 1e6 / full_img.size().area()));
//            is_compose_scale_set = true;
//
//            // Compute relative scales
//            //compose_seam_aspect = compose_scale / seam_scale;
//            compose_work_aspect = compose_scale / work_scale;
//
//            // Update warped image scale
//            warped_image_scale *= static_cast<float>(compose_work_aspect);
//            warper = warper_creator->create(warped_image_scale);
//
//            LOGE("CYG111 Warping images (auxiliary)... ");
//            LOGE("cameras Warping images (auxiliary)... = %d",cameras.size());
//            // Update corners and sizes
//            for (int i = 0; i < cameras.size(); ++i)
//            {
//                // Update intrinsics
//                cameras[i].focal *= compose_work_aspect;
//                cameras[i].ppx *= compose_work_aspect;
//                cameras[i].ppy *= compose_work_aspect;
//
//                // Update corner and size
//                Size sz = full_img_sizes[i];
//                if (std::abs(compose_scale - 1) > 1e-1)
//                {
//                	LOGE("CYG444 Warping images (auxiliary)... ");
//                    sz.width = cvRound(full_img_sizes[i].width * compose_scale);
//                    sz.height = cvRound(full_img_sizes[i].height * compose_scale);
//                }
//
//                Mat K;
//                cameras[i].K().convertTo(K, CV_32F);
//                Rect roi = warper->warpRoi(sz, K, cameras[i].R);
//                corners[i] = roi.tl();
//                sizes[i] = roi.size();
//            }
//        }
//
//        LOGE("CYG222 Warping images (auxiliary)... ");
//
//        if (abs(compose_scale - 1) > 1e-1)
//        {
//        	LOGE("CYG555 Warping images (auxiliary)... ");
//            resize(full_img, img, Size(), compose_scale, compose_scale);
//            LOGE("CYG333 Warping images (auxiliary)... ");
//        }
//
//        else
//            img = full_img;
//        full_img.release();
//        Size img_size = img.size();
//
//        Mat K;
//        cameras[img_idx].K().convertTo(K, CV_32F);
//
//        // Warp the current image
//        warper->warp(img, K, cameras[img_idx].R, INTER_LINEAR, BORDER_REFLECT, img_warped);
//
//        // Warp the current image mask
//        mask.create(img_size, CV_8U);
//        mask.setTo(Scalar::all(255));
//        warper->warp(mask, K, cameras[img_idx].R, INTER_NEAREST, BORDER_CONSTANT, mask_warped);
//
//        // Compensate exposure
//        compensator->apply(img_idx, corners[img_idx], img_warped, mask_warped);
//
//        img_warped.convertTo(img_warped_s, CV_16S);
//        img_warped.release();
//        img.release();
//        mask.release();
//        K.release();   //CYG
//        LOGE("CYGwwwwwwwwww Warping images (auxiliary)... ");
//        dilate(masks_warped[img_idx], dilated_mask, Mat());
//
//        LOGE("CYGqqqqq Warping images (auxiliary)... ");
//        resize(dilated_mask, seam_mask, mask_warped.size());
//
//        LOGE("CYGaaaaa Warping images (auxiliary)... ");
//        mask_warped = seam_mask & mask_warped;
//
//        if (blender.empty())
//        {
//        	LOGE("1111111 Warping images (auxiliary)... ");
//            blender = Blender::createDefault(blend_type, try_gpu);
//            LOGE("222222222 Warping images (auxiliary)... ");
//            Size dst_sz = resultRoi(corners, sizes).size();
//            LOGE("333333333 Warping images (auxiliary)... ");
//            float blend_width = sqrt(static_cast<float>(dst_sz.area())) * blend_strength / 100.f;
//            LOGE("44444444 Warping images (auxiliary)... ");
//            if (blend_width < 1.f)
//            {
//                blender = Blender::createDefault(Blender::NO, try_gpu);
//                LOGE("5555555 Warping images (auxiliary)... ");
//            }
//            else if (blend_type == Blender::MULTI_BAND)
//            {
//            	LOGE("6666666666666 Warping images (auxiliary)... ");
//                MultiBandBlender* mb = dynamic_cast<MultiBandBlender*>(static_cast<Blender*>(blender));
//                mb->setNumBands(static_cast<int>(ceil(log(blend_width)/log(2.)) - 1.));
//                LOGLN("Multi-band blender, number of bands: " << mb->numBands());
//            }
//            else if (blend_type == Blender::FEATHER)
//            {
//            	LOGE("7777777777777 Warping images (auxiliary)... ");
//                FeatherBlender* fb = dynamic_cast<FeatherBlender*>(static_cast<Blender*>(blender));
//                fb->setSharpness(1.f/blend_width);
//                LOGLN("Feather blender, sharpness: " << fb->sharpness());
//            }
//            LOGE("features.size before() = %d",features.size());
//            LOGE("8888888888888 Warping images (auxiliary)... = %d",corners.size());
//            LOGE("8000000000000 Warping images (auxiliary)... = %d",sizes.size());
//            blender->prepare(corners, sizes);
//            LOGE("99999999999999 Warping images (auxiliary)... ");
//        }
//        LOGE("CYGeeeeeeeeeeeee Warping images (auxiliary)... ");
//        // Blend the current image
//        blender->feed(img_warped_s, mask_warped, corners[img_idx]);
//        LOGE("CYGrrrrrrrrrrrrrrrr Warping images (auxiliary)... ");
//    }
//
//    Mat result, result_mask;
//    blender->blend(result, result_mask);
//    LOGE("CYGtttttttttttttttttttt Warping images (auxiliary)... ");
//    LOGLN("Compositing, time: " << ((getTickCount() - t) / getTickFrequency()) << " sec");
//
//    imwrite(result_name_cut_result, result); //middle stitch result
//
//    Mat Mat1;
//    Mat Mat2;
//    Mat Mat3;
//    Mat1 = imread(strPath1);
//
//    int y = result.rows - Mat1.rows;   //阴影长度
//    int x = cutLength-(result.cols - cutLength);   //重叠长度
//
//    LOGE("CYG ---------------...%d %d %d",x,y,result.cols);
//
//    Mat3 = result(Rect((result.cols-x)/2,y,x,Mat1.rows-y));
//    imwrite(result_name, Mat3); //middle stitch result
//
//    Mat1 = imread(strPath1);
//    imwrite(result_name_left_all,Mat1);
//    Mat2 = Mat1(Rect(0,0,Mat1.cols-result.cols/2,Mat1.rows-y));
//	imwrite(result_name_left,Mat2);
//
//	Mat1  = imread(strPath2);
//	imwrite(result_name_right_all,Mat1);
//	Mat2 = Mat1(Rect(0,0,Mat1.cols-result.cols/2,Mat1.rows-y));
//	flip(Mat2,Mat3,-1);
//	imwrite(result_name_right,Mat3);
//	Mat1.release();
//	Mat2.release();
//	Mat3.release();
//	LOGE("CYG End imwrite...");
//	return x;
//}

//JNIEXPORT void JNICALL Java_jp_co_ricoh_cheetah_app_jni_Stitch_plus(
//    JNIEnv* env, jobject obj, jstring path1, jstring path2) {
//
//	String strPath1 = jstringTostring(env,path1);
//	String strPath2 = jstringTostring(env,path2);
//
//	LOGE("strPath1=%s", strPath1.c_str());
//	LOGE("strPath2=%s", strPath2.c_str());
//
//
//	double scale=0.5;
////	vector<int> compression_params;
////	compression_params.push_back(CV_IMWRITE_JPEG_QUALITY);
////	compression_params.push_back(60);
//
//	Mat img=imread(strPath1);
////	Size dsize = Size(img.cols*scale,img.rows*scale);
////	Mat imgZoom = Mat(dsize,CV_32S);
////	resize(img,imgZoom,dsize);
////	//Mat img=imread("/mnt/hdd/file1.jpg");
////	imgs.push_back(imgZoom);
//	imgs.push_back(img);
//
//	img=imread(strPath2);
////	dsize = Size(img.cols*scale,img.rows*scale);
////	imgZoom = Mat(dsize,CV_32S);
////	resize(img,imgZoom,dsize);
////	imgs.push_back(imgZoom);
//	imgs.push_back(img);
//
////	Mat dst;
////	flip(img,dst,-1);
//
////	imwrite(result_file2_180,dst,compression_params);
//
//	//img=imread("/mnt/hdd/file2.jpg");
//
//	Mat pano;
//	Stitcher stitcher = Stitcher::createDefault(try_use_gpu);
//	Stitcher::Status status = stitcher.stitch(imgs, pano);
//
//	if (status != Stitcher::OK)
//	{
//		LOGE("stitcher.stitch() failed ! error=%d", int(status));
//	}
//	else
//	{
//		imwrite(result_name, pano);
//		LOGE("stitcher.stitch() success ! error=%d", 0);
//	}
//
//}

//JNIEXPORT void JNICALL Java_jp_co_ricoh_cheetah_app_jni_Stitch_StitchOpencv(
//		JNIEnv* env, jobject obj, jstring path1, jstring path2)
//{
//	String strPath1 = jstringTostring(env,path1);
//	String strPath2 = jstringTostring(env,path2);
//
//    cv::setBreakOnError(true);
//	int num_images = 2;
//    double work_scale = 1, seam_scale = 1, compose_scale = 1;
//    bool is_work_scale_set = false, is_seam_scale_set = false, is_compose_scale_set = false;
//
//	LOGE("CYG Finding features...");
//
//#if ENABLE_LOG
//    int64 t = getTickCount();
//#endif
//
//    Ptr<FeaturesFinder> finder;
//    if (features_type == "surf")
//    {
////#if defined(HAVE_OPENCV_NONFREE) && defined(HAVE_OPENCV_GPU)
////        if (try_gpu && gpu::getCudaEnabledDeviceCount() > 0)
////            finder = new SurfFeaturesFinderGpu();
////        else
////#endif
//    	initModule_nonfree();
//    	finder = new SurfFeaturesFinder();
//    }
//    else if (features_type == "orb")
//    {
//        finder = new OrbFeaturesFinder(Size(3,1), 1500, 1.3f, 5);
//    }
//    else
//    {
//        cout << "Unknown 2D features type: '" << features_type << "'.\n";
//        return;
//    }
//
//    Mat full_img, img;
//    vector<ImageFeatures> features(num_images);
//    vector<Mat> images(num_images);
//    vector<Size> full_img_sizes(num_images);
//    double seam_work_aspect = 1;
//
//	#pragma omp parallel for
//	for (int i = 0; i < num_images; ++i)
//    {
//		if (i == 0)
//		{
//			full_img = imread(strPath1);
//		}
//		else
//		{
//			full_img = imread(strPath2);
//		}
//        full_img_sizes[i] = full_img.size();
//
//        if (full_img.empty())
//        {
//            LOGE("CYG Can't open image ");
//            return;
//        }
//        if (work_megapix < 0)
//        {
//            img = full_img;
//            work_scale = 1;
//            is_work_scale_set = true;
//        }
//        else
//        {
//            if (!is_work_scale_set)
//            {
//                work_scale = min(1.0, sqrt(work_megapix * 1e6 / full_img.size().area()));
//                is_work_scale_set = true;
//            }
//            resize(full_img, img, Size(), work_scale, work_scale);
//        }
//        if (!is_seam_scale_set)
//        {
//            seam_scale = min(1.0, sqrt(seam_megapix * 1e6 / full_img.size().area()));
//            seam_work_aspect = seam_scale / work_scale;
//            is_seam_scale_set = true;
//        }
//
//        (*finder)(img, features[i]);
//        features[i].img_idx = i;
//        LOGLN("Features in image #" << i+1 << ": " << features[i].keypoints.size());
//
//        resize(full_img, img, Size(), seam_scale, seam_scale);
//        images[i] = img.clone();
//    }
//
////    finder->collectGarbage();
//    full_img.release();
//    img.release();
//
//    LOGLN("Finding features, time: " << ((getTickCount() - t) / getTickFrequency()) << " sec");
//
//    LOGE("CYG Pairwise matching");
//#if ENABLE_LOG
//    t = getTickCount();
//#endif
////    vector<MatchesInfo> pairwise_matches;
////    BestOf2NearestMatcher matcher(try_gpu, match_conf);
////    matcher(features, pairwise_matches);
////    matcher.collectGarbage();
//
//    vector<MatchesInfo> pairwise_matches;
//    BestOf2NearestMatcher matcher(try_gpu, match_conf);
//    Mat matchMask(features.size(),features.size(),CV_8U,Scalar(0));
//    for (int i = 0; i < num_images -1; ++i)
//    {
//    	matchMask.at<char>(i,i+1) =1;
//    }
//    matcher(features, pairwise_matches,matchMask);
//    matcher.collectGarbage();
//
//    LOGLN("Pairwise matching, time: " << ((getTickCount() - t) / getTickFrequency()) << " sec");
//
//    // Check if we should save matches graph
//    if (save_graph)
//    {
//        LOGE("CYG Saving matches graph...");
//        ofstream f(save_graph_to.c_str());
//        f << matchesGraphAsString(img_names, pairwise_matches, conf_thresh);
//    }
//
//
//    // Leave only images we are sure are from the same panorama
//    vector<int> indices = leaveBiggestComponent(features, pairwise_matches, conf_thresh);
//    vector<Mat> img_subset;
//    vector<string> img_names_subset;
//    vector<Size> full_img_sizes_subset;
//
//    LOGE("CYG Pairwise matching -- 2");
//
//    for (size_t i = 0; i < indices.size(); ++i)
//    {
//        //img_names_subset.push_back(img_names[indices[i]]);
//        img_subset.push_back(images[indices[i]]);
//        full_img_sizes_subset.push_back(full_img_sizes[indices[i]]);
//    }
//
//    images = img_subset;
//    //img_names = img_names_subset;
//    full_img_sizes = full_img_sizes_subset;
//
//    // Check if we still have enough images
//    //num_images = static_cast<int>(img_names.size());
//    //if (num_images < 2)
//    //{
//    //    LOGLN("Need more images");
//    //    return -1;
//    //}
//    LOGE("features [0] size = %d",features[0].keypoints.size());
//    LOGE("features [1] size = %d",features[1].keypoints.size());
//
//    LOGE("pairwise_matches [0] size = %d",pairwise_matches[0].matches.size());
//    LOGE("pairwise_matches [1] size = %d",pairwise_matches[1].matches.size());
//
//    if (pairwise_matches[0].matches.size() == 0 && pairwise_matches[1].matches.size() == 0) return;
//
//    HomographyBasedEstimator estimator;
//    vector<CameraParams> cameras;
//
//    LOGE("CYG Pairwise matching -- 3");
//
//    estimator(features, pairwise_matches, cameras);
//
//    LOGE("CYG Pairwise matching -- 4");
//
//    for (size_t i = 0; i < cameras.size(); ++i)
//    {
//        Mat R;
//        cameras[i].R.convertTo(R, CV_32F);
//        cameras[i].R = R;
//        LOGLN("Initial intrinsics #" << indices[i]+1 << ":\n" << cameras[i].K());
//    }
//
//    Ptr<detail::BundleAdjusterBase> adjuster;
//    if (ba_cost_func == "reproj") adjuster = new detail::BundleAdjusterReproj();
//    else if (ba_cost_func == "ray") adjuster = new detail::BundleAdjusterRay();
//    else
//    {
//        cout << "Unknown bundle adjustment cost function: '" << ba_cost_func << "'.\n";
//        return;
//    }
//    adjuster->setConfThresh(conf_thresh);
//    Mat_<uchar> refine_mask = Mat::zeros(3, 3, CV_8U);
//    if (ba_refine_mask[0] == 'x') refine_mask(0,0) = 1;
//    if (ba_refine_mask[1] == 'x') refine_mask(0,1) = 1;
//    if (ba_refine_mask[2] == 'x') refine_mask(0,2) = 1;
//    if (ba_refine_mask[3] == 'x') refine_mask(1,1) = 1;
//    if (ba_refine_mask[4] == 'x') refine_mask(1,2) = 1;
//    adjuster->setRefinementMask(refine_mask);
//    (*adjuster)(features, pairwise_matches, cameras);
//
//    // Find median focal length
//
//    vector<double> focals;
//    for (size_t i = 0; i < cameras.size(); ++i)
//    {
//        LOGLN("Camera #" << indices[i]+1 << ":\n" << cameras[i].K());
//        focals.push_back(cameras[i].focal);
//    }
//
//    sort(focals.begin(), focals.end());
//    float warped_image_scale;
//    if (focals.size() % 2 == 1)
//        warped_image_scale = static_cast<float>(focals[focals.size() / 2]);
//    else
//        warped_image_scale = static_cast<float>(focals[focals.size() / 2 - 1] + focals[focals.size() / 2]) * 0.5f;
//
//    if (do_wave_correct)
//    {
//        vector<Mat> rmats;
//        for (size_t i = 0; i < cameras.size(); ++i)
//            rmats.push_back(cameras[i].R);
//        waveCorrect(rmats, wave_correct);
//        for (size_t i = 0; i < cameras.size(); ++i)
//            cameras[i].R = rmats[i];
//    }
//
//    LOGE("CYG Warping images (auxiliary)... ");
//#if ENABLE_LOG
//    t = getTickCount();
//#endif
//
//    vector<Point> corners(num_images);
//    vector<Mat> masks_warped(num_images);
//    vector<Mat> images_warped(num_images);
//    vector<Size> sizes(num_images);
//    vector<Mat> masks(num_images);
//
//    // Preapre images masks
//    for (int i = 0; i < num_images; ++i)
//    {
//        masks[i].create(images[i].size(), CV_8U);
//        masks[i].setTo(Scalar::all(255));
//    }
//
//    // Warp images and their masks
//
//    Ptr<WarperCreator> warper_creator;
//#if defined(HAVE_OPENCV_GPU)
//    if (try_gpu && gpu::getCudaEnabledDeviceCount() > 0)
//    {
//        if (warp_type == "plane") warper_creator = new cv::PlaneWarperGpu();
//        else if (warp_type == "cylindrical") warper_creator = new cv::CylindricalWarperGpu();
//        else if (warp_type == "spherical") warper_creator = new cv::SphericalWarperGpu();
//    }
//    else
//#endif
//    {
//        if (warp_type == "plane") warper_creator = new cv::PlaneWarper();
//        else if (warp_type == "cylindrical") warper_creator = new cv::CylindricalWarper();
//        else if (warp_type == "spherical") warper_creator = new cv::SphericalWarper();
//        else if (warp_type == "fisheye") warper_creator = new cv::FisheyeWarper();
//        else if (warp_type == "stereographic") warper_creator = new cv::StereographicWarper();
//        else if (warp_type == "compressedPlaneA2B1") warper_creator = new cv::CompressedRectilinearWarper(2, 1);
//        else if (warp_type == "compressedPlaneA1.5B1") warper_creator = new cv::CompressedRectilinearWarper(1.5, 1);
//        else if (warp_type == "compressedPlanePortraitA2B1") warper_creator = new cv::CompressedRectilinearPortraitWarper(2, 1);
//        else if (warp_type == "compressedPlanePortraitA1.5B1") warper_creator = new cv::CompressedRectilinearPortraitWarper(1.5, 1);
//        else if (warp_type == "paniniA2B1") warper_creator = new cv::PaniniWarper(2, 1);
//        else if (warp_type == "paniniA1.5B1") warper_creator = new cv::PaniniWarper(1.5, 1);
//        else if (warp_type == "paniniPortraitA2B1") warper_creator = new cv::PaniniPortraitWarper(2, 1);
//        else if (warp_type == "paniniPortraitA1.5B1") warper_creator = new cv::PaniniPortraitWarper(1.5, 1);
//        else if (warp_type == "mercator") warper_creator = new cv::MercatorWarper();
//        else if (warp_type == "transverseMercator") warper_creator = new cv::TransverseMercatorWarper();
//    }
//
//    if (warper_creator.empty())
//    {
//        cout << "Can't create the following warper '" << warp_type << "'\n";
//        return;
//    }
//
//    Ptr<RotationWarper> warper = warper_creator->create(static_cast<float>(warped_image_scale * seam_work_aspect));
//
//    for (int i = 0; i < num_images; ++i)
//    {
//        Mat_<float> K;
//        cameras[i].K().convertTo(K, CV_32F);
//        float swa = (float)seam_work_aspect;
//        K(0,0) *= swa; K(0,2) *= swa;
//        K(1,1) *= swa; K(1,2) *= swa;
//
//        corners[i] = warper->warp(images[i], K, cameras[i].R, INTER_LINEAR, BORDER_REFLECT, images_warped[i]);
//        sizes[i] = images_warped[i].size();
//
//        warper->warp(masks[i], K, cameras[i].R, INTER_NEAREST, BORDER_CONSTANT, masks_warped[i]);
//    }
//
//    vector<Mat> images_warped_f(num_images);
//    for (int i = 0; i < num_images; ++i)
//        images_warped[i].convertTo(images_warped_f[i], CV_32F);
//
//    LOGLN("Warping images, time: " << ((getTickCount() - t) / getTickFrequency()) << " sec");
//
//    Ptr<ExposureCompensator> compensator = ExposureCompensator::createDefault(expos_comp_type);
//    compensator->feed(corners, images_warped, masks_warped);
//
//    Ptr<SeamFinder> seam_finder;
//    if (seam_find_type == "no")
//        seam_finder = new detail::NoSeamFinder();
//    else if (seam_find_type == "voronoi")
//        seam_finder = new detail::VoronoiSeamFinder();
//    else if (seam_find_type == "gc_color")
//    {
////#if defined(HAVE_OPENCV_GPU)
////        if (try_gpu && gpu::getCudaEnabledDeviceCount() > 0)
////            seam_finder = new detail::GraphCutSeamFinderGpu(GraphCutSeamFinderBase::COST_COLOR);
////        else
////#endif
//            seam_finder = new detail::GraphCutSeamFinder(GraphCutSeamFinderBase::COST_COLOR);
//    }
//    else if (seam_find_type == "gc_colorgrad")
//    {
////#if defined(HAVE_OPENCV_GPU)
////        if (try_gpu && gpu::getCudaEnabledDeviceCount() > 0)
////            seam_finder = new detail::GraphCutSeamFinderGpu(GraphCutSeamFinderBase::COST_COLOR_GRAD);
////        else
////#endif
//            seam_finder = new detail::GraphCutSeamFinder(GraphCutSeamFinderBase::COST_COLOR_GRAD);
//    }
//    else if (seam_find_type == "dp_color")
//        seam_finder = new detail::DpSeamFinder(DpSeamFinder::COLOR);
//    else if (seam_find_type == "dp_colorgrad")
//        seam_finder = new detail::DpSeamFinder(DpSeamFinder::COLOR_GRAD);
//    if (seam_finder.empty())
//    {
//        cout << "Can't create the following seam finder '" << seam_find_type << "'\n";
//        return;
//    }
//    seam_finder->find(images_warped_f, corners, masks_warped);
//
//    // Release unused memory
//    images.clear();
//    images_warped.clear();
//    images_warped_f.clear();
//    masks.clear();
//
//    LOGE("CYG Compositing...");
//#if ENABLE_LOG
//    t = getTickCount();
//#endif
//
//    Mat img_warped, img_warped_s;
//    Mat dilated_mask, seam_mask, mask, mask_warped;
//    Ptr<Blender> blender;
//    //double compose_seam_aspect = 1;
//    double compose_work_aspect = 1;
//
//	#pragma omp parallel for
//    for (int img_idx = 0; img_idx < num_images; ++img_idx)
//    {
//        LOGLN("Compositing image #" << indices[img_idx]+1);
//
//        // Read image and resize it if necessary
//		if (img_idx == 0)
//		{
//			full_img = imread(strPath1);
//		}
//		else
//		{
//			full_img = imread(strPath2);
//		}
//        if (!is_compose_scale_set)
//        {
//            if (compose_megapix > 0)
//                compose_scale = min(1.0, sqrt(compose_megapix * 1e6 / full_img.size().area()));
//            is_compose_scale_set = true;
//
//            // Compute relative scales
//            //compose_seam_aspect = compose_scale / seam_scale;
//            compose_work_aspect = compose_scale / work_scale;
//
//            // Update warped image scale
//            warped_image_scale *= static_cast<float>(compose_work_aspect);
//            warper = warper_creator->create(warped_image_scale);
//
//            // Update corners and sizes
//            for (int i = 0; i < num_images; ++i)
//            {
//                // Update intrinsics
//                cameras[i].focal *= compose_work_aspect;
//                cameras[i].ppx *= compose_work_aspect;
//                cameras[i].ppy *= compose_work_aspect;
//
//                // Update corner and size
//                Size sz = full_img_sizes[i];
//                if (std::abs(compose_scale - 1) > 1e-1)
//                {
//                    sz.width = cvRound(full_img_sizes[i].width * compose_scale);
//                    sz.height = cvRound(full_img_sizes[i].height * compose_scale);
//                }
//
//                Mat K;
//                cameras[i].K().convertTo(K, CV_32F);
//                Rect roi = warper->warpRoi(sz, K, cameras[i].R);
//                corners[i] = roi.tl();
//                sizes[i] = roi.size();
//            }
//        }
//        if (abs(compose_scale - 1) > 1e-1)
//            resize(full_img, img, Size(), compose_scale, compose_scale);
//        else
//            img = full_img;
//        full_img.release();
//        Size img_size = img.size();
//
//        Mat K;
//        cameras[img_idx].K().convertTo(K, CV_32F);
//
//        // Warp the current image
//        warper->warp(img, K, cameras[img_idx].R, INTER_LINEAR, BORDER_REFLECT, img_warped);
//
//        // Warp the current image mask
//        mask.create(img_size, CV_8U);
//        mask.setTo(Scalar::all(255));
//        warper->warp(mask, K, cameras[img_idx].R, INTER_NEAREST, BORDER_CONSTANT, mask_warped);
//
//        // Compensate exposure
//        compensator->apply(img_idx, corners[img_idx], img_warped, mask_warped);
//
//        img_warped.convertTo(img_warped_s, CV_16S);
//        img_warped.release();
//        img.release();
//        mask.release();
//        K.release();   //CYG
//
//        dilate(masks_warped[img_idx], dilated_mask, Mat());
//        resize(dilated_mask, seam_mask, mask_warped.size());
//        mask_warped = seam_mask & mask_warped;
//
//        if (blender.empty())
//        {
//            blender = Blender::createDefault(blend_type, try_gpu);
//            Size dst_sz = resultRoi(corners, sizes).size();
//            float blend_width = sqrt(static_cast<float>(dst_sz.area())) * blend_strength / 100.f;
//            if (blend_width < 1.f)
//                blender = Blender::createDefault(Blender::NO, try_gpu);
//            else if (blend_type == Blender::MULTI_BAND)
//            {
//                MultiBandBlender* mb = dynamic_cast<MultiBandBlender*>(static_cast<Blender*>(blender));
//                mb->setNumBands(static_cast<int>(ceil(log(blend_width)/log(2.)) - 1.));
//                LOGLN("Multi-band blender, number of bands: " << mb->numBands());
//            }
//            else if (blend_type == Blender::FEATHER)
//            {
//                FeatherBlender* fb = dynamic_cast<FeatherBlender*>(static_cast<Blender*>(blender));
//                fb->setSharpness(1.f/blend_width);
//                LOGLN("Feather blender, sharpness: " << fb->sharpness());
//            }
//            blender->prepare(corners, sizes);
//        }
//
//        // Blend the current image
//        blender->feed(img_warped_s, mask_warped, corners[img_idx]);
//    }
//
//    Mat result, result_mask;
//    blender->blend(result, result_mask);
//
//    LOGLN("Compositing, time: " << ((getTickCount() - t) / getTickFrequency()) << " sec");
//
//    imwrite(result_name, result);
//
//	LOGE("CYG End imwrite...");
//
//}


