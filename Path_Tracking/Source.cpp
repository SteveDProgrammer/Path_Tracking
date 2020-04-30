// GaitVelocity.cpp : This file contains the 'main' function. Program execution begins and ends there.

#include<opencv2/opencv.hpp>
#include <iostream>
#include <array>

using namespace cv;
using namespace std;

int sno = 0;

void straightLineFitting(vector<Point2f> all_center, Mat dst) //Using simple linear regression
{
    vector<float> slopes, biases;

    float slope = 0, bias = 0;
    int mean_index = all_center.size() / 2 - 1;

    Point2f mean = all_center[mean_index];

    //if all_center has even number of elements
    if (all_center.size() % 2 == 0)
    {
        mean.x = (all_center[mean_index].x + all_center[mean_index + 1].x) / 2;
        mean.y = (all_center[mean_index].y + all_center[mean_index + 1].y) / 2;

        auto itPos = all_center.begin() + mean_index;

        all_center.insert(itPos, mean);
    }

    float numerator = 0, denominator = 0;

    for (auto& center : all_center)
    {
        numerator += (center.x - mean.x) * (center.y - mean.y);
        denominator += (center.x - mean.x) * (center.x - mean.x);
    }

    slope = numerator / denominator;
    bias = mean.y - slope * mean.x;

    //Points in array are in all_center are in the oreder of right to left. last in all_center is first fp and vice versa

    Point2f fp = Point2f(all_center[all_center.size() - 1].x, slope * all_center[all_center.size() - 1].x + bias);
    Point2f lp = Point2f(all_center[0].x, slope * all_center[0].x + bias);

    cv::line(dst, all_center.at(0), all_center.at(all_center.size() - 1), (255, 255, 255), 2, LINE_AA, 0);

    //save files
    if (sno < 10) { imwrite("Output\\000" + to_string(sno++) + ".jpg", dst); }
    else if (sno < 100) { imwrite("Output\\00" + to_string(sno++) + ".jpg", dst); }
    else { imwrite("Output\\0" + to_string(sno++) + ".jpg", dst); }
}


void sineWaveFitting()
{

}

void cosineWaveFitting()
{

}

void Regression(vector<Point2f> all_center, Mat image, int x)
{
    int m = all_center.size();
    int n = 5; //(order of equation - 1) = number of columns
    Mat X(m, n, DataType<double>::type), XTrans, XInv, Y(m, 1, DataType<double>::type);

    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < m; j++)
            if (i == 0)
            {
                X.at<double>(j, i) = 1;
                Y.at<double>(j, i) = all_center.at(j).y;
            }
            else {
                X.at<double>(j, i) = pow(all_center.at(j).x, i);
                cout << X.at<double>(j, i) << " ";
            }
        cout << endl;
    }

    transpose(X, XTrans);

    Mat w = ((XTrans * X).inv()) * XTrans * Y;

    vector<Point> all_points;

    double y = 0;
    for (int i = 0; i < image.cols; i++)
    {
        y = w.at<double>(0,0);

        for (int j = 1; j < n; j++)
            y += w.at<double>(j, 0) *pow(i, j);

        all_points.push_back(Point(i,y));
    }

    polylines(image, all_points, false, Scalar(128), 1);
    
    namedWindow(to_string(x), WINDOW_NORMAL);
    imshow(to_string(x), image);
}

void Execute(string subpath, int x)
{
    //cout << "Processing Image " <<x<< endl << endl;
    //Getting all files
    
    vector<cv::String> filename;
    string path = "Thermal_data\\" + subpath + "\\*.jpg";

    glob(path, filename, false);

    vector<Mat> images;
    size_t count = filename.size(); //number of png files in images folder

    for (size_t i = 0; i < count; i++)
        images.push_back(imread(filename[i], IMREAD_GRAYSCALE));

    Mat src, dst(images[0].size(), images[0].type(), Scalar(0, 0, 0));

    float thresh = 250; //orig value 240
    float max_V = 255;

    int x_counter = 0; //Test value

    vector<Point2f> all_center;
    for (size_t i = 0; i < count; i++)
    {
        threshold(images[i], images[i], thresh, max_V, THRESH_BINARY); // Binary Thresholding
        GaussianBlur(images[i], images[i], Size(5, 5), 0); //Applying Gaussian Blur

        //find Canny edges
        Canny(src, src, 30, 200);

        //find contours
        vector<vector<Point> > contours;
        findContours(images[i], contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE, Point());
        drawContours(images[i], contours, -1, (192, 192, 192), 1);

        //finding MEC

        vector<Point2f> center(contours.size());
        vector<float> radius(contours.size());

        vector<Point2f> points;

        for (int i = 0; i < contours.size(); i++)
        {
            minEnclosingCircle(contours[i], center[i], radius[i]);

            if (radius[i] > 12)
            {
                all_center.push_back(center[i]);

                circle(dst, center[i], 1, Scalar(128), 1);
                //circle(dst, center[i], radius[i], Scalar(128), 1);
                points.push_back(center[i]);
            }
        }
    }
    //if (all_center.size() >= 2) straightLineFitting(all_center, dst);

    //if (all_center.size() >= 2) curveLineFitting(all_center, dst);

    if (all_center.size() >= 2) Regression(all_center, dst, x);

    //namedWindow(to_string(x), WINDOW_NORMAL);
    //imshow(to_string(x), dst);
}

