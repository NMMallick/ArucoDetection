#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/aruco.hpp>


int main(int argc, char **argv)
{

    // Create a dictionary
    cv::Ptr<cv::aruco::Dictionary> dictionary
        = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_4X4_50);

    // Create a grid board object
    cv::Ptr<cv::aruco::GridBoard> board = cv::aruco::GridBoard::create(5,7,0.04, 0.01, dictionary);

    cv::Mat boardImage;
    board->draw(cv::Size(600, 500), boardImage, 10, 1);


    // Display image
    cv::namedWindow("Display Grid", cv::WINDOW_AUTOSIZE);
    cv::imshow("Display Grid", boardImage);
    cv::imwrite("../imgs/grid.jpg", boardImage);

    cv::waitKey(0);

    return 0;
}