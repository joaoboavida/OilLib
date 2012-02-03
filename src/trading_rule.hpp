#ifndef OIL_TRADING_RULE_HPP
#define OIL_TRADING_RULE_HPP

#include <iostream>

#include "time_series.hpp"

namespace oil {

template< typename Type >
class trading_rule
{
public:
  typedef Type value_type;
  typedef time_series< Type > series_type;
  virtual ~trading_rule () {}
  typedef enum { SELL = -1, REFRAIN = 0, BUY = 1 } BuySignal;
  virtual BuySignal operator() ( series_type &s ) = 0;
  static BuySignal computeSignal ( double crossover )
  {
    BuySignal sig;
    if ( crossover == 0 ) sig = REFRAIN;
    else if ( crossover > 0 ) sig = BUY;
    else if ( crossover < 0 ) sig = SELL;
    return sig;
  }
  static int translate ( const BuySignal &s ) { return s; }
private:
};



template< typename Type >
class moving_average_crossover : public trading_rule< Type >
{
public:

  typedef Type value_type;
  typedef time_series< Type > series_type;

  moving_average_crossover( int speriod, int lperiod )
    :
      m_shortPeriod(speriod),
      m_longPeriod(lperiod)
  {
  }
  virtual ~moving_average_crossover () {}
  typename trading_rule< value_type >::BuySignal operator() ( series_type &s )
  {
    value_type sewma = s.getMovingAverage(m_shortPeriod),
           lewma = s.getMovingAverage(m_longPeriod),
           crossover = sewma - lewma;

    typename trading_rule< value_type >::BuySignal sig
             (
                 trading_rule< value_type >::computeSignal ( crossover )
             );

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



template< typename Type >
class lag_crossover : public trading_rule< Type >
{
public:

  typedef Type value_type;
  typedef time_series< Type > series_type;

  lag_crossover ( int speriod, int lperiod,
                 value_type oldShort = -1.18, value_type oldLong = -1.18)
    :
      m_shortPeriod(speriod),
      m_oldShort(oldShort),
      m_longPeriod(lperiod),
      m_oldLong(oldLong)
  {
  }
  virtual ~lag_crossover () {}
  typename trading_rule< value_type >::BuySignal operator() ( series_type &s )
  {
    value_type price = s.current(),
               ws = value_type(1) / m_shortPeriod,
               sewma = (value_type(1) - ws) * m_oldShort + ws * price,
               wl = value_type(1) / m_longPeriod,
               lewma = (value_type(1) - wl) * m_oldLong + wl * price,
               crossover = sewma - lewma;

    m_oldShort = sewma;
    m_oldLong = lewma;

    typename trading_rule< value_type >::BuySignal sig
             (
                 trading_rule< value_type >::computeSignal ( crossover )
             );

    std::cout << " sewma=" << m_oldShort
              << " lewma=" << m_oldLong
              << " crossover=" << crossover
              << " signal=" << sig;

    return sig;
  }
  value_type getOldShort() { return m_oldShort; }
  value_type getOldLong() { return m_oldLong; }

private:
  int m_shortPeriod;
  value_type m_oldShort;
  int m_longPeriod;
  value_type m_oldLong;
};

} // namespace oil

#endif

