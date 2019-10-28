#include "gtest/gtest.h"
#include "Image.hpp"
#include "Productions.hpp"
using namespace std;

TEST(ImageComparisonTest,test1)
{
    auto image = new Image("./test_files/face.bmp"); 
    auto image2 = new Image("test_files/face_test.bmp"); 
    int a = image->CompareWith(*image2);;
    EXPECT_EQ(363,a);
}