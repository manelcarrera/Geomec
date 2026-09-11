// 
// http://en.wikipedia.org/wiki/Shunting_yard_algorithm
// http://montcs.bloomu.edu/~bobmon/Information/RPN/infix2rpn.shtml
//
// See Diana mantis 2433 for remarks about CShunt and how it is used.
//
#include <cassert>
#include <string>
#include <queue>
#include <stack>
#include <iostream>
#include "shunt.h"
#include "Rpntoken.h"

namespace Shunt
{

CShunt::CShunt()
: m_pos(0)
, m_expressionCntr(0)
{;}

CShunt::~CShunt()
{
}

bool CShunt::convertInfixToPostfix
  (std::list<CRpnToken *> &tokens, std::queue<CShunt::Op_t> &output)
{
  m_expressionCntr= 0;// after conversion we should have ONE expression

  if ( setUnaryOperators(tokens) == false ) return false;
  if ( checkFunctions( tokens) == false ) return false;

  //For Geomec RPN calculator
  if ( checkUnaryOperators(tokens) == false ) return false;

  // While there are tokens to be read:
  std::list<CRpnToken *>::const_iterator it;

  m_pos= 0;
  for ( it= tokens.begin(); it != tokens.end(); ++it )
  {
    ++m_pos;
    //operator and operand pointers
    CRpnOperator *optor= NULL;
    CRpnOperand *opand= NULL;

    // Read a token.
    CRpnToken *token= *it;

    if ( ( optor= dynamic_cast<CRpnOperator *>( token) ) == NULL &&
         ( opand= dynamic_cast<CRpnOperand *>(  token) ) == NULL
       )
    {
      assert(0); // If this happens, we have some programming error.
      setMessage( tokens, m_pos, "Sorry, I do not understand what this token is.");
      return false;
    }

    // If the token is a number, then add it to the output queue.
    if ( opand )
    {
      addToOutput(  opand, output);
    }

    // If the token is a function token, then push it onto the stack.
    else if ( optor && optor->getOperator()->isFunction() )
    {
      m_stack.push( optor);
      optor= NULL;
    }

    // If the token is a function argument separator (e.g., a comma):
    else if ( optor && optor->getOperator()->isFunctionSeparator() )
    {
      // Until the topmost element of the stack is a left parenthesis,
      // pop the element onto the output queue.
      while ( m_stack.size() > 0 &&
              m_stack.top()->getOperator()->getName() != "(" )
      {
        addToOutput(  m_stack.top(), output);
        m_stack.pop();
      }

      if ( m_stack.size() == 0 ) 
      {
        // If no left parentheses are encountered,
        // either the separator was misplaced or parentheses were mismatched.
        setMessage( tokens, m_pos, "Please add a '('.");
        return false;
      }
    }

    // If the token is a left parenthesis, then push it onto the stack.
    else if ( optor && optor->getOperator()->getName() == "(" )
    {
      m_stack.push( optor);
      optor= NULL;
    }

    // If the token is a right parenthesis:
    else if ( optor && optor->getOperator()->getName() == ")" )
    {
      // Until the token at the top of the stack is a left parenthesis,
      // pop operators off the stack onto the output queue.
      while ( m_stack.size() > 0 &&
              m_stack.top()->getOperator()->getName() != "(" )
      {
        addToOutput(  m_stack.top(), output);
        m_stack.pop();
      }

      // If the stack runs out without finding a left parenthesis,
      // then there are mismatched parentheses.
      if ( m_stack.size() <= 0 )
      {
        setMessage( tokens, m_pos, "Please add a '('.");
        return false;
      }

      // Pop the left parenthesis from the stack, but not onto the output queue.
      m_stack.pop();

      // If the token at the top of the stack is a function token,
      // pop it and onto the output queue.
      if ( m_stack.size() > 0  && m_stack.top()->getOperator()->isFunction() )
      {
        addToOutput(  m_stack.top(), output);
        m_stack.pop();
      }
    }

    // If the token is an operator, o1, then:
    else if ( optor )
    {
      // while there is an operator, o2, at the top of the stack, and either
      while ( m_stack.size() > 0 &&
              (
                (
                  // o1 is associative or left-associative
                  optor->getOperator()->associativity() ==
                  CRpnOperators::CRpnOperatorType::ASSOC_LEFT &&
                  // and its precedence is less than (lower precedence)
                  // or equal to that of o2,
                  optor->getOperator()->precedence() <=
                  m_stack.top()->getOperator()->precedence()
                ) ||
                (
                  // or o1 is right-associative
                  optor->getOperator()->associativity() ==
                  CRpnOperators::CRpnOperatorType::ASSOC_RIGHT &&
                  // and its precedence is less than (lower precedence)
                  // that of o2,
                  optor->getOperator()->precedence() <
                  m_stack.top()->getOperator()->precedence()
                )
              )
            )
      {
        // pop o2 off the stack, onto the output queue;
        addToOutput(  m_stack.top(), output);
        m_stack.pop();
      }

      // push o1 onto the operator stack.
      m_stack.push( optor);
      optor= NULL;
    }
  } // end While there are tokens to be read.

  // When there are no more tokens to read:
  // While there are still operator tokens in the stack:
  while ( m_stack.size() > 0 )
  {
    // If the operator token on the top of the stack is a parenthesis,
    // then there are mismatched parenthesis.
    if ( m_stack.top()->getOperator()->getName() == "(" )
    {
      setMessage( tokens, m_pos, "Please balance the number of '(' with the number of ')'.");
      return false;
    }

    // Pop the operator onto the output queue.
    addToOutput(  m_stack.top(), output);
    m_stack.pop();
  }

  if ( m_expressionCntr < 1 ) 
  {
    setMessage( tokens, m_pos, "Please enter a number, variable or function to this formula.");
    return false;
  }
  else if ( m_expressionCntr > 1 ) 
  {
    setMessage( tokens, m_pos, "Please enter an operator like +, -, /, *, etc. to this formula.");
    return false;
  }
  // Exit.
  return true;
}

bool CShunt:: validFloatingPoint(const std::string & number)
{
  // Check if string converts to a valid floating point number
  // http://www.parashift.com/c++-faq-lite/misc-technical-issues.html#faq-39.2
  //
   std::istringstream i(number);
   double x;
   char c;
   if ( !(i >> x) ||  i.get(c) ) return false;
   return true;
}

bool CShunt::checkUnaryOperators( std::list<CRpnToken *> & tokens)
{
  // Check if a unary operator is followed by a numerical operand
  // The Geomec RPN calculator requires this for the - operator.
  //
  std::list<CRpnToken *>::iterator it;

  m_pos= 0;
  for ( it= tokens.begin(); it != tokens.end(); ++it )
  {
    ++m_pos;
    CRpnOperator *optor= dynamic_cast<CRpnOperator *>( *it);
    if ( optor == NULL ) continue; // not an operator

    if ( optor->getOperator()->isUnary() )
    {
      ++it;
      if (it != tokens.end())
      {
        CRpnOperand *opand= dynamic_cast<CRpnOperand *>( *it);
        if ( ! opand || ! validFloatingPoint( opand->getValue() ) )
        {
          setMessage( tokens, m_pos, "Please only use minus on numbers.");
          return false;
        }
      }
      else
      {
        setMessage( tokens, m_pos, "Please add a number after the minus operator.");
        return false;
      }
    }
  }
  return true;
}

bool CShunt::setUnaryOperators( std::list<CRpnToken *> & tokens)
{
  // Check if an operator must be an unary operator and replace it
  // if it exists. 
  //
  std::list<CRpnToken *>::iterator it;

  m_pos= 0;
  for ( it= tokens.begin(); it != tokens.end(); ++it )
  {
    ++m_pos;
    CRpnOperator *optor= dynamic_cast<CRpnOperator *>( *it);
    if ( optor == NULL ) continue; // not an operator

    if ( optor->getOperator()->isUnary() ) continue;
    if ( optor->getOperator()->isFunction() ) continue;
    if ( optor->getOperator()->getName() == "(") continue;
    if ( optor->getOperator()->getName() == ")") continue;

    CRpnOperator *prevOptor= NULL;
    if ( it != tokens.begin() )
    {
      std::list<CRpnToken *>::iterator itPrev= it; --itPrev;
      prevOptor= dynamic_cast<CRpnOperator *>( *itPrev);
    }

    bool mustBeUnary= false;

    if ( it == tokens.begin() )
      mustBeUnary= true;
    else if ( prevOptor != NULL && prevOptor->getOperator()->getName() != ")" )
      mustBeUnary= true;

    if ( mustBeUnary )
    {
      CRpnOperator unaryOptor( optor->getOperator()->getName(), true);

      if ( unaryOptor.getOperator() != NULL ) //valid
      {
        delete *it;
        *it= new CRpnOperator( unaryOptor);
      }
      else
      {
        std::string message=
          "Please input a number, variable or function before'"
          + optor->getOperator()->getName()
          + "'.";
        setMessage( tokens, m_pos, message);
        return false;
      }
    }
  } // for all tokens
  return true;
}

bool CShunt::checkFunctions( const std::list<CRpnToken *> & tokens)
{
  std::list<CRpnToken *>::const_iterator it;

  m_pos= 0;
  for ( it= tokens.begin(); it != tokens.end(); ++it )
  {
    ++m_pos;
    CRpnOperator *optor= dynamic_cast<CRpnOperator *>( *it);

    if ( optor && optor->getOperator()->isFunction() )
    {
      std::list<CRpnToken *>::const_iterator it2= it;
      if ( optor->getOperator()->getName() == "if" )
      {
        if ( checkIfThenElse( tokens, it2) == false )
          return false;
      }
      else
      {
        if ( checkFunctionParentheses( tokens, it2) == false )
          return false;
      }
    }
  }

  m_pos= 0;
  for ( it= tokens.begin(); it != tokens.end(); ++it )
  {
    ++m_pos;
    CRpnOperator *optor= dynamic_cast<CRpnOperator *>( *it);

    if ( optor && optor->getIfThenElseOk() == false )
    {
      setMessage( tokens, m_pos, "Please make sure your if-statement looks like this: 'if(A) then(B) else(C)'.");
      return false;
    }
  }
  return true;
}

bool CShunt::checkIfThenElse
  ( const std::list<CRpnToken *> & tokens
  , std::list<CRpnToken *>::const_iterator it
  )
{
  CRpnOperator *optor= NULL;

  if ( it == tokens.end() ||
       (optor= dynamic_cast<CRpnOperator *>( *it))==NULL ||
       optor->getOperator()->getName() != "if" )
  {
    setMessage( tokens, m_pos, "Please make sure your if-statement looks like this: 'if(A) then(B) else(C)'.");
    return false;
  }
  if ( checkFunctionParentheses( tokens, it) == false ) return false;
  optor->setIfThenElseOk();

  ++it;
  if ( it == tokens.end() ||
       (optor= dynamic_cast<CRpnOperator *>( *it))==NULL ||
       optor->getOperator()->getName() != "then" )
  {
    setMessage( tokens, m_pos, "Please make sure your if-statement looks like this: 'if(A) then(B) else(C)'.");
    return false;
  }
  if ( checkFunctionParentheses( tokens, it) == false ) return false;
  optor->setIfThenElseOk();

  ++it;
  if ( it == tokens.end() ||
       (optor= dynamic_cast<CRpnOperator *>( *it))==NULL ||
       optor->getOperator()->getName() != "else" )
  {
    setMessage( tokens, m_pos, "Please make sure your if-statement looks like this: 'if(A) then(B) else(C)'.");
    return false;
  }
  if ( checkFunctionParentheses( tokens, it) == false ) return false;
  optor->setIfThenElseOk();

  return true;
}

// 'it' must point to a function. On successful return 'it' points to
// the closing parenthesis. abs( sdfsf )
//
bool CShunt::checkFunctionParentheses
  ( const std::list<CRpnToken *> & tokens
  , std::list<CRpnToken *>::const_iterator &it
  )
{
  int parLevel=0;
  int argCnt= 0;
  bool foundArgument= false;

  if ( it == tokens.end() ) return false;
  CRpnOperator *function= dynamic_cast<CRpnOperator *>( *it);
  if ( function == NULL ) return false;
  if ( function->getOperator()->isFunction() == false ) return false;

  do
  {
    ++it;

    if ( it == tokens.end() ) break;

    CRpnOperator *optor= dynamic_cast<CRpnOperator *>( *it);
    if ( optor != 0 )
    {
      if ( optor->getOperator()->getName() == "(" )
      {
        ++parLevel;
      }
      else if ( optor->getOperator()->getName() == ")" )
      {
        --parLevel;

        if ( foundArgument && parLevel == 0) 
        {
          foundArgument= false;
          ++argCnt;
        }
      }
      else if ( optor->getOperator()->isFunctionSeparator() )
      {
        if ( foundArgument && parLevel == 1) 
        {
          foundArgument= false;
          ++argCnt;
        }
      }
      else
        foundArgument= true;
    }
    else foundArgument= true;
  }
  while ( parLevel != 0 );

  if ( parLevel != 0 )
  {
    std::string message=
      "Please make sure your function '"
      + function->getOperator()->getName()
      + "' has a '(' and a ')'.";
    setMessage( tokens, m_pos, message);
    return false;
  }

  if ( argCnt != function->getOperator()->getArgCnt() )
  {
    std::string message=
      "Please make sure your function '" +
      function->getOperator()->getName()
      +"' has the correct number of arguments.";
    setMessage( tokens, m_pos, message);
    return false;
  }

  return true;
}
            

void CShunt::print( const std::list<CRpnToken *> & tokens)
{
  std::list<CRpnToken *>::const_iterator it;

  for ( it= tokens.begin(); it != tokens.end(); ++it )
  {
    CRpnOperator *optor= dynamic_cast<CRpnOperator *>( *it);
    if ( optor )
    {
      if ( optor->getOperator()->isUnary() )
        std::cout << "." << optor->getOperator()->getName() << ".  ";
      else
        std::cout << "\"" << optor->getOperator()->getName() << "\"  ";
    }
    else
    {
      CRpnOperand *opand= dynamic_cast<CRpnOperand *>( *it);
      if ( opand ) 
      {
        std::cout << "'" << opand->getValue() <<"'  ";
      }
    }
  }
//  std::cout << std::endl;
}

void CShunt::addToOutput(CRpnOperator *optor, std::queue<CShunt::Op_t> &output)
{
  Op_t op;
  op.data= optor->getOperator()->getName();
  if ( optor->getOperator()->isUnary() ) op.type= CShunt::E_UNOPER;
  else op.type= CShunt::E_BINOPER;
  output.push( op );
  addExpressionCntr( optor );

}

void CShunt::addToOutput(CRpnOperand *opand, std::queue<CShunt::Op_t> &output)
{
  Op_t op;
  op.data= opand->getValue();
  op.type= CShunt::E_OPAND;
  output.push( op );
  addExpressionCntr();
}

void CShunt::addExpressionCntr( CRpnOperator *optor )
{
  if ( optor == NULL ) return;
  if ( optor->getOperator()->isFunctionSeparator() ) return;
  if ( optor->getOperator()->getName() == "(" ) return;
  if ( optor->getOperator()->getName() == ")" ) return;

  // if(A) then(B) else(C) results in a single expression.
  // We fix the expression count by ignoring the if and then 'function'.
  //
  if (  optor->getOperator()->getName() == "if"
     || optor->getOperator()->getName() == "then" )
  {
    --m_expressionCntr;
    return;
  }
  if ( optor->getOperator()->isUnary() ) return; // count remains the same
  if ( optor->getOperator()->isFunction() ) 
  {
    // the function arguments are expressions. These are "eaten up" by the
    // function and it returns an expression. So, the expressionCntr is
    // decreased by the number of arguments, minus one.
    //
    m_expressionCntr -= optor->getOperator()->getArgCnt() - 1;
    return;
  }

  // Else, it must be a binary operator
  // example:
  // If the output contains "1 2", the expressionCntr == 2
  // If this is then followed by a "+": "1 2 +", we have a single expression
  //
  --m_expressionCntr;
}

void CShunt::setMessage
  ( const std::list<CRpnToken *> &tokens
  , int errorPos
  , const std::string & msg)
{
  std::list<CRpnToken *>::const_iterator it;

  int curPos= 0;

  //operator and operand pointers
  CRpnOperator *optor= NULL;
  CRpnOperand *opand= NULL;

  m_message= "";
  for ( it= tokens.begin(); it != tokens.end(); ++it )
  {
    ++curPos;
    if ( ( optor= dynamic_cast<CRpnOperator *>( *it) ) != NULL )
    {
      m_message += optor->getOperator()->getName() + " ";
    }
    else if ( ( opand= dynamic_cast<CRpnOperand *>( *it) ) != NULL )
    {
      m_message += opand->getValue() + " ";
    }

    if ( curPos == errorPos ) break;
  }

  if ( m_message.length() > 30 )
    m_message= std::string("... ") + m_message.substr(m_message.length()-30);

  m_message = "\"" + m_message + "\": " + msg;
}

};
