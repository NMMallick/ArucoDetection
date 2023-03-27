#include <opencv2/opencv.hpp>//OpenCV header to use VideoCapture class//
#include <iostream>

using namespace std;
using namespace cv;

int main() {

   //Declaring a matrix to load the frames
   Mat myImage, resizedImage;

   //Declaring the video to show the video
   namedWindow("Video Player");

   //Declaring an object to capture stream of frames from default camera
   VideoCapture cap(0);

   //This section prompt an error message if no video stream is found
   if (!cap.isOpened())
   {
      system("pause");
      cout << "No video stream detected" << endl;
      return-1;
   }

   //Taking an everlasting loop to show the video
   while (true)
   {

      cap >> myImage;

      //Breaking the loop if no video frame is detected
      if (myImage.empty())
         break;

      // Resize the image
      cv::resize(myImage, resizedImage, cv::Size(), 0.4, 0.4);

      //Showing the video
      imshow("Video Player", resizedImage);

      //Allowing 25 milliseconds frame processing time and initiating break condition
      char c = (char)waitKey(1);

      //If 'Esc' is entered break the loop
      if (c == 27)
         break;

   }

   //Releasing the buffer memory
   cap.release();
   return 0;
}