/*************************************************************************
*   Author: Zhong Chong
*   Date:   2015-10-1
*   Description: The file implement class AliscIntf
**************************************************************************/

#include "alisc_intf.hpp"
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <cmath>
#include <fstream>

using namespace cv;
using namespace std;

AliscIntf* AliscIntf::_pInstance = NULL;

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

    int dim[] = {8,12,3};
    Mat hist(3, dim, CV_32SC(5), (void*)feat_buf);

    //convert color space from BGR to HSV
    cvtColor(mat, mat, CV_BGR2HSV);

    //get the center point, the size of the image, caculate axes
    Point center = Point(mat.cols/2, mat.rows/2);
    Size size = mat.size();
    Size axes = Size( int(size.width*0.75)/2, int(size.height*0.75)/2 );

    //define features mat and argument
    vector<Mat> histvec(5);
    float h_range[] = {0,180}, s_range[] = {0, 255}, v_range[] = {0,255};
    const float *histrange[] = {h_range, s_range, v_range};
    int h_bins = 8, s_bins = 12, v_bins = 3;
    int histsize[] = {h_bins, s_bins, v_bins};
    int histchannels[] = {0, 1, 2};

    //define ellipmask mat and extract feature
    Mat ellipmask = Mat( size, CV_8UC1, Scalar(0));
    ellipse(ellipmask, center, axes, 0, 0, 360, Scalar(255), -1);
    calcHist( &mat, 1, histchannels, ellipmask, histvec.at(4), 3, histsize, histrange);

    //define cornermask mat and extract feature
    int subsize[4][4] = {{0,0,center.x,center.y}, {center.x,0,size.width,center.y},
        {0,center.y,center.x,size.height}, {center.x,center.y,size.width,size.height}};
    for(int i=0; i<4; ++i)
    {
        Mat cornermask = Mat(size,CV_8UC1, Scalar(0));
        rectangle( cornermask, Point( subsize[i][0], subsize[i][1] ), Point( subsize[i][2], subsize[i][3] ),
            Scalar(255), -1);
        subtract( cornermask, ellipmask, cornermask );
        calcHist(&mat, 1, histchannels, cornermask, histvec.at(i), 3, histsize, histrange);
    }
    Mat tmp;
    merge(histvec, tmp);
    tmp.convertTo(hist, CV_32SC(5));

    return 1440;
}

float AliscIntf::match(const int* feat_buf1, const int* feat_buf2, long buf_size)
{
    int dimsize[] = {8,12,3};
    Mat featA(3,dimsize,CV_32SC(5),(void*)feat_buf1);
    Mat featB(3,dimsize,CV_32SC(5),(void*)feat_buf2);
    Mat feat1, feat2;
    featA.convertTo(feat1,CV_32F);
    featB.convertTo(feat2,CV_32F);
    vector<Mat> feat1vec, feat2vec;
    split(feat1, feat1vec);
    split(feat2, feat2vec);
    float res = 0;
    for(int i=0; i<5; ++i)
    {
        res += compareHist(feat1vec.at(i), feat2vec.at(i),  CV_COMP_CORREL);
    }
    res = 1 - res /5;
    return res;
}

int AliscIntf::init(const string& param)
{
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
