#ifndef OIL_AGENT_HPP
#define OIL_AGENT_HPP

#include <iostream>

#include "time_series.hpp"
#include "trading_rule.hpp"

namespace oil {

template< typename Type > class market;

template< typename Type >
class agent
{
public:

  typedef Type value_type;
  typedef time_series< Type > series_type;
  typedef trading_rule< Type > rule_type;
  typedef market< Type > market_type;

  agent () : m_market(0) {}
  agent ( value_type cash )
   :
     m_market ( 0 ),
     m_initialCash ( cash ),
     m_currentCash ( cash ),
     m_positions ( 0 )
  {
  }
  ~agent () {}

  void init ( value_type cash, market_type &m )
  {
    m_initialCash = cash;
    m_currentCash = cash;
    m_positions = 0;
    m_market = &m;
  }

  void setTradingRule ( rule_type *r ) { m_rule = r; }

  value_type getCash() { return m_currentCash; }
  void setCash ( value_type c ) { m_currentCash = c; }
  value_type getInitialCash() { return m_initialCash; }
  bool isSolvent() { return (getWealth() > 0); }

  value_type getWealth() { return m_currentCash; }
  value_type getDemand() { return m_positions; }

  typename rule_type::BuySignal decide()
  {
    return (*m_rule)( m_market->getOilPrice() );
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
                  ( m_market->getContractSize() * m_market->getOilPrice().getVolatility() );

    return m_positions;
  }

  value_type trade ( int nFinalPositions )
  {
    value_type PnL = nFinalPositions
                 *
                 m_market->getOilPrice().getReturn() * m_market->getContractSize(); // unit return
    m_currentCash += PnL;
    m_positions = 0;
    return PnL;
  }

private:

  market_type *m_market;
  rule_type *m_rule;

  value_type m_initialCash;
  value_type m_currentCash;

  int m_positions;

};

} // namespace oil

#endif
