#include <iostream>

#include "rosmsg_cpp/rosmsg_cpp.h"

int main(int argc, char** argv)
{
  const auto usage = std::string("Usage: ") + argv[0] + " show|md5|fields|field-types|file|md5-text package/MessageType";
  
  if (argc <= 2)
  {
    std::cerr << usage << std::endl;
    return 1;
  }
  
  std::string action{argv[1]};
  
  if (action != "show" && action != "md5" && action != "fields" && action != "field-types" && action != "file" && action != "md5-text")
  {
    std::cerr << usage << std::endl;
    return 2;
  }
  
  std::string type{argv[2]};

  std::string result;
  
  if (action == "show")
  {
    result = ros::message::getFullDef(type);
    if (result.empty())
    {
      std::cerr << "Definition for message type [" << type << "] could not be found!" << std::endl;
      return 3;
    }
  }
  else if (action == "md5")
  {
    result = ros::message::getMD5Sum(type);
    if (result.empty())
    {
      std::cerr << "MD5 sum for message type [" << type << "] could not be found!" << std::endl;
      return 4;
    }
  }
  else if (action == "fields")
  {
    const auto fields = ros::message::getFieldNames(type);
    if (fields.empty())
    {
      std::cerr << "Field names for message type [" << type << "] could not be found!" << std::endl;
      return 5;
    }
    for (const auto& field : fields)
      result += field + "\n";
  }
  else if (action == "field-types")
  {
    const auto fields = ros::message::getFieldTypes(type);
    if (fields.empty())
    {
      std::cerr << "Field types for message type [" << type << "] could not be found!" << std::endl;
      return 5;
    }
    for (const auto& field : fields)
      result += field + "\n";
  }
  else if (action == "file")
  {
    result = ros::message::getMsgFile(type);
    if (result.empty())
    {
      std::cerr << "Message proto file for message type [" << type << "] could not be found!" << std::endl;
      return 6;
    }
  }
  else if (action == "md5-text")
  {
    result = ros::message::getMD5Text(type);
    if (result.empty())
    {
      std::cerr << "MD5 sum source text for message type [" << type << "] could not be found!" << std::endl;
      return 7;
    }
  }
  
  std::cout << result; // intentionally no \n
  return 0;
}
