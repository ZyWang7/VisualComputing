/*

To run the file:
$ g++ myOpencv.cpp -o test `pkg-config --cflags --libs opencv4`
$ ./test

-> you can change the 'test' to whatever you like.


The image path is obtained by the user input at the terminal.
Once the program checks that the path is valid, 2 windows will be displayed:
1. THRESH_BINARY window with a trackbar and an initial threshold value of 0;
2. THRESH_OTSU window.

Press 'p' or 's' to save the THRESH_BINARY image with the current threshold.
Press Esc to stop and exit the program.

Note: 
All other keys are ignored.
All the file is saved in '.png'.

*/

#include <iostream>
#include "opencv2/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"

#include <string>
#include <fstream>

using namespace std;
using namespace cv;

// global variables
Mat greyImage;
Mat out_bin;
Mat out_otsu;
int threshold_value = 0;

static void myTrack(int, void*) {
    threshold(greyImage, out_bin, threshold_value, 255, THRESH_BINARY);
    // add image to window for type THRESH_BINARY
    imshow("Threshold Binary", out_bin);
}

int main(int argc, char *argv[]){
    // get user input in the terminal to get the image path and check
    string path; 
    ifstream file;
    cout << "Please enter the image path: ";
    getline(cin, path);      // in order to read the whole line
    // check the image path
    file.open(path);
    if (!file) {
        cout << "File does not exists." << endl;
        return 1;
    }
    file.close();

    // read image and check is loaded or not
    Mat inputImage = imread(path, IMREAD_UNCHANGED);
    if (inputImage.data == NULL) {
        cout << "Error: failed to open image file." << endl;
        return 1;
    }

    // convert to an 8-bit greyscale image
    cvtColor(inputImage, greyImage, COLOR_BGR2GRAY, 0);

    // display trackbar of type THRESH_BINARY in a window
    namedWindow("Threshold Binary", WINDOW_AUTOSIZE);
    // Create a Trackbar to choose Threshold value for THRESH_BINARY
    createTrackbar("Threshold Value", "Threshold Binary", &threshold_value, 255, myTrack);
    myTrack(0, 0);

    // display type THRESH_OTSU in another window
    namedWindow("Threshold OTSU", WINDOW_AUTOSIZE);
    threshold(greyImage, out_otsu, 0, 255, THRESH_OTSU);
    imshow("Threshold OTSU", out_otsu);

    // get the image name without suffix
    size_t pos = path.find(".");
    string newName = path.substr(0,pos);

    // Wait for user to press key
    int c;
    while (true) {
        c = waitKey();
        // press Esc key to stop
        if (c == 27) {
            destroyAllWindows();
            break;
        // when the user presses 'p' or 's'
        } else if (c == 112 || c == 115) {
            // save the image at current threshold value for THRESH_BINARY
            string outname = newName + "_bin_" + to_string(threshold_value) + ".png";
            imwrite(outname, out_bin);
        }
    }

    // save the image for THRESH_OTSU
    imwrite(newName + "_otsu.png", out_otsu);

    return 0;
}

