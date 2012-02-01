
#include <iostream>

#include "time_series.hpp"

using namespace oil;

typedef time_series< double > TimeSeries;

int main(int argc, char *argv[])
{
  TimeSeries s;

  for(int i = 0; i < 100; ++i)
  {
    s.push_back(i + 1);
    std::cout << "i=" << i << ": size=" << s.size()
              << "  last=" << s.back()
              << "     av1=" << s.getMovingAverage( 1 )
              << "  av2=" << s.getMovingAverage( 2 )
              << "  av4=" << s.getMovingAverage( 4 )
              << "  av8=" << s.getMovingAverage( 8 )
              << std::endl;
  }

  return 0;
}


