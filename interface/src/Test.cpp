/*************************************************************************
*   Author: Zhong Chong
*   Date:   2015-10-8
*   Description: The file implement Test main function
**************************************************************************/

#include "alisc_intf.hpp"
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <fstream>
#include <iostream>
#include <cstdio>

using namespace cv;
using namespace std;

const int max_feat_size = 200*128;

int main()
{
    int *feat_buff1 = new int[max_feat_size];
    int *feat_buff2 = new int[max_feat_size];
    int feat_size = 0;
    string imgid;

    Mat mat1,mat2;
    vector<uchar> img_buff1,img_buff2;
    vector<int> param(2);
    param[0] = CV_IMWRITE_JPEG_QUALITY;
    param[1] = 95;

    AliscIntf *p = create();
    p->init("../../model");

    mat1 = imread("../../data/query_image/337100001963.jpg");
    imencode(".jpg", mat1, img_buff1, param);
    feat_size = p->extract((const char*)img_buff1.data(), img_buff1.size(), feat_buff1, max_feat_size);

    mat2 = imread("../../data/eval_image/337000944245.jpg");
    imencode(".jpg", mat2, img_buff2, param);
    feat_size = p->extract((const char*)img_buff2.data(), img_buff2.size(), feat_buff2, max_feat_size);


    float res = p->match(feat_buff1, feat_buff2, feat_size);
    printf("匹配结果%.6f\n",res);

/*
    fstream fs;
    fs.open("features.txt",ios::out);
    for(int i=0;i<feat_size;++i)
    {
        fs<<feat_buff[i]<<endl;
    }
    fs.close();
*/

    delete[] feat_buff1;
    delete[] feat_buff2;
    destory(p);
}
