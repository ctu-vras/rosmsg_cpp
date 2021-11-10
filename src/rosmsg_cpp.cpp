#include "rosmsg_cpp/rosmsg_cpp.h"

#include <iostream>
#include <cstdlib>

#include <Python.h>

#include <ros/package.h>
#include <ros/platform.h>

namespace ros
{
namespace message
{

static bool pythonInitialized = [](){
  // the ROS_ROOT workaround is needed (probably) just on ROS buildfarm
  std::string tmp;
  if (!ros::get_environment_variable(tmp, "ROS_ROOT"))
  {
    std::cerr << "ROS_ROOT not found" << std::endl;
    std::string rosDistro;
    if (!ros::get_environment_variable(rosDistro, "ROS_DISTRO"))
    {
      rosDistro = ROS_DISTRO;
      std::cerr << "ROS_DISTRO not found, setting " << rosDistro << std::endl;
    }
    const auto rosRoot = "/opt/ros/" + rosDistro + "/share/ros";
    std::cerr << "Setting ROS_ROOT to " << rosRoot << std::endl;
    ::setenv("ROS_ROOT", rosRoot.c_str(), false);
  }
  Py_Initialize();
  return true;
}();

PyObject* stringToPython(const std::string& input)
{
#if PY_MAJOR_VERSION >= 3
  return PyUnicode_FromStringAndSize(input.c_str(), input.size());
#else
  return PyString_FromStringAndSize(input.c_str(), input.size());
#endif
}

std::string stringFromPython(PyObject* input)
{
  if (input == nullptr)
    return {};
  Py_ssize_t size;
#if PY_MAJOR_VERSION >= 3
  const char* data;
  data = PyUnicode_AsUTF8AndSize(input, &size);
#else
  char* data;
  PyString_AsStringAndSize(input, &data, &size);
#endif
  return {data, static_cast<size_t>(size)};
}

PyObject* pythonBorrowAttrString(PyObject* o, const std::string& name)
{
  PyObject *r = PyObject_GetAttrString(o, name.c_str());
  Py_XDECREF(r);
  return r;
}

void printPythonError()
{
  if (PyErr_Occurred())
    PyErr_Print();
}

PyObject* getMsgField(const std::string& messageType, const std::string& field)
{
  if (messageType.empty())
    return {};

  const auto slashPos = messageType.find('/');
  if (slashPos == std::string::npos)
  {
    std::cerr << "Message type [" << messageType << "] does not contain a slash. It has to be of form "
              << "package/MessageType." << std::endl;
    return {};
  }
  
  const auto package = messageType.substr(0, slashPos);
  const auto baseMessageType = messageType.substr(slashPos + 1);

  if (package.empty())
  {
    std::cerr << "Package is empty for message type [" << messageType << "]" << std::endl;
    return {};
  }
  
  if (baseMessageType.empty())
  {
    std::cerr << "Message type after package is empty for message type [" << messageType << "]" << std::endl;
    return {};
  }
  
  if (ros::package::getPath(package).empty())
  {
    std::cerr << "Could not find package [" << package << "] for message type [" << messageType << "]" << std::endl;
    return {};
  }
  
  // now we know that the package where the message should be exists
  
  PyObject* def {nullptr};
  
  auto pName = stringToPython(package + ".msg");
  auto pModule = PyImport_Import(pName);
  Py_DECREF(pName);

  if (pModule != nullptr) {
    auto pMessageClass = pythonBorrowAttrString(pModule, baseMessageType);
    if (pMessageClass != nullptr) {
      auto pMsgDef = pythonBorrowAttrString(pMessageClass, field);
      if (pMsgDef != nullptr)
      {
        def = pMsgDef;
        Py_INCREF(pMsgDef);
      }
      else
      {
        printPythonError();
        std::cerr << "Error reading message definition from " << package << ".msg." << baseMessageType << " !"
                  << std::endl;
      }
    }
    else {
      printPythonError();
      std::cerr << "Failed to load message class " << package << ".msg." << baseMessageType << " !" << std::endl;
    }
  }
  else
  {
    printPythonError();
    std::cerr << "Failed to load module " << package << ".msg !" << std::endl;
  }
  Py_XDECREF(pModule);
  
  return def;
}

std::string getFullDef(const std::string& messageType)
{
  auto pMsgDef = getMsgField(messageType, "_full_text");
  auto def = stringFromPython(pMsgDef);
  Py_XDECREF(pMsgDef);
  return def;
}

std::string getMD5Sum(const std::string& messageType)
{
  auto pMD5Sum = getMsgField(messageType, "_md5sum");
  auto md5Sum = stringFromPython(pMD5Sum);
  Py_XDECREF(pMD5Sum);
  return md5Sum;
}

bool hasHeader(const std::string& messageType)
{
  auto pHasHeader = getMsgField(messageType, "_has_header");
  if (pHasHeader == nullptr)
    return false;
  auto hasHeader = PyObject_IsTrue(pHasHeader);
  Py_XDECREF(pHasHeader);
  return hasHeader;
}

std::vector<std::string> getFieldNames(const std::string& messageType)
{
  std::vector<std::string> fieldNames;
  auto pFieldNames = getMsgField(messageType, "__slots__");
  if (pFieldNames != nullptr && PyList_Check(pFieldNames))
  {
    for(Py_ssize_t i = 0; i < PyList_Size(pFieldNames); i++) {
      const auto pFieldName = PyList_GetItem(pFieldNames, i);
      if (pFieldName != nullptr)
        fieldNames.push_back(stringFromPython(pFieldName));
    }
  }
  Py_XDECREF(pFieldNames);
  return fieldNames;
}

std::vector<std::string> getFieldTypes(const std::string& messageType)
{
  std::vector<std::string> fieldTypes;
  auto pFieldTypes = getMsgField(messageType, "_slot_types");
  if (pFieldTypes != nullptr && PyList_Check(pFieldTypes))
  {
    for(Py_ssize_t i = 0; i < PyList_Size(pFieldTypes); i++) {
      const auto pFieldType = PyList_GetItem(pFieldTypes, i);
      if (pFieldType != nullptr)
        fieldTypes.push_back(stringFromPython(pFieldType));
    }
  }
  Py_XDECREF(pFieldTypes);
  return fieldTypes;
}

std::string getMsgFile(const std::string& messageType)
{
  if (messageType.empty())
    return {};

  const auto slashPos = messageType.find('/');
  if (slashPos == std::string::npos)
  {
    std::cerr << "Message type [" << messageType << "] does not contain a slash. It has to be of form "
              << "package/MessageType." << std::endl;
    return {};
  }

  const auto package = messageType.substr(0, slashPos);
  const auto baseMessageType = messageType.substr(slashPos + 1);

  if (package.empty())
  {
    std::cerr << "Package is empty for message type [" << messageType << "]" << std::endl;
    return {};
  }

  if (baseMessageType.empty())
  {
    std::cerr << "Message type after package is empty for message type [" << messageType << "]" << std::endl;
    return {};
  }

  const auto& packagePath = ros::package::getPath(package);
  if (packagePath.empty())
  {
    std::cerr << "Could not find package [" << package << "] for message type [" << messageType << "]" << std::endl;
    return {};
  }

  const char sep =
#ifdef _WIN32
    '\\';
#else
    '/';
#endif

  auto path = packagePath + sep + "msg" + sep + baseMessageType + ".msg";

  // check for existence
  struct stat buffer;
  if (stat(path.c_str(), &buffer) != 0)
    path = "";
  
  return path;
}

std::string getMD5Text(const std::string& messageType)
{
  if (messageType.empty())
    return {};

  const auto msgFile = getMsgFile(messageType);
  if (msgFile.empty())
  {
    std::cerr << "Could not find message proto file for message type [" << messageType << "]" << std::endl;
    return {};
  }

  std::string def {};

  auto pName = stringToPython("roslib.gentools");
  auto pModule = PyImport_Import(pName);
  Py_DECREF(pName);

  if (pModule != nullptr) {
    auto pDict = PyModule_GetDict(pModule);
    pName = stringToPython("get_file_dependencies");
    auto pFunc = PyDict_GetItem(pDict, pName);
    Py_DECREF(pName);
    if (pFunc != nullptr)
    {
      pName = stringToPython(msgFile);
      auto pGetDepsDict = PyObject_CallFunctionObjArgs(pFunc, pName, nullptr);
      Py_DECREF(pName);

      if (pGetDepsDict != nullptr)
      {
        pName = stringToPython("spec");
        auto pSpec = PyDict_GetItem(pGetDepsDict, pName);
        Py_DECREF(pName);

        if (pSpec != nullptr)
        {
          pName = stringToPython("compute_md5_text");
          auto pFunc2 = PyDict_GetItem(pDict, pName);
          Py_DECREF(pName);
          if (pFunc2 != nullptr)
          {
            auto pMD5Text = PyObject_CallFunctionObjArgs(pFunc2, pGetDepsDict, pSpec, nullptr);
            if (pMD5Text != nullptr)
            {
              def = stringFromPython(pMD5Text);
            } else printPythonError();
            Py_XDECREF(pMD5Text);
          } else printPythonError();
        } else printPythonError();
      } else printPythonError();
      Py_XDECREF(pGetDepsDict);
    } else printPythonError();
  }
  else
  {
    printPythonError();
    std::cerr << "Failed to load module roslib.gentools !" << std::endl;
  }
  Py_XDECREF(pModule);

  return def;
}

}
}