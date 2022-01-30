#ifndef ANMS_H
#define ANMS_H

#include "range-tree/ranget.h"
#include <iostream>
#include <nanoflann.hpp>
#include <opencv2/opencv.hpp>
#if CV_MAJOR_VERSION >= 4
#include "opencv2/imgcodecs/legacy/constants_c.h"
#endif
#include <stdlib.h>

using namespace std;
using namespace nanoflann;

vector<cv::KeyPoint> topN(vector<cv::KeyPoint> keyPoints, int numRetPoints) {
  vector<cv::KeyPoint> kp;
  for (int i = 0; i < numRetPoints; i++)
    kp.push_back(keyPoints[i]); // simply extracting numRetPoints keyPoints

  return kp;
}

#if CV_MAJOR_VERSION < 3 // If you are using OpenCV 2
vector<cv::KeyPoint> gridFAST(cv::Mat Image, int numRetPoints, int gridRows,
                              int gridCols) {
  vector<cv::KeyPoint> kp;
  cv::Ptr<cv::FeatureDetector> featdetectorK =
      new cv::GridAdaptedFeatureDetector(cv::FeatureDetector::create("FAST"),
                                         numRetPoints, gridRows, gridCols);
  featdetectorK->detect(Image, kp);

  return kp;
}
#endif

struct sort_pred {
  bool operator()(const pair<float, int> &left, const pair<float, int> &right) {
    return left.first > right.first;
  }
};

vector<cv::KeyPoint> brownANMS(vector<cv::KeyPoint> keyPoints,
                               int numRetPoints) {
  vector<pair<float, int>> results;
  results.push_back(make_pair(FLT_MAX, 0));
  for (unsigned int i = 1; i < keyPoints.size();
       ++i) { // for every keyPoint we get the min distance to the previously
              // visited keyPoints
    float minDist = FLT_MAX;
    for (unsigned int j = 0; j < i; ++j) {
      float exp1 = (keyPoints[j].pt.x - keyPoints[i].pt.x);
      float exp2 = (keyPoints[j].pt.y - keyPoints[i].pt.y);
      float curDist = sqrt(exp1 * exp1 + exp2 * exp2);
      minDist = min(curDist, minDist);
    }
    results.push_back(make_pair(minDist, i));
  }
  sort(results.begin(), results.end(), sort_pred()); // sorting by radius
  vector<cv::KeyPoint> kp;
  for (int i = 0; i < numRetPoints; ++i)
    kp.push_back(
        keyPoints[results[i].second]); // extracting numRetPoints keyPoints

  return kp;
}

vector<cv::KeyPoint> sdc(vector<cv::KeyPoint> keyPoints, int numRetPoints,
                         float tolerance, int cols, int rows) {
  double eps_var = 0.25; // this parameter is chosen to be the most optimal in
                         // the original paper

  int low = 1;
  int high = cols; // binary search range initialization
  int radius;
  int prevradius = -1;

  vector<int> ResultVec;
  bool complete = false;
  unsigned int K = numRetPoints;
  unsigned int Kmin = round(K - (K * tolerance));
  unsigned int Kmax = round(K + (K * tolerance));

  vector<int> result;
  result.reserve(keyPoints.size());
  while (!complete) {
    radius = low + (high - low) / 2;
    if (radius == prevradius ||
        low >
            high) { // needed to reassure the same radius is not repeated again
      ResultVec = result; // return the keypoints from the previous iteration
      break;
    }
    result.clear();
    double c = eps_var * radius / sqrt(2); // initializing Grid
    int numCellCols = floor(cols / c);
    int numCellRows = floor(rows / c);
    vector<vector<bool>> coveredVec(numCellRows + 1,
                                    vector<bool>(numCellCols + 1, false));

    for (unsigned int i = 0; i < keyPoints.size(); ++i) {
      int row =
          floor(keyPoints[i].pt.y /
                c); // get position of the cell current point is located at
      int col = floor(keyPoints[i].pt.x / c);
      if (coveredVec[row][col] == false) { // if the cell is not covered
        result.push_back(i);
        int rowMin = ((row - floor(radius / c)) >= 0)
                         ? (row - floor(radius / c))
                         : 0; // get range which current radius is covering
        int rowMax = ((row + floor(radius / c)) <= numCellRows)
                         ? (row + floor(radius / c))
                         : numCellRows;
        int colMin =
            ((col - floor(radius / c)) >= 0) ? (col - floor(radius / c)) : 0;
        int colMax = ((col + floor(radius / c)) <= numCellCols)
                         ? (col + floor(radius / c))
                         : numCellCols;
        for (int rowToCov = rowMin; rowToCov <= rowMax; ++rowToCov) {
          for (int colToCov = colMin; colToCov <= colMax; ++colToCov) {
            double dist = sqrt((rowToCov - row) * (rowToCov - row) +
                               (colToCov - col) * (colToCov - col));
            if (dist <= ((double)radius) / c)
              coveredVec[rowToCov][colToCov] =
                  true; // check the distance to every cell
          }
        }
      }
    }
    if (result.size() >= Kmin && result.size() <= Kmax) { // solution found
      ResultVec = result;
      complete = true;
    } else if (result.size() < Kmin)
      high = radius - 1; // update binary search range
    else
      low = radius + 1;
  }
  // retrieve final keypoints
  vector<cv::KeyPoint> kp;
  for (unsigned int i = 0; i < ResultVec.size(); i++)
    kp.push_back(keyPoints[ResultVec[i]]);

  return kp;
}

