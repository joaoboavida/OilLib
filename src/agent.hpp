#ifndef OIL_AGENT_HPP
#define OIL_AGENT_HPP

#include <iostream>

#include "time_series.hpp"
#include "trading_rule.hpp"

namespace oil {

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

} // namespace oil

#endif
