/*************************************************************************
*   Author: Zhong Chong
*   Date:   2015-10-1
*   Description: The file implement class AliscIntf
**************************************************************************/

#include "alisc_intf.hpp"

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/nonfree/nonfree.hpp>

#include <cmath>
#include <fstream>
#include <iostream>
#include <vector>

using namespace cv;
using namespace std;

AliscIntf* AliscIntf::_pInstance = NULL;
string AliscIntf::dictionaryPath = "";

AliscIntf::AliscIntf()
{

}

AliscIntf::~AliscIntf()
{

}

int AliscIntf::extract(const char* img_buf, long img_size, int* feat_buf, long max_size)
{
    //check argument valid, init opencv mat
    if(img_buf == NULL || feat_buf == NULL)
        return -1;
    vector<char> buff(img_buf, img_buf+img_size);
    Mat mat = imdecode(Mat(buff),CV_LOAD_IMAGE_COLOR);
    Mat descriptor;
    Mat features = Mat(1,2000,CV_32SC1,(void*)feat_buf);
    vector<KeyPoint> keypoints;

    Mat dictionary;
    FileStorage fs(dictionaryPath, FileStorage::READ);
    fs["dictionary"] >> dictionary;
    fs.release();

    Ptr<DescriptorMatcher> matcher(new FlannBasedMatcher);
    Ptr<FeatureDetector> detector(new SiftFeatureDetector());
    Ptr<DescriptorExtractor> extractor(new SiftDescriptorExtractor);
    BOWImgDescriptorExtractor bowDE(extractor,matcher);
    bowDE.setVocabulary(dictionary);

    detector->detect(mat,keypoints);
    bowDE.compute(mat,keypoints,descriptor);
    descriptor.convertTo(features,CV_32S,1e10);

    return 2000;
}

float AliscIntf::match(const int* feat_buf1, const int* feat_buf2, long buf_size)
{
    Mat featA(1,2000,CV_32SC1,(void*)feat_buf1);
    Mat featB(1,2000,CV_32SC1,(void*)feat_buf2);
    Mat feat1, feat2;
    featA.convertTo(feat1,CV_32F,1e-10);
    featB.convertTo(feat2,CV_32F,1e-10);

    float res = norm(featA,featB,NORM_L2);
    return res;
}

int AliscIntf::init(const string& param)
{
    dictionaryPath = param+"/dictionary.xml";
    return 0;
}

int AliscIntf::release()
{
    return 0;
}

extern "C" {
  AliscIntf* create() {
    if (NULL == AliscIntf::_pInstance) {
      AliscIntf::_pInstance = new AliscIntf();
    }
    return AliscIntf::_pInstance;
  }
  void destory(AliscIntf* p) {
    if(p != NULL) {
      p->release();
      delete p;
      p = NULL;
    }
  }
}
