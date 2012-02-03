
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


typedef std::vector< TradingRule * > ExistingRules;

int main(int argc, char *argv[])
{
  Market myMarket ( 100 );

  myMarket.setContractSize(1000);

  ExistingRules rules;
  for ( int i = 0; i < 8; ++i )
  {
    int s = pow(2, i+1);
    rules.push_back ( new TradingRuleLAG( s, s*2 ) );
    rules.push_back ( new TradingRuleAVG( s, s*2 ) );
  }

  for ( Market::iterator it = myMarket.begin(); it != myMarket.end(); ++it)
  {
    Agent& a = *(*it);

    double initialCash = 1000;
    a.init(initialCash, myMarket);

    int pos = rules.size() * ((double) rand() / (double) RAND_MAX);
    a.setTradingRule( rules[pos] );

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

  // clean the trading rules
  for ( ExistingRules::iterator it = rules.begin(); it != rules.end(); ++it )
  {
    delete *it;
  }

  return 0;

}

