// GaitVelocity.cpp : This file contains the 'main' function. Program execution begins and ends there.

#include<opencv2/opencv.hpp>
#include <iostream>

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

void curveLineFitting(vector<Point2f> all_center, Mat dst)
{
    int i, j, k, n, N;
   // cout.precision(4);
   // cout.setf(ios::fixed);
    
    N = all_center.size(); //No. of centers/points

    vector<float> x(N), y(N);
    
    for (i = 0; i < N; i++)
        x[i] = all_center[i].x;

    for (i = 0; i < N; i++)
        y[i] = all_center[i].y;

    //degree of polynomial to use
    n = 3;

    vector<float> X(2*n+1);

    for (i = 0; i < 2 * n + 1; i++)
    {
        X[i] = 0;
        for (j = 0; j < N; j++)
            X[i] = X[i] + pow(x[j], i);
    }

    vector<vector<float>> B(n + 1, vector<float>(n + 2));
    vector<float> a(n + 1) ;
    for (i = 0; i <= n; i++)
        for (j = 0; j <= n; j++)
            B[i][j] = X[i + j];

    vector<float> Y(n + 1);
    for (i = 0; i <= n; i++)
    {
        Y[i] = 0;
        for (j = 0; j < N; j++)
            Y[i] = Y[i] + pow(x[j], i) * y[j];
    }

    for (i = 0; i <= n; i++)
        B[i][n + 1] = Y[i];
    n = n + 1;

    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    //cout << "\nThe normal augmmented matrix is as follows:" << endl;

    //for (i = 0; i < n; i++)
    //{
    //    for (j = 0; j < N; j++)
    //        cout << B[i][j] << setw(16);
    //    cout << endl;
    //}

    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    for(i=0; i<n;i++)
        for (k = 0;  k< n; k++)
            if(B[i][i]<B[k][i])
                for (j = 0; j < n; j++)
                {
                    float temp = B[i][j];
                    B[i][j] = B[k][j];
                    B[k][j] = temp;
                }

    for (i = 0; i < n - 1; i++)
        for (k = i + 1; k < n - 1; k++)
        {
            float t = B[k][i] / B[i][i];
            for (j = 0; j <= n; j++)
                B[k][j] - t * B[i][j];
        }

    for (i = n - 1; i >= 0; i--)
    {
        a[i] = B[i][n];
        for (j = 0; j < n; j++)
            if (j != i)
                a[i] = a[i] - B[i][j] * a[j];
        a[i] = a[i] / B[i][i];
    }

    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    cout << "\nThe values of the coefficients are as follows:" << endl;
    for (i = 0; i < n; i++)
        cout << "x^" << i << "=" << a[i] << endl;
    cout << "\nHence the fitted polynomial is given by: \ny=";
    for (i = 0; i < n; i++)
        cout << " + (" << a[i] << ")" << "x^" << i;
    cout << endl;


    //int begin = all_center[all_center.size() - 1].x;
    //int end = all_center[0].x;
    //int the_y = 0;
    //for (; begin <= end; begin++)
    //{
    //    the_y = a[0]*1 + a[1] * pow(begin,1) + a[2] * pow(begin,2) + a[3] * pow(begin,3);
    //    dst.at<uchar>(begin, the_y) = 255;
    //}

    for (int i =0; i<all_center.size(); i++)
    {
        dst.at<uchar>(all_center.at(i).x, all_center.at(i).y) = 255;
    }
    
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

                //circle(dst, center[i], 1, Scalar(128), 1);
                //circle(dst, center[i], radius[i], Scalar(128), 1);
                points.push_back(center[i]);
            }
        }

    }
    //if (all_center.size() >= 2) straightLineFitting(all_center, dst);

    if (all_center.size() >= 2) curveLineFitting(all_center, dst);
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

int main()
{
    int x = 1;
    string subpath = "LCleft\LCleft_00" + 1;

    string name = "Thermal_data\LCleft\LCleft_001\\00001.jpg";
    //Mat src(imread(name).size(), IMREAD_GRAYSCALE, Scalar(0, 0, 0));

    string out_path = "LCleft\\";

    vector<Point2f> temp;
    while (x <= 50)
    {
        if (x > 99)
        {
            subpath = "LCleft\\LCleft_" + to_string(x);
        }
        else if (x > 9)
        {
            subpath = "LCleft\\LCleft_0" + to_string(x);
        }
        else subpath = "LCleft\\LCleft_00" + to_string(x);
        cout << x << endl;
        Execute(subpath, x++);

        //cv::line(src, temp.at(0), temp.at(1), Scalar(128), 1, LINE_AA, 0);
    }
    /* namedWindow("gg", WINDOW_NORMAL);
     imshow("gg", src);*/
    waitKey(0);
    return 0;
}