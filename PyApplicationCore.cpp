
#include <map>
#include <string>
#include <iostream>

#include "MockApplicationCore.h"

using namespace py::literals;

/*********************************************************************************************************************/

class PythonApplicationModuleTrampoline : public PythonApplicationModule {
public:
  using PythonApplicationModule::PythonApplicationModule;

  void mainLoop() override
  {
    PYBIND11_OVERRIDE_PURE(
        void,                    /* Return type */
        PythonApplicationModule, /* Parent class */
        mainLoop                 /* Name of function in C++ (must match Python name) */
    );
  }
};

/*********************************************************************************************************************/

PYBIND11_MODULE(PyApplicationCore, m)
{

  py::class_<PythonApplicationModule, PythonApplicationModuleTrampoline> cam(m, "ApplicationModule");
  // The class docstring does not appear in the stub, see: https://github.com/python/mypy/issues/16543
  cam.doc() = "HAAAAAALLLOOOO!";

  cam.def(py::init<const std::string &>(), R"(
  Documentation of constructor.

  blah blubb

    indented line...

  END
  )")
      .def("getName", &PythonApplicationModule::getName, "HALLO!!!")
      .def("mainLoopWrapper", &PythonApplicationModule::mainLoopWrapper)
      .def("interruption_point", &PythonApplicationModule::interruption_point);

  py::class_<ScalarAccessor>(m, "ScalarAccessor")
      .def(py::init<const std::string &>())
      .def("readAndGet", [](ScalarAccessor &self) -> int
           { py::gil_scoped_release release;
           return self.readAndGet(); })
      .def("setAndWrite", &ScalarAccessor::setAndWrite);
}

/*********************************************************************************************************************/
