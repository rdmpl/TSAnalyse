#ifndef ULITILITIES_HPP__
#define ULITILITIES_HPP__
#include <stdio.h>
#define LOG_INFO(fmt, args...) \
  printf("%s:%s[%d]: " fmt "\n", __FILE__, __FUNCTION__, __LINE__, ##args)
#define lengthof(A) ((sizeof(A) / sizeof((A)[0])))
#endif