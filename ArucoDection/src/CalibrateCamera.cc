// #include <opencv2/highgui.hpp>
// #include <opencv2/calib3d.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/aruco/charuco.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>

#include <vector>
#include <iostream>

namespace {

const char* about =
        "Calibration using a ChArUco board\n"
        "  To capture a frame for calibration, press 'c',\n"
        "  If input comes from video, press any key for next frame\n"
        "  To finish capturing, press 'ESC' key and calibration starts.\n";

// Keys for commandline parser
// TODO: this might be changed for more a custom application
const char* keys  =
        "{w        |       | Number of squares in X direction }"
        "{h        |       | Number of squares in Y direction }"
        "{sl       |       | Square side length (in meters) }"
        "{ml       |       | Marker side length (in meters) }"
        "{d        |       | dictionary: DICT_4X4_50=0, DICT_4X4_100=1, DICT_4X4_250=2,"
        "DICT_4X4_1000=3, DICT_5X5_50=4, DICT_5X5_100=5, DICT_5X5_250=6, DICT_5X5_1000=7, "
        "DICT_6X6_50=8, DICT_6X6_100=9, DICT_6X6_250=10, DICT_6X6_1000=11, DICT_7X7_50=12,"
        "DICT_7X7_100=13, DICT_7X7_250=14, DICT_7X7_1000=15, DICT_ARUCO_ORIGINAL = 16}"
        "{cd       |       | Input file with custom dictionary }"
        "{@outfile |<none> | Output file with calibrated camera parameters }"
        "{v        |       | Input from video file, if ommited, input comes from camera }"
        "{ci       | 0     | Camera id if input doesnt come from video (-v) }"
        "{dp       |       | File of marker detector parameters }"
        "{rs       | false | Apply refind strategy }"
        "{zt       | false | Assume zero tangential distortion }"
        "{a        |       | Fix aspect ratio (fx/fy) to this value }"
        "{pc       | false | Fix the principal point at the center }"
        "{sc       | false | Show detected chessboard corners after calibration }";



inline static bool readCameraParameters(std::string filename, cv::Mat &camMatrix, cv::Mat &distCoeffs) {
    cv::FileStorage fs(filename, cv::FileStorage::READ);
    if (!fs.isOpened())
        return false;
    fs["camera_matrix"] >> camMatrix;
    fs["distortion_coefficients"] >> distCoeffs;
    return true;
}

inline static bool saveCameraParams(const std::string &filename, cv::Size imageSize, float aspectRatio, int flags,
                                    const cv::Mat &cameraMatrix, const cv::Mat &distCoeffs, double totalAvgErr) {
    cv::FileStorage fs(filename, cv::FileStorage::WRITE);
    if (!fs.isOpened())
        return false;

    time_t tt;
    time(&tt);
    struct tm *t2 = localtime(&tt);
    char buf[1024];
    strftime(buf, sizeof(buf) - 1, "%c", t2);

    fs << "calibration_time" << buf;
    fs << "image_width" << imageSize.width;
    fs << "image_height" << imageSize.height;

    if (flags & cv::CALIB_FIX_ASPECT_RATIO) fs << "aspectRatio" << aspectRatio;

    if (flags != 0) {
        sprintf(buf, "flags: %s%s%s%s",
                flags & cv::CALIB_USE_INTRINSIC_GUESS ? "+use_intrinsic_guess" : "",
                flags & cv::CALIB_FIX_ASPECT_RATIO ? "+fix_aspectRatio" : "",
                flags & cv::CALIB_FIX_PRINCIPAL_POINT ? "+fix_principal_point" : "",
                flags & cv::CALIB_ZERO_TANGENT_DIST ? "+zero_tangent_dist" : "");
    }
    fs << "flags" << flags;
    fs << "camera_matrix" << cameraMatrix;
    fs << "distortion_coefficients" << distCoeffs;
    fs << "avg_reprojection_error" << totalAvgErr;
    return true;

}

}

