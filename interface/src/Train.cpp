/*************************************************************************
*   Author: Zhong Chong
*   Date:   2015-10-16
*   Description: The file implement Train main function
**************************************************************************/
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/nonfree/nonfree.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/features2d/features2d.hpp>

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <ctime>

using namespace std;
using namespace cv;

string eval_image = "../../data/eval_tags/eval_image.txt";
string evalPath = "../../data/eval_image/";

int train(Mat &descriptor)
{
    int dictionarySize=2000;
    TermCriteria tc(CV_TERMCRIT_ITER,100,0.001);
    int retries=1;
    int flags=KMEANS_PP_CENTERS;
    BOWKMeansTrainer bowTrainer(dictionarySize,tc,retries,flags);

    Mat dictionary=bowTrainer.cluster(descriptor);

    FileStorage fs("../../model/dictionary.xml", FileStorage::WRITE);
    fs << "dictionary" << dictionary;
    fs.release();

    return 0;
}

int main()
{
    fstream fs;
    fs.open(eval_image, ios::in);

    if( !fs.is_open() )
    {
        cerr<<"eval_image file failed to open"<<endl;
        cerr<<eval_image<<endl;
        return -1;
    }

    string imgid;
    int count = 0;
    Mat uncluster;
    srand(time(0));

    Mat mat;
    SIFT sift;
    vector<KeyPoint> kp;
    Mat descriptor;

    cout<<"training start..."<<endl;

    while( !fs.eof() )
    {
        imgid.clear();
        descriptor = Mat();

        fs>>imgid;
        mat = imread( evalPath + imgid + ".jpg");
        if( !mat.data && 0 == rand()%50)
        {
            sift.detect(mat, kp);
            sift.compute(mat, kp, descriptor);
            uncluster.push_back(descriptor);
            ++count;
        }
    }
    cout<<"total train image number == "<<count<<endl;
    fs.close();
    train(uncluster);
    cout<<"training done..."<<endl;
    return 0;
}
