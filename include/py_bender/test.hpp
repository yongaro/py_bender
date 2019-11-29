#ifndef PY_BENDER_TEST_HPP
#define PY_BENDER_TEST_HPP
#pragma once

#include "py_bender.hpp"

namespace py_bender{



    static void test(){
        std::string search_path = "./assets/engine/python_modules";
        std::vector<std::string> modules = {"test_module"};
//        std::wstring w_search_path = {search_path.begin(), search_path.end()};

        Py_Initialize();
        py_module::set_search_path(search_path);
        py_module::run_string("import test_module\n"
                              "test_module.test_func()");


        PyObject* py_test_module = py_module::import("test_module");
        py_bender::PyFunction<void, int, const char*>::dispatch(py_test_module, "test_func_wargs", 12, "TEST");

        Py_Finalize();
    }
}

#endif
