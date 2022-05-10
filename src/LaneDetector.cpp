#include "../include/LaneDetector.h"

/**
 * @brief Compute the transformation matrix to convert each frame ROI to bird's eye.
 * @return cv::Mat transformation matrix.
 */
cv::Mat LaneDetector::getTransformationMatrix() {
    cv::Point2f dst[4] = {{.0f, .0f}, {this->wWidth, .0f}, {.0f, this->wHeight}, {this->wWidth, this->wHeight}};
    cv::Mat transformationMatrix = cv::getPerspectiveTransform(this->roi, dst); // convert to bird's eye

    return transformationMatrix;
}

/**
 * @brief Detect lanes by performing the following steps:
 * 1. Detect both white and orange (lane colors).
 * 2. Perform bitwise operations to keep only the detected lanes.
 * 3. Perform typical edge detection flow: GaussianBlur to remove noise, Canny algorithm for edge detection
 * and dilation and erosion to fill and connected edges. 
 * @param frame video frame that is being processed
 * @return cv::Mat
 */
cv::Mat LaneDetector::detectLanes(cv::Mat frame) {
    const int thresholdVal = 150;
    cv::Mat hsvImg, grayImg, whiteMask, orangeMask, bitwiseOr, wholeMask, imgBlurred, edges;

    // this values are obtained empirically by performing HSV analysis of a single frame.
    cv::Scalar orangeLower(0, 110, 153);
    cv::Scalar orangeUpper(19, 240, 255);
    cv::Scalar whiteLower(0, 0, 170);
    cv::Scalar whiteUpper(170, 110, 255);

    // obtain grayscale and HSV images
    cv::cvtColor(frame, grayImg, cv::COLOR_BGR2GRAY);
    cv::cvtColor(frame, hsvImg, cv::COLOR_BGR2HSV);

    cv::inRange(hsvImg, orangeLower, orangeUpper, orangeMask); // detect orange color
    cv::inRange(hsvImg, whiteLower, whiteUpper, whiteMask); // detect white color

    // joint both orange and white masks
    cv::bitwise_or(whiteMask, orangeMask, bitwiseOr);
    cv::bitwise_and(grayImg, bitwiseOr, wholeMask);

    cv::GaussianBlur(wholeMask, imgBlurred, cv::Size(3,3), 3, 0); // smooth the noise
    cv::Canny(imgBlurred, edges, 50.0, 150.0); // edge detection

    // try to fill the gaps
    cv::dilate(edges, edges, cv::Mat::ones(15, 15, CV_8U));
    cv::erode(edges, edges, cv::Mat::ones(15, 15, CV_8U));
    cv::morphologyEx(edges, edges, cv::MORPH_CLOSE, cv::Mat::ones(15, 15, CV_8U));
    
    // below thresholdVal, black
    cv::threshold(edges, edges, thresholdVal, 255, cv::THRESH_BINARY);

    return edges;
}

/**
 * @brief Compute the sliding window containing the ROI. It does so by computing average X coordinates
 * of the detected pixels in each rectangle, and linking the dots.
 * 
 * @param edges 
 * @param slidingROI 
 * @return std::vector<cv::Point2f> 
 */
std::vector<cv::Point2f> LaneDetector::computeSlidingROI(cv::Mat edges, cv::Rect slidingROI) {
    std::vector<cv::Point2f> points;
    const cv::Size imgSize = edges.size();
    bool exit = false;
    
    while (true) {
        float currentX = slidingROI.x + slidingROI.width * 0.5f;
        cv::Mat roi = edges(slidingROI);

        std::vector<cv::Point2f> locations;
        // get the nonzero pixels i.e. white ones
        cv::findNonZero(roi, locations);

        // compute avg x
        float avgX = 0.0f;
        for (int i = 0; i < locations.size(); ++i) {
            float x = locations[i].x;
            avgX += slidingROI.x + x;
        }
        avgX = locations.empty() ? currentX : avgX / locations.size();

        cv::Point point(avgX, slidingROI.y + slidingROI.height * 0.5f);
        points.push_back(point);

        slidingROI.y -= slidingROI.height;

        // for the uppermost position
        if (slidingROI.y < 0) {
            slidingROI.y = 0;
            exit = true;
        }

        // move x position
        slidingROI.x += (point.x - currentX);

        // make sure the slidingROI doesn't overflow (error if trying to get data outside the matrix)
        if (slidingROI.x < 0) {
            slidingROI.x = 0;
        }

        if (slidingROI.x + slidingROI.width >= imgSize.width) {
            slidingROI.x = imgSize.width - slidingROI.width - 1;
        }

        if (exit) {
            break;
        }
    }

    return points;
}

/**
 * @brief Draw the actual sliding ROI window by using the previously presented functions.
 * It doesn't return anything but displays the processed frame.
 * 
 * @param frame 
 */
void LaneDetector::slidingROI(cv::Mat frame) {
    cv::Mat invTransformationMatrix, warpedImg;

    // used for the end polygon at the end
    std::vector<cv::Point> allPts; 
    std::vector<cv::Point2f> outPts;

    cv::Mat transformationMatrix = this->LaneDetector::getTransformationMatrix();
    cv::invert(transformationMatrix, invTransformationMatrix);

    cv::warpPerspective(frame, warpedImg, transformationMatrix, cv::Point(wWidth, wHeight));
    cv::Mat edges = this->detectLanes(warpedImg);

    
    std::vector<cv::Point2f> pts = computeSlidingROI(edges, cv::Rect(0, this->wHeight-60.0f, 120, 60)); // left most 120x60 rect of the warped image
    cv::perspectiveTransform(pts, outPts, invTransformationMatrix); // transform points back into original image space

    // draw left lane 
    for (int i = 0; i < outPts.size() - 1; ++i) {
        cv::line(frame, outPts[i], outPts[i+1], cv::Scalar(0, 0, 255), 3);
        allPts.push_back(cv::Point(outPts[i].x, outPts[i].y));
    }

    allPts.push_back(cv::Point(outPts[outPts.size() - 1].x, outPts[outPts.size() - 1].y));

    pts = computeSlidingROI(edges, cv::Rect(this->wWidth - 120.0f, this->wHeight-60.0f, 120, 60)); // right most 120x60 rect of the warped image
    cv::perspectiveTransform(pts, outPts, invTransformationMatrix);

    // draw right lane
    for (int i = 0; i < outPts.size() - 1; ++i) {
        cv::line(frame, outPts[i], outPts[i+1], cv::Scalar(0, 0, 255), 3);
        allPts.push_back(cv::Point(outPts[outPts.size() - i - 1].x, outPts[outPts.size() - i - 1].y));
    }

    allPts.push_back(cv::Point(outPts[0].x - (outPts.size() - 1) , outPts[0].y));

    // draw green area
    std::vector<std::vector<cv::Point>> greenArea;
    greenArea.push_back(allPts);
    cv::Mat overlay = cv::Mat::zeros(frame.size(), frame.type());
    cv::fillPoly(overlay, greenArea, cv::Scalar(0, 255, 100));
    cv::addWeighted(frame, 1, overlay, 0.5, 0, frame);

    cv::imshow("LANE DETECTION", frame);
}         