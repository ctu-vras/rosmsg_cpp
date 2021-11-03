#include "rosmsg_cpp/rosmsg_cpp.h"

#include <geometry_msgs/Point.h>
#include <geometry_msgs/TwistWithCovarianceStamped.h>
#include <geometry_msgs/Vector3.h>

#include <gtest/gtest.h>

using namespace ros;
using namespace ros::message;

std::string appendNewline(const std::string& str)
{
  if (str.at(str.size() - 1) != '\n')
    return str + "\n";
  return str;
}

TEST(CppMsgDef, getFullDef)
{
  EXPECT_EQ(
    appendNewline(getFullDef("geometry_msgs/Vector3")),
    message_traits::definition<geometry_msgs::Vector3>());
  
  EXPECT_EQ(
    appendNewline(getFullDef("geometry_msgs/Point")),
    message_traits::definition<geometry_msgs::Point>());
  
  EXPECT_EQ(
    appendNewline(getFullDef("geometry_msgs/TwistWithCovarianceStamped")),
    message_traits::definition<geometry_msgs::TwistWithCovarianceStamped>());
  
  EXPECT_EQ(getFullDef("nonexistent"), "");
  EXPECT_EQ(getFullDef("nonexistent/message"), "");
}

TEST(CppMsgDef, getMD5Sum)
{
  EXPECT_EQ(
    getMD5Sum("geometry_msgs/Vector3"),
    message_traits::md5sum<geometry_msgs::Vector3>());
  
  EXPECT_EQ(
    getMD5Sum("geometry_msgs/Point"),
    message_traits::md5sum<geometry_msgs::Point>());
  
  EXPECT_EQ(
    getMD5Sum("geometry_msgs/TwistWithCovarianceStamped"),
    message_traits::md5sum<geometry_msgs::TwistWithCovarianceStamped>());

  EXPECT_EQ(getMD5Sum("nonexistent"), "");
  EXPECT_EQ(getMD5Sum("nonexistent/message"), "");
}

TEST(CppMsgDef, hasHeader)
{
  EXPECT_EQ(
    hasHeader("geometry_msgs/Vector3"),
    message_traits::hasHeader<geometry_msgs::Vector3>());
  
  EXPECT_EQ(
    hasHeader("geometry_msgs/Point"),
    message_traits::hasHeader<geometry_msgs::Point>());
  
  EXPECT_EQ(
    hasHeader("geometry_msgs/TwistWithCovarianceStamped"),
    message_traits::hasHeader<geometry_msgs::TwistWithCovarianceStamped>());

  EXPECT_EQ(hasHeader("nonexistent"), false);
  EXPECT_EQ(hasHeader("nonexistent/message"), false);
}

TEST(CppMsgDef, getFieldNames)
{
  std::vector<std::string> fields;
  
  fields = getFieldNames("geometry_msgs/Vector3");
  ASSERT_EQ(fields.size(), 3);
  EXPECT_EQ("x", fields[0]);
  EXPECT_EQ("y", fields[1]);
  EXPECT_EQ("z", fields[2]);

  fields = getFieldNames("geometry_msgs/Point");
  ASSERT_EQ(fields.size(), 3);
  EXPECT_EQ("x", fields[0]);
  EXPECT_EQ("y", fields[1]);
  EXPECT_EQ("z", fields[2]);

  fields = getFieldNames("geometry_msgs/TwistWithCovarianceStamped");
  ASSERT_EQ(fields.size(), 2);
  EXPECT_EQ("header", fields[0]);
  EXPECT_EQ("twist", fields[1]);

  EXPECT_EQ(getFieldNames("nonexistent").size(), 0);
  EXPECT_EQ(getFieldNames("nonexistent/message").size(), 0);
}

TEST(CppMsgDef, getFieldTypes)
{
  std::vector<std::string> fields;
  
  fields = getFieldTypes("geometry_msgs/Vector3");
  ASSERT_EQ(fields.size(), 3);
  EXPECT_EQ("float64", fields[0]);
  EXPECT_EQ("float64", fields[1]);
  EXPECT_EQ("float64", fields[2]);

  fields = getFieldTypes("geometry_msgs/Point");
  ASSERT_EQ(fields.size(), 3);
  EXPECT_EQ("float64", fields[0]);
  EXPECT_EQ("float64", fields[1]);
  EXPECT_EQ("float64", fields[2]);

  fields = getFieldTypes("geometry_msgs/TwistWithCovarianceStamped");
  ASSERT_EQ(fields.size(), 2);
  EXPECT_EQ("std_msgs/Header", fields[0]);
  EXPECT_EQ("geometry_msgs/TwistWithCovariance", fields[1]);

  EXPECT_EQ(getFieldTypes("nonexistent").size(), 0);
  EXPECT_EQ(getFieldTypes("nonexistent/message").size(), 0);
}

TEST(CppMsgDef, getMsgFile)
{
  EXPECT_NE(getMsgFile("geometry_msgs/Vector3").size(), 0);
  EXPECT_NE(getMsgFile("geometry_msgs/Point").size(), 0);
  EXPECT_NE(getMsgFile("geometry_msgs/TwistWithCovariance").size(), 0);

  EXPECT_EQ(getMsgFile("nonexistent").size(), 0);
  EXPECT_EQ(getMsgFile("nonexistent/message").size(), 0);
}

TEST(CppMsgDef, getMD5Text)
{
  EXPECT_EQ(getMD5Text("geometry_msgs/Vector3"),
    "float64 x\n"
    "float64 y\n"
    "float64 z"
  );
  
  EXPECT_EQ(getMD5Text("geometry_msgs/Point"),
    "float64 x\n"
    "float64 y\n"
    "float64 z"
  );
  
  EXPECT_EQ(getMD5Text("geometry_msgs/TwistWithCovarianceStamped"),
    getMD5Sum("std_msgs/Header") + " header\n" +
    getMD5Sum("geometry_msgs/TwistWithCovariance") + " twist"
  );

  EXPECT_EQ(getMD5Text("geometry_msgs/TwistWithCovariance"),
    getMD5Sum("geometry_msgs/Twist") + " twist\n" +
    "float64[36] covariance"
  );

  EXPECT_EQ(getMD5Text("nonexistent"), "");
  EXPECT_EQ(getMD5Text("nonexistent/message"), "");
}

int main(int argc, char **argv){
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}