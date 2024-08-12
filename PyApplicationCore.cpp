
#include "MockApplicationCore.h"

#include <ChimeraTK/SupportedUserTypes.h>

#include <boost/fusion/algorithm.hpp>
#include <boost/fusion/container/map.hpp>

#include <iostream>
#include <map>
#include <string>
#include <variant>

#include <pybind11/stl.h>

using namespace py::literals;
using ChimeraTK::DataType;


/*********************************************************************************************************************/

class PythonApplicationModuleTrampoline : public PythonApplicationModule {
 public:
  using PythonApplicationModule::PythonApplicationModule;

  void mainLoop() override {
    PYBIND11_OVERRIDE_PURE(void, /* Return type */
        PythonApplicationModule, /* Parent class */
        mainLoop                 /* Name of function in C++ (must match Python name) */
    );
  }
};

/*********************************************************************************************************************/

class PyScalarAccessor {

  // Helper for constructor
  static userTypeTemplateVariant<ScalarAccessor> createAccessor(ChimeraTK::DataType type, std::string name) {
    std::optional<userTypeTemplateVariant<ScalarAccessor>> rv;
    ChimeraTK::callForType(type, [&](auto t) {
      using UserType = decltype(t);
      rv = userTypeTemplateVariant<ScalarAccessor>(std::in_place_type<ScalarAccessor<UserType>>, name);
    });
    return rv.value();
  }

 public:

  PyScalarAccessor(ChimeraTK::DataType type, std::string name)
  : _accessor(createAccessor(type, name)) {}

  userTypeVariant readAndGet() { std::optional<userTypeVariant> rv;
    std::visit([&](auto acc) { rv = acc.readAndGet(); }, _accessor);
    return rv.value();
  }

  void setAndWrite(userTypeVariant val) {
    std::visit(
        [&](auto acc) {
          using ACC = decltype(acc);
          using expectedUserType = typename std::result_of<decltype (&ACC::readAndGet)(ACC)>::type;
          std::visit(
              [&](auto value) {
                acc.setAndWrite(ChimeraTK::userTypeToUserType<expectedUserType>(value));
              },
              val);
        },
        _accessor);
  }

 private:
  userTypeTemplateVariant<ScalarAccessor> _accessor;
};

/*********************************************************************************************************************/

PYBIND11_MODULE(PyApplicationCore, m) {
  py::class_<DataType> mDataType(m, "DataType");

  mDataType.def(py::init<DataType::TheType>());

  py::enum_<DataType::TheType>(mDataType, "TheType")
      .value("none", DataType::none)
      .value("int8", DataType::int8)
      .value("uint8", DataType::uint8)
      .value("int16", DataType::int16)
      .value("uint16", DataType::uint16)
      .value("int32", DataType::int32)
      .value("uint32", DataType::uint32)
      .value("int64", DataType::int64)
      .value("uint64", DataType::uint64)
      .value("float32", DataType::float32)
      .value("float64", DataType::float64)
      .value("string", DataType::string)
      .value("Boolean", DataType::Boolean)
      .value("Void", DataType::Void)
      .export_values();

  py::class_<PythonApplicationModule, PythonApplicationModuleTrampoline> cam(m, "ApplicationModule");
  // The class docstring does not appear in the stub, see: https://github.com/python/mypy/issues/16543
  cam.doc() = "HAAAAAALLLOOOO!";

  cam.def(py::init<const std::string&>(), R"(
  Documentation of constructor.

  blah blubb

    indented line...

  END
  )")
      .def("getName", &PythonApplicationModule::getName, "HALLO!!!")
      .def("mainLoopWrapper", &PythonApplicationModule::mainLoopWrapper)
      .def("interruption_point", &PythonApplicationModule::interruption_point);

  py::class_<PyScalarAccessor>(m, "ScalarAccessor")
      .def(py::init<ChimeraTK::DataType, const std::string&>())
      .def(py::init<ChimeraTK::DataType::TheType, const std::string&>())
      .def("readAndGet",
          [](PyScalarAccessor& self) {
            py::gil_scoped_release release;
            return self.readAndGet();
          })
      .def("setAndWrite", &PyScalarAccessor::setAndWrite);
}

/*********************************************************************************************************************/
