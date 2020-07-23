package com.bailool.anms;

import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;
import java.util.List;

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
	public static void main( String[] args ) {
		//Loading the core library 
		System.loadLibrary(Core.NATIVE_LIBRARY_NAME);
		System.out.println("Demo of the ANMS algorithms");
	     
		//Instantiating the Imagecodecs class 
	    Imgcodecs imageCodecs = new Imgcodecs(); 
	    
	    //Read and display input image
		String testImgPath = "../Images/test.png"; // Path to image
		Mat testImg = imageCodecs.imread(testImgPath, imageCodecs.IMREAD_GRAYSCALE);   // Read the file
		HighGui.imshow("Input Image", testImg);
		HighGui.waitKey();
		
		int fastThresh = 1; // Fast threshold. Usually this value is set to be in range [10,35]      
		MatOfKeyPoint keypoints = new MatOfKeyPoint();
		FastFeatureDetector fastDetector = FastFeatureDetector.create(fastThresh);
        fastDetector.detect(testImg, keypoints);
        System.out.println("Number of points detected : " + keypoints.size().height);
		
        //Visualize FAST keypoints
        Mat fastDetectionResults = new Mat();
        Features2d.drawKeypoints(testImg, keypoints, fastDetectionResults, new Scalar(94.0, 206.0, 165.0), 0);
        HighGui.imshow("FAST Detections", fastDetectionResults);
		HighGui.waitKey();

		//Sorting keypoints by decreasing order of strength
		List<KeyPoint> listKeyPointsSorted = keypoints.toList();
        Collections.sort(listKeyPointsSorted, new Comparator<KeyPoint>() {
            public int compare(KeyPoint kp1, KeyPoint kp2) {
                return (int) (kp2.response - kp1.response);
            }
        });
        
        int numRetPoints = 750; //choose exact number of return points
        float tolerance = (float) 0.1; // tolerance of the number of return points
    
        System.out.println("Start SSC ANMS");
        long startTime = System.currentTimeMillis();
        List<KeyPoint> sscKeyPoints = ssc(listKeyPointsSorted, numRetPoints, tolerance, testImg.cols(), testImg.rows());
        long stopTime = System.currentTimeMillis();
        System.out.println("Finish SSC ANMS " + (stopTime - startTime) + " miliseconds.");
        
        //Visualize results
        Mat resultImg = new Mat();
        MatOfKeyPoint sscKeyPointsMat = new MatOfKeyPoint();
        sscKeyPointsMat.fromList(sscKeyPoints);
        Features2d.drawKeypoints(testImg, sscKeyPointsMat, resultImg, new Scalar(94.0, 206.0, 165.0), 0);
        HighGui.imshow("SSC KeyPoints", resultImg);
		HighGui.waitKey();
	}

	private static List<KeyPoint> ssc(List<KeyPoint> keyPoints, int numRetPoints, float tolerance, int cols, int rows) {
	    // several temp expression variables to simplify solution equation
	    int exp1 = rows + cols + 2*numRetPoints;
	    long exp2 = ((long) 4*cols + (long)4*numRetPoints + (long)4*rows*numRetPoints + (long)rows*rows + (long) cols*cols - (long)2*rows*cols + (long)4*rows*cols*numRetPoints);
	    double exp3 = Math.sqrt(exp2);
	    double exp4 = numRetPoints - 1;
	    
	    double sol1 = -Math.round((exp1+exp3)/exp4); // first solution
	    double sol2 = -Math.round((exp1-exp3)/exp4); // second solution
	    
	    int high = (int) ((sol1>sol2)? sol1 : sol2); //binary search range initialization with positive solution
	    int low = (int)Math.floor(Math.sqrt((double)keyPoints.size()/numRetPoints));
	    
	    int width;
	    int prevWidth = -1;
	    
	    ArrayList<Integer> resultVec = new ArrayList<Integer>();
	    boolean complete = false;
	    int K = numRetPoints; int Kmin = Math.round(K-(K*tolerance)); int Kmax = Math.round(K+(K*tolerance));
	    
	    ArrayList<Integer> result = new ArrayList<Integer>(keyPoints.size());
	    while (!complete){
	        width = low + (high - low) / 2;
	        if (width == prevWidth || low>high) { //needed to reassure the same radius is not repeated again
	        	resultVec = result; //return the keypoints from the previous iteration
	            break;
	        }
	        result.clear();
	        double c = width / 2; //initializing Grid
	        int numCellCols = (int) Math.floor(cols / c);
	        int numCellRows = (int) Math.floor(rows / c);
	        
	        //Fill temporary boolean array
	        ArrayList<ArrayList<Boolean>> coveredVec = new ArrayList<ArrayList<Boolean>>(numCellRows + 1); 
	        for (int temp_row = 0; temp_row < numCellRows + 1; temp_row++) {
	        	ArrayList<Boolean> tempRow = new ArrayList<Boolean>(numCellCols + 1);
	        	coveredVec.add(tempRow);
	        	for (int temp_col = 0; temp_col < numCellCols + 1; temp_col++) {
	        		tempRow.add(false);
	        	}
	        }
	        
	        //Perform square suppression
	        for (int i=0; i<keyPoints.size(); i++){
	            int row = (int)Math.floor(keyPoints.get(i).pt.y / c); //get position of the cell current point is located at
	            int col = (int)Math.floor(keyPoints.get(i).pt.x / c);
	            if (coveredVec.get(row).get(col) == false){ // if the cell is not covered
	                result.add(i);
	                int rowMin = (int) (((row - (int)Math.floor(width / c)) >= 0)? (row - Math.floor(width / c)) : 0); //get range which current radius is covering
	                int rowMax = (int) (((row + Math.floor(width / c)) <= numCellRows)? (row + Math.floor(width / c)) : numCellRows);
	                int colMin = (int) (((col - Math.floor(width / c)) >= 0)? (col - Math.floor(width / c)) : 0);
	                int colMax = (int) (((col + Math.floor(width / c)) <= numCellCols)? (col + Math.floor(width / c)) : numCellCols);
	                for (int rowToCov = rowMin; rowToCov <= rowMax; rowToCov++){
	                    for (int colToCov = colMin; colToCov <= colMax; colToCov++){
	                        if (!coveredVec.get(rowToCov).get(colToCov)) coveredVec.get(rowToCov).set(colToCov, true); //cover cells within the square bounding box with width w
	                    }
	                }
	            }
	        }

	        if (result.size() >= Kmin && result.size() <= Kmax){ //solution found
	        	resultVec = result;
	            complete = true;
	        }
	        else if (result.size() < Kmin) high = width - 1; //update binary search range
	        else low = width + 1;
	        prevWidth = width;
	    }
	    
	    //Retrieve final keypoints
	    List<KeyPoint> kp = new ArrayList<KeyPoint>();
	    for (int i = 0; i < resultVec.size(); i++) kp.add(keyPoints.get(resultVec.get(i)));

	    return kp;
	}
}
