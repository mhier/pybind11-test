#include <string>
#include <iostream>
#include <cassert>

#include <pybind11/embed.h>
namespace py = pybind11;

// mock for boost::thread_interrupted exception type
class thread_interrupted
{
public:
  const char *what() const { return "Thread interrupted exception"; }
};


/*********************************************************************************************************************/

// This is a mock of the normal ApplicationModule class
class ApplicationModule
{
public:
  ApplicationModule(const std::string &name) : _name(name) {}

  std::string getName() const {
    return _name;
  }

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

  void terminate()
  {
    _isRunning = false;
  }

  void interruption_point()
  {
    if (!_isRunning)
    {
      throw thread_interrupted();
    }
  }

private:
  std::string _name;

  bool _isRunning{false};
};

/*********************************************************************************************************************/

class PythonApplicationModule : public ApplicationModule
{
public:
  PythonApplicationModule(const std::string &name);

  void run();

private:
  py::object _myThread;
};

/*********************************************************************************************************************/

extern std::map<std::string, PythonApplicationModule*> pythonApplicationModuleStore;

/*********************************************************************************************************************/
