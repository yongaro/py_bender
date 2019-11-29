#ifndef PY_BENDER_BASIS_HPP
#define PY_BENDER_BASIS_HPP
#pragma once

#ifdef _DEBUG
  #undef _DEBUG
  #include <Python.h>
  #define _DEBUG
#else
  #include <Python.h>
#endif

#include <string>
#include <vector>
#include <type_traits>
#include <iostream>


#define PY_BENDER_ERROR(msg) std::cerr << "[LUA_BENDER_ERROR] " << std::endl\
    << "file : " << __FILE__ << std::endl\
    << "line " << __LINE__ << " in " << __func__ << "()"\
    << std::endl << msg << std::endl;\
    exit(EXIT_FAILURE)

#define PY_BENDER_WARNING(msg) std::cerr << "[LUA_BENDER_WARNING] " << std::endl\
    << "file : " << __FILE__ << std::endl\
    << "line " << __LINE__ << " in " << __func__ << "()"\
    << std::endl << msg << std::endl

#define PY_BENDER_LOG(msg) std::cout << "[LUA_BENDER] " << msg << std::endl


namespace py_bender{
    typedef PyObject*(*PyCFunction)(PyObject*, PyObject*);

    template<typename T>
    struct const_ref{
        typedef typename std::add_const<T>::type& type;
    };

    struct PyBenderObject{
        PyObject* m_object;

        PyBenderObject(PyObject* obj = nullptr): m_object(obj){}
        virtual ~PyBenderObject(){
            if( m_object != nullptr ){
                Py_DecRef(m_object);
            }
        }

        PyBenderObject& operator=(const PyBenderObject& obj){
            if( m_object != nullptr){
                Py_DecRef(m_object);
            }
            m_object = obj.m_object;
            Py_IncRef(m_object);
            return *this;
        }

        PyBenderObject& operator=(PyObject* obj){
            if( m_object != nullptr){
                Py_DecRef(m_object);
            }
            m_object = obj;
            return *this;
        }
    };

    template<typename T>
    struct value{};

    template<>
    struct value<const double&>{
        static PyObject* to_PyObject(const double& obj){ return PyFloat_FromDouble(obj); }
        static double from_PyObject(PyObject* obj){
            return (obj != nullptr) ? PyFloat_AsDouble(obj) : 0.0;
        }
    };

    template<>
    struct value<const float&>{
        static PyObject* to_PyObject(const float& obj){ return PyFloat_FromDouble(float(obj)); }
        static float from_PyObject(PyObject* obj){
            return (obj != nullptr) ? float(PyFloat_AsDouble(obj)) : 0.0f;
        }
    };

    template<>
    struct value<const int&>{
        static PyObject* to_PyObject(const int& obj){ return PyLong_FromLong(obj); }
        static int from_PyObject(PyObject* obj){
            return (obj != nullptr) ? int(PyLong_AsLong(obj)) : 0;
        }
    };

    template<>
    struct value<const long&>{
        static PyObject* to_PyObject(const long& obj){ return PyLong_FromLong(obj); }
        static long from_PyObject(PyObject* obj){
            return (obj != nullptr) ? PyLong_AsLong(obj) : 0;
        }
    };

    template<>
    struct value<const bool&>{
        static PyObject* to_PyObject(const int& obj){ return PyBool_FromLong(obj); }
        static int from_PyObject(PyObject* obj){
            return (obj != nullptr) ? bool(PyLong_AsLong(obj)) : 0;
        }
    };

    template<>
    struct value<const std::string&>{
        static PyObject* to_PyObject(const std::string& obj){ return PyBytes_FromString(obj.c_str()); }
        static std::string from_PyObject(PyObject* obj){
            return (obj != nullptr) ? std::string(PyBytes_AsString(obj)) : std::string();
        }
    };

    template<>
    struct value<const char* const&>{
        static PyObject* to_PyObject(const char* obj){ return PyBytes_FromString(obj); }
        static const char* from_PyObject(PyObject* obj){
            return (obj != nullptr) ? PyBytes_AsString(obj) : nullptr;
        }
    };

    struct py_state{
        py_state(){ Py_Initialize(); }
        virtual ~py_state(){ Py_Finalize(); }
    };



    struct py_module{
        static int set_search_path(const std::string& search_path){
            if( !search_path.empty() ){
                PyObject* py_sys_path = PySys_GetObject("path");
                PyObject* py_search_path = PyUnicode_FromString(search_path.c_str());

                if( PyList_Insert(py_sys_path, 0, py_search_path) < 0 ){
                    PY_BENDER_ERROR("Failed to insert " << search_path << " in the python system path.");
                    return -1;
                }

                Py_DecRef(py_sys_path);
                Py_DecRef(py_search_path);
            }

            return 0;
        }

        static PyObject* import(const std::string& module){
            PyObject* py_module = PyImport_ImportModule(module.c_str());
            PyObject* error_object = PyErr_Occurred();

            if( py_module == nullptr || error_object != nullptr ){
                PyErr_Print();
                PY_BENDER_ERROR("Failed to import the " << module  << " module.");
                return nullptr;
            }

            return py_module;
        }

        static int run_string(const std::string& script){
            if( script.empty() ){ return -1; }

            PyRun_SimpleString(script.c_str());

            return 0;
        }

        static int run_file(const std::string& script){
            if( script.empty() ){ return -1; }

            FILE* file = _Py_fopen(script.c_str(), "r");
            PyRun_SimpleFile(file, script.c_str());
            fclose(file);

            return 0;
        }
    };

    struct py_function{
        std::string m_name;

        virtual void call(){
            PyObject* python_path = PySys_GetObject("path");
        }
    };
}

#endif
