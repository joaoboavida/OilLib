#ifndef OIL_MARKET_HPP
#define OIL_MARKET_HPP

#include <iostream>

#include "time_series.hpp"
#include "agent.hpp"


namespace oil {

template< typename Type >
class market
{
public:
  typedef Type value_type;
  typedef agent< value_type > agent_type;
  typedef time_series< value_type > series_type;

  typedef std::vector< agent_type* > container_type;

  typedef typename container_type::iterator iterator;
  typedef typename container_type::const_iterator const_iterator;
  typedef typename container_type::reverse_iterator reverse_iterator;
  typedef typename container_type::const_reverse_iterator const_reverse_iterator;

  market ( unsigned int n ) : m_agents(n), m_alpha(0.01)
  {
    for( unsigned i = 0; i < n; ++i )
    {
      m_agents[i] = new agent_type;
    }
  }
  ~market ()
  {
    typename container_type::size_type n = m_agents.size();
    for( typename container_type::size_type i = 0; i < n; ++i )
    {
      delete m_agents[i];
      m_agents[i] = 0;
    }
  }

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

  static int getContractSize () { return m_contractSize; }
  static void setContractSize ( const int size ) { m_contractSize = size; }

  series_type& getOilPrice () { return m_oilPrice; }
  const series_type& getOilPrice () const { return m_oilPrice; }

  /** @return missing positions */
  int trade ()
  {

    std::cout << "MARKET: price=" << m_oilPrice.current()
              << " return=" << m_oilPrice.getReturn()
              << " vol=" << m_oilPrice.getVolatility()
              << " adj=" << m_oilPrice.getAdjustedReturn()
              << std::endl;

    int demand = 0, supply = 0;
    for (
          typename container_type::iterator it = m_agents.begin();
          it != m_agents.end();
          ++it
        )
    {
      agent_type& a = *(*it);
      int positions = a.computeDemand();
std::cout << std::endl;

      if(positions > 0) demand += positions;
      else if(positions < 0) supply -= positions;
    }

    int missing = demand - supply;
    double ratioDemand, ratioSupply;


    if ( demand > supply )
    {
      ratioDemand = value_type(supply) / value_type(demand);
      ratioSupply = value_type(1);
    }
    else if ( demand == supply )
    {
      ratioDemand = value_type(1);
      ratioSupply = value_type(1);
    }
    else
    {
      ratioDemand = value_type(1);
      ratioSupply = value_type(demand) / value_type(supply);
    }

    std::cout << "MARKET: demand=" << demand
              << " supply=" << supply
              << " missing=" << missing
              << " ratioDemand=" << ratioDemand
              << " ratioSupply=" << ratioSupply
              << " OLD_PRICE=" << m_oilPrice.current()
              << " NEW PRICE=" << computeNewOilPrice( missing )
              << std::endl;

    unsigned int i = 0;
    for (
          typename container_type::iterator it = m_agents.begin();
          it != m_agents.end();
          ++it, ++i
        )
    {
      std::cout << "AGENT[" << i << "]: ";
      agent_type& a = *(*it);

      int positions = a.getDemand();
      int trade = positions;
      trade *= ( positions >= 0 ) ? ratioDemand : ratioSupply;

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
    std::cout << std::endl;
    std::cout << std::endl;

    return missing;
  }

  value_type computeNewOilPrice( int missingPositions )
  {
    return m_oilPrice.current() + m_alpha * missingPositions;
  }

private:

  std::vector< agent_type* > m_agents;

  /** oil price elasticity */
  value_type m_alpha;

  series_type m_oilPrice;

  static int m_contractSize;
};

template< typename Type >
int market< Type >::m_contractSize = 1000;



} // namespace oil

#endif
