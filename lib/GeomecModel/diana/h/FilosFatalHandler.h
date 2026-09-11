#pragma once

class IFilosFatalStrategy {
public:
  IFilosFatalStrategy() {}
  virtual ~IFilosFatalStrategy() {}

  virtual void Execute(const char *) = 0;
};

class CDefaultFilosFatalStrategy : public IFilosFatalStrategy {
public:
  CDefaultFilosFatalStrategy() {}
  virtual ~CDefaultFilosFatalStrategy() {}

  virtual void Execute(const char *) {}
};

class FilosFatalHandler {
public:
  static FilosFatalHandler *instance();

  IFilosFatalStrategy &Strategy() const;
  void Strategy(IFilosFatalStrategy *strategy);

private:
  FilosFatalHandler();

  IFilosFatalStrategy *m_strategy;
};
