#include <opencv2/aruco/charuco.hpp>
#include <opencv2/aruco.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>

namespace {
    const char *about = "Create a(n) ArUco/ChArUco board";

    const char* keys =
        "{@outfile  |<none> | Output Image}"
        "{w         |       | Number of squares in X direction}"
        "{h         |       | Number of squares in Y direction}"
        "{sl        |       | Square side length (in pixels)}"
        "{ml        |       | Marker side length (in pixels)}"
        "{d         |       | dictionary: DICT_4X4_50=0, DICT_4X4_100=1, DICT_4X4_250=2,"
        "DICT_4X4_100=3, DICT_5X5_50=4, DICT_5X5_100=5, DICT_5X5_250=6, DICT_5X5_1000=7"
        "DICT_6X6_50=8, DICT_6X6_100=9, DICT_6X6_250=10, DICT_6X6_1000=11, DICT_7X7_50=12,"
        "DICT_7X7_100=13, DICT_7X7_250=14, DICT_7X7_1000=15, DICT_ARUCO_ORIGINAL=16}"
        "{cd        |       | Input file with custom dictionary}"
        "{m         |       | Margin size (in pixels). Default is (squareLength-markerLength)}"
        "{ch        |false  | Generate a ChArUco board instead of ArUco}"
        "{bb        | 1     | Number of bits in marker borders}"
        "{si        |false  | show generated image}";
}



void drawBoard(cv::Ptr<cv::aruco::CharucoBoard> board, cv::Size imgSize, int margins, int borderBits)
{
    cv::Mat boardImage;
    std::cout << imgSize.area() << std::endl;
    board->draw(imgSize, boardImage, margins, borderBits);

    // Display image
    cv::namedWindow("Display Grid", cv::WINDOW_AUTOSIZE);
    cv::imshow("Display Grid", boardImage);
    // cv::imwrite("../imgs/grid.jpg", boardImage);

    cv::waitKey(0);
}

void drawBoard(cv::Ptr<cv::aruco::GridBoard> board, cv::Size imgSize, int margins, int borderBits)
{
    cv::Mat boardImage;
    board->draw(imgSize, boardImage, margins, borderBits);

    // Display image
    cv::namedWindow("Display Grid", cv::WINDOW_AUTOSIZE);
    cv::imshow("Display Grid", boardImage);
    // cv::imwrite("../imgs/grid.jpg", boardImage);

    cv::waitKey(0);
}

int main(int argc, char **argv)
{

    // Command line input (see keys above)
    cv::CommandLineParser parser(argc, argv, keys);
    parser.about(about);

    if (argc < 7)
    {
        parser.printMessage();
        return 0;
    }

    // Get input from command line
    int squaresX = parser.get<int>("w");
    int squaresY = parser.get<int>("h");
    int squareLength = parser.get<int>("sl");
    int markerLength = parser.get<int>("ml");
    int margins = squareLength- markerLength;

    bool genChArUco = parser.get<bool>("ch");

    if (parser.has("m"))
    {
        margins = parser.get<int>("m");
    }

    int borderBits = parser.get<int>("bb");
    int showImage = parser.get<bool>("si");

    std::string out = parser.get<std::string>(0);

    if (!parser.check())
    {
        parser.printErrors();
        return 0;
    }

    // Create a dictionary
    cv::Ptr<cv::aruco::Dictionary> dictionary
        = cv::aruco::getPredefinedDictionary(0);

    if (parser.has("d"))
    {
        auto dictionaryId = parser.get<int>("d");
        dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::PREDEFINED_DICTIONARY_NAME(dictionaryId));
    }
    // TODO : The cv::aruco::Dictionary::readDictionary() function does not exist.
    //          Need to find an alternative to reading custom dictionaries
    // else if (parser.has("cd"))
    // {
    //     cv::FileStorage fs(parser.get<std::string>("cd"), cv::FileStorage::READ);
    //     // bool ok = dictionary->cv::aruco::Dictionary::readDictionary(fs.root()); // Doesn't exist
    // }
    else
    {
        std::cerr << "Dictionary not specified" << std::endl;
        return 0;
    }

    // Formulate image size
    cv::Size  imageSize;
    imageSize.width = squaresX * squareLength + 2 * margins;
    imageSize.height = squaresY * squareLength + 2 * margins;
    std::cout << "Width: " << squaresX * squareLength + 2 * margins << std::endl;
    std::cout << "Height: " << squaresY * squareLength + 2 * margins << std::endl;
    // std::cout << "Square Length: " << squareLength << std::endl;
    std::cout << "Image size: " << imageSize.area() << std::endl;

    // Base class for all boards (GridBoard, ChArUco, ...)
    cv::Ptr<cv::aruco::Board> board;

    if (genChArUco)
    {
        board = cv::aruco::CharucoBoard::create(squaresX, squaresY,
                                                squareLength,
                                                markerLength,
                                                dictionary);
        drawBoard(board.staticCast<cv::aruco::CharucoBoard>(), imageSize, margins, borderBits);
    }
    else
    {
        board = cv::aruco::GridBoard::create(5,7,0.04, 0.01, dictionary);
        drawBoard(board.staticCast<cv::aruco::GridBoard>(), imageSize, margins, borderBits);
    }


    return 0;
}