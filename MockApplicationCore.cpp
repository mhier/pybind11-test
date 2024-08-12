
#include <map>
#include <string>
#include <iostream>

#include "MockApplicationCore.h"

using namespace py::literals;

/*********************************************************************************************************************/

std::map<std::string, PythonApplicationModule *> pythonApplicationModuleStore;
std::map<std::string, userTypeTemplateVariant<the_future_queue>> accessorQueueMap;

/*********************************************************************************************************************/

void PythonApplicationModule::run()
{
  std::cout << "PythonApplicationModule::run()" << std::endl;
  auto locals = py::dict("theModule"_a = *this);
  py::exec(R"(
          def mainLoopWrapper2(self):
            try:
              self.mainLoopWrapper()
            except ThreadInterrupted as e:
              print("Got ThreadInterrupted in "+self.getName())
            except Exception as e:
              print("Exception in module "+self.getName()+":")
              traceback.print_exception(e)
          theModule.mainLoopWrapper2 = mainLoopWrapper2.__get__(theModule)
        )",
           py::globals(), locals);
  _myThread = py::eval("threading.Thread(target=theModule.mainLoopWrapper2)", py::globals(), locals);
  _myThread.attr("start")();
}

/*********************************************************************************************************************/

PythonApplicationModule::PythonApplicationModule(const std::string &name) : ApplicationModule(name)
{
  pythonApplicationModuleStore[name] = this;
}

/*********************************************************************************************************************/
