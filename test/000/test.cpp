
#include <iostream>
#include <fstream>

#include "time_series.hpp"

using namespace oil;

typedef time_series< double > TimeSeries;

class trading_rule
{
public:
  virtual ~trading_rule () {}
  typedef enum { SELL = -1, REFRAIN = 0, BUY = 1 } BuySignal;
  virtual BuySignal operator() ( TimeSeries &s ) = 0;
  static int translate ( const BuySignal &s ) { return s; }
private:
};


class moving_average_crossover : public trading_rule
{
public:
  moving_average_crossover( int speriod, int lperiod )
    :
      m_shortPeriod(speriod),
      m_longPeriod(lperiod)
  {
  }
  virtual ~moving_average_crossover () {}
  trading_rule::BuySignal operator() ( TimeSeries &s )
  {
    double sewma = s.getMovingAverage(m_shortPeriod),
           lewma = s.getMovingAverage(m_longPeriod),
           crossover = sewma - lewma;

    trading_rule::BuySignal sig;
    if ( crossover == 0 ) sig = REFRAIN;
    else if ( crossover > 0 ) sig = BUY;
    else if ( crossover < 0 ) sig = SELL;

    std::cout << " sewma=" << sewma
              << " lewma=" << lewma
              << " crossover=" << crossover
              << " signal=" << sig;

    return sig;
  }
private:
  int m_shortPeriod;
  int m_longPeriod;
};

class lag_crossover : public trading_rule
{
public:
  lag_crossover ( int speriod, int lperiod,
                 double oldShort = -1.18, double oldLong = -1.18)
    :
      m_shortPeriod(speriod),
      m_oldShort(oldShort),
      m_longPeriod(lperiod),
      m_oldLong(oldLong)
  {
  }
  virtual ~lag_crossover () {}
  trading_rule::BuySignal operator() ( TimeSeries &s )
  {
    double price = s.current();

    double ws = double(1) / m_shortPeriod;
    double sewma = (double(1) - ws) * m_oldShort + ws * price;
    double wl = double(1) / m_longPeriod;
    double lewma = (double(1) - wl) * m_oldLong + wl * price;

    m_oldShort = sewma;
    m_oldLong = lewma;
    double crossover = sewma - lewma;

    trading_rule::BuySignal sig;
    if ( crossover == 0 ) sig = REFRAIN;
    else if ( crossover > 0 ) sig = BUY;
    else if ( crossover < 0 ) sig = SELL;

    std::cout << " sewma=" << m_oldShort
              << " lewma=" << m_oldLong
              << " crossover=" << crossover
              << " signal=" << sig;

    return sig;
  }
  double getOldShort() { return m_oldShort; }
  double getOldLong() { return m_oldLong; }

private:
  int m_shortPeriod;
  double m_oldShort;
  int m_longPeriod;
  double m_oldLong;
};


class agent
{
public:

  agent () {}
  agent ( double cash )
   :
     m_initialCash ( cash ),
     m_currentCash ( cash ),
     m_positions ( 0 )
  {
  }
  ~agent () {}

  void init ( double cash )
  {
    m_initialCash = cash;
    m_currentCash = cash;
    m_positions = 0;
  }

  void setOilPrice ( TimeSeries &s ) { m_oilPrice = &s; }
  void setTradingRule ( trading_rule *r ) { m_rule = r; }
  static void setContractSize ( const double size ) { m_contractSize = size; }

  double getCash() { return m_currentCash; }
  void setCash ( double c ) { m_currentCash = c; }
  double getInitialCash() { return m_initialCash; }
  bool isSolvent() { return (getWealth() > 0); }

  double getWealth() { return m_currentCash; }
  double getDemand() { return m_positions; }

  trading_rule::BuySignal decide()
  {
    return (*m_rule)( *m_oilPrice );
  }

  int computeDemand ()
  {
    // analyze the series - compute the signal
    trading_rule::BuySignal sig ( decide() );

    // translate the signal to an int
    int sigval = trading_rule::translate( sig ); // -1, 0, 1

    // compute wanted position - absolute
    m_positions = ( sigval * m_currentCash )
                  /
                  ( m_contractSize * m_oilPrice->getVolatility() );

    return m_positions;
  }

  double trade ( int nFinalPositions )
  {
    double PnL = nFinalPositions
                 *
                 m_oilPrice->getReturn() * m_contractSize; // unit return
    m_currentCash += PnL;
    m_positions = 0;
    return PnL;
  }

private:

  TimeSeries *m_oilPrice;
  trading_rule *m_rule;

  double m_initialCash;
  double m_currentCash;

  static int m_contractSize;

  int m_positions;

};

int agent::m_contractSize = 1000;






void blabla( agent& a )
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

  moving_average_crossover ruleAvg(2, 8);
  lag_crossover ruleLag(2, 8);

  agent firstAgent, secondAgent;

  agent::setContractSize(1000);

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

