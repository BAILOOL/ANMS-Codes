#include <anms.h>
#include <numeric>

int main(int argc, char *argv[]) {
  cout << "Demo of the ANMS algorithms" << endl;

  string testImgPath = "../../../Images/test.png"; // Path to image
  cv::Mat testImg =
      cv::imread(testImgPath, CV_LOAD_IMAGE_GRAYSCALE); // Read the file

  cv::namedWindow("Input Image", cv::WINDOW_AUTOSIZE);
  cv::imshow("Input Image", testImg);

  int fastThresh =
      1; // Fast threshold. Usually this value is set to be in range [10,35]
  vector<cv::KeyPoint> keyPoints; // vector to keep detected KeyPoints
#if CV_MAJOR_VERSION < 3          // If you are using OpenCV 2
  cv::FastFeatureDetector fastDetector(fastThresh, true);
  fastDetector.detect(testImg, keyPoints);
#else
  cv::FAST(testImg, keyPoints, fastThresh, true);
#endif
  cout << "Number of points detected : " << keyPoints.size() << endl;

  cv::Mat fastDetectionResults; // draw FAST detections
  cv::drawKeypoints(testImg, keyPoints, fastDetectionResults,
                    cv::Scalar(94.0, 206.0, 165.0, 0.0));
  cv::namedWindow("FAST Detections", cv::WINDOW_AUTOSIZE);
  cv::imshow("FAST Detections", fastDetectionResults);

  // Sorting keypoints by deacreasing order of strength
  vector<float> responseVector;
  for (unsigned int i = 0; i < keyPoints.size(); i++)
    responseVector.push_back(keyPoints[i].response);
  vector<int> Indx(responseVector.size());
  std::iota(std::begin(Indx), std::end(Indx), 0);
  cv::sortIdx(responseVector, Indx, CV_SORT_DESCENDING);
  vector<cv::KeyPoint> keyPointsSorted;
  for (unsigned int i = 0; i < keyPoints.size(); i++)
    keyPointsSorted.push_back(keyPoints[Indx[i]]);

  int numRetPoints = 750; // choose exact number of return points
  // float percentage = 0.1; //or choose percentage of points to be return
  // int numRetPoints = (int)keyPoints.size()*percentage;

  float tolerance = 0.1; // tolerance of the number of return points

  cout << "\nStart TopN" << endl;
  clock_t topNStart = clock();
  vector<cv::KeyPoint> topnKP = topN(keyPointsSorted, numRetPoints);
  clock_t topNTotalTime =
      double(clock() - topNStart) * 1000 / (double)CLOCKS_PER_SEC;
  cout << "Finish TopN in " << topNTotalTime << " miliseconds." << endl;

#if CV_MAJOR_VERSION < 3 // Bucketing is no longer available in opencv3
  cout << "\nStart GridFAST" << endl;
  clock_t gridFASTStart = clock();
  vector<cv::KeyPoint> gridFASTKP =
      gridFAST(testImg, numRetPoints, 7,
               4); // change gridRows=7 and gridCols=4 parameters if necessary
  clock_t gridFASTTotalTime =
      double(clock() - gridFASTStart) * 1000 / (double)CLOCKS_PER_SEC;
  cout << "Finish GridFAST in " << gridFASTTotalTime << " miliseconds." << endl;
#endif

  cout << "\nBrown ANMS" << endl;
  clock_t brownStart = clock();
  vector<cv::KeyPoint> brownKP = brownANMS(keyPointsSorted, numRetPoints);
  clock_t brownTotalTime =
      double(clock() - brownStart) * 1000 / (double)CLOCKS_PER_SEC;
  cout << "Brown ANMS " << brownTotalTime << " miliseconds." << endl;

  cout << "\nSDC ANMS" << endl;
  clock_t sdcStart = clock();
  vector<cv::KeyPoint> sdcKP =
      sdc(keyPointsSorted, numRetPoints, tolerance, testImg.cols, testImg.rows);
  clock_t sdcTotalTime =
      double(clock() - sdcStart) * 1000 / (double)CLOCKS_PER_SEC;
  cout << "SDC ANMS " << sdcTotalTime << " miliseconds." << endl;

  cout << "\nStart K-d Tree ANMS" << endl;
  clock_t kdtreeStart = clock();
  vector<cv::KeyPoint> kdtreeKP = kdTree(keyPointsSorted, numRetPoints,
                                         tolerance, testImg.cols, testImg.rows);
  clock_t kdtreeTotalTime =
      double(clock() - kdtreeStart) * 1000 / (double)CLOCKS_PER_SEC;
  cout << "Finish K-d Tree ANMS " << kdtreeTotalTime << " miliseconds." << endl;

  cout << "\nStart Range Tree ANMS" << endl;
  clock_t rangetreeStart = clock();
  vector<cv::KeyPoint> rangetreeKP = rangeTree(
      keyPointsSorted, numRetPoints, tolerance, testImg.cols, testImg.rows);
  clock_t rangetreeTotalTime =
      double(clock() - rangetreeStart) * 1000 / (double)CLOCKS_PER_SEC;
  cout << "Finish Range Tree ANMS " << rangetreeTotalTime << " miliseconds."
       << endl;

  cout << "\nStart SSC ANMS" << endl;
  clock_t sscStart = clock();
  vector<cv::KeyPoint> sscKP =
      ssc(keyPointsSorted, numRetPoints, tolerance, testImg.cols, testImg.rows);
  clock_t sscTotalTime =
      double(clock() - sscStart) * 1000 / (double)CLOCKS_PER_SEC;
  cout << "Finish SSC ANMS " << sscTotalTime << " miliseconds." << endl;

  // results visualization
  VisualizeAll(testImg, topnKP, "TopN KeyPoints");
#if CV_MAJOR_VERSION < 3
  VisualizeAll(testImg, gridFASTKP, "Grid FAST KeyPoints");
#endif
  VisualizeAll(testImg, brownKP, "Brown ANMS KeyPoints");
  VisualizeAll(testImg, sdcKP, "SDC KeyPoints");
  VisualizeAll(testImg, kdtreeKP, "K-d Tree KeyPoints");
  VisualizeAll(testImg, rangetreeKP, "Range Tree KeyPoints");
  VisualizeAll(testImg, sscKP, "SSC KeyPoints");

  cv::waitKey(0); // Wait for a keystroke in the window
  return 0;
}