/*kdtree algorithm*/
template <typename T> struct PointCloud {
  struct Point {
    T x, y;
  };
  std::vector<Point> pts;
  inline size_t kdtree_get_point_count() const {
    return pts.size();
  } // Must return the number of data points
  // Returns the distance between the vector "p1[0:size-1]" and the data point
  // with index "idx_p2" stored in the class:
  inline T kdtree_distance(const T *p1, const size_t idx_p2,
                           size_t /*size*/) const {
    const T d0 = p1[0] - pts[idx_p2].x;
    const T d1 = p1[1] - pts[idx_p2].y;
    return d0 * d0 + d1 * d1;
  }
  // Returns the dim'th component of the idx'th point in the class:
  // Since this is inlined and the "dim" argument is typically an immediate
  // value, the
  //  "if/else's" are actually solved at compile time.
  inline T kdtree_get_pt(const size_t idx, int dim) const {
    if (dim == 0)
      return pts[idx].x;
    else if (dim == 1)
      return pts[idx].y;
    return 0;
  }
  // Optional bounding-box computation: return false to default to a standard
  // bbox computation loop.
  //   Return true if the BBOX was already computed by the class and returned in
  //   "bb" so it can be avoided to redo it again. Look at bb.size() to find out
  //   the expected dimensionality (e.g. 2 or 3 for point clouds)
  template <class BBOX> bool kdtree_get_bbox(BBOX & /*bb*/) const {
    return false;
  }
};

template <typename T>
void generatePointCloud(PointCloud<T> &point, vector<cv::KeyPoint> keyPoints) {
  point.pts.resize(keyPoints.size());
  for (size_t i = 0; i < keyPoints.size(); i++) {
    point.pts[i].x = keyPoints[i].pt.x;
    point.pts[i].y = keyPoints[i].pt.y;
  }
}

