#include <tesseract/baseapi.h>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include "sensor_msgs/Image.h"
#include "geometry_msgs/Point32.h"
#include "geometry_msgs/Polygon.h"
#include "std_msgs/String.h"
#include "ros/ros.h"
#include <cv_bridge/cv_bridge.h>
#include <string>
#include <tess_ros/PolyImage.h>
tesseract::TessBaseAPI tess;
 ros::Subscriber img_sub;
 ros::Publisher str_pub;
void ImageCallback(const tess_ros::PolyImage::ConstPtr& msg){
  
  //Mat try_tess = imread("/home/zhouming/data/tmp/test_zhm2.png",CV_LOAD_IMAGE_GRAYSCALE);
  sensor_msgs::Image img = msg->imgpatch;
  cv_bridge::CvImagePtr cv_ptr;
    try
    {
      cv_ptr = cv_bridge::toCvCopy(img, sensor_msgs::image_encodings::BGR8);
    }
    catch (cv_bridge::Exception& e)
    {
      ROS_ERROR("cv_bridge exception: %s", e.what());
    }

 // Mat cv_im;
 if(cv_ptr == nullptr||cv_ptr->image.cols == 0||cv_ptr->image.rows == 0)
    {std::cout<<"error: nulldata\n";}
 else
 {
    tess.SetImage((uchar*)cv_ptr->image.data, cv_ptr->image.cols, cv_ptr->image.rows, 3, cv_ptr->image.step);
//     char* out = tess.GetUTF8Text();
//      std::string text = out;
//      std::cout <<"text: "<< text <<" "<<text.length()<< std::endl;
    char* out = tess.GetUTF8Text();
    std::string text = out;
    
	if(text.length()>4||text.length()<3){
	  //std::cout<<"aborted"<<std::endl;
	}
	else{
	 char c1 = text.at(0);
	 char c2 = text.at(1);
 	 if(text.length()==4&&((c2=='C')||(c2=='c')))
	{
	  text.erase(text.begin());
	}
	else if(text.length()==4&&((c1=='C')||(c1=='c')))
	{
	  text.erase(text.end()-1);
	}
	std::cout<<"find results "<<text.find("1")<<std::endl;
	if(text.find("i")!=std::string::npos) //this is important: not nullptr or 0
	{
	text.replace(text.find("i"),1,"1");
	}
	if(text.find("\n")!=std::string::npos) //this is important: not nullptr or 0
	{
	text.replace(text.find("\n"),1,"");
	}
	/*if(text.length()==2&&(text.at(0)=='0'||text.at(0)=='1'))
	{
	  text = "C"+text;
	}*/
	std::cout<<"text: "<<text<<"\n";
    if((text.at(0)=='c'||text.at(0)=='C')&&text.length()==3)
    {
      tess_ros::PolyImage pol_ocr;
      pol_ocr.imgpatch = msg->imgpatch;
      pol_ocr.polygon = msg->polygon;
      pol_ocr.header.stamp = ros::Time::now();
      pol_ocr.str.data = text;
      str_pub.publish(pol_ocr);
    }
   }
 }
}


int main(int argc, char **argv){
  
  ros::init(argc,argv,"useTESS");
  ros::NodeHandle n;
  img_sub = n.subscribe("/pol_im",100,ImageCallback);
  str_pub = n.advertise<tess_ros::PolyImage>("pol_ocr",100);
  tess.Init(NULL, "zhm2", tesseract::OEM_DEFAULT);
  //tess.Init("/home/zhouming/data/tessdata_best", "eng", tesseract::OEM_LSTM_ONLY);
  tess.SetPageSegMode(tesseract::PSM_SINGLE_LINE);
  ros::spin();
  
  
}