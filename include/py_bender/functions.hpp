#ifndef PY_BENDER_FUNCTIONS_HPP
#define PY_BENDER_FUNCTIONS_HPP
#pragma once

#include "basis.hpp"

namespace py_bender{
    struct TestStruct{
        int    a;
        float  b;
        double c;
    };

    template<class C, typename ...Args>
    void generic_set(C& obj, Args... args){
        obj = {args...};
    }

    template<class C, typename mtype, int offset>
    struct accessor{
        static mtype get(C& obj){
            uint8_t* base_ptr = reinterpret_cast<uint8_t*>(&obj);
            mtype* member = reinterpret_cast<mtype*>(base_ptr + offset);
            return *member;
        }
    };

    template<class C, typename mtype, int offset>
    void mutator(C& obj, const mtype& value){
        uint8_t* base_ptr = reinterpret_cast<uint8_t*>(&obj);
        mtype* member = reinterpret_cast<mtype*>(base_ptr + offset);
        *member = value;
    }

#define GENERATE_ACCESSOR(type, member_type, member_name)\
    accessor<type, member_type, offsetof(type, member_name)>::get

#define GENERATE_MUTATOR(type, member_type, member_name)\
    mutator<type, member_type, offsetof(type, member_name)>

    void test_func(){
        TestStruct t;
        generic_set<TestStruct, int, float, double>(t, 10, 12.0f, 0.0);
        void(*mutator_ptr)(TestStruct&, const float&) = mutator<TestStruct, float, offsetof(TestStruct, b)>;
        void(*mutator_ptr2)(TestStruct&, const float&) = GENERATE_MUTATOR(TestStruct, float, b);
    }

    template<typename R, typename ...Args>
    struct PyFunction{
        static R dispatch(PyObject* py_module, const char* func_name, Args... args){
            PyBenderObject py_function = PyObject_GetAttrString(py_module, func_name);
            if( py_function.m_object != nullptr && PyCallable_Check(py_function.m_object) ){
                PyBenderObject args_tuple = PyTuple_Pack(sizeof...(Args),
                                                         py_bender::value<typename const_ref<Args>::type>::to_PyObject(args)...);

                PyBenderObject ret_value = PyObject_CallObject(py_function.m_object, args_tuple.m_object);

                return py_bender::value< typename const_ref<R>::type >::from_PyObject(ret_value.m_object);
            }
            else{
                PY_BENDER_ERROR("Could not load and call the " << func_name << " function");
            }

            return R();
        }
    };


    template<typename ...Args>
    struct PyFunction<void, Args...>{
        static void dispatch(PyObject* py_module, const char* func_name, Args... args){
            PyBenderObject py_function = PyObject_GetAttrString(py_module, func_name);
            if( py_function.m_object != nullptr && PyCallable_Check(py_function.m_object) ){

                PyBenderObject args_tuple = PyTuple_Pack(sizeof...(Args),
                                                         py_bender::value<typename const_ref<Args>::type>::to_PyObject(args)...);

                PyObject_CallObject(py_function.m_object, args_tuple.m_object);
            }
            else{
                PY_BENDER_ERROR("Could not load and call the " << func_name << " function");
            }
        }
    };


    template<typename Fn, Fn func, typename R, typename ...Args>
    struct Cfunction{
        static PyObject* adapter(PyObject* self, PyObject* args){
            Py_ssize_t first_index = 1;
            return py_bender::value<typename const_ref<R>::type>::to_PyObject(
                        func(py_bender::value<typename const_ref<Args>::type>::from_PyObject(PyTuple_GetItem(args, first_index++))...)
                        );
        }
    };

    template<typename Fn, Fn func, typename ...Args>
    struct Cfunction<Fn, func, void, Args...>{
        static PyObject* adapter(PyObject* self, PyObject* args){
            Py_ssize_t first_index = 1;
            func(py_bender::value<typename const_ref<Args>::type>::from_PyObject(PyTuple_GetItem(args, first_index++))...);
            return 0;
        }
    };
}


#endif
