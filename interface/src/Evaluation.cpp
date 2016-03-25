/*************************************************************************
*   Author: Zhong Chong
*   Date:   2015-10-12
*   Description: The file implement program Evaluation main function
**************************************************************************/
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

string valid_query = "../../data/eval_tags/valid_image.txt";
string match_query = "../../data/eval_tags/match";

bool check(string &s1, string &s2)
{
    int pos1=0, pos2=0;
    pos1 = s1.find(",");
    pos2 = s2.find(",");
    if( pos1 == s1.npos || pos2 == s2.npos)
        return false;
    if(s1.substr(0,pos1) == s2.substr(0,pos2))
    {
        s1 = s1.substr(pos1+1,s1.size()-pos1-1);
        s2 = s2.substr(pos2+1,s2.size()-pos2-1);
        return true;
    }
    return false;
}

int split(string s1, vector<string> &vec)
{
    int pos = 0;
    vec.clear();
    pos = s1.find(";");
    while(pos != s1.npos)
    {
        string substr = s1.substr(0,pos);
        if( !substr.empty() )
            vec.push_back(substr);
        s1 = s1.substr(pos+1,s1.size()-pos-1);
        pos = s1.find(";");
    }
    if( !s1.empty() )
        vec.push_back(s1);
    return 0;
}

double calculate(vector<string> &valid, vector<string> &match)
{
    double res = 0.0;
    int count = 0;
    for(int i = 0; i<match.size(); ++i)
    {
        string matchID = match.at(i);
        for(int j=0; j<valid.size(); ++j)
        {
            string validID = valid.at(j);
            if(validID == matchID)
            {
                ++count;
                res += (double)count/(double)(i+1);
                break;
            }
        }
    }
    if( valid.size()<=20 )
        res /= (double)valid.size();
    else
        res /= 20.0;
    return res;
}

int main()
{
    double mAP = 0.0;
    int count = 0;
    vector<string> match, valid;
    fstream fs1,fs2;

    fs1.open(valid_query, ios::in);
    fs2.open(match_query, ios::in);

    while( !fs1.eof() && !(fs2.eof()) )
    {
        string s1, s2;
        fs1>>s1;
        fs2>>s2;

        if( check(s1,s2))
        {
            split(s1,valid);
            split(s2,match);
            double ap = calculate(valid,match);
            mAP += ap;
            ++count;
        }
    }
    if( 0 != count)
        mAP /= count;
    cout<<"MAP = "<<mAP<<endl;

    fs1.close();
    fs2.close();

    return 0;
}
