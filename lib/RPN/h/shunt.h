#ifndef SHUNT_H
#define SHUNT_H
#include "Rpntoken.h"
#include <list>
#include <queue>
#include <set>
#include <sstream>
#include <stack>
#include <string>

namespace Shunt {

class CShunt {
public:
  typedef enum { E_OPAND, E_BINOPER, E_UNOPER } Type;
  typedef struct {
    std::string data;
    Type type;
  } Op_t;

private:
  std::string m_message;
  int m_pos;            // current position on the input line
  int m_expressionCntr; // After conversion there should be 1 expression
  std::stack<CRpnOperator *> m_stack;

  void print(const std::list<CRpnToken *> &tokens);
  bool checkFunctionParentheses(const std::list<CRpnToken *> &tokens, std::list<CRpnToken *>::const_iterator &it);
  bool checkIfThenElse(const std::list<CRpnToken *> &tokens, std::list<CRpnToken *>::const_iterator it);
  void addToOutput(CRpnOperator *optor, std::queue<Op_t> &output);
  void addToOutput(CRpnOperand *opand, std::queue<Op_t> &output);
  void addExpressionCntr() { ++m_expressionCntr; };
  void addExpressionCntr(CRpnOperator *optor);
  bool setUnaryOperators(std::list<CRpnToken *> &tokens);
  bool checkUnaryOperators(std::list<CRpnToken *> &tokens);
  bool checkFunctions(const std::list<CRpnToken *> &tokens);

public:
  CShunt();
  ~CShunt();
  void setMessage(const std::list<CRpnToken *> &tokens, int pos, const std::string &msg);
  bool convertInfixToPostfix(std::list<CRpnToken *> &tokens, std::queue<Op_t> &output);
  const std::string &getMessage() const { return m_message; };
  int getTokenPosition() const { return m_pos; }
  static bool validFloatingPoint(const std::string &number);
};
} // namespace Shunt
#endif // SHUNT_H
