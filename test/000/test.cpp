
#include <iostream>
#include <fstream>

#include "agent.hpp"

using namespace oil;

typedef time_series< double > TimeSeries;
typedef trading_rule< double > TradingRule;
typedef moving_average_crossover< double > TradingRuleAVG;
typedef lag_crossover< double > TradingRuleLAG;
typedef agent< double > Agent;


template< typename Type >
class market
{
public:
  typedef Type value_type;
  typedef agent< value_type > agent_type;

  typedef std::vector< agent_type* > container_type;

  typedef typename container_type::iterator iterator;
  typedef typename container_type::const_iterator const_iterator;
  typedef typename container_type::reverse_iterator reverse_iterator;
  typedef typename container_type::const_reverse_iterator const_reverse_iterator;

  market ( unsigned int n ) : m_agents(n) {}

  iterator begin () { return m_agents.begin(); }
  const_iterator begin () const { return m_agents.begin(); }
  iterator end () { return m_agents.end(); }
  const_iterator end () const { return m_agents.end(); }

  reverse_iterator rbegin () { return m_agents.rbegin(); }
  const_reverse_iterator rbegin () const { return m_agents.rbegin(); }
  reverse_iterator rend () { return m_agents.rend(); }
  const_reverse_iterator rend () const { return m_agents.rend(); }

  const unsigned int size() const { return m_agents.size(); }

  value_type& back() { return m_agents.back().value; }
  const value_type& back () const { return m_agents.back().value; }

private:
  std::vector< agent_type* > m_agents;
};

typedef market< double > Market;


void blabla( Market& a )
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

