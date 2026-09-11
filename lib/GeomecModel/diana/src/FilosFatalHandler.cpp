#include "FilosFatalHandler.h"

FilosFatalHandler::FilosFatalHandler() : m_strategy(new CDefaultFilosFatalStrategy()) {}

FilosFatalHandler *FilosFatalHandler::instance() {
  static FilosFatalHandler filosFatalHandler;

  return &filosFatalHandler;
}

IFilosFatalStrategy &FilosFatalHandler::Strategy() const { return *m_strategy; }

void FilosFatalHandler::Strategy(IFilosFatalStrategy *strategy) {
  if (strategy != nullptr) {
    delete m_strategy;
    m_strategy = strategy;
  }
}
