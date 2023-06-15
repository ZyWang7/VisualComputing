/*

To run the file:
$ g++ myOpencv.cpp -o test `pkg-config --cflags --libs opencv4`
$ ./test

-> you can change the 'test' to whatever you like.

The program will ask the user to enter the path of the image.
if the path is incorrect, it will directly exit without doing anything.

for the window with trackbar displayed:
1. Press 'Esc' -> exit the current window
2. Press 's' or 'p' to save the image with current threshold
3. Any other key will be ignored.
Note: please do not close the window by clicking the x button, only use 'Esc'.

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

//Polynomial regression function
std::vector<double> fitPoly(std::vector<cv::Point> points, int n)
{
  //Number of points
  int nPoints = points.size();

  //Vectors for all the points' xs and ys
  std::vector<float> xValues = std::vector<float>();
  std::vector<float> yValues = std::vector<float>();

  //Split the points into two vectors for x and y values
  for(int i = 0; i < nPoints; i++)
  {
    xValues.push_back(points[i].x);
    yValues.push_back(points[i].y);
  }

  //Augmented matrix
  double matrixSystem[n+1][n+2];
  for(int row = 0; row < n+1; row++)
  {
    for(int col = 0; col < n+1; col++)
    {
      matrixSystem[row][col] = 0;
      for(int i = 0; i < nPoints; i++)
        matrixSystem[row][col] += pow(xValues[i], row + col);
    }

    matrixSystem[row][n+1] = 0;
    for(int i = 0; i < nPoints; i++)
      matrixSystem[row][n+1] += pow(xValues[i], row) * yValues[i];

  }

  //Array that holds all the coefficients
  double coeffVec[n+2] = {};  // the "= {}" is needed in visual studio, but not in Linux

  //Gauss reduction
  for(int i = 0; i <= n-1; i++)
    for (int k=i+1; k <= n; k++)
    {
      double t=matrixSystem[k][i]/matrixSystem[i][i];

      for (int j=0;j<=n+1;j++)
        matrixSystem[k][j]=matrixSystem[k][j]-t*matrixSystem[i][j];

    }

  //Back-substitution
  for (int i=n;i>=0;i--)
  {
    coeffVec[i]=matrixSystem[i][n+1];
    for (int j=0;j<=n+1;j++)
      if (j!=i)
        coeffVec[i]=coeffVec[i]-matrixSystem[i][j]*coeffVec[j];

    coeffVec[i]=coeffVec[i]/matrixSystem[i][i];
  }

  //Construct the cv vector and return it
  std::vector<double> result = std::vector<double>();
  for(int i = 0; i < n+1; i++)
    result.push_back(coeffVec[i]);
  return result;
}

//Returns the point for the equation determined
//by a vector of coefficents, at a certain x location
cv::Point pointAtX(std::vector<double> coeff, double x)
{
  double y = 0;
  for(int i = 0; i < coeff.size(); i++)
  y += pow(x, i) * coeff[i];
  return cv::Point(x, y);
}


Mat gblur;
Mat canny_out;
int thre_L = 0;
int thre_H = 0;
static void canny_track(int, void*) {
    Canny(gblur, canny_out, thre_L, thre_H);
    // add image to window
    imshow("Canny Filter", canny_out);
}

Mat hough_out;
vector<Vec4i> dest_vec;     // will hold the results of the detection
// float rho = 1;// the resolution of the accumulator in pixels
// float theta = CV_PI/180;// the angle resolution of the accumulator in degrees
int hough_thre = 0;     // accumulator threshold parameter
int line_gap = 0;
static void hough_track(int, void*) {
    // leave the min length to zero
    HoughLinesP(canny_out, dest_vec, 1, CV_PI/180, hough_thre, 0, line_gap);
    // Draw the lines
    cvtColor(canny_out, hough_out, COLOR_GRAY2BGR);
    for( size_t i = 0; i < dest_vec.size(); i++ ){
        Vec4i l = dest_vec[i];
        line(hough_out, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0,0,255), 3, LINE_AA);
    }
    imshow("Hough Line Trans", hough_out);
}

Mat length_out;
int length_thre = 50;
static void length_track(int, void*) {
    double len = 0;
    cvtColor(canny_out, length_out, COLOR_GRAY2BGR);
    for( size_t i = 0; i < dest_vec.size(); i++ ){
        Vec4i l = dest_vec[i];
        // calculate the length of the line
        len = sqrt(pow(l[0] - l[2], 2) + pow(l[1] - l[3], 2));
        if (len >= length_thre) {
            line(length_out, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0,0,255), 3, LINE_AA);
        }
    }
    imshow("Short Line Filter", length_out);
}

vector<Vec4i> long_lines;   // record lines after removing the short lines
Mat angle_out;
int tan_thre = 0;
static void angle_track(int, void*) {
    float angle = 0;
    cvtColor(canny_out, angle_out, COLOR_GRAY2BGR);
    for( size_t i = 0; i < long_lines.size(); i++ ){
        Vec4i l = long_lines[i];
        // calculate the inverse tangent
        int hor = abs(l[0] - l[2]);
        int ver = abs(l[1] - l[3]);
        angle = atan2(ver, hor) * 180 / CV_PI;
        if (angle <= tan_thre) {
            line(angle_out, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0,0,255), 3, LINE_AA);
        }
    }
    imshow("Vertical Line Filter", angle_out);
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

    // read the image and preprocessing the image ------------------------------
    Mat inputImage = imread(path, IMREAD_UNCHANGED);
    if (inputImage.data == NULL) {
        cout << "Error: failed to open image file." << endl;
        return 1;
    }
    // convert to an 8-bit greyscale image
    Mat greyImage;
    cvtColor(inputImage, greyImage, COLOR_BGR2GRAY, 0);
    // pre-procession image -> GaussianBlur
    GaussianBlur(greyImage, gblur, Size(15,15), 1.0);

    // apply Canny edge detection ----------------------------------------------
    // display trackbar for two threshold for canny filter
    namedWindow("Canny Filter", WINDOW_AUTOSIZE);

    createTrackbar("Low Threshold", "Canny Filter", &thre_L, 100, canny_track);
    createTrackbar("High Threshold", "Canny Filter", &thre_H, 255, canny_track);
    canny_track(0, 0);

    // get the image name without suffix
    size_t pos = path.find(".");
    string newName = path.substr(0,pos);
    // Wait for user to press any key
    int c;
    while (true) {
        c = waitKey();
        // press Esc key to quit
        if (c == 27) {
            destroyAllWindows();
            break;
        // when the user presses 'p' or 's'
        } else if (c == 112 || c == 115) {
            // save the image at current threshold value
            string outname = newName + "_canny_" + to_string(thre_L) + "_" + to_string(thre_H) + ".png";
            imwrite(outname, canny_out);
        }
    }

    // apply probabilistic Hough transformation --------------------------------
    namedWindow("Hough Line Trans", WINDOW_AUTOSIZE);

    // Create a Trackbar to choose Threshold value and maxlinegap for Hough Line
    createTrackbar("Hough Line Threshold", "Hough Line Trans", &hough_thre, 255, hough_track);
    createTrackbar("Max Line Gap", "Hough Line Trans", &line_gap, 50, hough_track);
    hough_track(0, 0);

    while (true) {
        c = waitKey();
        if (c == 27) {
            destroyAllWindows();
            break;
        } else if (c == 112 || c == 115) {
            string outname = newName + "_hough_" + to_string(hough_thre) + "_" + to_string(line_gap) + ".png";
            imwrite(outname, hough_out);
        }
    }

    // filter out the short lines ----------------------------------------------
    namedWindow("Short Line Filter", WINDOW_AUTOSIZE);
    createTrackbar("Length Threshold", "Short Line Filter", &length_thre, 200, length_track);
    length_track(0, 0);

    while (true) {
        c = waitKey();
        if (c == 27) {
            destroyAllWindows();
            break;
        } else if (c == 112 || c == 115) {
            string outname = newName + "_short_remove_" + to_string(length_thre) + ".png";
            imwrite(outname, length_out);
        }
    }

    // save the remain lines into a new vector long_lines
    double len;
    for( size_t i = 0; i < dest_vec.size(); i++ ){
        Vec4i l = dest_vec[i];
        // calculate the length of the line
        len = sqrt(pow(l[0] - l[2], 2) + pow(l[1] - l[3], 2));
        if (len >= length_thre) {
            long_lines.push_back(l);
        }
    }

    // filter out the vertical lines -------------------------------------------
    namedWindow("Vertical Line Filter", WINDOW_AUTOSIZE);
    createTrackbar("Inverse Tangent(Degree)", "Vertical Line Filter", &tan_thre, 100, angle_track);
    angle_track(0, 0);

    while (true) {
        c = waitKey();
        if (c == 27) {
            destroyAllWindows();
            break;
        } else if (c == 112 || c == 115) {
            string outname = newName + "_hor_lines_" + to_string(tan_thre) + ".png";
            imwrite(outname, angle_out);
        }
    }

    // save the remain points into a new vector points
    vector<Point> points;
    float angle = 0;
    for( size_t i = 0; i < long_lines.size(); i++ ){
        Vec4i l = long_lines[i];
        int hor = abs(l[0] - l[2]);
        int ver = abs(l[1] - l[3]);
        angle = atan2(ver, hor) * 180 / CV_PI;
        if (angle <= tan_thre) {
            points.push_back(Point(l[0], l[1]));
            points.push_back(Point(l[2], l[3]));
        }
    }

    // polynomial regression -------------------------------------------
    vector<double> coeff = fitPoly(points, 2);
    vector<Point> pol_points;
    for (size_t i=0; i<2000;i++){
        Point polynomial_point = pointAtX(coeff, i);
        pol_points.push_back(polynomial_point);
    }

    // draw curve
    cv::polylines(inputImage, pol_points, false, Scalar(0, 0, 255), 3);
    namedWindow("Final Line", WINDOW_AUTOSIZE);
    imshow("Final Line", inputImage);
    waitKey(0);
    destroyAllWindows();
    string outname = newName + "_final" + ".png";
    imwrite(outname, inputImage);

    return 0;
}

