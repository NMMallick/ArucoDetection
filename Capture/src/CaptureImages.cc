#include <iostream>
#include <opencv2/opencv.hpp>

#include <termios.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>

static volatile sig_atomic_t done = 0;

void handlr(int sig)
{
    done = 1;
}

int main(int argc, char **argv)
{
    // Terminal Interface
    static struct termios curr_t, new_t;
    tcgetattr(STDIN_FILENO, &curr_t);
    new_t = curr_t;
    new_t.c_lflag &= ~ICANON;
    tcsetattr(STDIN_FILENO, 0, &new_t);

    struct sigaction act;
    act.sa_handler = handlr;
    sigaction(SIGINT, &act, NULL);

    // Grab the camera stream
    cv::VideoCapture cap(0);

    if (!cap.isOpened())
    {
        std::cout << "No video stream detected" << std::endl;
        return -1;
    }

    cv::Mat frame;
    int seq = 0;

    while (true)
    {
        cap >> frame;

        // No frame
        if (frame.empty())
            break;

        char k = getchar();
        std::cout << k << std::endl;

        if (k == 27)
            break;

        if (k == 32)
        {
            cv::imwrite("image_"+std::to_string(seq)+".png", frame);
            seq++;
            std::cout << "Image wrote!" << std::endl;
        }

    }

    // Reset the terminal mode to what it was before
    tcsetattr(STDIN_FILENO, 0, &curr_t);

    cap.release();
    return 0;
}

