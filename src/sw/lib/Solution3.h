#ifndef SOLUTION3_H_
#define SOLUTION3_H_

#ifndef __cplusplus
#error a C++ compiler is required
#endif

#include <cstring>
#include <sstream>
#include "Solution.h"

using namespace std;

#define Q_MAX 512
#define T_MAX 1024


class Solution3 : public Solution {
public:
  Solution3();
  ~Solution3();

  virtual int process(const string& b, const string& a, int qsc, int qec,
                 int mm, int mi, int o, int e, int dir,
                 int *opt, int *te, int *qe, int *n_best);
private:
  int n;
  uint8_t query[Q_MAX];
  uint8_t target[T_MAX];
};

#endif
