package com.bailool.anms;

import java.util.ArrayList;
import java.util.List;
import java.util.logging.Level;
import java.util.logging.Logger;

import org.opencv.core.Core;
import org.opencv.core.KeyPoint;
import org.opencv.core.Mat;
import org.opencv.core.MatOfKeyPoint;
import org.opencv.core.Scalar;
import org.opencv.features2d.FastFeatureDetector;
import org.opencv.features2d.Features2d;
import org.opencv.imgcodecs.Imgcodecs;

import org.opencv.highgui.HighGui;

public class AnmsExample {
	public static void main(String[] args) {
		Logger logger = Logger.getLogger(AnmsExample.class.getName(), null);

		// Loading the core library
		System.loadLibrary(Core.NATIVE_LIBRARY_NAME);
		logger.log(Level.INFO, "Demo of the ANMS algorithms");

		// Read and display input image
		String testImgPath = "../Images/test.png"; // Path to image
		Mat testImg = Imgcodecs.imread(testImgPath, Imgcodecs.IMREAD_GRAYSCALE); // Read the file
		HighGui.imshow("Input Image", testImg);
		HighGui.waitKey();

		int fastThresh = 1; // Fast threshold. Usually this value is set to be in range [10,35]
		MatOfKeyPoint keypoints = new MatOfKeyPoint();
		FastFeatureDetector fastDetector = FastFeatureDetector.create(fastThresh);
		fastDetector.detect(testImg, keypoints);
		logger.log(Level.INFO, "Number of points detected : {0}", keypoints.size().height);

		// Visualize FAST keypoints
		Mat fastDetectionResults = new Mat();
		Features2d.drawKeypoints(testImg, keypoints, fastDetectionResults, new Scalar(94.0, 206.0, 165.0), 0);
		HighGui.imshow("FAST Detections", fastDetectionResults);
		HighGui.waitKey();

		// Sorting keypoints by decreasing order of strength
		List<KeyPoint> listKeyPointsSorted = keypoints.toList();
		listKeyPointsSorted.sort((kp1, kp2) -> (int) (kp2.response - kp1.response));

		int numRetPoints = 750; // choose exact number of return points
		float tolerance = (float) 0.1; // tolerance of the number of return points

		logger.log(Level.INFO, "Start SSC ANMS");
		long startTime = System.currentTimeMillis();
		List<KeyPoint> sscKeyPoints = ssc(listKeyPointsSorted, numRetPoints, tolerance, testImg.cols(), testImg.rows());
		long stopTime = System.currentTimeMillis();
		logger.log(Level.INFO, "Finish SSC ANMS {0} miliseconds", stopTime - startTime);

		// Visualize results
		Mat resultImg = new Mat();
		MatOfKeyPoint sscKeyPointsMat = new MatOfKeyPoint();
		sscKeyPointsMat.fromList(sscKeyPoints);
		Features2d.drawKeypoints(testImg, sscKeyPointsMat, resultImg, new Scalar(94.0, 206.0, 165.0), 0);
		HighGui.imshow("SSC KeyPoints", resultImg);
		HighGui.waitKey();
	}

	/*
	 * Suppression via Square Convering (SSC) algorithm. Check Algorithm 2 in the
	 * paper:
	 * https://www.sciencedirect.com/science/article/abs/pii/S016786551830062X
	 */
	private static List<KeyPoint> ssc(List<KeyPoint> keyPoints, int numRetPoints, float tolerance, int cols, int rows) {
		// Several temp expression variables to simplify equation solution
		int expression1 = rows + cols + 2 * numRetPoints;
		long expression2 = ((long) 4 * cols + (long) 4 * numRetPoints + (long) 4 * rows * numRetPoints
				+ (long) rows * rows + (long) cols * cols - (long) 2 * rows * cols
				+ (long) 4 * rows * cols * numRetPoints);
		double expression3 = Math.sqrt(expression2);
		double expression4 = (double) numRetPoints - 1;

		double solution1 = -Math.round((expression1 + expression3) / expression4); // first solution
		double solution2 = -Math.round((expression1 - expression3) / expression4); // second solution

		// binary search range initialization with positive solution
		int high = (int) ((solution1 > solution2) ? solution1 : solution2);
		int low = (int) Math.floor(Math.sqrt((double) keyPoints.size() / numRetPoints));
		int width;
		int prevWidth = -1;

		ArrayList<Integer> resultVec = new ArrayList<>();
		boolean complete = false;
		int kMin = Math.round(numRetPoints - (numRetPoints * tolerance));
		int kMax = Math.round(numRetPoints + (numRetPoints * tolerance));

		ArrayList<Integer> result = new ArrayList<>(keyPoints.size());
		while (!complete) {
			width = low + (high - low) / 2;
			if (width == prevWidth || low > high) { // needed to reassure the same radius is not repeated again
				resultVec = result; // return the keypoints from the previous iteration
				break;
			}
			result.clear();
			double c = (double) width / 2; // initializing Grid
			int numCellCols = (int) Math.floor(cols / c);
			int numCellRows = (int) Math.floor(rows / c);

			// Fill temporary boolean array
			boolean[][] coveredVec = new boolean[numCellRows + 1][numCellCols + 1];

			// Perform square suppression
			for (int i = 0; i < keyPoints.size(); i++) {
				// get position of the cell current point is located at
				int row = (int) Math.floor(keyPoints.get(i).pt.y / c);
				int col = (int) Math.floor(keyPoints.get(i).pt.x / c);
				if (!coveredVec[row][col]) { // if the cell is not covered
					result.add(i);

					// get range which current radius is covering
					int rowMin = (int) (((row - (int) Math.floor(width / c)) >= 0) ? (row - Math.floor(width / c)) : 0);
					int rowMax = (int) (((row + Math.floor(width / c)) <= numCellRows) ? (row + Math.floor(width / c))
							: numCellRows);
					int colMin = (int) (((col - Math.floor(width / c)) >= 0) ? (col - Math.floor(width / c)) : 0);
					int colMax = (int) (((col + Math.floor(width / c)) <= numCellCols) ? (col + Math.floor(width / c))
							: numCellCols);

					// cover cells within the square bounding box with width w
					for (int rowToCov = rowMin; rowToCov <= rowMax; rowToCov++) {
						for (int colToCov = colMin; colToCov <= colMax; colToCov++) {
							if (!coveredVec[rowToCov][colToCov])
								coveredVec[rowToCov][colToCov] = true;
						}
					}
				}
			}

			if (result.size() >= kMin && result.size() <= kMax) { // solution found
				resultVec = result;
				complete = true;
			} else if (result.size() < kMin)
				high = width - 1; // update binary search range
			else
				low = width + 1;
			prevWidth = width;
		}

		// Retrieve final keypoints
		List<KeyPoint> kp = new ArrayList<>();
		for (int i : resultVec)
			kp.add(keyPoints.get(i));

		return kp;
	}
}
