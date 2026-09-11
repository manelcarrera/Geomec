#ifndef RPNTOKEN_H
#define RPNTOKEN_H

#include <string>
#include <vector>
#include <list>

namespace Shunt
{
class CRpnToken;

class CRpnOperators
{
  public:
    class CRpnOperatorType
    {
      public:
        typedef enum {ASSOC_NONE=0, ASSOC_LEFT, ASSOC_RIGHT} Assoc_t;

        CRpnOperatorType( const std::string & o
                        , int p
                        , Assoc_t a
                        , int fA = 0
                        , bool fS = false
                        , bool u = false )  : m_op(o)
                                            , m_prec(p)
                                            , m_assoc(a)
                                            , m_functionArgs(fA)
                                            , m_functionSep(fS)
                                            , m_unary(u)
                                            {;}

        const std::string & getName() const { return m_op;}
        int precedence() const { return m_prec;}
        Assoc_t associativity() const { return m_assoc;}
        bool isFunction() const { return m_functionArgs!=0;}
        int getArgCnt() const { return m_functionArgs;}
        bool isFunctionSeparator() const { return m_functionSep;}
        bool isUnary() const { return m_unary;}

      private:
        std::string m_op;
        int m_prec;
        Assoc_t m_assoc;
        int m_functionArgs;
        bool m_functionSep;
        bool m_unary;
    };


    static CRpnOperators *instance();
    static void removeSpaces( std::string &s);

    bool createTokens(std::string input, std::list<CRpnToken *> &output);
    void deleteTokens(std::list<CRpnToken *> &tokens);

    CRpnOperatorType *getOperatorType(const std::string & op, bool isUnary= false) const;
    CRpnOperatorType *getOperatorType(size_t index) const;
    size_t getOperatorTypeCount() const;

  protected:
    CRpnOperators() {;}

  private:
    static CRpnOperators *m_instance;
    static std::vector<CRpnOperatorType *> m_vOperatorTypes;
    void fixDivide(std::list<CRpnToken *> &tokens);
    static size_t HasPositiveFloat( const std::string &myString);
    
};

class CRpnToken
{
  public:
    CRpnToken() {;}
    virtual ~CRpnToken() {;}
  private:
    virtual const std::string & getString() const =0; // for derivation
};

class CRpnOperand : public CRpnToken
{
  public:
    CRpnOperand( const std::string & value ) { m_value= value; }
    const std::string & getValue() { return m_value; }
  private:
    std::string m_value;
    const std::string & getString() const { return m_value;} // for derivation
};

class CRpnOperator : public CRpnToken
{
  public:
    CRpnOperator( const std::string & op, bool isUnary= false )
      :m_ifthenelse_ok(false)
      { m_pOperator= CRpnOperators::instance()->getOperatorType(op, isUnary);}
    CRpnOperator( const CRpnOperator & other )
      : m_ifthenelse_ok(other.m_ifthenelse_ok)
      , m_pOperator( other.m_pOperator) {;}
    CRpnOperators::CRpnOperatorType *getOperator()
      { return m_pOperator; }
    void setIfThenElseOk() { m_ifthenelse_ok= true; }
    bool getIfThenElseOk()
    {
      if ( m_pOperator == NULL ) return false;
      if ( m_pOperator->getName() == "IF"   ) return m_ifthenelse_ok;
      if ( m_pOperator->getName() == "THEN" ) return m_ifthenelse_ok;
      if ( m_pOperator->getName() == "ELSE" ) return m_ifthenelse_ok;
      return true;
    }
  private:
    bool m_ifthenelse_ok; //if()then()else() complete and in the right order
    CRpnOperators::CRpnOperatorType *m_pOperator;
    const std::string & getString() const { return m_pOperator->getName();} // for derivation
};


};
#endif //RPNTOKEN_H
