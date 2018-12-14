#include "quant_cpu.h"

#define RFLOAT_TO_BITS(x) (*reinterpret_cast<unsigned int*>(x))
#define RBITS_TO_FLOAT(x) (*reinterpret_cast<float*>(x))
#define FLOAT_TO_BITS(f, i) assert(sizeof f == sizeof i); std::memcpy(&i, &f, sizeof i)
#define BITS_TO_FLOAT(i, f) assert(sizeof f == sizeof i); std::memcpy(&f, &i, sizeof f)


unsigned int clip_exponent(int exp_bits, int man_bits,
                           unsigned int old_num,
                           unsigned int quantized_num) {
  int quantized_exponent_store = quantized_num << 1 >> 1 >> 23; // 1 sign bit, 23 mantissa bits
  int min_exponent_store = -((1 << (exp_bits-1))-1) + 126;
  int max_exponent_store = (1 << (exp_bits-1)) + 126;
  if (quantized_exponent_store > max_exponent_store) {
    unsigned int max_man = (unsigned int ) -1 << 9 >> 9 >> (23-man_bits) << (23-man_bits); // 1 sign bit, 8 exponent bits, 1 virtual bit
    unsigned int max_num = ((unsigned int) max_exponent_store << 23) | max_man;
    unsigned int old_sign = old_num >> 31 << 31;
    quantized_num = old_sign | max_num;
  } else if (quantized_exponent_store < min_exponent_store) {
    unsigned int min_num = ((unsigned int) min_exponent_store << 23);
    unsigned int old_sign = old_num >> 31 << 31;
    quantized_num = old_sign | min_num;
  }
  return quantized_num;
}

// unsigned int extract_exponent(float *a) {
//   unsigned int temp = *(reinterpret_cast<unsigned int*>(a));
//   temp = (temp << 1 >> 24); // single precision, 1 sign bit, 23 mantissa bits
//   return temp-127+1; // exponent offset and virtual bit
// }