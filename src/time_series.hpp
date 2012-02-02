#ifndef OIL_TIME_SERIES_HPP
#define OIL_TIME_SERIES_HPP

#include "config.hpp"

#include <cassert>
#include <vector>
#include <map>
#include <cmath>

namespace oil {

template< typename Type >
class time_series
{
public:

  typedef Type value_type;

  struct Entry
  {
    value_type value;
    value_type volatility;
  };

  typedef std::vector< Entry > container_type;

private:

  struct AverageTracker
  {
    unsigned int n;
    value_type avg;

    unsigned int count;
    const container_type& vals;

    AverageTracker ( const unsigned int n_, const container_type& vals_ )
      :
        n(n_), avg(0), count(0), vals(vals_)
    {
      assert ( n > 0 );
      typename container_type::size_type s ( vals.size() );
      count = ( s < n ) ? s : n;
      int stop = ( s >= 1+count ) ? s-1-count : 0;
      for ( int i = s-1; i >= stop; --i )
      {
        avg += vals[i].value;
      }
      avg /= count;
    }
    void push_back ( const value_type v )
    {
      typename container_type::size_type s ( vals.size() );
      if ( count == n )
      {
        avg = ( avg * n - vals[s-1-n].value + v ) / n;
      }
      else
      {
        avg = ( avg * count + v ) / (count+1);
        ++count;
      }
    }
  };

public:

  typedef typename container_type::iterator iterator;
  typedef typename container_type::const_iterator const_iterator;
  typedef typename container_type::reverse_iterator reverse_iterator;
  typedef typename container_type::const_reverse_iterator const_reverse_iterator;

public:

  typedef std::map< unsigned int, AverageTracker* > average_container_type;

  time_series () : m_period(22) {}

  ~time_series ()
  {
    for(
         typename average_container_type::iterator it = m_averages.begin();
         it != m_averages.end();
         ++it
       )
    {
      delete it->second;
    }
  }

  iterator begin () { return m_values.begin(); }
  const_iterator begin () const { return m_values.begin(); }
  iterator end () { return m_values.end(); }
  const_iterator end () const { return m_values.end(); }

  reverse_iterator rbegin () { return m_values.rbegin(); }
  const_reverse_iterator rbegin () const { return m_values.rbegin(); }
  reverse_iterator rend () { return m_values.rend(); }
  const_reverse_iterator rend () const { return m_values.rend(); }

  const unsigned int size() const { return m_values.size(); }

  value_type& back() { return m_values.back().value; }
  const value_type& back () const { return m_values.back().value; }

  int getPeriod () const { return m_period; }
  void setPeriod ( const int period ) { m_period = period; }

  void push_back ( const value_type v )
  {
    Entry e;
    e.value = v;
    typename container_type::size_type s = m_values.size();
    const Entry &prev = m_values[s-1];
    e.volatility = (s > 0 ) ?
                        computeVolatility(v, prev.value, prev.volatility, m_period)
                      :
                        /** @todo FIXME fix this value */
                        value_type(0.3440106587);
    m_values.push_back(e);
    for(
         typename average_container_type::iterator it = m_averages.begin();
         it != m_averages.end();
         ++it
       )
    {
      it->second->push_back (v);
    }
  }

  value_type getMovingAverage ( const unsigned int n )
  {
    typename average_container_type::iterator it ( m_averages.find ( n ) );

    if ( it != m_averages.end() )
    {
      return it->second->avg;
    }
    else
    {
      AverageTracker *ptr = new AverageTracker(n, m_values);
      m_averages[n] = ptr;
      return ptr->avg;
    }
  }

  value_type getMovingAverage ( const unsigned int n, const iterator &it )
  {
    value_type avg ( 0 );
    int count ( 0 );
    for (
          typename container_type::iterator ita ( it );
          count < n && count < size();
          --ita
        )
    {
      avg += ita->value;
      ++count;
    }
    avg /= count;

    return avg;
  }

  value_type getReturn ( iterator &it )
  {
    if(it != m_values.begin())
    {
      iterator prev = it;
      --prev;
      return it->value - prev->value;
    }
    else return value_type(0);
  }

  value_type getLogReturn ( iterator &it )
  {
    if(it != m_values.begin())
    {
      iterator prev = it;
      --prev;
      return log(it->value) - log(prev->value);
    }
    else return value_type(0);
  }

  value_type getAdjustedReturn ( iterator &it )
  {
    return getReturn ( it ) / getVolatility ( it );
  }

  value_type getVolatility ( iterator &it )
  {
    return it->volatility;
  }

private:

  container_type m_values;
  average_container_type m_averages;
  int m_period;

  value_type computeVolatility ( const value_type pn, const value_type pnm1,
                                 const value_type volm1, const int period )
  {
    value_type w_tn = value_type(1.0) / period,
               ret = pn - pnm1;
    return sqrt (
                  (value_type(1.0) - w_tn) * volm1 * volm1
                 +
                  w_tn * ret * ret
                );
  }


};

} // namespace oil

#endif // OIL_TIME_SERIES_HPP

