#include "Image.hpp"
#include <string>
#include <iostream>



Image::Image(std::string filename)
{
    boost::gil::read_image(filename, img, boost::gil::bmp_tag());
    view = boost::gil::view(img);
}

Image::Image(uint8_t*** pixels, int width, int height, int channels)
{
    img = boost::gil::rgb8_image_t(width, height);
    boost::gil::fill_pixels(img._view, boost::gil::rgb8_pixel_t(200,0,0));
    for(int y=0;y<height;y++)
        for(int x=0;x<width;x++)
            for(int channel=0; channel < std::min(channels, 3); channel++)
                img._view[y*width+x][channel]=pixels[x][y][channel];
    view = boost::gil::view(img);
}

long long Image::CompareWith(Image& other, int x0, int y0, int width, int height)
{
    long long sum = 0;
    for(int y = y0; y < height; y++)
    {
        for(int x = x0; x < width; x++)
        {
            for(int channel=0; channel<3; channel++)
                sum += abs(view[y * width + x][channel] - other.view[y * width + x][channel]);
        }
    }
    return sum;
}

long long Image::CompareWith(Image& other)
{
    return CompareWith(other, 0, 0, width(), height());
}

int Image::width()
{
    return img.width();
}

int Image::height()
{
    return img.height();
}

std::tuple<int, int, int> Image::getPixel(int x, int y)
{
    return getPixelInternal(x, y);
}

std::tuple<int, int, int> Image::getPixelInternal(int x, int y)
{
    int r = view[y * img.width() + x][0];
    int g = view[y * img.width() + x][1];
    int b = view[y * img.width() + x][2];
    //std::cerr<<x<<" "<<y<<" "<<r<<" "<<g<<" "<<b<<std::endl;
    return std::make_tuple(r,g,b);   
}

int Image::getPixelInternal(int x, int y, int channel)
{
    return view[y * img.width() + x][channel];
}

double Image::GetInterpolatedPixel(int x1, int x2, int y1, int y2, int x, int y, int channel)
{
        int r11,r21,r12,r22;
        r11 = getPixelInternal(x1,y1, channel);
        r21 = getPixelInternal(x2,y1, channel);
        r12 = getPixelInternal(x1,y2, channel);
        r22 = getPixelInternal(x2,y2, channel);
        double coef22 = (x-x1)*(y-y1)/(1.0*(x2-x1)*(y2-y1));
        double coef12 = (x2-x)*(y-y1)/(1.0*(x2-x1)*(y2-y1));
        double coef21 = (x-x1)*(y2-y)/(1.0*(x2-x1)*(y2-y1));
        double coef11 = (x2-x)*(y2-y)/(1.0*(x2-x1)*(y2-y1));
        double rInterpolated = r11 * coef11 + r21 * coef21 + r12 * coef12 + r22 * coef22;
        return rInterpolated;
}

double Image::SquaredErrorOfInterpolation(int x1, int x2, int y1, int y2, int channel)
{
    double sum = 0;
    //std::cout<<"x2 -x1, y2-y1: "<<x2-x1<<" "<<y2-y1<<std::endl;
    if (x2-x1==0 or y2-y1==0) return 0;

    for(int x=x1;x<=x2;x++)
        for(int y=y1;y<=y2;y++)
        {
            int rOriginal;
            rOriginal = getPixelInternal(x, y, channel);
            double rInterpolated = GetInterpolatedPixel(x1,x2,y1,y2,x,y,channel);
            sum+=(rInterpolated - rOriginal)*(rInterpolated - rOriginal);
      //      std::cout<<x<<" "<<y<<" "<<rOriginal<<" "<<rInterpolated<<" "<<sum<<std::endl;
            ;
        }
    return sum;
}

double Image::CompareWithInterpolation(int x1, int x2, int y1, int y2, int channel)
{
    double maxSum = 255*255*(x2-x1+1)*(y2-y1+1);
    return SquaredErrorOfInterpolation(x1, x2, y1, y2, channel)/maxSum;
}

double ImageMagnifier::SquaredErrorOfInterpolation(int x1, int x2, int y1, int y2, int channel)
{
    int originalRatio = ratio;
    ratio = 1;
    auto result = Image::SquaredErrorOfInterpolation(x1/originalRatio, x2/originalRatio, y1/originalRatio, y2/originalRatio, channel);
    ratio = originalRatio;
return result;
}
double ImageMagnifier::GetInterpolatedPixel(int x1, int x2, int y1, int y2, int x, int y, int channel)
{
    int originalRatio = ratio;
    ratio = 1;
    auto result = Image::GetInterpolatedPixel(x1/originalRatio, x2/originalRatio, y1/originalRatio, y2/originalRatio, x/originalRatio, y/originalRatio, channel);
    ratio = originalRatio;
return result;
}
long long ImageMagnifier::CompareWith(Image& other, int x, int y, int width, int height)
{
    int originalRatio = ratio;
    ratio = 1;
    auto result = Image::CompareWith(other, x/originalRatio, y/originalRatio, width/originalRatio, height/originalRatio);
    ratio = originalRatio;
return result;
}
double ImageMagnifier::CompareWithInterpolation(int x1, int x2, int y1, int y2, int channel)
{
    int originalRatio = ratio;
    ratio = 1;
    auto result = Image::CompareWithInterpolation(x1/originalRatio, x2/originalRatio, y1/originalRatio, y2/originalRatio, channel);
    ratio = originalRatio;
return result;
}
std::tuple<int, int, int> ImageMagnifier::getPixel(int x, int y)
{
    int originalRatio = ratio;
    ratio = 1;
    auto result = Image::getPixel(x/originalRatio, y/originalRatio);
    ratio = originalRatio;
return result;
}
int ImageMagnifier::width()
{
    int originalRatio = ratio;
    ratio = 1;
    auto result = Image::width()*originalRatio;
    ratio = originalRatio;
return result;
}
int ImageMagnifier::height()
{
    int originalRatio = ratio;
    ratio = 1;
    auto result = Image::height()*originalRatio;
    ratio = originalRatio;
return result;
}
