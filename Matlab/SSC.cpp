#include <cstring>
#include <math.h>
#include <mex.h>
#include <vector>

using namespace std;

vector<int> ssc(double *pointLocation, int numInPoints, int numRetPoints,
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

  int high =
      (sol1 > sol2)
          ? sol1
          : sol2; // binary search range initialization with positive solution
  int low = floor(sqrt((double)numInPoints / numRetPoints));

  int width;
  int prevWidth = -1;

  vector<int> ResultVec;
  bool complete = false;
  unsigned int K = numRetPoints;
  unsigned int Kmin = round(K - (K * tolerance));
  unsigned int Kmax = round(K + (K * tolerance));

  vector<int> result;
  result.reserve(numInPoints);
  while (!complete) {
    width = low + (high - low) / 2;
    if (width == prevWidth ||
        low >
            high) { // needed to reassure the same radius is not repeated again
      ResultVec = result; // return the keypoints from the previous iteration
      break;
    }
    result.clear();
    double c = width / 2; // initializing Grid
    int numCellCols = floor(cols / c);
    int numCellRows = floor(rows / c);
    vector<vector<bool>> coveredVec(numCellRows + 1,
                                    vector<bool>(numCellCols + 1, false));

    for (unsigned int i = 0; i < numInPoints; ++i) {
      int row =
          floor(pointLocation[i + numInPoints] /
                c); // get position of the cell current point is located at
      int col = floor(pointLocation[i] / c);
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
  return ResultVec;
}

void DisplayMatrix(char *Name, double *Data, int M, int N) {
  int m, n;
  mexPrintf("%s = \n", Name);
  for (m = 0; m < M; ++m, mexPrintf("\n"))
    for (n = 0; n < N; ++n)
      mexPrintf("%1.1f ", Data[m + m * n]);
}

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) {
  if (nrhs != 5)
    mexErrMsgTxt("Wrong number of input arguments.");

  // reading inputs from matlab
  int numInPoints = mxGetM(prhs[0]); // number of input points
  int numInDimen = mxGetN(prhs[0]);  // number of dimensions
  double *pointLocation = (double *)mxGetData(prhs[0]); // input point locations
  int numRetPoints = mxGetScalar(prhs[1]);
  float tolerance = mxGetScalar(prhs[2]);
  int cols = mxGetScalar(prhs[3]);
  int rows = mxGetScalar(prhs[4]);

  /*
  mexPrintf("numInPoints = %d\n", numInPoints);
  mexPrintf("numInDimen = %d\n", numInDimen);
  mexPrintf("numRetPoints = %d\n", numRetPoints);
  mexPrintf("tolerance = %f\n", tolerance);
  mexPrintf("cols = %d\n", cols);
  mexPrintf("rows = %d\n", rows);
  */

  // running SSC algorithms
  vector<int> selectedIdx =
      ssc(pointLocation, numInPoints, numRetPoints, tolerance, cols, rows);
  int nSelectedPoints = (int)selectedIdx.size();

  // preparing outputs for matlab
  plhs[0] = mxCreateNumericMatrix(1, nSelectedPoints, mxINT32_CLASS, mxREAL);
  memcpy(mxGetData(plhs[0]), &selectedIdx[0], nSelectedPoints * sizeof(int));
}
