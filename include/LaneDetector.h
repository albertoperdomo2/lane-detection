#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <vector>

/**
 * @brief Implements a Lane Detector.
 * 
 * It allows the user to define the ROI, the warped image width and height, as well as 
 * draw the sliding ROI window. It does so by detecting edges, computing the lines for
 * both the left and right lane and shadowind the sliding ROI per frame.
 * 
 */
class LaneDetector {
    private:
        cv::Mat getTransformationMatrix();
        cv::Mat detectLanes(cv::Mat frame);
        std::vector<cv::Point2f> computeSlidingROI(cv::Mat edges, cv::Rect window);
    public:
        cv::Point2f roi[4] = {{350, 400}, {850, 400}, {260, 719}, {1050, 719}};
        float wWidth = 640.0f;
        float wHeight = 480.0f;
        void slidingROI(cv::Mat frame);
};