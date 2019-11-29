#ifndef PY_BENDER_STRUCTS_HPP
#define PY_BENDER_STRUCTS_HPP

#include "basis.hpp"

namespace py_bender{
    template<class T>
    struct PyStructWrapper{
        PyObject_HEAD
    };

}


#endif
