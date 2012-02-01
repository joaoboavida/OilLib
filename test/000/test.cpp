
#include <iostream>
#include <fstream>

#include "time_series.hpp"

using namespace oil;

typedef time_series< double > TimeSeries;

int main(int argc, char *argv[])
{
  /*
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
*/
  std::ifstream prices("prices.dat");
  std::string sdate, sprice;
  prices >> sdate >> sprice;
  std::cout << sdate << "  " << sprice << std::endl;

  TimeSeries s;
  double fundSize = 10000,
         contractSize = 1000;

  int i = 0;
  int oldPositions = 0;
  double oldSewma = -1.18,
         oldLewma = -1.18;
  for(;;)
  {
    double v;
    prices >> sdate >> v;
    if(prices.good()) s.push_back(v);
    else break;

    TimeSeries::iterator it = s.end();
    --it;
    double price = it->value,
           ret = s.getReturn(it),
           vol = s.getVolatility(it),
           adj = s.getAdjustedReturn(it),
           ws = double(1) / double(2),
           sewma = (1-ws)*oldSewma + ws*price, //s.getMovingAverage(2),
           wl = double(1) / double(8),
           lewma = (1-wl)*oldLewma + wl*price, //s.getMovingAverage(8),
           crossover = sewma - lewma;

    int signal;
    if( crossover > 0 ) signal = 1;
    else if( crossover < 0 ) signal = -1;
    else signal = 0;

    int positions = fundSize * signal / ( contractSize * vol ),
        trade = positions - oldPositions;

    double PnL = oldPositions * ret * contractSize;

    std::cout << "price[" << ++i << "]=" << price
              << " return=" << ret
              << " vol=" << vol
              << " adj=" << adj
              << " sewma=" << sewma
              << " lewma=" << lewma
              << " crossover=" << crossover
              << " signal=" << signal
              << " posn=" << positions
              << " trade=" << trade
              << " PnL=" << PnL
              << " pcPnL=" << PnL/fundSize * 100.0
              << std::endl;

    oldPositions = positions;
    oldSewma = sewma;
    oldLewma = lewma;
  }

  return 0;

}

