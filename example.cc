#include <string.h>
#include <errno.h>

#include "print.h"

int main(int argc, char **argv)
{
  using kiva::print;
  
  print("hello", "world");
  print(0, 0.1, (short) 2, 'c');
  print("test error message:", std::string(strerror(ENOMEM)));
  
  int32_t i32 = 1;
  int64_t i64 = 2;
  int16_t i16 = 3;
  int8_t  i8  = 4;
  print(i32, i64, i16, i8);
  
  uint32_t ui32 = 5;
  uint64_t ui64 = 6;
  uint16_t ui16 = 7;
  uint8_t  ui8  = 8;
  print(ui32, ui64, ui16, ui8);
  
  print(&ui32);
}