vector<cv::KeyPoint> kdTree(vector<cv::KeyPoint> keyPoints, int numRetPoints,
                            float tolerance, int cols, int rows) {
  // several temp expression variables to simplify solution equation
  int exp1 = rows + cols + 2 * numRetPoints;
  long long exp2 =
      ((long long)4 * cols + (long long)4 * numRetPoints +
       (long long)4 * rows * numRetPoints + (long long)rows * rows +
       (long long)cols * cols - (long long)2 * rows * cols +
       (long long)4 * rows * cols * numRetPoints);
  double exp3 = sqrt(exp2);
  double exp4 = numRetPoints - 1;

  double sol1 = -round((exp1 + exp3) / exp4); // first solution
  double sol2 = -round((exp1 - exp3) / exp4); // second solution

  // binary search range initialization with positive solution
  int high = (sol1 > sol2) ? sol1 : sol2;
  int low = floor(sqrt((double)keyPoints.size() / numRetPoints));
  low = max(1, low);

  PointCloud<int> cloud; // creating k-d tree with keypoints
  generatePointCloud(cloud, keyPoints);
  typedef KDTreeSingleIndexAdaptor<L2_Simple_Adaptor<int, PointCloud<int>>,
                                   PointCloud<int>, 2>
      my_kd_tree_t;
  my_kd_tree_t index(2, cloud,
                     KDTreeSingleIndexAdaptorParams(25 /* max leaf */));
  index.buildIndex();

  bool complete = false;
  unsigned int K = numRetPoints;
  unsigned int Kmin = round(K - (K * tolerance));
  unsigned int Kmax = round(K + (K * tolerance));
  vector<int> ResultVec;
  int radius;
  int prevradius = -1;

  vector<int> result;
  result.reserve(keyPoints.size());
  while (!complete) {
    vector<bool> Included(keyPoints.size(), true);
    radius = low + (high - low) / 2;
    if (radius == prevradius ||
        low >
            high) { // needed to reassure the same radius is not repeated again
      ResultVec = result; // return the keypoints from the previous iteration
      break;
    }
    result.clear();

    for (unsigned int i = 0; i < keyPoints.size(); ++i) {
      if (Included[i] == true) {
        Included[i] = false;
        result.push_back(i);
        const int search_radius = static_cast<int>(radius * radius);
        vector<pair<size_t, int>> ret_matches;
        nanoflann::SearchParams params;
        const int query_pt[2] = {(int)keyPoints[i].pt.x,
                                 (int)keyPoints[i].pt.y};
        const size_t nMatches = index.radiusSearch(&query_pt[0], search_radius,
                                                   ret_matches, params);

        for (size_t nmIdx = 0; nmIdx < nMatches; nmIdx++) {
          if (Included[ret_matches[nmIdx].first])
            Included[ret_matches[nmIdx].first] = false;
        }
      }
    }

    if (result.size() >= Kmin && result.size() <= Kmax) { // solution found
      ResultVec = result;
      complete = true;
    } else if (result.size() < Kmin)
      high = radius - 1; // update binary search range
    else
      low = radius + 1;

    prevradius = radius;
  }

  // retrieve final keypoints
  vector<cv::KeyPoint> kp;
  for (unsigned int i = 0; i < ResultVec.size(); i++)
    kp.push_back(keyPoints[ResultVec[i]]);

  return kp;
}

vector<cv::KeyPoint> rangeTree(vector<cv::KeyPoint> keyPoints, int numRetPoints,
                               float tolerance, int cols, int rows) {
  // several temp expression variables to simplify solution equation
  int exp1 = rows + cols + 2 * numRetPoints;
  long long exp2 =
      ((long long)4 * cols + (long long)4 * numRetPoints +
       (long long)4 * rows * numRetPoints + (long long)rows * rows +
       (long long)cols * cols - (long long)2 * rows * cols +
       (long long)4 * rows * cols * numRetPoints);
  double exp3 = sqrt(exp2);
  double exp4 = numRetPoints - 1;

  double sol1 = -round((exp1 + exp3) / exp4); // first solution
  double sol2 = -round((exp1 - exp3) / exp4); // second solution

  // binary search range initialization with positive solution
  int high = (sol1 > sol2) ? sol1 : sol2;
  int low = floor(sqrt((double)keyPoints.size() / numRetPoints));
  low = max(1, low);

  rangetree<u16, u16> treeANMS(
      keyPoints.size(), keyPoints.size()); // creating range tree with keypoints
  for (unsigned int i = 0; i < keyPoints.size(); i++)
    treeANMS.add(keyPoints[i].pt.x, keyPoints[i].pt.y, (u16 *)(intptr_t)i);
  treeANMS.finalize();

  bool complete = false;
  unsigned int K = numRetPoints;
  unsigned int Kmin = round(K - (K * tolerance));
  unsigned int Kmax = round(K + (K * tolerance));
  vector<int> ResultVec;
  int width;
  int prevwidth = -1;

  vector<int> result;
  result.reserve(keyPoints.size());
  while (!complete) {
    vector<bool> Included(keyPoints.size(), true);
    width = low + (high - low) / 2;
    if (width == prevwidth ||
        low > high) { // needed to reassure the same width is not repeated again
      ResultVec = result; // return the keypoints from the previous iteration
      break;
    }
    result.clear();

    for (unsigned int i = 0; i < keyPoints.size(); ++i) {
      if (Included[i] == true) {
        Included[i] = false;
        result.push_back(i);
        int minx = keyPoints[i].pt.x - width;
        int maxx = keyPoints[i].pt.x +
                   width; // defining square boundaries around the point
        int miny = keyPoints[i].pt.y - width;
        int maxy = keyPoints[i].pt.y + width;
        if (minx < 0)
          minx = 0;
        if (miny < 0)
          miny = 0;

        std::vector<u16 *> *he = treeANMS.search(minx, maxx, miny, maxy);
        for (unsigned int j = 0; j < he->size(); j++)
          if (Included[(u64)(*he)[j]])
            Included[(u64)(*he)[j]] = false;
        delete he;
        he = NULL;
      }
    }
    if (result.size() >= Kmin && result.size() <= Kmax) { // solution found
      ResultVec = result;
      complete = true;
    } else if (result.size() < Kmin)
      high = width - 1; // update binary search range
    else
      low = width + 1;
    prevwidth = width;
  }
  // retrieve final keypoints
  vector<cv::KeyPoint> kp;
  for (unsigned int i = 0; i < ResultVec.size(); i++)
    kp.push_back(keyPoints[ResultVec[i]]);

  return kp;
}

