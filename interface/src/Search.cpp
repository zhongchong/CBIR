/*************************************************************************
*   Author: Zhong Chong
*   Date:   2015-10-1
*   Description: The file implement Search main function
**************************************************************************/
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <utility>
#include <cstring>
#include <list>

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "alisc_intf.hpp"

using namespace std;
using namespace cv;

string query_image = "../../data/eval_tags/query_image.txt";
string queryPath = "../../data/query_image/";
string eval_image = "../../data/eval_tags/eval_image.txt";
string evalPath = "../../data/eval_image/";
string featPath = "../../data/eval_feat/";
string match_image = "../../data/eval_tags/match";

const int MAX_FEAT_SIZE = 2001;
bool isStorage = false;

AliscIntf *p = create();

struct Rank
{
    string imgid;
    float score;
    Rank(){}
    Rank(string a, float b):imgid(a),score(b){}
};

void write(string &imgid, int *feat_buff, int feat_size)
{
    fstream fs;
    fs.open(featPath+imgid+".txt",ios::out);
    for(int i=0;i<feat_size;++i)
    {
        fs<<feat_buff[i]<<endl;
    }
    fs.close();
}

void read(string &imgid, int *feat_buff, int feat_size)
{
    fstream fs;
    fs.open(featPath+imgid+".txt",ios::in);
    for(int i=0;i<feat_size;++i)
    {
        fs>>feat_buff[i];
    }
    fs.close();
}

int storage()
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
    int feat_size = 0;
    int *feat_buff = new int[MAX_FEAT_SIZE];

    Mat mat;
    vector<uchar> img_buff;
    vector<int> param(2);
    param[0] = CV_IMWRITE_JPEG_QUALITY;
    param[1] = 95;

    cout<<"step1->image storage start..."<<endl;
    while( !fs.eof() )
    {
        string imgid;
        fs>>imgid;
        mat = imread(evalPath+imgid+".jpg");
        if(mat.data)
        {
            imencode(".jpg", mat, img_buff, param);
            feat_size = p->extract((const char*)img_buff.data(), img_buff.size(), feat_buff, MAX_FEAT_SIZE);
            write(imgid,feat_buff,feat_size);
        }
    }

    cout<<"step1->image storage done..."<<endl;
    isStorage = false;
    delete[] feat_buff;
    return 0;
}

int updateRank(list<Rank> &imgrank, string &imgid, float &score)
{
    auto it= imgrank.begin();
    for( ;it != imgrank.end(); ++it)
    {
        if( (*it).score > score)
            break;
    }
    if( it == imgrank.end() )
    {
        if(imgrank.size() < 20)
            imgrank.push_back(Rank(imgid,score));
    }
    else
    {
        imgrank.insert(it, Rank(imgid, score));
        if(imgrank.size() > 20)
            imgrank.resize(20);
    }
    return 0;
}

int writeDown(list<Rank> &imgrank, string &imgid)
{
    fstream fs;
    fs.open(match_image, ios::out | ios::app);
    fs<<imgid<<",";
    for(auto it = imgrank.begin(); it!=imgrank.end(); ++it)
    {
        if(it == imgrank.begin())
            fs<<(*it).imgid;
        else
            fs<<";"<<(*it).imgid;
    }
    fs<<endl;
    fs.close();
    return 0;
}

int match(string &queryid)
{
    fstream fs;
    fs.open(eval_image, ios::in);
    if( !fs.is_open() )
    {
        cerr<<"eval_image file failed to open"<<endl;
        cerr<<eval_image<<endl;
        return -1;
    }

    list<Rank> imgrank;
    string imgid;
    int *feat_buff1 = new int[MAX_FEAT_SIZE];
    int *feat_buff2 = new int[MAX_FEAT_SIZE];
    int feat_size = 0;

    Mat mat;
    vector<uchar> img_buff;
    vector<int> param(2);
    param[0] = CV_IMWRITE_JPEG_QUALITY;
    param[1] = 95;

    float score = -1;

    mat = imread(queryPath+queryid+".jpg");
    if( !mat.data )
    {
        cerr<<"query_image file failed to read"<<endl;
        return -1;
    }
    imencode(".jpg", mat, img_buff, param);
    memset(feat_buff1, sizeof(int)*MAX_FEAT_SIZE, 0);
    feat_size = p->extract((const char*)img_buff.data(), img_buff.size(), feat_buff1, MAX_FEAT_SIZE);

    while( !fs.eof() )
    {
        string imgid;
        memset(feat_buff2, sizeof(int)*MAX_FEAT_SIZE, 0);
        score = -1;

        fs>>imgid;
        if( !imgid.empty() )
        {
            read(imgid,feat_buff2,feat_size);
            score = p->match(feat_buff1, feat_buff2, feat_size);
            if(score != -1)
                updateRank(imgrank, imgid, score);
        }
    }

    writeDown(imgrank,queryid);

    delete[] feat_buff1;
    delete[] feat_buff2;
    fs.close();

    return 0;
}



int main( int argc, char **argv)
{
    fstream fs;
    fs.open(query_image, ios::in);

    p->init("../../model");
    if( isStorage )
        storage();
    else
        cout<<"step1->image storage has done..."<<endl;

    if( !fs.is_open() )
    {
        cerr<<"query_image file failed to open"<<endl;
        cerr<<query_image<<endl;
        return -1;
    }

    cout<<"step2->image retrieval start..."<<endl;

    while( !fs.eof() )
    {
        string imgid;
        fs>>imgid;
        int pos = imgid.find(",");
        if( pos != imgid.npos )
            imgid = imgid.substr(0, pos);

        if( !imgid.empty() )
        {
            match(imgid);
        }
    }
    fs.close();
    cout<<"step2->image retrieval done..."<<endl;
    destory(p);
    return 0;
}
