#include "gtest/gtest.h"
#include "PbiHelper.hpp"

using namespace std;

TEST(PbiHelperTests, GetSquaredIntegral)
{
    auto fun = GetXSquareIntegral(1,2);
    ASSERT_EQ(fun(1,1), 1/3.0-3/2.0+2);
    ASSERT_EQ(fun(2,1), 8/3.0-12/2.0+4);
}

TEST(PbiHelperTests, GetX4Integral)
{
    auto fun = GetX4Integral(1,2);
    ASSERT_EQ(fun(1,1), 1/5.0 - 3/2.0 + 13/3.0 - 6 + 4);
    ASSERT_EQ(fun(2,1), 32/5.0 - 24 + 13*8/3.0 - 24 + 8);
}

TEST(PbiHelperTests, GetX4Integral2)
{
    auto fun = GetX4Integral(0,0);
    ASSERT_EQ(fun(1,1), 1/5.0 );
    ASSERT_EQ(fun(2,1), 32/5.0);
}

TEST(PbiHelperTests, GetX4Integral3)
{
    auto fun = GetX4Integral(0,1);
    ASSERT_EQ(fun(1,1), 1/5.0 - 1/2.0 + 1/3.0);
    ASSERT_EQ(fun(2,1), 32/5.0 - 8 + 8/3.0);
}

TEST(PbiHelperTests, GetSquareInterpolationOfEdge)
{
    Array2D a = Array2D(11,1);
    for(int i=0;i<11;i++)
        a[i][0] = i*(10.0-i)/2.0;
    auto coefficient = GetSquareInterpolationOfEdge(a,0,10,0);
    ASSERT_TRUE(abs(coefficient+0.5)<0.1);
}

TEST(PbiHelperTests, GetSquareInterpolationOfYEdge)
{
    Array2D a = Array2D(1,11);
    for(int i=0;i<11;i++)
        a[0][i] = i*(10.0-i)/2.0;
    auto coefficient = GetSquareInterpolationOfYEdge(a,0,0,10);
    ASSERT_TRUE(abs(coefficient+0.5)<0.1);
}

TEST(PbiHelperTests, GetSquareInterpolationOfEdge2)
{
    Array2D a = Array2D(2000,2000);
    a.FillWith(0);
    a[1787][1999]=-0.5;
    double y = GetSquareInterpolationOfEdge(a, 1786, 1788, 1999);
    ASSERT_FALSE(std::isnan(y));
    ASSERT_NE(y,0);
}

TEST(PbiHelperTests, GetSquareInterpolationOfYEdge2)
{
    Array2D a = Array2D(2000,2000);
    a.FillWith(0);
    a[1999][1787]=-0.5;
    double y = GetSquareInterpolationOfYEdge(a, 1999, 1786, 1788);
    ASSERT_FALSE(std::isnan(y));
    ASSERT_NE(y,0);
}

TEST(PbiHelperTests, GetSquareInterpolationOfRectangle)
{
    Array2D a = Array2D(1001,1001);
    for(int x=0;x<1001;x++)
        for(int y=0;y<1001;y++)
            a[x][y] = x*(1000.0-x)*y*(1000.0-y)/2.0;
    auto coefficient = GetSquareInterpolationOfRectangle(a, 0, 1000, 0, 1000);
     ASSERT_TRUE(abs(coefficient+0.5)<0.1);
}

TEST(PbiHelperTests, GetSquareInterpolationOfRectangle2)
{
    Array2D a = Array2D(5001,5001);
    for(int x=4000;x<5001;x++)
        for(int y=4000;y<5001;y++)
            a[x][y] = (x-4000)*(5000.0-x)*(y-4000)*(5000.0-y)/2.0;
    auto coefficient = GetSquareInterpolationOfRectangle(a, 4000, 5000, 4000, 5000);
     ASSERT_TRUE(abs(coefficient+0.5)<0.1);
}

TEST(PbiHelperTests, integralOfTestFunction_checkCorrectness)
{
    double correctAnswers[]{1/2.0, 1/2.0, 1/6.0, 0, 1/30.0, 0, 1/70.0, 0 , 1/126.0, 0, 1/198.0, 0, 1/286.0}; //values from wolfram alpha
    for(int n=0;n<=12;n++)
    {
        ASSERT_TRUE(abs(correctAnswers[n]-integralOfTestFunction[n](1.0, 0.0))<0.01);
    }
}

TEST(PbiHelperTests, GetNthOrderInterpolationOfEdge)
{
    int sampling = 100;
    Array2D a = Array2D(sampling, 1);
    std::function<double(double, double)> f = [sampling](double x, double y){return (x/sampling)*(1.0-x/sampling)/2.0;};
    auto fDelta = [sampling](double x, double y){return (2*x/sampling-1.0);};
    for(int n=2;n<=12;n++)
    {
        for(int x=0;x<sampling;x++)
            a[x][0] = f(x,0);
        auto coefficient = GetNthOrderInterpolationOfEdge(a, 0, sampling - 1, 0, n);
        f = Multiply(f, fDelta);
        ASSERT_TRUE(abs(coefficient-0.5)<0.01);
    }
}