vector<cv::KeyPoint> ssc(vector<cv::KeyPoint> keyPoints, int numRetPoints,
                         float tolerance, int cols, int rows) {
  // several temp expression variables to simplify solution equation
  int exp1 = rows + cols + 2 * numRetPoints;
  long long exp2 =
      ((long long)4 * cols + (long long)4 * numRetPoints +
       (long long)4 * rows * numRetPoints + (long long)rows * rows +
       (long long)cols * cols - (long long)2 * rows * cols +
       (long long)4 * rows * cols * numRetPoints);
  double exp3 = sqrt(exp2);
  double exp4 = numRetPoints - 1;

  double sol1 = -round((exp1 + exp3) / exp4); // first solution
  double sol2 = -round((exp1 - exp3) / exp4); // second solution

  // binary search range initialization with positive solution
  int high = (sol1 > sol2) ? sol1 : sol2;
  int low = floor(sqrt((double)keyPoints.size() / numRetPoints));
  low = max(1, low);

  int width;
  int prevWidth = -1;

  vector<int> ResultVec;
  bool complete = false;
  unsigned int K = numRetPoints;
  unsigned int Kmin = round(K - (K * tolerance));
  unsigned int Kmax = round(K + (K * tolerance));

  vector<int> result;
  result.reserve(keyPoints.size());
  while (!complete) {
    width = low + (high - low) / 2;
    if (width == prevWidth ||
        low >
            high) { // needed to reassure the same radius is not repeated again
      ResultVec = result; // return the keypoints from the previous iteration
      break;
    }
    result.clear();
    double c = (double)width / 2.0; // initializing Grid
    int numCellCols = floor(cols / c);
    int numCellRows = floor(rows / c);
    vector<vector<bool>> coveredVec(numCellRows + 1,
                                    vector<bool>(numCellCols + 1, false));

    for (unsigned int i = 0; i < keyPoints.size(); ++i) {
      int row =
          floor(keyPoints[i].pt.y /
                c); // get position of the cell current point is located at
      int col = floor(keyPoints[i].pt.x / c);
      if (coveredVec[row][col] == false) { // if the cell is not covered
        result.push_back(i);
        int rowMin = ((row - floor(width / c)) >= 0)
                         ? (row - floor(width / c))
                         : 0; // get range which current radius is covering
        int rowMax = ((row + floor(width / c)) <= numCellRows)
                         ? (row + floor(width / c))
                         : numCellRows;
        int colMin =
            ((col - floor(width / c)) >= 0) ? (col - floor(width / c)) : 0;
        int colMax = ((col + floor(width / c)) <= numCellCols)
                         ? (col + floor(width / c))
                         : numCellCols;
        for (int rowToCov = rowMin; rowToCov <= rowMax; ++rowToCov) {
          for (int colToCov = colMin; colToCov <= colMax; ++colToCov) {
            if (!coveredVec[rowToCov][colToCov])
              coveredVec[rowToCov][colToCov] =
                  true; // cover cells within the square bounding box with width
                        // w
          }
        }
      }
    }

    if (result.size() >= Kmin && result.size() <= Kmax) { // solution found
      ResultVec = result;
      complete = true;
    } else if (result.size() < Kmin)
      high = width - 1; // update binary search range
    else
      low = width + 1;
    prevWidth = width;
  }
  // retrieve final keypoints
  vector<cv::KeyPoint> kp;
  for (unsigned int i = 0; i < ResultVec.size(); i++)
    kp.push_back(keyPoints[ResultVec[i]]);

  return kp;
}

void VisualizeAll(cv::Mat Image, vector<cv::KeyPoint> keyPoints,
                  string figureTitle) {
  cv::Mat resultImg;
  cv::drawKeypoints(Image, keyPoints, resultImg,
                    cv::Scalar(94.0, 206.0, 165.0, 0.0));
  cv::namedWindow(figureTitle, cv::WINDOW_AUTOSIZE);
  cv::imshow(figureTitle, resultImg);
  return;
}

#endif // ANMS_H
