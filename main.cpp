
#include <map>
#include <string>
#include <iostream>

#include <pybind11/embed.h>

namespace py = pybind11;

#include "MockApplicationCore.h"

/*********************************************************************************************************************/

int main() {
    {
        py::scoped_interpreter pyint{}; // start the interpreter and keep it alive

        py::register_exception<thread_interrupted>(py::module_::import("__main__"), "ThreadInterrupted");

        py::exec("import threading, traceback");

        std::cout << "======= eval_file" << std::endl;
        py::eval_file("test.py");

        std::cout << "======= Launch PyAMs" << std::endl;
        for (auto [name, mod] : pythonApplicationModuleStore)
        {
            std::cout << "   name = " << name << std::endl;
            mod->run();
        }
        std::cout << "======= All PyAMs launched." << std::endl;

        {
            py::gil_scoped_release release;
            usleep(6000000);
        }

        std::cout << "======= Restart SomeName2" << std::endl;
        pythonApplicationModuleStore["SomeName2"]->run();

        {
            py::gil_scoped_release release;
            usleep(3000000);
        }

        std::cout << "======= Terminate running modules" << std::endl;
        for(auto [name, var]: accessorQueueMap) {
            var.push_exception(std::make_exception_ptr(thread_interrupted()));
        }

        for (auto [name, mod] : pythonApplicationModuleStore)
        {
            std::cout << "   name = " << name << std::endl;
            mod->terminate();
        }
        std::cout << "======= Destroying interpreter" << std::endl;
    }
    std::cout << "======= End of C++ main()" << std::endl;
}

/*********************************************************************************************************************/
