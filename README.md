# Efficient adaptive non-maximal suppression algorithms for homogeneous spatial keypoint distribution

This is the implemetation of the [paper](https://www.researchgate.net/publication/323388062_Efficient_adaptive_non-maximal_suppression_algorithms_for_homogeneous_spatial_keypoint_distribution) *"Efficient adaptive non-maximal suppression algorithms for homogeneous spatial keypoint distribution"* that is accepted to Pattern Recognition Letters (PRL).

![Algorithm Overview](https://github.com/BAILOOL/ANMS-Codes/blob/master/Images/algorithm_overview.png?raw=true?raw=true "Algorithm Overview")

While competing ANMS methods have similar performance in terms of spatial keypoints distribution, the proposed method SSC is substantially faster and scales better:

| ![Retrieve 10%](https://github.com/BAILOOL/ANMS-Codes/blob/master/Images/Time10.png?raw=true "Retrieve 10%") |
![Retrieve 40%](https://github.com/BAILOOL/ANMS-Codes/blob/master/Images/Time40.png?raw=true "Retrieve 40%") |
![Retrieve 70%](https://github.com/BAILOOL/ANMS-Codes/blob/master/Images/Time70.png?raw=true "Retrieve 70%") |
|:---:|:---:|:---:|

Here is how proposed ANMS method visually compares to traditional methods: TopM | Bucketing | SSC (proposed)

| ![TopM](https://github.com/BAILOOL/ANMS-Codes/blob/master/Images/TopM.png?raw=true "TopM") |
![Bucketing](https://github.com/BAILOOL/ANMS-Codes/blob/master/Images/Bucketing.png?raw=true "Bucketing") |
![SSC](https://github.com/BAILOOL/ANMS-Codes/blob/master/Images/SSC.png?raw=true "SSC") |
|:---:|:---:|:---:|

For more details about the algorithm, experiments as well as the importance of homogenously distributed keypoints for SLAM please refer to the [paper](https://www.researchgate.net/publication/323388062_Efficient_adaptive_non-maximal_suppression_algorithms_for_homogeneous_spatial_keypoint_distribution).

## How to run
1. Clone this repository: ``` git clone https://github.com/BAILOOL/ANMS-Codes.git ```
2. Visit [CmakeProject](https://github.com/BAILOOL/ANMS-Codes/tree/master/CmakeProject), [QtProject](https://github.com/BAILOOL/ANMS-Codes/tree/master/QtProject), [Python](https://github.com/BAILOOL/ANMS-Codes/tree/master/Python), or [Matlab](https://github.com/BAILOOL/ANMS-Codes/tree/master/Matlab) folders for your favorite compiling method.

3. Make sure the [path to test image](https://github.com/BAILOOL/ANMS-Codes/blob/d907ca805fcf1ea670ac75a9ea9b46446421e573/CmakeProject/source/main.cpp#L8) is set correctly.

4. Run the code: ``` ./ANMS_Codes```

Codes are tested with OpenCV 2.4.8, OpenCV 3.3.1 and Ubuntu 14.04, 16.04.
