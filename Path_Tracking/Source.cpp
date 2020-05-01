// GaitVelocity.cpp : This file contains the 'main' function. Program execution begins and ends there.

#include <opencv2/opencv.hpp>
#include <iostream>
#include <array>
#include <algorithm>

#include <boost/filesystem.hpp>
#include <boost/algorithm/string/replace.hpp>

using namespace boost::filesystem;

using namespace cv;
using namespace std;

Mat Regression(vector<Point2f> all_center, Mat image, int features)
{
    int m = all_center.size();
    int n = features+1; //number of columns
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

    return image;
}

void genOutput(Mat image, string parent_folder_path)
{
    //Output paths

    vector<path> out{
        "Output_data/LCleft",
        "Output_data/LCright",
        "Output_data/SCleft",
        "Output_data/SCright",
        "Output_data/sineleft",
        "Output_data/sineright",
        "Output_data/straightLine"
    };

    for (int i = 0; i < out.size(); i++) if (!exists(out.at(i))) create_directories(out.at(i)); //Create if output directories don't exist
    //Output the files
    boost::replace_all(parent_folder_path, "/", "//");
    boost::replace_all(parent_folder_path, "Thermal_data", "Output_data");
    cout << parent_folder_path+".jpg" << endl;
    imwrite(parent_folder_path+".jpg",image);
}

void Execute(string subpath, int x = 0)
{
    //Getting all files
    
    vector<cv::String> filename;
    string path = subpath + "\\*.jpg";

    glob(path, filename, false);

    vector<Mat> images;
    size_t no_of_images = filename.size(); //number of png files in images folder

    for (size_t i = 0; i < no_of_images; i++)
        images.push_back(imread(filename[i], IMREAD_GRAYSCALE));

    Mat src, dst(images[0].size(), images[0].type(), Scalar(0, 0, 0));

    float thresh = 250; //orig value 240
    float max_V = 255;

    int x_counter = 0; //Test value

    vector<Point2f> all_center;
    for (size_t i = 0; i < no_of_images; i++)
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
    
    int no_of_features = 0;

    if (path.find("straight") != string::npos) no_of_features = 1;
    else no_of_features = 4;

    if (all_center.size() >= 2) genOutput(Regression(all_center, dst,no_of_features), subpath);
}

int main()
{
    vector<path> all_subfolders;

    path p("Thermal_data");
    
    try
    {
        if (exists(p) && is_directory(p))
        {
            for (recursive_directory_iterator dir(p), end; dir != end; dir++)
                if (boost::filesystem::extension(*dir) == ".jpg")
                {
                    string temp = ((path)(*dir)).string();
                    temp = temp.substr(0,temp.rfind("\\"));
                    all_subfolders.push_back(temp);
                }
            all_subfolders.erase(unique(all_subfolders.begin(), all_subfolders.end()), all_subfolders.end());
        }
        else cout << p << " does not exist\n";

        for (int i = 0; i < all_subfolders.size(); i++)
        {
           Execute(all_subfolders.at(i).string());
        }
    }

    catch (const filesystem_error & ex)
    {
        cout << ex.what() << '\n';
    }

    return 0;
}