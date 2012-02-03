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

  market ( unsigned int n ) : m_agents(n)
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

  void trade ()
  {

    std::cout << "MARKET: price=" << m_oilPrice.current()
              << " return=" << m_oilPrice.getReturn()
              << " vol=" << m_oilPrice.getVolatility()
              << " adj=" << m_oilPrice.getAdjustedReturn()
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
    std::cout << std::endl;
    std::cout << std::endl;
  }

private:

  std::vector< agent_type* > m_agents;
  series_type m_oilPrice;

  static int m_contractSize;
};

template< typename Type >
int market< Type >::m_contractSize = 1000;



} // namespace oil

#endif
