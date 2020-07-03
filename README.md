# Efficient adaptive non-maximal suppression algorithms for homogeneous spatial keypoint distribution

This is the implementation of the [paper](https://www.researchgate.net/publication/323388062_Efficient_adaptive_non-maximal_suppression_algorithms_for_homogeneous_spatial_keypoint_distribution) *"Efficient adaptive non-maximal suppression algorithms for homogeneous spatial keypoint distribution"* that is published in Pattern Recognition Letters (PRL).

![Algorithm Overview](https://github.com/BAILOOL/ANMS-Codes/blob/master/Images/algorithm_overview.png?raw=true?raw=true "Algorithm Overview")

While competing ANMS methods have similar performance in terms of spatial keypoints distribution, the proposed method SSC is substantially faster and scales better:

| ![Retrieve 10%](https://github.com/BAILOOL/ANMS-Codes/blob/master/Images/Time10.png?raw=true "Retrieve 10%") | ![Retrieve 40%](https://github.com/BAILOOL/ANMS-Codes/blob/master/Images/Time40.png?raw=true "Retrieve 40%") | ![Retrieve 70%](https://github.com/BAILOOL/ANMS-Codes/blob/master/Images/Time70.png?raw=true "Retrieve 70%") |
|:----:|:---:|:---:|

Here is how proposed ANMS method visually compares to traditional methods: TopM | Bucketing | SSC (proposed)

| ![TopM](https://github.com/BAILOOL/ANMS-Codes/blob/master/Images/TopM.png?raw=true "TopM") | ![Bucketing](https://github.com/BAILOOL/ANMS-Codes/blob/master/Images/Bucketing.png?raw=true "Bucketing") | ![SSC](https://github.com/BAILOOL/ANMS-Codes/blob/master/Images/SSC.png?raw=true "SSC") |
|:---:|:---:|:---:|

Related algorithms that are implemented in this repository are:

 - *"Visual Odometry based on Stereo Image Sequences with RANSAC-based Outlier Rejection Scheme"* - bucketing
 - *"Multi-Image Matching using Multi-Scale Oriented Patches"* - original ANMS
 - *"Efficiently selecting spatially distributed keypoints for visual tracking"* - more efficient ANMS

For more details about the algorithm, experiments as well as the importance of homogeneously distributed keypoints for SLAM please refer to the [paper](https://www.researchgate.net/publication/323388062_Efficient_adaptive_non-maximal_suppression_algorithms_for_homogeneous_spatial_keypoint_distribution).

## How to run
1. Clone this repository: ``` git clone https://github.com/BAILOOL/ANMS-Codes.git ```
2. Choose your language: [C++](https://github.com/BAILOOL/ANMS-Codes/tree/master/C++), [Python](https://github.com/BAILOOL/ANMS-Codes/tree/master/Python), or [Matlab](https://github.com/BAILOOL/ANMS-Codes/tree/master/Matlab).

3. Make sure the [path to test image](https://github.com/BAILOOL/ANMS-Codes/blob/d907ca805fcf1ea670ac75a9ea9b46446421e573/C++/CmakeProject/source/main.cpp#L8) is set correctly.

4. Run produced executable ``` ./ANMS_Codes``` for C++ or relevant script for other languages.

Codes are tested with OpenCV 2.4.8, OpenCV 3.3.1 and Ubuntu 14.04, 16.04.

## Citation
If you use these codes in your research, please cite:
```
@article{bailo2018efficient,
  title={Efficient adaptive non-maximal suppression algorithms for homogeneous spatial keypoint distribution},
  author={Bailo, Oleksandr and Rameau, Francois and Joo, Kyungdon and Park, Jinsun and Bogdan, Oleksandr and Kweon, In So},
  journal={Pattern Recognition Letters},
  volume={106},
  pages={53--60},
  year={2018},
  publisher={Elsevier}
}
```
