#include <stdio.h>
#include <stdint.h>

#include <string>

/**
 * Documentation? What's that?
 * @doctodo
 */
namespace kiva {

using StringType = const char*;


#pragma mark - CompileTimeString

/**
 * 编译时刻就构建的字符串，不用等到运行时
 * 巧妙的利用了模板编译规则和 C++11 变长模板
 */
template <char... A>
struct CompileTimeString
{
  using C = const char;
  
  /**
   * 从模板列表中构建字符串
   */
  static C* value() {
    static C s[sizeof...(A)] = {A...};
    return s;
  }
};


#pragma mark - Type Format

using ObjectFormat = CompileTimeString<'O', 'b', 'j', 'e', 'c', 't', '@', '%', 'p'>;
using PointerFormat = CompileTimeString<'%', 'p'>;
using StringFormat = CompileTimeString<'%', 's'>;

using Int32Format = CompileTimeString<'%', 'd'>;
using Int16Format = Int32Format;
using Int8Format = Int32Format;
using CharFormat = CompileTimeString<'%', 'c'>;

using Uint32Format = CompileTimeString<'%', 'u'>;
using Uint16Format = Uint32Format;
using Uint8Format = Uint32Format;

using LongFormat = CompileTimeString<'%', 'l', 'd'>;
using UlongFormat = CompileTimeString<'%', 'l', 'u'>;

using FloatFormat = CompileTimeString<'%', '.', '2', 'f'>;
using DoubleFormat = CompileTimeString<'%', '.', '2', 'l', 'f'>;


# pragma mark - ValuePrinter Declaration

/**
 * 所有对象输出的缺省输出
 */
template <typename T>
struct ValuePrinter
{
  using F = ObjectFormat;
  static inline StringType format()
  {
    return F::value();
  }
  
  static inline int print(const T &t)
  {
    return printf(format(), &t);
  }
};

/**
 * 对所有指针的缺省输出
 */
template <typename T>
struct ValuePrinter<T*>
{
  using F = PointerFormat;
  static inline StringType format()
  {
    return F::value();
  }
  
  static inline int print(const T *t)
  {
    return printf(format(), t);
  }
};


/**
 * 由于 linux 和 windows 
 * 对 int64/uint64 的格式化字符串不同
 * 所以需要单独提出来用通用的方法打印
 */
template <>
struct ValuePrinter<int64_t>
{
  using F = Int32Format;
  static inline StringType format()
  {
    return F::value();
  }
  
  static inline int print(int64_t n)
  {
    int r = 0;
    if (n >= 10) {
      r += print(n / 10);
    }
    
    r += printf(format(), int32_t(n % 10));
    return r;
  }
};

template <>
struct ValuePrinter<uint64_t>
{
  using F = Int32Format;
  static inline StringType format()
  {
    return F::value();
  }
  
  static inline int print(uint64_t n)
  {
    int r = 0;
    if (n >= 10) {
      r += print(n / 10);
    }
    
    r += printf(format(), uint32_t(n % 10));
    return r;
  }
};


/**
 * 利用宏，快速生成通用的 ValuePrinter 代码
 */
#define _KP(T, F, P, V) \
  template <> \
  struct ValuePrinter<T> \
  { \
    static inline StringType format() \
    { \
      return F::value(); \
    } \
    \
    static inline int print(P) \
    { \
      return printf(format(), V); \
    } \
  }


_KP(int32_t,  Int32Format,  int32_t i,  i);
_KP(int16_t,  Int16Format,  int16_t i,  i);
_KP(int8_t,   Int8Format,   int8_t  i,  i);
_KP(uint32_t, Uint32Format, uint32_t i, i);
_KP(uint16_t, Uint16Format, uint16_t i, i);
_KP(uint8_t,  Uint8Format,  uint8_t i,  i);
_KP(char,     CharFormat,   char c,     c);
_KP(long,          LongFormat,  long l,          l);
_KP(unsigned long, UlongFormat, unsigned long l, l);
_KP(float,  FloatFormat,  float f,  f);
_KP(double, DoubleFormat, double d, d);

/**
 * 对 C++ 字符串的输出
 */
_KP(std::string, StringFormat, const std::string &s, s.c_str());

/**
 * 对 C 字符串的输出
 */
_KP(StringType, StringFormat, StringType s, s);

/**
 * 对非const C 字符串的输出
 */
_KP(char*, StringFormat, char *s, s);

#undef _KP


#pragma mark - Printer

class MultiplePrinter
{
private:
  int count;

public:
  MultiplePrinter(int count)
    :count(count)
  {
  }
  
  int print(StringType str)
  {
    int r = ValuePrinter<StringType>::print(str);
    r += putchar(' ');
    return r;
  }

  template <typename T>
  int print(const T &t)
  {
    int r = ValuePrinter<T>::print(t);
    r += putchar(' ');
    return r;
  }
  
  template <typename H, typename... A>
  int print(const H &h, const A&... a)
  {
    // 为了多编译器的兼容性, 不可以直接使用
    // return print(h) + print(a...);
    // 因为不同的编译器会生成不同的代码
    // 比如: int i = f(1) + f(2);
    // 可能会先执行 f(1) 也可能会先执行 f(2)
    // 无法保证输出顺序为参数的传递顺序
    
    int r = print(h);
    r += print(a...);
    return r;
  }
};


template <typename... A>
int print(const A&... a)
{
  MultiplePrinter printer(sizeof...(A));
  
	int r = printer.print(a...);
	r += printf("\n");
	return r;
}

}
