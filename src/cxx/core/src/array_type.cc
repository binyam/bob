/**
 * @author <a href="mailto:andre.dos.anjos@gmail.com">Andre Anjos</a> 
 * @date Tue  8 Feb 16:35:57 2011 
 *
 * @brief Some type-related array utilities 
 */

#include "core/array_type.h"
#include "core/Exception.h"

namespace tca = Torch::core::array;

static const char* t_bool_string = "bool";
static const char* t_int8_string = "int8";
static const char* t_int16_string = "int16";
static const char* t_int32_string = "int32";
static const char* t_int64_string = "int64";
static const char* t_uint8_string = "uint8";
static const char* t_uint16_string = "uint16";
static const char* t_uint32_string = "uint32";
static const char* t_uint64_string = "uint64";
static const char* t_float32_string = "float32";
static const char* t_float64_string = "float64";
static const char* t_float128_string = "float128";
static const char* t_complex64_string = "complex64";
static const char* t_complex128_string = "complex128";
static const char* t_complex256_string = "complex256";
static const char* t_unknown_string = "unknown";

size_t tca::getElementSize(ElementType t) {
  switch(t) {
    case tca::t_bool:
      return sizeof(bool);
    case tca::t_int8:
      return sizeof(int8_t);
    case tca::t_int16:
      return sizeof(int16_t);
    case tca::t_int32:
      return sizeof(int32_t);
    case tca::t_int64:
      return sizeof(int64_t);
    case tca::t_uint8:
      return sizeof(uint8_t);
    case tca::t_uint16:
      return sizeof(uint16_t);
    case tca::t_uint32:
      return sizeof(uint32_t);
    case tca::t_uint64:
      return sizeof(uint64_t);
    case tca::t_float32:
      return sizeof(float);
    case tca::t_float64:
      return sizeof(double);
    case tca::t_float128:
      return sizeof(long double);
    case tca::t_complex64:
      return sizeof(std::complex<float>);
    case tca::t_complex128:
      return sizeof(std::complex<double>);
    case tca::t_complex256:
      return sizeof(std::complex<long double>);
    default:
      throw Exception();
  }
}

const char* tca::stringize(ElementType t) {
  switch(t) {
    case tca::t_bool: 
      return t_bool_string;
    case tca::t_int8:
      return t_int8_string;
    case tca::t_int16:
      return t_int16_string;
    case tca::t_int32:
      return t_int32_string;
    case tca::t_int64:
      return t_int64_string;
    case tca::t_uint8:
      return t_uint8_string;
    case tca::t_uint16:
      return t_uint16_string;
    case tca::t_uint32:
      return t_uint32_string;
    case tca::t_uint64:
      return t_uint64_string;
    case tca::t_float32:
      return t_float32_string;
    case tca::t_float64:
      return t_float64_string;
    case tca::t_float128:
      return t_float128_string;
    case tca::t_complex64:
      return t_complex64_string;
    case tca::t_complex128:
      return t_complex128_string;
    case tca::t_complex256:
      return t_complex256_string;
    default:
      return t_unknown_string;
  }
}

tca::ElementType tca::unstringize(const char* s) {
  std::string sc(s);
  if (sc == t_bool_string) return tca::t_bool;
  if (sc == t_int8_string) return tca::t_int8;
  if (sc == t_int16_string) return tca::t_int16;
  if (sc == t_int32_string) return tca::t_int32;
  if (sc == t_int64_string) return tca::t_int64;
  if (sc == t_uint8_string) return tca::t_uint8;
  if (sc == t_uint16_string) return tca::t_uint16;
  if (sc == t_uint32_string) return tca::t_uint32;
  if (sc == t_uint64_string) return tca::t_uint64;
  if (sc == t_float32_string) return tca::t_float32;
  if (sc == t_float64_string) return tca::t_float64;
  if (sc == t_float128_string) return tca::t_float128;
  if (sc == t_complex64_string) return tca::t_complex64;
  if (sc == t_complex128_string) return tca::t_complex128;
  if (sc == t_complex256_string) return tca::t_complex256;
  return tca::t_unknown;
}