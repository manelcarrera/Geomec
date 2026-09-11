#include "Rpntoken.h"
#include <ctype.h>
#include <sstream>
#include <iostream>

namespace Shunt
{
CRpnOperators *CRpnOperators::m_instance= NULL;

std::vector<CRpnOperators::CRpnOperatorType *> CRpnOperators::m_vOperatorTypes;

CRpnOperators *CRpnOperators::instance()
{
  if ( m_instance == NULL )
  {
    m_instance= new CRpnOperators;
    //
    // Order is important: make sure to put 'sinh' before 'sin',
    // or 'sinh' will never get a match.
    //
    // Put binary operators above their unary counter parts (currently only minus)
    // Currently only unary operators are allowed that have a binary counterpart.
    // 'not' is a function, not an unary operator. not(TRUE) vs. not TRUE
    // This is a design ... feature!
    //
m_vOperatorTypes.push_back( new CRpnOperatorType("(",      0, CRpnOperators::CRpnOperatorType::ASSOC_NONE));
m_vOperatorTypes.push_back( new CRpnOperatorType(")",      0, CRpnOperators::CRpnOperatorType::ASSOC_NONE));
m_vOperatorTypes.push_back( new CRpnOperatorType(",",      0, CRpnOperators::CRpnOperatorType::ASSOC_NONE, 0, true));
m_vOperatorTypes.push_back( new CRpnOperatorType("arcsin", 1, CRpnOperators::CRpnOperatorType::ASSOC_NONE, 1));
m_vOperatorTypes.push_back( new CRpnOperatorType("arccos", 1, CRpnOperators::CRpnOperatorType::ASSOC_NONE, 1));
m_vOperatorTypes.push_back( new CRpnOperatorType("arctan", 1, CRpnOperators::CRpnOperatorType::ASSOC_NONE, 1));
m_vOperatorTypes.push_back( new CRpnOperatorType("round",  1, CRpnOperators::CRpnOperatorType::ASSOC_NONE, 1));
m_vOperatorTypes.push_back( new CRpnOperatorType("else",   1, CRpnOperators::CRpnOperatorType::ASSOC_NONE, 1));
m_vOperatorTypes.push_back( new CRpnOperatorType("then",   1, CRpnOperators::CRpnOperatorType::ASSOC_NONE, 1));
m_vOperatorTypes.push_back( new CRpnOperatorType("sign",   1, CRpnOperators::CRpnOperatorType::ASSOC_NONE, 1));
m_vOperatorTypes.push_back( new CRpnOperatorType("sqrt",   1, CRpnOperators::CRpnOperatorType::ASSOC_NONE, 1));
m_vOperatorTypes.push_back( new CRpnOperatorType("sinh",   1, CRpnOperators::CRpnOperatorType::ASSOC_NONE, 1));
m_vOperatorTypes.push_back( new CRpnOperatorType("cosh",   1, CRpnOperators::CRpnOperatorType::ASSOC_NONE, 1));
m_vOperatorTypes.push_back( new CRpnOperatorType("tanh",   1, CRpnOperators::CRpnOperatorType::ASSOC_NONE, 1));
m_vOperatorTypes.push_back( new CRpnOperatorType("abs",    1, CRpnOperators::CRpnOperatorType::ASSOC_NONE, 1));
m_vOperatorTypes.push_back( new CRpnOperatorType("exp",    1, CRpnOperators::CRpnOperatorType::ASSOC_NONE, 1));
m_vOperatorTypes.push_back( new CRpnOperatorType("sin",    1, CRpnOperators::CRpnOperatorType::ASSOC_NONE, 1));
m_vOperatorTypes.push_back( new CRpnOperatorType("cos",    1, CRpnOperators::CRpnOperatorType::ASSOC_NONE, 1));
m_vOperatorTypes.push_back( new CRpnOperatorType("tan",    1, CRpnOperators::CRpnOperatorType::ASSOC_NONE, 1));
m_vOperatorTypes.push_back( new CRpnOperatorType("log",    1, CRpnOperators::CRpnOperatorType::ASSOC_NONE, 1));
m_vOperatorTypes.push_back( new CRpnOperatorType("ln",     1, CRpnOperators::CRpnOperatorType::ASSOC_NONE, 1));

m_vOperatorTypes.push_back( new CRpnOperatorType("ft_m",     1, CRpnOperators::CRpnOperatorType::ASSOC_NONE, 1));
m_vOperatorTypes.push_back( new CRpnOperatorType("m_ft",     1, CRpnOperators::CRpnOperatorType::ASSOC_NONE, 1));
m_vOperatorTypes.push_back( new CRpnOperatorType("ppg_kgm3",     1, CRpnOperators::CRpnOperatorType::ASSOC_NONE, 1));
m_vOperatorTypes.push_back( new CRpnOperatorType("kgm3_ppg",     1, CRpnOperators::CRpnOperatorType::ASSOC_NONE, 1));
m_vOperatorTypes.push_back( new CRpnOperatorType("C_K",     1, CRpnOperators::CRpnOperatorType::ASSOC_NONE, 1));
m_vOperatorTypes.push_back( new CRpnOperatorType("K_C",     1, CRpnOperators::CRpnOperatorType::ASSOC_NONE, 1));
m_vOperatorTypes.push_back( new CRpnOperatorType("F_C",     1, CRpnOperators::CRpnOperatorType::ASSOC_NONE, 1));
m_vOperatorTypes.push_back( new CRpnOperatorType("C_F",     1, CRpnOperators::CRpnOperatorType::ASSOC_NONE, 1));
m_vOperatorTypes.push_back( new CRpnOperatorType("psi_MPa",     1, CRpnOperators::CRpnOperatorType::ASSOC_NONE, 1));
m_vOperatorTypes.push_back( new CRpnOperatorType("MPa_psi",     1, CRpnOperators::CRpnOperatorType::ASSOC_NONE, 1));

m_vOperatorTypes.push_back( new CRpnOperatorType("not",    9, CRpnOperators::CRpnOperatorType::ASSOC_NONE, 1));
m_vOperatorTypes.push_back( new CRpnOperatorType("and",    1, CRpnOperators::CRpnOperatorType::ASSOC_LEFT));
m_vOperatorTypes.push_back( new CRpnOperatorType("or",     1, CRpnOperators::CRpnOperatorType::ASSOC_LEFT));
m_vOperatorTypes.push_back( new CRpnOperatorType("if",     1, CRpnOperators::CRpnOperatorType::ASSOC_LEFT, 1));
m_vOperatorTypes.push_back( new CRpnOperatorType("min",    1, CRpnOperators::CRpnOperatorType::ASSOC_LEFT, 2));
m_vOperatorTypes.push_back( new CRpnOperatorType("max",    1, CRpnOperators::CRpnOperatorType::ASSOC_LEFT, 2));
m_vOperatorTypes.push_back( new CRpnOperatorType("pow",    1, CRpnOperators::CRpnOperatorType::ASSOC_LEFT, 2));
m_vOperatorTypes.push_back( new CRpnOperatorType("!=",     1, CRpnOperators::CRpnOperatorType::ASSOC_LEFT));
m_vOperatorTypes.push_back( new CRpnOperatorType("<=",     1, CRpnOperators::CRpnOperatorType::ASSOC_LEFT));
m_vOperatorTypes.push_back( new CRpnOperatorType("<",      1, CRpnOperators::CRpnOperatorType::ASSOC_LEFT));
m_vOperatorTypes.push_back( new CRpnOperatorType(">=",     1, CRpnOperators::CRpnOperatorType::ASSOC_LEFT));
m_vOperatorTypes.push_back( new CRpnOperatorType(">",      1, CRpnOperators::CRpnOperatorType::ASSOC_LEFT));
m_vOperatorTypes.push_back( new CRpnOperatorType("=",      1, CRpnOperators::CRpnOperatorType::ASSOC_LEFT));
m_vOperatorTypes.push_back( new CRpnOperatorType("+",      5, CRpnOperators::CRpnOperatorType::ASSOC_LEFT));
m_vOperatorTypes.push_back( new CRpnOperatorType("-",      5, CRpnOperators::CRpnOperatorType::ASSOC_LEFT));
m_vOperatorTypes.push_back( new CRpnOperatorType("/",      7, CRpnOperators::CRpnOperatorType::ASSOC_LEFT));
m_vOperatorTypes.push_back( new CRpnOperatorType("*",      7, CRpnOperators::CRpnOperatorType::ASSOC_LEFT));
m_vOperatorTypes.push_back( new CRpnOperatorType("-",      9, CRpnOperators::CRpnOperatorType::ASSOC_RIGHT, 0, false, true));
  }
  return m_instance;
}

/// \brief Remove spaces from the beginning and the end of the input string
void CRpnOperators::removeSpaces
( std::string &s ///< The string from which the spaces are removed.
)
{
  // remove leading and trailing spaces
  //
  while ( !s.empty() && s[0]==' ') s= s.substr(1);
  while ( !s.empty() && s[s.length()-1]==' ') s= s.substr(0, s.length()-1 );
}

/// \brief Determine if myString starts with a positive float.
/// \detailed Zero is a positive number.
/// HasPositiveFloat("1.23-aap") returns 4.
/// HasPositiveFloat("-1.23-aap") returns 0.
/// HasPositiveFloat("aap-1.23") returns 0.
///
/// \return The first position that is not part of the float or '0' (eof).
size_t CRpnOperators::HasPositiveFloat
( const std::string &myString ///< the string to be tested for the float
)
{
  std::istringstream iss(myString);
  float f= -1;
  iss >> f;
  // Check either failbit or badbit is set
  if (  f>=0 && !iss.fail() && !iss.eof() )
    return iss.tellg();
  else return 0;
}

void  CRpnOperators::deleteTokens(std::list<CRpnToken *> &tokens)
{
  while ( ! tokens.empty() ) 
  {
    delete tokens.back();
    tokens.pop_back();
  }
}

/// \brief split string 'input' into a list of parsable tokens 
/// \return true
bool CRpnOperators::createTokens
( std::string input ///< the input string
, std::list<CRpnToken *> &output ///< the list of tokens
)
{
  bool retval= true;

  size_t pos= 0; // current position in input string
  std::vector<std::string> splitString;

  // We search through the input string for an operator. If found, we store
  // everything upto the operator in 'splitString' and then the operator itself
  //
  while ( pos < input.size() )
  {
    bool b_Found= false;

    if (pos == 0 && input.at(0) == '\'')
    {
      size_t i;
      for (i = 1; i < input.size() && input.at(i) != '\''; ++i);
      if (i != input.size())
        ++i;
      std::string s = input.substr(0, i);
      removeSpaces(s);
      splitString.push_back(s);
      input = input.substr(i);
    }

    if ( pos == 0 )
    {
      size_t endOfFloat= CRpnOperators::HasPositiveFloat(input);
      if ( endOfFloat > 0 )
      {
        std::string s= input.substr(0, endOfFloat);
        removeSpaces( s);
        splitString.push_back(s);
        input= input.substr(endOfFloat);
      }
    }

    for ( size_t ii= 0; ii< getOperatorTypeCount(); ++ii)
    {
      if ( getOperatorType(ii)->isFunction() ) continue;

      size_t size= getOperatorType(ii)->getName().size();

      // do we have an operator string match?
      //
      if ( input.substr(pos, size ) ==
           getOperatorType(ii)->getName() )
      {
        // if the named operator is part of a word, skip it
        //
        if ( isalpha( input[pos] ) ) //if operator has a name "or", "and"
        {
          //do not find "or" in "por" but do find it in "( or"
          if ( pos > 0 && input[pos-1] !=' ' )
            continue;
          //do not find "or" in "ordinary" but do find it in "or 1"
          if ( pos+size+1 < input.size() &&  input[pos+size]!=' ' )
            continue;
        }

        // Store the input upto the operator
        //
        if ( pos>0 )
        {
          std::string s= input.substr(0, pos);
          removeSpaces(s);
          if (!s.empty() ) splitString.push_back(s);

          input= input.substr(pos); pos= 0;
        }

        // Store the operator
        //
        splitString.push_back( getOperatorType(ii)->getName() );
        input= input.substr(getOperatorType(ii)->getName().size());
        b_Found= true;
        pos= 0;
        break;
      }
    }
    if (! b_Found ) 
      ++pos;
  }

  // Store the input after the last operator
  //
  if ( !input.empty() )
  {
    removeSpaces(input);
    if ( !input.empty() )
    {
      splitString.push_back(input);
    }
  }

  // each string in splitString becomes a token
  // If we recognize the string as an operator, 
  // we create an operator else we create an operand.
  //
  for (size_t tokenCntr= 0; tokenCntr< splitString.size(); ++tokenCntr)
  {
    CRpnOperator optor( splitString[tokenCntr] );
    if ( optor.getOperator() != NULL )
      output.push_back( new CRpnOperator( optor) );
    else
      output.push_back( new CRpnOperand( splitString[tokenCntr] ) );
  }

  fixDivide(output);

  return retval;
}

// 'SHtot/Svtot' tokenizes into 'SHtot' '/' 'Svtot', we want 'SHtot/Svtot'. 
// 'Shtot/Svtot' too.
//
void CRpnOperators::fixDivide(std::list<CRpnToken *> &tokens)
{
  std::list<CRpnToken *>::iterator it1, it2, it3; //'SHtot' '/' 'Svtot'

  for ( it1= tokens.begin(); it1 != tokens.end(); ++it1)
  {
    it2= it1; ++it2;
    if ( it2 == tokens.end() ) break;
    it3= it2; ++it3;
    if ( it3 == tokens.end() ) break;

    CRpnOperator *optor= dynamic_cast<CRpnOperator *>( *it2);
    if ( optor == NULL ) continue; // not an operator
    if ( optor->getOperator()->getName() != "/") continue;

    CRpnOperand *opand1= dynamic_cast<CRpnOperand *>( *it1);
    if ( opand1 == NULL ) continue;
    if ( opand1->getValue() != "SHtot" &&
         opand1->getValue() != "Shtot" ) continue;

    CRpnOperand *opand3= dynamic_cast<CRpnOperand *>( *it3);
    if ( opand3 == NULL ) continue;

    // Also accept SHTot/Svtot_D1_L as a single operand
    // Basically, we accept everything for opand3, as long as
    // it starts with Svtot.
    //
    if ( opand3->getValue().find("Svtot") != 0 ) continue;

    // combine 'SHtot' '/' 'Svtot' into a single operand 'SHtot/Svtot'
    CRpnOperand *opand= new CRpnOperand(
       opand1->getValue()
     + optor->getOperator()->getName()
     + opand3->getValue());

    // insert 'SHtot/Svtot' before the 3 tokens it will replace
    //
    tokens.insert( it1, opand);
    --it1; //point it1 to the new 'SHtot/Svtot' token

    // delete tokens 'SHtot' '/' 'Svtot'
    //
    delete opand1;
    delete optor;
    delete opand3;

    it2= it1; ++it2;
    ++it3;
    tokens.erase(it2, it3); //remove list elements of 'SHtot' '/' 'Svtot'
  }
}



// Get the operator pointer based on the operator string "+"/"-", etc.
CRpnOperators::CRpnOperatorType
*CRpnOperators::getOperatorType(const std::string & op, bool isUnary) const
{
  for ( size_t ii= 0 ; ii< m_vOperatorTypes.size(); ++ii)
  {
    if ( m_vOperatorTypes[ii]->getName() == op &&
         m_vOperatorTypes[ii]->isUnary() == isUnary )
    return m_vOperatorTypes[ii] ;
  }
  return NULL;
}

// Get the operator pointer at position index.
CRpnOperators::CRpnOperatorType *CRpnOperators::getOperatorType(size_t index) const
{
  if ( index >= m_vOperatorTypes.size() ) return NULL;
  return m_vOperatorTypes[index];
}

size_t CRpnOperators::getOperatorTypeCount() const
{
  return m_vOperatorTypes.size();
}

};