//int main()
//{
//    int x = 1;
//    string subpath = "straightLine\\straight_00" + 1;
//
//    string name = "Thermal_data\\straightLine\\straight_001\\00001.jpg";
//    //Mat src(imread(name).size(), IMREAD_GRAYSCALE, Scalar(0, 0, 0));
//
//    string out_path = "straightLine\\";
//
//    vector<Point2f> temp;
//    while (x <= 10)
//    {
//        if (x > 99)
//        {
//            subpath = "straightLine\\straight_" + to_string(x);
//        }
//        else if (x > 9)
//        {
//            subpath = "straightLine\\straight_0" + to_string(x);
//        }
//        else subpath = "straightLine\\straight_00" + to_string(x);
//        cout << x << endl;
//        Execute(subpath, x++);
//
//        //cv::line(src, temp.at(0), temp.at(1), Scalar(128), 1, LINE_AA, 0);
//    }
//   /* namedWindow("gg", WINDOW_NORMAL);
//    imshow("gg", src);*/
//    waitKey(0);
//    return 0;
//}

//int main()
//{
//    int x = 1;
//    string subpath = "LCleft\LCleft_00" + 1;
//
//    string name = "Thermal_data\LCleft\LCleft_001\\00001.jpg";
//    //Mat src(imread(name).size(), IMREAD_GRAYSCALE, Scalar(0, 0, 0));
//
//    string out_path = "LCleft\\";
//
//    vector<Point2f> temp;
//    while (x <= 1)
//    {
//        if (x > 99)
//        {
//            subpath = "LCleft\\LCleft_" + to_string(x);
//        }
//        else if (x > 9)
//        {
//            subpath = "LCleft\\LCleft_0" + to_string(x);
//        }
//        else subpath = "LCleft\\LCleft_00" + to_string(x);
//        cout << x << endl;
//        Execute(subpath, x++);
//
//        //cv::line(src, temp.at(0), temp.at(1), Scalar(128), 1, LINE_AA, 0);
//    }
//    /* namedWindow("gg", WINDOW_NORMAL);
//     imshow("gg", src);*/
//    waitKey(0);
//    return 0;
//}

//int main()
//{
//    int x = 1;
//    string subpath = "sineleft\\sineleft_00" + 1;
//
//    string name = "Thermal_data\\sineleft\\sineleft_001\\0001.jpg";
//    //Mat src(imread(name).size(), IMREAD_GRAYSCALE, Scalar(0, 0, 0));
//
//    string out_path = "sineleft\\";
//
//    vector<Point2f> temp;
//    while (x <= 10)
//    {
//        if (x > 99)
//        {
//            subpath = "sineleft\\sineleft_" + to_string(x);
//        }
//        else if (x > 9)
//        {
//            subpath = "sineleft\\sineleft_0" + to_string(x);
//        }
//        else subpath = "sineleft\\sineleft_00" + to_string(x);
//        cout << x << endl;
//        Execute(subpath, x++);
//
//        //cv::line(src, temp.at(0), temp.at(1), Scalar(128), 1, LINE_AA, 0);
//    }
//    /* namedWindow("gg", WINDOW_NORMAL);
//     imshow("gg", src);*/
//    waitKey(0);
//    return 0;
//}

int main()
{
    int x = 1;
    string subpath = "sineright\\sineright_00" + 1;

    string name = "Thermal_data\\sineright\\sineright_001\\0001.jpg";
    //Mat src(imread(name).size(), IMREAD_GRAYSCALE, Scalar(0, 0, 0));

    string out_path = "sineright\\";

    vector<Point2f> temp;
    while (x <= 10)
    {
        if (x > 99)
        {
            subpath = "sineright\\sineright_" + to_string(x);
        }
        else if (x > 9)
        {
            subpath = "sineright\\sineright_0" + to_string(x);
        }
        else subpath = "sineright\\sineright_00" + to_string(x);
        cout << x << endl;
        Execute(subpath, x++);

        //cv::line(src, temp.at(0), temp.at(1), Scalar(128), 1, LINE_AA, 0);
    }
    /* namedWindow("gg", WINDOW_NORMAL);
     imshow("gg", src);*/
    waitKey(0);
    return 0;
}