TEST(PbiHelperTests, GetMultipleOrderCoefs2D)
{
    int sampling = 100;
    Array2D a = Array2D(sampling, sampling);
    std::function<double(double, double)> fx = [sampling](double x, double y){return (x/(sampling-1))*(1.0-x/(sampling-1))/2.0;};
    std::function<double(double, double)> fxOriginal = [sampling](double x, double y){return (x/(sampling-1))*(1.0-x/(sampling-1))/2.0;};
    std::function<double(double, double)> fy = [sampling](double x, double y){return (y/(sampling-1))*(1.0-y/(sampling-1))/2.0;};
    std::function<double(double, double)> fyOriginal = [sampling](double x, double y){return (y/(sampling-1))*(1.0-y/(sampling-1))/2.0;};
    auto fXDelta = [sampling](double x, double y){return (2*x/(sampling-1)-1.0);};
    auto fYDelta = [sampling](double x, double y){return (2*y/(sampling-1)-1.0);};
    for(int n1=1;n1<=10;n1++)
    {
        fy=fyOriginal;
        for(int n2=1;n2<=10;n2++)
        {
            auto multiplied = Multiply(fx,fy);
            for(int x=0;x<sampling;x++)
                for(int y=0;y<sampling;y++)
                    a[x][y] = multiplied(x,y);
            auto coefficient = GetInterpolationsOfRectangleOfDifferentOrders(a, 0, sampling - 1, 0, sampling - 1, std::max(n1,n2));
            fy = Multiply(fy, fYDelta);
            ASSERT_TRUE(abs(coefficient[(n1-1)*10+(n2-1)]-0.25)<0.1);
        }
        fx=Multiply(fx,fXDelta);
    }
}

TEST(PbiHelperTests, GetMultipleOrderCoefsForHorizontalEdge)
{
    int sampling = 100;
    Array2D a = Array2D(sampling, 1);
    std::function<double(double, double)> fx = [sampling](double x, double y){return (x/(sampling-1))*(1.0-x/(sampling-1))/2.0;};
    std::function<double(double, double)> fxOriginal = [sampling](double x, double y){return (x/(sampling-1))*(1.0-x/(sampling-1))/2.0;};
    auto fXDelta = [sampling](double x, double y){return (2*x/(sampling-1)-1.0);};
    for(int n1=1;n1<=3;n1++)
    {
        
        for(int x=0;x<sampling;x++)
            for(int y=0;y<1;y++)
                a[x][y] = fx(x,y);
        auto coefficient = GetInterpolationsOfEdgeOfDifferentOrders(a, 0, sampling - 1, 0, 0, n1);
        ASSERT_TRUE(abs(coefficient[n1-1]-0.5)<0.1);
        fx=Multiply(fx,fXDelta);
    }
}

TEST(PbiHelperTests, GetMultipleOrderCoefsForVerticalEdge)
{
    int sampling = 100;
    Array2D a = Array2D(1, sampling);
    std::function<double(double, double)> fy = [sampling](double x, double y){return (y/(sampling-1))*(1.0-y/(sampling-1))/2.0;};
    std::function<double(double, double)> fyOriginal = [sampling](double x, double y){return (y/(sampling-1))*(1.0-y/(sampling-1))/2.0;};
    auto fYDelta = [sampling](double x, double y){return (2*y/(sampling-1)-1.0);};
    for(int n1=1;n1<=3;n1++)
    {
        
        for(int x=0;x<1;x++)
            for(int y=0;y<sampling;y++)
                a[x][y] = fy(x,y);
        auto coefficient = GetInterpolationsOfEdgeOfDifferentOrders(a, 0, 0, 0, sampling-1, n1);
        ASSERT_TRUE(abs(coefficient[n1-1]-0.5)<0.1);
        fy=Multiply(fy,fYDelta);
    }
}

TEST(PbiHelperTests, GetBilinearInterpolationFunctionFromOneCornerTest1)
{
    auto f = GetBilinearInterpolationFunctionFromOneCorner(0,10,0,10,0,0,10);
    ASSERT_DOUBLE_EQ(f(0,0),10);
    ASSERT_DOUBLE_EQ(f(10,0),0);
    ASSERT_DOUBLE_EQ(f(0,10),0);
    ASSERT_DOUBLE_EQ(f(10,10),0);
}

TEST(PbiHelperTests, GetBilinearInterpolationFunctionFromOneCornerTest2)
{
    auto f = GetBilinearInterpolationFunctionFromOneCorner(0,1000,0,2000,0,0,10);
    auto f2 = GetBilinearInterpolationFunctionFromOneCorner(0,1000,0,2000,0,2000,10);
    auto f3 = GetBilinearInterpolationFunctionFromOneCorner(0,1000,0,2000,1000,0,10);
    auto f4 = GetBilinearInterpolationFunctionFromOneCorner(0,1000,0,2000,1000,2000,10);
    ASSERT_DOUBLE_EQ(f(0,0),10);
    ASSERT_DOUBLE_EQ(f(500,1000),2.5);
    ASSERT_DOUBLE_EQ(f2(500,1000),2.5);
    ASSERT_DOUBLE_EQ(f3(500,1000),2.5);
    ASSERT_DOUBLE_EQ(f4(500,1000),2.5);
    ASSERT_DOUBLE_EQ(f(0,500),7.5);
    ASSERT_DOUBLE_EQ(f2(0,500),2.5);
    ASSERT_DOUBLE_EQ(f3(0,500),0);
    ASSERT_DOUBLE_EQ(f4(0,500),0);
    ASSERT_DOUBLE_EQ(f(250,0),7.5);
    ASSERT_DOUBLE_EQ(f2(250,0),0);
    ASSERT_DOUBLE_EQ(f3(250,0),2.5);
    ASSERT_DOUBLE_EQ(f4(250,0),0);
}