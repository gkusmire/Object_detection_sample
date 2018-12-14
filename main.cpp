#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <iostream>

long getObjectArea(cv::Mat &image, cv::Vec3b color);

long getObjectCircuit(cv::Mat image, cv::Vec3b color);

long computeW3(long area, long circuit);

double computeM3(cv::Mat &image, cv::Vec3b color);

double computeM7(cv::Mat &image, cv::Vec3b color);

double getM(const cv::Mat& I, const cv::Vec3b& color, int x, int y)
{
    double n = 0;
    cv::Mat_<cv::Vec3b> _I = I;
    for (int i = 0; i < I.rows; ++i)
        for (int j = 0; j < I.cols; ++j)
            if (_I(i, j) == color) {
                n += pow(i, x) * pow(j, y);
            }

    return n;
}

cv::Rect getObjectRect(cv::Mat& image, cv::Vec3b color) {
    int minX = image.cols;
    int minY = image.rows;
    int maxX = 0;
    int maxY = 0;

    cv::Mat_<cv::Vec3b> _I = image;

    for(int x=0; x<_I.cols; ++x)
        for(int y=0; y<_I.rows; ++y)
        {
            if(_I(x,y) == color)
            {
                if(x < minX)
                    minX = x;
                if(y < minY)
                    minY = y;
                if(maxX < x)
                    maxX = x;
                if(maxY < y)
                    maxY = y;
            }
        }
    return cv::Rect(minY-5, minX-5, maxY-minY+10, maxX-minX+10);
}

std::vector<cv::Vec3b> getPictureColors(cv::Mat& image) {
    std::vector<cv::Vec3b> colors;
    cv::Mat_<cv::Vec3b> _I = image;
    cv::Vec3b white = {255,255,255};

    for (int i = 0; i < _I.cols; ++i)
        for (int j = 0; j < _I.rows; ++j)
        {
            if(_I(i,j) != white)
                if(std::find(colors.begin(), colors.end(), _I(i,j)) == colors.end())
                    colors.push_back(_I(i,j));
        }

    return colors;
}

std::pair<int, int> getGeoCenter(const cv::Mat& image, const cv::Vec3b& color)
{
    return std::make_pair(image.rows / 2, image.cols / 2);
}

std::pair<int, int> getMassCenter(const cv::Mat& image, const cv::Vec3b& color)
{
    int x = 0, y = 0, nbOfPixels = 0;
    cv::Mat_<cv::Vec3b> _I = image;

    for (int i = 0; i < image.rows; ++i)
        for (int j = 0; j < image.cols; ++j)
            if (_I(i, j) == color) {
                x += i; y += j; ++nbOfPixels;
            }

    return std::make_pair(x/nbOfPixels,y/nbOfPixels);
}

double computeAngle(cv::Mat& image, const cv::Vec3b& color)
{
    cv::Mat_<cv::Vec3b> _I = image;

    auto geo_center = getGeoCenter(image, color);
    auto mass_center = getMassCenter(image, color);

    return atan2(mass_center.second-geo_center.second,mass_center.first-geo_center.first)*180/CV_PI;
}

void showInfoForArrowPicture(std::string picture_name, cv::Mat& image, cv::Vec3b color) {
    std::cout<<"Strzałka R="<<computeAngle(image, color)<<std::endl;
}

void showInfoForNonArrowPicture(std::string picture_name, cv::Mat& image, cv::Vec3b color) {
    long area = getObjectArea(image, color);
    long circuit = getObjectCircuit(image, color);
    long w3 = computeW3(area, circuit);
    double m3 = computeM3(image, color);
    double m7 = computeM7(image, color);



    std::cout<<"Plik:"<<picture_name<<" S="<<area<<" L="<<circuit<<" W3="<<w3<<" M3="<<m3<<" M7="<<m7<<std::endl;
}

double computeM7(cv::Mat &image, cv::Vec3b color) {
    double M20, M02, M11;

    double m20 = getM(image,color,2,0);
    double m10 = getM(image,color,1,0);
    double m00 = getM(image,color,0,0);
    double m02 = getM(image,color,0,2);
    double m01 = getM(image,color,0,1);
    double m11 = getM(image,color,1,1);

    M02 = m02 - pow(m01,2)/m00;
    M20 = m20 - pow(m10,2)/m00;
    M11 = m11 - m10*m01/m00;

    return (M20*M02 - pow(M11,2))/pow(m00,4);
}

double computeM3(cv::Mat &image, cv::Vec3b color) {
    double M30, M12, M21, M03;

    double m00 = getM(image,color,0,0);
    double m30 = getM(image,color,3,0);
    double m20 = getM(image,color,2,0);
    double m10 = getM(image,color,1,0);
    double m12 = getM(image,color,1,2);
    double m11 = getM(image,color,1,1);
    double m02 = getM(image,color,0,2);
    double m21 = getM(image,color,2,1);
    double m03 = getM(image,color,0,3);
    double m01 = getM(image,color,0,1);
    double i = m10/m00;
    double j = m01/m00;

    M30 = m30 - 3*m20*i + 2*m10*pow(i,2);
    M12 = m12 - 2*m11*j - m02*i + 2*m10*pow(j,2);
    M21 = m21 - 2*m11*i - m20*j + 2*m01*pow(i,2);
    M03 = m03 - 3*m02*j + 2*m01*pow(j,2);

    return pow(M30-3*M12,2) + pow(3*M21-M03,2)/pow(m00,5);
}

long computeW3(long area, long circuit) {
    return (double)( circuit / (2 * sqrt( CV_PI * area )) ) - 1;
}

long getObjectCircuit(cv::Mat image, cv::Vec3b color) {
    cv::Vec3b marker_color = {10,10,10};
    cv::Vec3b white = {255,255,255};
    cv::Mat_<cv::Vec3b> _I = image;
    int x[] = {-1,-1,-1,0,0,1,1,1};
    int y[] = {1,0,-1,1,-1,1,0,-1};
    long counter = 0;

    for (int i = 0; i < _I.cols; ++i)
        for (int j = 0; j < _I.rows; ++j)
        {
            if(_I(i,j) == color)
                for(int m=0; m<8; ++m)
                {
                    if(_I(i+x[m],j+y[m]) == white) {
                        _I(i + x[m], j + y[m]) = marker_color;
                        ++counter;
                    }
                }
                ++counter;
        }
    return counter;
}

long getObjectArea(cv::Mat &image, cv::Vec3b color) {
    long counter = 0;

    cv::Mat_<cv::Vec3b> _I = image;
    for (int i = 0; i < _I.cols; ++i)
        for (int j = 0; j < _I.rows; ++j)
        {
            if(_I(i,j) == color)
                ++counter;
        }
    return counter;
}

int main(int, char *[]) {
    std::cout<<"Podaj nazwę obrazu: ";
    std::string picture_name;
    std::cin>>picture_name;

    cv::Mat image = cv::imread(picture_name);
    std::vector<cv::Vec3b> colors = getPictureColors(image);

    for(cv::Vec3b c : colors)
    {
        cv::Mat object_image = image(getObjectRect(image, c));

        if(picture_name.compare("strzalki_1.dib") == 0)
        {
            showInfoForArrowPicture(picture_name, object_image, c);
        }
        else
        {
            showInfoForNonArrowPicture(picture_name, object_image, c);
        }
    }
    cv::waitKey(-1);
    return 0;
}
