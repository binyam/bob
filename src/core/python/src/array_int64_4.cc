/**
 * @author <a href="mailto:andre.anjos@idiap.ch">Andre Anjos</a> 
 * @brief blitz::Array<int64_t,4> to and from python converters
 */
#include "core/python/array.h"
declare_integer_array(int64_t, 4, int64, bind_core_array_int64_4)