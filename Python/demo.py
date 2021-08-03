#!/usr/bin/env python

import argparse

import cv2  # pylint: disable=import-error
from ssc import ssc


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--image_path", type=str, default="../Images/test.png")
    parser.add_argument("--num_ret_points", type=int, default=10)
    parser.add_argument("--tolerance", type=float, default=0.1)
    args = parser.parse_args()

    img = cv2.imread(args.image_path)
    cv2.imshow("Input Image", img)
    cv2.waitKey(0)

    fast = cv2.FastFeatureDetector_create()
    keypoints = fast.detect(img, None)
    img2 = cv2.drawKeypoints(img, keypoints, outImage=None, color=(255, 0, 0))
    cv2.imshow("Detected FAST keypoints", img2)
    cv2.waitKey(0)

    # keypoints should be sorted by strength in descending order
    # before feeding to SSC to work correctly
    keypoints = sorted(keypoints, key=lambda x: x.response, reverse=True)

    selected_keypoints = ssc(
        keypoints, args.num_ret_points, args.tolerance, img.shape[1], img.shape[0]
    )

    img3 = cv2.drawKeypoints(img, selected_keypoints, outImage=None, color=(255, 0, 0))
    cv2.imshow("Selected keypoints", img3)
    cv2.waitKey(0)


if __name__ == "__main__":
    main()
