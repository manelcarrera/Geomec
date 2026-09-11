#include "mlxmlException.h"

namespace mlxml {

/*!
 * \class CException
 * \brief Exception class for the XML loading and saving functions
 * The mlxml classes throw a CException when an error occurs. A
 * description of the error is provided with the error() method.
 */

/*!
 * \brief Constructor
 * \param strError The description of the error
 */
CException::CException(const QString& strError)
: m_strError(strError)
{
}

/*!
 * \brief Get a description of the error
 * \return A description of the error
 */
const QString& CException::error() const
{
  return m_strError;
}

} // namespace mlxml
