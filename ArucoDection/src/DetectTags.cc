#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/aruco.hpp>

#include <ArucoUtils.hh>

namespace {
    const char *about = "Aruco detection module motivated by the OpenCV library";

    const char *keys =
        "{@cameraParams |<none> | Camera calibrated parameters for pose detection }"
        "{d             |false  | Enable debug mode}";
}

int main(int argc, char **argv)
{

    // command line arguments
    cv::CommandLineParser parser(argc, argv, keys);
    parser.about(about);

    if (argc < 2)
    {
        parser.printMessage();
        return 0;
    }

    auto debug = false;
    if (parser.has("d"))
        debug = parser.get<bool>("d");

    // Configure video input
    cv::VideoCapture inputVideo;
    inputVideo.open(0);

    // Get predefined dictionary
    cv::Ptr<cv::aruco::Dictionary> dictionary
        = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_4X4_50);

    // Camera calibrations for pose estimation
    cv::Mat cameraMatrix, distCoeffs;
    std::string filename = parser.get<std::string>(0); // filename for camera matrix and distance coefficients

    // Read camera calibration parameters
    readCameraParameters(filename, cameraMatrix, distCoeffs);

    while (inputVideo.grab())
    {
        cv::Mat image, imageCopy;

        inputVideo.retrieve(image);
        image.copyTo(imageCopy);

        // Vec to store ids
        std::vector<int> ids;
        std::vector<std::vector<cv::Point2f>> corners;
        cv::aruco::detectMarkers(image, dictionary, corners, ids);

        // if at least one marker detected
        if (ids.size() > 0)
        {
            cv::aruco::drawDetectedMarkers(imageCopy, corners, ids);

            std::vector<cv::Vec3d> rvecs, tvecs;
            cv::aruco::estimatePoseSingleMarkers(corners, 0.0520, cameraMatrix, distCoeffs, rvecs, tvecs);

            if (rvecs.size() == tvecs.size())
            {
                for(int i = 0; i < ids.size(); i++)
                {
                    cv::drawFrameAxes(imageCopy, cameraMatrix, distCoeffs, rvecs[i], tvecs[i], 0.05);

                    std::cout << "Tag ID: " << ids[i] << std::endl;
                    std::cout << "x: " << tvecs[i][0] <<"\ty: " << tvecs[i][1] << "\tz: " << tvecs[i][2] << std::endl;
                }

            }
        }

        cv::resize(imageCopy, imageCopy, cv::Size(), 0.6, 0.6);

        // Display the image
        cv::imshow("out", imageCopy);
        char key = (char) cv::waitKey(1);

        if (key == 27)
            break;
    }


    return 0;
}