int main(int argc, char **argv)
{
    cv::CommandLineParser parser(argc, argv, keys);
    parser.about(about);

    if (argc < 7)
    {
        parser.printMessage();
        return 0;
    }


    int squaresX = parser.get<int>("w");
    int squaresY = parser.get<int>("h");
    float squareLength = parser.get<float>("sl");
    float markerLength = parser.get<float>("ml");

    std::string outputFile = parser.get<std::string>(0);

    bool showChessboardCorners = parser.get<bool>("sc");

    int calibrationFlags = 0;
    float aspectRatio = 1;

    if (parser.has("a"))
    {
        calibrationFlags |= cv::CALIB_FIX_ASPECT_RATIO;
        aspectRatio = parser.get<float>("a");
    }

    if (parser.get<bool>("zt"))
        calibrationFlags |= cv::CALIB_ZERO_TANGENT_DIST;
    if (parser.get<bool>("pc"))
        calibrationFlags |= cv::CALIB_FIX_PRINCIPAL_POINT;

    // Detector Parameters not working in sample???
    cv::Ptr<cv::aruco::DetectorParameters> detectorParams = cv::aruco::DetectorParameters::create();
    // if(parser.has("dp")) {
    //     cv::FileStorage fs(parser.get<std::string>("dp"), cv::FileStorage::READ);
    //     bool readOk = detectorParams->readDetectorParameters(fs.root());
    //     if(!readOk) {
    //         std::cerr << "Invalid detector parameters file" << std::endl;
    //         return 0;
    //     }
    // }

    bool refindStrategy = parser.get<bool>("rs");
    int camId = parser.get<int>("ci");

    cv::String video;

    if (parser.has("v"))
        video = parser.get<cv::String>("v");

    if (!parser.check())
    {
        parser.printErrors();
        return 0;
    }

    cv::VideoCapture inputVideo;
    int waitTime;

    if (!video.empty())
    {
        inputVideo.open(video);
        waitTime = 0;
    } else
    {
        inputVideo.open(camId);
        waitTime = 10;
    }

    cv::Ptr<cv::aruco::Dictionary> dictionary = cv::aruco::getPredefinedDictionary(0);
    if (parser.has("d"))
    {
        int dictionaryId = parser.get<int>("d");
        dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::PREDEFINED_DICTIONARY_NAME(dictionaryId));
    } else
    {
        std::cerr << "Dictionary not specified" << std::endl;
        return 0;
    }

    // create charuco board object
    cv::Ptr<cv::aruco::CharucoBoard> charucoboard =
        cv::aruco::CharucoBoard::create(squaresX, squaresY, squareLength, markerLength, dictionary);
    cv::Ptr<cv::aruco::Board> board = charucoboard.staticCast<cv::aruco::Board>();

    // collect data from each fram
    std::vector<std::vector<std::vector<cv::Point2f>>> allCorners;
    std::vector<std::vector<int>> allIds;
    std::vector<cv::Mat> allImgs;
    cv::Size imgSize;

    // Capture video input
    while (inputVideo.grab())
    {
        cv::Mat image, imageCopy;
        inputVideo.retrieve(image);

        std::vector<int> ids;
        std::vector<std::vector<cv::Point2f>> corners, rejected;

        // detect markers
        cv::aruco::detectMarkers(image, dictionary, corners, ids, detectorParams, rejected);

        // refind strategy to detect more markers
        if (refindStrategy)
            cv::aruco::refineDetectedMarkers(image, board, corners, ids, rejected);

        // interpolate charuco corners
        cv::Mat currentCharucoCorners, currentCharucoIds;
        if (ids.size() > 0)
            cv::aruco::interpolateCornersCharuco(corners, ids, image, charucoboard, currentCharucoCorners, currentCharucoIds);

        // draw results
        image.copyTo(imageCopy);
        if (ids.size() > 0 )
            cv::aruco::drawDetectedCornersCharuco(imageCopy, currentCharucoCorners, currentCharucoIds);

        cv::putText(imageCopy, "Press 'c' to add current frame. 'ESC' to finish and calibrate",
                        cv::Point(10, 20), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 0 , 0), 2);

        cv::imshow("out", imageCopy);

        std::cout << "am i looping" << std::endl;

        char key = (char)cv::waitKey(waitTime);
        if (key == 27)
            break;

        if (key == 'c' && ids.size() > 0)
        {
            std::cout << "Frame captured" << std::endl;
            allCorners.push_back(corners);
            allIds.push_back(ids);
            allImgs.push_back(image);
            imgSize = image.size();
        }

    }

    if (allIds.size() < 1)
    {
        std::cerr << "not enough captures for calibration" << std::endl;
    }

    cv::Mat cameraMatrix, distCoeffs;
    std::vector<cv::Mat> rvecs, tvecs;

    double repError;

    if (calibrationFlags & cv::CALIB_FIX_ASPECT_RATIO)
    {
        cameraMatrix = cv::Mat::eye(3, 3, CV_64F);
        cameraMatrix.at<double>(0,0) = aspectRatio;
    }

    // prepare data for calibration
    std::vector<std::vector<cv::Point2f>> allCornersConcatenated;
    std::vector<int> allIdsConcatenated;
    std::vector<int> markerCounterPerFrame;

    markerCounterPerFrame.reserve(allCorners.size());

    for (unsigned int i = 0; i < allCorners.size(); i++)
    {
        markerCounterPerFrame.push_back((int)allCorners[i].size());

        for(unsigned int j = 0; j < allCorners[i].size(); j++)
        {
            allCornersConcatenated.push_back(allCorners[i][j]);
            allIdsConcatenated.push_back(allIds[i][j]);
        }
    }

    // calibrate camera using arcuo makers
    double arucoRepErr;
    arucoRepErr = cv::aruco::calibrateCameraAruco(allCornersConcatenated, allIdsConcatenated,
                                                    markerCounterPerFrame, board, imgSize, cameraMatrix,
                                                    distCoeffs, cv::noArray(), cv::noArray(), calibrationFlags);

    // prepare data for charuco calibration
    int nFrames = (int)allCorners.size();
    std::vector<cv::Mat> allCharucoCorners;
    std::vector<cv::Mat> allCharucoIds;
    std::vector<cv::Mat> filteredImages;
    allCharucoCorners.reserve(nFrames);
    allCharucoIds.reserve(nFrames);

    for (int i = 0; i < nFrames; i ++)
    {
        // interpolate using camera parameters
        cv::Mat currentCharucoCorners, currentCharucoIds;
        cv::aruco::interpolateCornersCharuco(allCorners[i], allIds[i], allImgs[i], charucoboard,
                                                currentCharucoCorners, currentCharucoIds, cameraMatrix,
                                                distCoeffs);

        allCharucoCorners.push_back(currentCharucoCorners);
        allCharucoIds.push_back(currentCharucoIds);
        filteredImages.push_back(allImgs[i]);
    }

    if (allCharucoCorners.size() < 4)
    {
        std::cerr << "Not enough corners for calibration" << std::endl;
        return 0;
    }

    // calibrate camera using charcuo
    repError =
        cv::aruco::calibrateCameraCharuco(allCharucoCorners, allCharucoIds, charucoboard, imgSize,
                                            cameraMatrix, distCoeffs, rvecs, tvecs, calibrationFlags);

    // No function saveCameraParams ???
    bool saveOk = saveCameraParams(outputFile, imgSize, aspectRatio, calibrationFlags,
                            cameraMatrix, distCoeffs, repError);
    if (!saveOk)
    {
        std::cerr << "Cannot save output file" << std::endl;
        return 0;
    }

    std::cout << "Rep Error: " << repError << std::endl;
    std::cout << "Rep Error Aruco: " << arucoRepErr << std::endl;
    std::cout << "Calibration saved to " << outputFile << std::endl;

    // show interpolated charuco corners for debugging
    if(showChessboardCorners) {
        for(unsigned int frame = 0; frame < filteredImages.size(); frame++) {
            cv::Mat imageCopy = filteredImages[frame].clone();
            if(allIds[frame].size() > 0) {

                if(allCharucoCorners[frame].total() > 0) {
                    cv::aruco::drawDetectedCornersCharuco( imageCopy, allCharucoCorners[frame],
                                                    allCharucoIds[frame]);
                }
            }

            imshow("out", imageCopy);
            char key = (char)cv::waitKey(0);
            if(key == 27) break;
        }
    }

    return 0;
}

