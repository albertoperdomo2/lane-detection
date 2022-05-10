#define BOOST_LOG_DYN_LINK 1

#include <boost/log/trivial.hpp>
#include "../include/LaneDetector.h"

int main(int argc, char **argv) {
    if (argv[1] == NULL) {
        BOOST_LOG_TRIVIAL(error) << "missing media file!";
        std::exit(EXIT_FAILURE);
    } 
    const std::string videoPath = argv[1];

    cv::VideoCapture cap(videoPath);
    cv::Mat frame;

    // custom for this camera/video setup
    // cv::Point2f src[4] = {{350, 400}, {850, 400}, {260, 719}, {1050, 719}}; 

    LaneDetector detector;
    detector.wWidth = 640.0f;
    detector.wHeight = 480.0f;
    // detector.roi = src; // TODO: cv::Point2f does not support assigment but I'm not sure how to copy the src object into the class one. 

    BOOST_LOG_TRIVIAL(info) << "playing video...";
    while (true) {
        if (!cap.isOpened()) {
            BOOST_LOG_TRIVIAL(error) << "cannot open video stream";
            std::exit(EXIT_FAILURE);
        }

        bool notEmpty = cap.read(frame);
        if (notEmpty) {
            detector.slidingROI(frame);

            if((char)cv::waitKey(1) == 27) {
                BOOST_LOG_TRIVIAL(info) << "ESC key pressed - exiting";
                break;
            };
        } else {
            BOOST_LOG_TRIVIAL(info) << "empty frame, assuming EOF - exiting";
            break;
        }
    };
    
    return 0;
} 