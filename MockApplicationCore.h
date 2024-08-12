#include <pybind11/embed.h>

#include <ChimeraTK/cppext/future_queue.hpp>
#include <ChimeraTK/SupportedUserTypes.h>

#include <cassert>
#include <iostream>
#include <map>
#include <string>
#include <variant>
namespace py = pybind11;

// mock for boost::thread_interrupted exception type
class thread_interrupted {
 public:
  const char* what() const { return "Thread interrupted exception"; }
};

/*********************************************************************************************************************/

// This should maybe go to DeviceAccess SupportedUserTypes.h
// NOTE: The strictest type must come first, as pybind11 will pick the first matching type in order when converting
// from a Python type into the variant.
template<template<typename> class TPL>
using userTypeTemplateVariant =
    std::variant<TPL<ChimeraTK::Boolean>, TPL<int8_t>, TPL<uint8_t>, TPL<int16_t>, TPL<uint16_t>, TPL<int32_t>,
        TPL<uint32_t>, TPL<int64_t>, TPL<uint64_t>, TPL<float>, TPL<double>, TPL<std::string>, TPL<ChimeraTK::Void>>;

using userTypeVariant = std::variant<ChimeraTK::Boolean, int8_t, uint8_t, int16_t, uint16_t, int32_t, uint32_t, int64_t,
    uint64_t, float, double, std::string, ChimeraTK::Void>;

/*********************************************************************************************************************/

// This is a mock of the normal ApplicationModule class
class ApplicationModule {
 public:
  ApplicationModule(const std::string& name) : _name(name) {}

  std::string getName() const { return _name; }

  virtual void mainLoop() = 0;

  void run() {
    // this would be the standard ApplicationCore implementation for C++ modules, will be overridden below
    assert(false);
  }

  void mainLoopWrapper() {
    std::cout << "ApplicationModule::mainLoopWrapper()" << std::endl;
    _isRunning = true;
    mainLoop();
  }

  void terminate() { _isRunning = false; }

  void interruption_point() {
    if(!_isRunning) {
      throw thread_interrupted();
    }
  }

 private:
  std::string _name;

  bool _isRunning{false};
};

/*********************************************************************************************************************/

// The future_queue has more template arguments which we want to keep at their defaults. userTypeTemplateVariant
// expects exactly one template argument
template<typename UserType>
using the_future_queue = cppext::future_queue<UserType>;

extern std::map<std::string, userTypeTemplateVariant<the_future_queue>> accessorQueueMap;

/*********************************************************************************************************************/

template<typename UserType>
class ScalarAccessor {
 public:
  ScalarAccessor(std::string name) : _name(name) {
    if(accessorQueueMap.find(_name) == accessorQueueMap.end()) {
      accessorQueueMap.emplace(_name, cppext::future_queue<UserType>(2));
    }
  }

  ScalarAccessor() = default;
  ScalarAccessor(const ScalarAccessor& other) = default;
  ScalarAccessor(ScalarAccessor&& other) = default;
  ScalarAccessor& operator=(const ScalarAccessor& other) = default;
  ScalarAccessor& operator=(ScalarAccessor&& other) = default;

  UserType readAndGet() {
    UserType val;
    std::get<cppext::future_queue<UserType>>(accessorQueueMap[_name]).pop_wait(val);
    return val;
  }

  void setAndWrite(UserType val) { std::get<cppext::future_queue<UserType>>(accessorQueueMap[_name]).push(val); }

 private:
  std::string _name;
};

/*********************************************************************************************************************/

class PythonApplicationModule : public ApplicationModule {
 public:
  PythonApplicationModule(const std::string& name);

  void run();

 private:
  py::object _myThread;
};

/*********************************************************************************************************************/

extern std::map<std::string, PythonApplicationModule*> pythonApplicationModuleStore;

/*********************************************************************************************************************/
