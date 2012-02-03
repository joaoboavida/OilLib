
#include <iostream>
#include <fstream>
#include <cstdlib>

#include <string>

#include "market.hpp"

using namespace oil;

typedef time_series< double > TimeSeries;
typedef trading_rule< double > TradingRule;
typedef moving_average_crossover< double > TradingRuleAVG;
typedef lag_crossover< double > TradingRuleLAG;
typedef agent< double > Agent;
typedef market< double > Market;

int main(int argc, char *argv[])
{
  Market myMarket ( 10 );

  myMarket.setContractSize(1000);

  TradingRuleLAG ruleLag(2, 8);
  TradingRuleAVG ruleAvg(2, 8);

  for ( Market::iterator it = myMarket.begin(); it != myMarket.end(); ++it)
  {
    Agent& a = *(*it);

    double initialCash = 1000;
    a.init(initialCash, myMarket);

    double r = (double) rand() / (double) RAND_MAX;
    if ( r < 0.5 ) a.setTradingRule( &ruleLag );
    else a.setTradingRule( &ruleAvg );
  }

  std::ifstream prices("prices.dat");
  std::string sdate, sprice;
  prices >> sdate >> sprice;
  std::cout << sdate << "  " << sprice << std::endl;

  int timeLevel = 0;
  for(;;)
  {
    double v;
    prices >> sdate >> v;
    if(prices.good()) myMarket.getOilPrice().push_back(v);
    else break;

    std::cout << "timeLevel=" << timeLevel << " " << std::endl;
    myMarket.trade();

    timeLevel++;
    if ( timeLevel > 20 ) break;
  }



//  TimeSeries s;
/*
  for(int i = 0; i < 100; ++i)
  {
    s.push_back(i + 1);
    std::cout << "i=" << i << ": size=" << s.size()
              << "  last=" << s.back()
              << "     av1=" << s.getMovingAverage( 1 )
              << " .v1=" << s.getMovingAverage( 1, --s.end() )
              << "  av2=" << s.getMovingAverage( 2 )
              << " .v2=" << s.getMovingAverage( 2, --s.end() )
              << "  av4=" << s.getMovingAverage( 4 )
              << " .v4=" << s.getMovingAverage( 4, --s.end() )
              << "  av8=" << s.getMovingAverage( 8 )
//              << " .v8=" << s.getMovingAverage( 8, --s.end() )
              << std::endl;
  }
  return 0;
*/
/*
  std::ifstream prices("prices.dat");
  std::string sdate, sprice;
  prices >> sdate >> sprice;
  std::cout << sdate << "  " << sprice << std::endl;

  double fundSize = 10000,
         contractSize = 1000;

  TradingRuleLAG ruleLag(2, 8);
  TradingRuleAVG ruleAvg(2, 8);

  Agent firstAgent, secondAgent;

  Agent::setContractSize(1000);

  firstAgent.setTradingRule(&ruleLag);
  firstAgent.setOilPrice(s);
  firstAgent.init(fundSize);

  secondAgent = firstAgent;
  secondAgent.setTradingRule(&ruleAvg);

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
           sewma = (1-ws)*oldSewma + ws*price, //
        // sewma = s.getMovingAverage(2),
           wl = double(1) / double(8),
           lewma = (1-wl)*oldLewma + wl*price, //
        // lewma = s.getMovingAverage(8),
           crossover = sewma - lewma;

    int signal;
    if( crossover > 0 ) signal = 1;
    else if( crossover < 0 ) signal = -1;
    else signal = 0;

    int positions = fundSize * signal / ( contractSize * vol ),
        trade = positions - oldPositions;

    double PnL = oldPositions * ret * contractSize;

    std::cout << "MARKET: price[" << i << "]=" << price
              << " return=" << ret
              << " vol=" << vol
              << " adj=" << adj
              << std::endl;

    std::cout << "EXCELOLD:"
              << " sewma=" << sewma
              << " lewma=" << lewma
              << " crossover=" << crossover
              << " signal=" << signal
              << " posn=" << positions
              << " trade=" << trade
              << " PnL=" << PnL
              << " pcPnL=" << PnL/fundSize * 100.0
              << " cash=" << fundSize
              << std::endl;

    std::cout << "AGENTLAG: ";
    blabla ( firstAgent );
    std::cout << "AGENTAVG: ";
    blabla ( secondAgent );
    std::cout << std::endl;

    i++;
    oldPositions = positions;
    oldSewma = sewma;
    oldLewma = lewma;

    myMarket.trade();
  }
*/

  prices.close();
  return 0;

}

