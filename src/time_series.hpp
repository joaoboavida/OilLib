#ifndef OIL_TIME_SERIES_HPP
#define OIL_TIME_SERIES_HPP

#include "config.hpp"

#include <cassert>
#include <vector>
#include <map>

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

private:

  struct average_tracker
  {
    unsigned int n;
    value_type avg;

    unsigned int count;
    const container_type& vals;

    average_tracker ( const unsigned int n_, const container_type& vals_ )
      :
        n(n_), avg(0), count(0), vals(vals_)
    {
      assert ( n > 0 );
      typename container_type::size_type s ( vals.size() );
      count = ( s < n ) ? s : n;
      int stop = ( s >= 1+count ) ? s-1-count : 0;
      for ( int i = s-1; i >= stop; --i )
      {
        avg += vals[i];
      }
      avg /= count;
    }
    void push_back ( const value_type v )
    {
      typename container_type::size_type s ( vals.size() );
      if ( count == n )
      {
        avg = ( avg * n - vals[s-1-n] + v ) / n;
      }
      else
      {
        avg = ( avg * count + v ) / (count+1);
        ++count;
      }
    }
  };

public:

  typedef std::map< unsigned int, average_tracker* > average_container_type;

  time_series () {}
  time_series ( const value_type v ) { addNewVal(v); }

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

  value_type& back() { return m_values.back(); }
  const value_type& back () const { return m_values.back(); }

  void push_back ( const value_type v )
  {
    m_values.push_back(v);
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
      average_tracker *ptr = new average_tracker(n, m_values);
      m_averages[n] = ptr;
      return ptr->avg;
    }
  }

private:

  container_type m_values;
  average_container_type m_averages;

};

} // namespace oil

#endif // OIL_TIME_SERIES_HPP

