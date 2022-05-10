# Lane detector
This repo contains a simple weekend project to create a lane detection tool. It could be even considered a POC, and it heavily realies on OpenCV, as expected. 

The way in which the ROI is detected is by using warped perspectives, which might not be suitable for all videos. As explained in the `data/` dir, the video used for this project can be found in [Kaggle](https://www.kaggle.com/datasets/dpamgautam/video-file-for-lane-detection-project). 

In order to keep things clean and tidy, there is a `LaneDetector` class which implements the lane detector. The default values present in the project are the ones that work with the aforementioned video, so other videos might need adjustment of those values (primarly the **roi** definition, but the HSV thresholds too). 

## Logic
The logic behind the code is, actually, really simple. In the following lines, the code is briefly explained:

In the private method `LaneDetector::getTransformationMatrix()`, the transformation matrix is obtained, which is later used to obtain the warped (bird's eye) image. 

In the private method `LaneDetector::detectLanes(cv::Mat frame)` is where the lanes *per se*, are detected. I followed the typical workflow: Converting to HSV, to use HSV thresholds to detect, in this case, the white and orange colors defining the lanes. Then, the detected lanes are combined using bitwise operations of the resulting images, and edges are detected using the Canny edge detector. 
Finally, erosion and dilation are applied to join the edges (and for potential contours).

In the private method `LaneDetector::computeSlidingROI(cv::Mat edges, cv::Rect slidingROI)` the defined **slidingROI** is use to compute the sliding window that will be used later to draw the ROI. In this method, we used the most-left and most-right edges to compute the side lines defining the ROI. To do so, the average X coordinates are computed. 

Finally, in the public method `LaneDetector::slidingROI(cv::Mat frame)` is where all the previous methods are used to detect the lane and the ROI of each frame. 

## Use
In order to compile and run the project, two steps should be followed:
1. Clone the repo:
```
git clone https://github.com/albertoperdomo2/lane-detection.git 
```

2. Download the video following the instructions in the `data/` directory (this is not mandatory, but keep in mind that in order to work with other videos, the ROI, HSV values and so on, should be changed to yours). 

3. Run the build script, which invokes **cmake**, present in the `build/` directory:
```
cd build/ && sh build.sh
``` 

4. Run the binary (in the build dir):
```
./main /path/to/video.mp4
```
