#ifndef OIL_CURRENCY_HPP
#define OIL_CURRENCY_HPP

#include "config.hpp"

namespace oil {

template< typename Type >
class currency
{
public:

  typedef Type value_type;

  currency () {}
  explicit currency ( const char *name ) : m_name(name) {}
  explicit currency ( const std::string &name ) : m_name(name) {}

  void setName ( const char *name ) { m_name.assign(name); }
  void setName ( const std::string &name ) { m_name.assign(name); }
  const std::string& getName() const { return m_name; }

  /** @return this->m_ref/c.m_ref */
  value_type getExchangeRatio ( const currency& c ) { return m_ref / c.m_ref; }

private:

  std::string m_name;
  value_type m_ref;

};

} // namespace oil

#endif // OIL_CURRENCY_HPP

