
#include <iostream>
#include <fstream>

#include "time_series.hpp"
#include "trading_rule.hpp"

using namespace oil;

typedef time_series< double > TimeSeries;
typedef trading_rule< double > TradingRule;
typedef moving_average_crossover< double > TradingRuleAVG;
typedef lag_crossover< double > TradingRuleLAG;

template< typename Type >
class agent
{
public:

  typedef Type value_type;
  typedef time_series< Type > series_type;
  typedef trading_rule< Type > rule_type;

  agent () {}
  agent ( value_type cash )
   :
     m_initialCash ( cash ),
     m_currentCash ( cash ),
     m_positions ( 0 )
  {
  }
  ~agent () {}

  void init ( value_type cash )
  {
    m_initialCash = cash;
    m_currentCash = cash;
    m_positions = 0;
  }

  void setOilPrice ( series_type &s ) { m_oilPrice = &s; }
  void setTradingRule ( rule_type *r ) { m_rule = r; }
  static void setContractSize ( const value_type size ) { m_contractSize = size; }

  value_type getCash() { return m_currentCash; }
  void setCash ( value_type c ) { m_currentCash = c; }
  value_type getInitialCash() { return m_initialCash; }
  bool isSolvent() { return (getWealth() > 0); }

  value_type getWealth() { return m_currentCash; }
  value_type getDemand() { return m_positions; }

  typename rule_type::BuySignal decide()
  {
    return (*m_rule)( *m_oilPrice );
  }

  int computeDemand ()
  {
    // analyze the series - compute the signal
    typename rule_type::BuySignal sig ( decide() );

    // translate the signal to an int
    int sigval = rule_type::translate( sig ); // -1, 0, 1

    // compute wanted position - absolute
    m_positions = ( sigval * m_currentCash )
                  /
                  ( m_contractSize * m_oilPrice->getVolatility() );

    return m_positions;
  }

  value_type trade ( int nFinalPositions )
  {
    value_type PnL = nFinalPositions
                 *
                 m_oilPrice->getReturn() * m_contractSize; // unit return
    m_currentCash += PnL;
    m_positions = 0;
    return PnL;
  }

private:

  TimeSeries *m_oilPrice;
  rule_type *m_rule;

  value_type m_initialCash;
  value_type m_currentCash;

  static int m_contractSize;

  int m_positions;

};

template< typename Type >
int agent< Type >::m_contractSize = 1000;

typedef agent< double > Agent;




void blabla( Agent& a )
{
  int positions = a.computeDemand();
  int trade = positions;
//  a.setCash ( 10000 );
  double PnL = a.trade( trade );
//  a.setCash ( 10000 );

  std::cout << " posn=" << positions
            << " trade=" << trade
            << " PnL=" << PnL
            << " pcPnL=" << PnL / a.getWealth() * 100.0
            << " cash=" << a.getCash()
            << std::endl;
}


int main(int argc, char *argv[])
{

  TimeSeries s;
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
  }

  return 0;
}

