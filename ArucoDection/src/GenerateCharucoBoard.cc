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



void drawBoard(cv::aruco::CharucoBoard board, cv::Size imgSize,
                int margins, int borderBits, std::string output)
{
    cv::Mat boardImage;
    board.generateImage(imgSize, boardImage, margins, borderBits);

    // Display image
    cv::namedWindow("Display Grid", cv::WINDOW_AUTOSIZE);
    cv::imshow("Display Grid", boardImage);
    cv::imwrite(output, boardImage);

    cv::waitKey(0);
}

void drawBoard(cv::aruco::GridBoard board, cv::Size imgSize,
                int margins, int borderBits, std::string output)
{
    cv::Mat boardImage;
    board.generateImage(imgSize, boardImage, margins, borderBits);

    // Display image
    cv::namedWindow("Display Grid", cv::WINDOW_AUTOSIZE);
    cv::imshow("Display Grid", boardImage);
    cv::imwrite(output, boardImage);

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
    std::cout << out << std::endl;
    if (!parser.check())
    {
        parser.printErrors();
        return 0;
    }

    // Create a dictionary
    cv::aruco::Dictionary dictionary = cv::aruco::getPredefinedDictionary(0);

    if (parser.has("d"))
    {
        auto dictionaryId = parser.get<int>("d");
        dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::PredefinedDictionaryType(dictionaryId));
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

    // Base class for all boards (GridBoard, ChArUco, ...)

    if (genChArUco)
    {
        cv::aruco::CharucoBoard board(cv::Size(squaresX, squaresY),
                                                squareLength,
                                                markerLength,
                                                dictionary);
        drawBoard(board, imageSize, margins, borderBits, out);
    }
    else
    {
        cv::aruco::GridBoard board(cv::Size(squaresX, squaresY),
                                                squareLength,
                                                markerLength,
                                                dictionary);
        drawBoard(board, imageSize, margins, borderBits, out);
    }


    return 0;
}