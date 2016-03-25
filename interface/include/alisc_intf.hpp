/*************************************************************************
*   Author: Zhong Chong
*   Date:   2015-10-1
*   Description: The file is class AliscIntf header file
**************************************************************************/
#pragma once
#include <string>

using namespace std;

class AliscIntf {

public:
  explicit AliscIntf();
  virtual ~AliscIntf();

  static AliscIntf* _pInstance;
  static string dictionaryPath;

  int init(const string& param);

  /**
   * extract features
   * @return feat_buf size
   */
  int extract(const char* img_buf, long img_size, int* feat_buf, long max_size);

  /**
   * less value, higher rank
   * @return match value
   */
  float match(const int* feat_buf1, const int* feat_buf2, long buf_size);

  int release();

protected:
  // you can add extra functions here
  // int your_function();
};

extern "C" {
    extern void destory(AliscIntf* p);
    extern AliscIntf* create();
}

typedef AliscIntf* create_t();
typedef void destory_t(AliscIntf* p);
