#ifndef OIL_TIME_SERIES_HPP
#define OIL_TIME_SERIES_HPP

#include "config.hpp"

#include <vector>

namespace oil {

template< typename Type >
class time_series
{
public:

  typedef Type value_type;

  typedef std::vector< value_type > container_type;

  typedef typename container_type::iterator iterator;
  typedef typename container_type::const_iterator const_iterator;
  typedef typename container_type::reverse_iterator reverse_iterator;
  typedef typename container_type::const_reverse_iterator const_reverse_iterator;

  time_series () {}
  time_series ( const value_type v ) { addNewVal(v); }

  iterator begin () { return m_values.begin(); }
  const_iterator begin () const { return m_values.begin(); }
  iterator end () { return m_values.end(); }
  const_iterator end () const { return m_values.end(); }

  reverse_iterator rbegin () { return m_values.rbegin(); }
  const_reverse_iterator rbegin () const { return m_values.rbegin(); }
  reverse_iterator rend () { return m_values.rend(); }
  const_reverse_iterator rend () const { return m_values.rend(); }

  const unsigned int size() const { return m_values.size(); }

  void push_back ( const value_type v ) { m_values.push_back(v); }

  value_type movingAverage ( const unsigned int n )
  {
    value_type sum ( 0 );
    unsigned int count ( 0 );

    for
    (
      reverse_iterator rit = m_values.rbegin();
      count < n && rit != m_values.rend();
      ++rit, count++
    )
    {
      sum += *rit;
    }

    return sum / count;
  }

private:

  container_type m_values;

};

} // namespace oil

#endif // OIL_TIME_SERIES_HPP

