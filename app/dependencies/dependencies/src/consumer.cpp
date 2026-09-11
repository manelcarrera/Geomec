#include "consumer.h"

#include <QtCore/QDebug>

CConsumer::CConsumer(CThread &_thread, CDependencies &dependencies, QObject *parent)
    : QThread(parent), m_thread(_thread), m_dependencies(dependencies) {}

void CConsumer::run() {
  while (1) {
    m_thread.m_mutex.lock();

    if (!m_thread.m_cmds.size())
      m_thread.m_cmds_available.wait(&m_thread.m_mutex);
    QString cmd_to_proceess = m_thread.m_cmds.takeFirst();

    m_thread.m_mutex.unlock();

    // process
    print();
    // QThread::sleep( 1 );
    // qDebug() << "consumer: " << cmd_to_proceess;
  }
}

void CConsumer::print() {
  m_dependencies.print_results(CDependencies::PRINT_PROJECT);

  // d.print_results(	CDependencies::PRINT_NOT_FOUND );

  // d.print_results();

  qDebug().noquote() << QString("Dependent projects:");
  qDebug().noquote() << QString("===================");
  m_dependencies.new_print_results(CDependencies::CProject::ProjectsType::Dependent);

  qDebug().noquote() << QString();
  qDebug().noquote() << QString("Linked-by projects:");
  qDebug().noquote() << QString("===================");
  m_dependencies.new_print_results(CDependencies::CProject::ProjectsType::LikedBy);

  qDebug().noquote() << QString();
  qDebug().noquote() << QString("Dependencies not found:");
  qDebug().noquote() << QString("=======================");
  m_dependencies.new_print_results(CDependencies::CProject::ProjectsType::DependenciesNotFound);

  qDebug().noquote() << QString();
  qDebug().noquote() << QString("Parsing errors:");
  qDebug().noquote() << QString("===============");
  for (int i = 0; i < m_dependencies.m_parsing_errors.size(); i++)
    qDebug().noquote() << i << m_dependencies.m_parsing_errors[i];

  // qDebug().noquote() << "Elapsed time 2:" << myTimer.elapsed()/1000;*/

  // d.print_data( "GeomecModel", CDependencies::DataType::Headers );
  // qDebug() << QString("DerivedResult.h") << ":" << d.find_dependent_project( QString("DerivedResult.h") );

  // qDebug() << "number of projects:" << projects.size();
}