/* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#include "FvToken.h"

/*!
  Construction of token
*/
CToken::CToken(const QString &sToken) : m_sToken(sToken) {}

/*!
  Destruction of token
*/
CToken::~CToken() {}

/*!
  Returns the string of the token
*/
const QString &CToken::Token() const { return m_sToken; }

/*!
  Function to sort the tokens
*/
bool CToken::operator<(const CToken &rhs) const { return m_sToken.toUpper() < rhs.m_sToken.toUpper(); }
