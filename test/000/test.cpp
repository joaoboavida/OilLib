
#include <iostream>

#include "time_series.hpp"

using namespace oil;

typedef time_series< double > TimeSeries;

int main(int argc, char *argv[])
{
  TimeSeries s;


  for(int i = 0; i < 100; ++i)
  {
    s.push_back(1.0 + i);
    std::cout << i << ": size=" << s.size()
              << "  av1=" << s.movingAverage( 1 )
              << "  av2=" << s.movingAverage( 2 )
              << "  av4=" << s.movingAverage( 4 )
              << "  av8=" << s.movingAverage( 8 )
              << std::endl;
  }

  return 0;
}


