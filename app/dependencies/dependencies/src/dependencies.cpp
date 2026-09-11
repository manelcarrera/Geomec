#include "stdafx.h"

#include "dependencies.h"

#include <QtCore/QSet>
#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtCore/QDirIterator>
#include <QtCore/QStringList>

#include <QtWidgets/QApplication>

//#include <QtCore/QThread>
//#include <QtCore/QByteArray>



#include <iostream>

#include <algorithm>


const int CDependencies::PRINT_PROJECT		= 0x00000100; 
const int CDependencies::PRINT_LIB			= 0x00000001; 
const int CDependencies::PRINT_NOT_FOUND	= 0x00000010; 
const int CDependencies::PRINT_ALL			= CDependencies::PRINT_PROJECT | CDependencies::PRINT_LIB | CDependencies::PRINT_NOT_FOUND; 


static const QString ProjectNotFound = "not_found";

static const QString OIV_ProjectName = "OIV-E";
static const QString OIV_IncludePath = "E:\\Work\\OIV\\include";

static const QString QT_ProjectName = "QT-E";
static const QString QT_IncludePath = "E:\\Work\\VS2013\\Qt5.6.1\\5.6\\msvc2013_64\\include";

static const QString QWT_ProjectName = "QWT-E";
static const QString QWT_IncludePath = "E:\\Work\\VS2013\\qwt-6.1.3\\src";

static const QString MFC_ProjectName = "MFC-E";
static const QString MFC_IncludePath = "C:\\Program Files (x86)\\Microsoft Visual Studio 12.0\\VC\\atlmfc\\include";

static const QString VC_ProjectName = "VC-E";
static const QString VC_IncludePath = "C:\\Program Files (x86)\\Microsoft Visual Studio 12.0\\VC\\include";

static const QString GTEST_ProjectName = "GTEST-E";
static const QString GTEST_IncludePath = "C:\\dev\\Geomec\\trunk\\lib\\gmock-1.7.0\\gtest\\include\\gtest";

static const QString DS_MODEL_ProjectName = "DSealectModel-E";
static const QString DS_MODEL_IncludePath = "C:\\dev\\Geomec\\trunk\\DSealect\\Model\\src";

static const QString DS_SPREAD_ProjectName = "DSealectSpreadsheet-E";
static const QString DS_SPREAD_IncludePath = "C:\\dev\\Geomec\\trunk\\DSealect\\spreadsheet";

static const QString DIANA_ProjectName = "Diana-E";
static const QString DIANA_IncludePath = "C:\\dev\\diana\\diana";

static const QString BLOSC_ProjectName = "Blosc-E";
static const QString BLOSC_IncludePath = "C:\\dev\\Geomec\\trunk\\lib\\c-blosc-1.9.0";

static const QString HDF5_ProjectName = "HDF5-E";
static const QString HDF5_IncludePath = "C:\\dev\\Geomec\\trunk\\lib\\hdf5";


static const QString MSSDK_ProjectName = "MS-SDK-E"; 
static const QString MSSDK_IncludePath = "C:\\Program Files (x86)\\Microsoft SDKs\\Windows\\v7.1A\\Include"; //'windows.h'


static const QString ProjectSuffix = "vcxproj";


static const int DefaultPaddingLength = 20;

//----------------------------------------------------- Public methods -----------------------------------------------------


CDependencies::CDependencies(){}
CDependencies::~CDependencies(){}

void CDependencies::run( const QString& path, const QList< QString >& exclude_projects )
{
	//QObject::connect( &m_tcp_server, SIGNAL( newConnection() ), this, SLOT( accept_connection() ) );

	find_projects( path, exclude_projects ); 
	add_external_projects();

	find_source_files();
	find_dependencies();

	find_dependent_projects();
	
	set_linked_by();
}


/*void CDependencies::listen()
{
    while( !m_tcp_server.isListening() && !m_tcp_server.listen() ) 
		QThread::sleep( 1 );
}

void CDependencies::accept_connection()
{
    m_tcp_server_connection = m_tcp_server.nextPendingConnection();

    QObject::connect( m_tcp_server_connection, SIGNAL( readyRead() ),							this, SLOT( handle_command() ) );

	//FIXME:
    //connect( m_tcpServerConnection, SIGNAL( error(QAbstractSocket::SocketError)),	this, SLOT( displayError( QAbstractSocket::SocketError ) ) );

    m_tcp_server.close();
}

void CDependencies::handle_command()
{
    //int bytesReceived += (int)m_tcp_server_connection->bytesAvailable();
 
	QString data = QString( m_tcp_server_connection->readAll() ); //QByteArray

	print_results(	CDependencies::PRINT_PROJECT ); //FIXME

	m_tcp_server_connection->close(); //FIXME: Mirar si comando viene troceado

    //if( bytesReceived == TotalBytes ) 
	//{
    //    m_tcp_server_connection->close();
    //}
}*/


void CDependencies::clean()
{
	m_projects.clear();
}

void CDependencies::print_data( const QString& project_name, CDependencies::DataType data_type )
{
	CProject* project = &m_projects[ project_name ];

	switch( data_type)
	{
		case CDependencies::DataType::Dependencies:
		{
			qDebug().noquote() << project->dependencies;
			break;
		}
		case CDependencies::DataType::Headers:
		{
			QList< QString > names_headers;
			for( int i = 0; i < project->headers.size(); i++ )
				names_headers << project->headers[ i ].fileName();
			qDebug().noquote() << names_headers;
			break;
		}
		case CDependencies::DataType::DependentProjects:
		{
			qDebug().noquote() << project->dependent_projects;
			break;
		}
		case CDependencies::DataType::DependenciesnNotFound:
		{
			QList< QString > names_dependencies_not_found;
			for( int i = 0; i < project->dependencies_not_found.size(); i++ )
				names_dependencies_not_found << project->dependencies[ project->dependencies_not_found[ i ] ];
			qDebug().noquote() << names_dependencies_not_found;
			break;
		}
		default:
			break;
	}
}

void CDependencies::add_external_projects()
{
	m_projects[ OIV_ProjectName ]		= CProject( OIV_ProjectName,		OIV_IncludePath,	true ); 
	m_projects[ QT_ProjectName ]		= CProject( QT_ProjectName,			QT_IncludePath,		true );
	m_projects[ QWT_ProjectName ]		= CProject( QWT_ProjectName,		QWT_IncludePath,	true );
	m_projects[ MFC_ProjectName ]		= CProject( MFC_ProjectName,		MFC_IncludePath,	true );	
	m_projects[ VC_ProjectName ]		= CProject( VC_ProjectName,			VC_IncludePath,		true );
	m_projects[ GTEST_ProjectName ]		= CProject( GTEST_ProjectName,		GTEST_IncludePath,	true );
	m_projects[ DIANA_ProjectName ]		= CProject( DIANA_ProjectName,		DIANA_IncludePath,	true );
	m_projects[ BLOSC_ProjectName ]		= CProject( BLOSC_ProjectName,		BLOSC_IncludePath,	true );
	m_projects[ HDF5_ProjectName ]		= CProject( HDF5_ProjectName,		HDF5_IncludePath,	true );
	m_projects[ MSSDK_ProjectName ]		= CProject( MSSDK_ProjectName,		MSSDK_IncludePath,	true );

	//project discontinued
	m_projects[ DS_MODEL_ProjectName ]	= CProject( DS_MODEL_ProjectName,	DS_MODEL_IncludePath,	true );
	m_projects[ DS_SPREAD_ProjectName ]	= CProject( DS_SPREAD_ProjectName,	DS_SPREAD_IncludePath,	true );
}

QList< QPair< QString, int > > CDependencies::get_number_of_projects( CDependencies::CProject::ProjectsType _type )
{
	QList< QPair< QString, int > > number_of_projects;

	QMapIterator< QString, CProject > i( m_projects ); //for each project
	while( i.hasNext() ) 
	{
		i.next();
		const CProject* project = &( i.value() );

		int size =	_type == CDependencies::CProject::ProjectsType::Dependent	?	project->dependent_projects.size() : 
					_type == CDependencies::CProject::ProjectsType::LikedBy		?	project->linked_by.size() :
																					project->dependencies_not_found.size();

		if( size )
			number_of_projects << QPair< QString, int >( project->name, size );
	}
	return number_of_projects;
}

int CDependencies::get_max( const QList< QPair< QString, int > >& list )
{
	int _max = 0;
	for( int i=0; i < list.size(); i++ )
		_max = list[ i ].second > _max ? list[ i ].second : _max;
	return _max;
}


void CDependencies::new_print_results( CDependencies::CProject::ProjectsType _type )
{
	QMapIterator< QString, CProject > i( m_projects ); //for each project

	QList< QString > lines;

	QList< QPair< QString, int > > number_of_projects = get_number_of_projects( _type );
	int max_number_of_projects = get_max( number_of_projects );

	QString line, line2;
	for( int j=0; j < number_of_projects.size(); j++ ) //header
	{
		QString project_name = number_of_projects[ j ].first;
		line += project_name.left( 14 ).leftJustified(	14+1 /*DefaultPaddingLength*/,' ');	// 16: estimated max project name length 
		
		line2 += QString( project_name.left( 14 ).size(), QChar('-') ).leftJustified(	14+1 /*DefaultPaddingLength*/,' ');
																						// FIXME: to calculate real max length 
	}
	lines << line << line2;//QString();
			
	for( int i=0; i < max_number_of_projects; i++ ) //dependencies position of whatever project : y coordenate
	{
		line = QString();
		QString project;
		for( int j=0; j < number_of_projects.size(); j++ ) //for each project : x coordenate
		{
			//x
			QString project_name	= number_of_projects[ j ].first;
			int _number_of_projects	= number_of_projects[ j ].second;

			//y
			CProject* p = &m_projects[ project_name ];
			if( i < _number_of_projects )	
				project =	_type == CDependencies::CProject::ProjectsType::Dependent ?	p->dependent_projects[ i ] : 
							_type == CDependencies::CProject::ProjectsType::LikedBy	?	p->linked_by[ i ] : 
																						p->dependencies[ p->dependencies_not_found[ i ] ];
			else
				project = QString();

			line += project.left( 14 ).leftJustified( 14+1 /*DefaultPaddingLength*/,' '); 
		}
		lines << line;
	}

	for( int i=0; i < lines.size() ; i++ )
		qDebug().noquote() << lines[ i ];
}


void CDependencies::print_results( const int what_to_print )
{
	//QDebug debug = qDebug();
	//debug.noquote();

	qDebug() << "";

	if( (what_to_print & PRINT_PROJECT) == PRINT_PROJECT )
	{
		qDebug().noquote()	<< QString("   Name").leftJustified(	33,' ') 
							<< QString("h").rightJustified(		3,' ') 
							<< QString("d").rightJustified(		3,' ') 
							<< QString("dp").rightJustified(	3,' ') 
							<< QString("nf").rightJustified(	3,' ')
							<< QString("lb").rightJustified(	3,' ');
		qDebug() << "";
	}

	int nNotFound=0;

	QMapIterator< QString, CProject > i( m_projects );
	while( i.hasNext() )	//for each project 
							//	search dependent projects from its dependencies (external includes)
	{
		i.next();

		const CProject* project = &( i.value() );

		if( project->dependencies.size() )
		{
			if( what_to_print & PRINT_PROJECT )
			{

				if( project->dependencies_not_found.size() )
				{
					if( (what_to_print & PRINT_PROJECT) == PRINT_PROJECT ) //FIXME: not working this filter
					{
						nNotFound +=  project->dependencies_not_found.size();
						qDebug().noquote()	<< "->" 
											<<  project->name.leftJustified(											30,' ') 
											<< QString::number( project->headers.size() ).rightJustified(				3,' ') 
											<< QString::number( project->dependencies.size() ).rightJustified(			3,' ')  
											<< QString::number( project->dependent_projects.size() ).rightJustified(	3,' ')  
											<< QString::number( project->dependencies_not_found.size() ).rightJustified(3,' ')
											<< QString::number( project->linked_by.size() ).rightJustified(			3,' '); // print numer of headers and dependencies (headers) of the project
					}
					else
					{
						qDebug().noquote()	<< "->"		<<  project->name.leftJustified(											30,' ') 
											<< "h:"		<< QString::number( project->headers.size() ).rightJustified(				3,' ') 
											<< "d:"		<< QString::number( project->dependencies.size() ).rightJustified(			3,' ')  
											<< "dp:"	<< QString::number( project->dependent_projects.size() ).rightJustified(	3,' ')  
											<< "nf:"	<< QString::number( project->dependencies_not_found.size() ).rightJustified(3,' '); // print numer of headers and dependencies (headers) of the project*/
					}
				}
			}

			if( what_to_print & PRINT_LIB )
				qDebug() << "\t" << project->dependent_projects.size() << ":" << project->dependent_projects;
	
			if( what_to_print & PRINT_NOT_FOUND )
			{
				//print_data( project->name , CDependencies::DataType::DependenciesnNotFound ):

				if( project->dependencies_not_found.size() )
				{
					QList< QString > names_dependencies_not_found;
					for( int j = 0; j < project->dependencies_not_found.size(); j++ )
						names_dependencies_not_found << project->dependencies[ project->dependencies_not_found[ j ] ];
					qDebug() << "\t nf:" << project->dependencies_not_found.size() << ":" << names_dependencies_not_found;
				}
			}
			if( (what_to_print & PRINT_ALL) == PRINT_ALL )
				qDebug() << "";
		}
	}
	if( what_to_print & PRINT_PROJECT )
	{
		qDebug() << "";
		qDebug().noquote() << QString::number( nNotFound ).rightJustified( 49,' ');
	}
}

//----------------------------------------------------- Pricvate methods -----------------------------------------------------

void CDependencies::find_projects( const QString& path, const QList< QString >& exclude_projects )
{
	QDirIterator dirIt( path, QDirIterator::Subdirectories );
	while( dirIt.hasNext() ) // look for all projects in a given path
	{
		dirIt.next();

		if( QFileInfo( dirIt.filePath() ).isFile() )
		{
			QFileInfo info = QFileInfo( dirIt.filePath() );

			if( info.suffix() == ProjectSuffix )
			{
				bool excluded = false;
				QString project_name = info.baseName();
				for( int i=0; i < exclude_projects.size(); i++ )
				{
					if( ! QString::compare( project_name, exclude_projects[  i ], Qt::CaseInsensitive ) )
					{
						excluded = true;
						break;
					}
				}
				if( !excluded )
				{
					CProject p;
					p.path = info.absolutePath();
					p.name = info.baseName();

					m_projects[ p.name ] = p;
				}
			}
		}
	}
}

void CDependencies::find_source_files()
{
	QMutableMapIterator< QString, CProject > i( m_projects );
	while( i.hasNext() ) //for each projects find includes and external dependencies
	{
		i.next();

		CProject* project = &( i.value() );

		project->headers = get_files( project->path, QList< QString >() << "h" );  //QList< QFileInfo >
		project->sources = get_files( project->path, QList< QString >() << "cpp" << "c" );  //QList< QFileInfo >
	}
}


void CDependencies::find_dependencies()
{
	QMutableMapIterator< QString, CProject > i( m_projects );
	while( i.hasNext() ) //for each projects find includes and external dependencies
	{
		i.next();

		CProject* project = &( i.value() );

		if( !project->is_external ) // not to find dependencies of external projects
		{
			for( int j = 0; j < project->headers.size(); ++j ) //headers
			{
				QList< QString > dependencies = get_dependencies( project->headers[ j ].filePath(), project->name );

				if( dependencies.size () )
					project->dependencies.append( dependencies );  // set project dependencies 
			}
			for( int j = 0; j < project->sources.size(); ++j ) //source files
			{
				QList< QString > dependencies = get_dependencies( project->sources[ j ].filePath(), project->name );

				if( dependencies.size () )
					project->dependencies.append( dependencies );  // set project dependencies 
			}
			project->dependencies = project->dependencies.toSet().toList();
		}
	}
}

QString CDependencies::find_dependent_project( QString& dependency )
{
	QMapIterator< QString, CProject > i( m_projects );

	QString project_name;

	while( i.hasNext() ) 
	{
		i.next();

		project_name = i.key();

		const CProject* p =  &( i.value() );

		for( int j=0; j < p->headers.size(); j++ )
			//if( dependency == p->headers[ j ].fileName() )
			if( !QString::compare(	dependency, 
									p->headers[ j ].fileName(), 
									Qt::CaseInsensitive ) ) //FIXME: Attention for Linux !!!!!!
				return project_name;
	}
	return ProjectNotFound;
}

void CDependencies::find_dependent_projects()
{
	QMutableMapIterator< QString, CProject > i( m_projects );
	while( i.hasNext() ) //for each projects search dependent projects from its dependencies (external includes)
	{
		i.next();

		CProject* project = &( i.value() );

		if( project->dependencies.size() )
		{
			QList< QString > dependencies = project->dependencies;

			QList< QString > dependent_projects;
			QList< int > dependencies_not_found;
			
			for( int m = 0; m < dependencies.size(); ++m ) // look for the dependent projects
			{
				QString _project = find_dependent_project( dependencies[ m ] );
				if( _project == ProjectNotFound ) 
					dependencies_not_found.append( m );
				else
					dependent_projects.append( _project );	
			}
			project->dependent_projects		= dependent_projects.toSet().toList();
			project->dependencies_not_found = dependencies_not_found.toSet().toList();
		}
	}
}

void CDependencies::set_linked_by()
{
	QMutableMapIterator< QString, CProject > i( m_projects );

	while( i.hasNext() )
	{
		i.next();

		CProject* project = &( i.value() );

		project->linked_by = find_linked_by( project->name );

	}
}


QList< QString > CDependencies::find_linked_by( const QString& project_name )
{
	QMutableMapIterator< QString, CProject > i( m_projects );

	QList< QString > linked_by;

	while( i.hasNext() )
	{
		i.next();

		CProject* project = &( i.value() );

		for( int m = 0; m < project->dependent_projects.size(); ++m ) // look for the dependent projects
			if( project->dependent_projects[ m ] == project_name )
				linked_by << project->name;
	}
	return linked_by;
}



QString CDependencies::get_included_file( QString& _line ) // parser of the lines that contain the reserved word "#include"
{
	QString line = _line;
	
	line = line.left( line.indexOf("//") );
	line = line.left( line.indexOf("/*") );

	int ini, fin;
	if( line.contains("<") )
	{
		int ini1 = line.indexOf( "<" ) + 1;
		int ini2 = line.lastIndexOf( "/" ) + 1;
		int ini3 = line.lastIndexOf( "\\" ) + 1;

		ini = std::max( ini1, ini2 );
		ini = std::max( ini, ini3 );

		fin =  line.lastIndexOf( ">" );
	}
	else
	{
		int ini1 = line.indexOf( "\"" ) + 1;
		int ini2 = line.lastIndexOf( "/" ) + 1;
		int ini3 = line.lastIndexOf( "\\" ) + 1;

		ini = std::max( ini1, ini2 );
		ini = std::max( ini, ini3 );

		fin =  line.lastIndexOf( "\"" );
	}
	QString s = line.mid( ini, fin - ini );
	if( s.isEmpty() )
		m_parsing_errors << _line;
	return s;
}

QList< QString > CDependencies::get_dependencies( QString header, const QString& project_name )
{
	CProject* project = &m_projects[ project_name ];
 
	QList< QString > dependencies;

	QFile inputFile(header);
	if (inputFile.open(QIODevice::ReadOnly))
	{
		QTextStream in(&inputFile);
		while (!in.atEnd())
		{
			QString line = in.readLine();

			if( line.startsWith("#include") && line.contains(".h") ) //FIXME: starts -> be aware with macros
			{
				QString dependency = get_included_file( line );

				if( !dependency.isEmpty() ) // FIXME: This shouldn't be possible but it happens -> check out the parser
				{
					bool found = false; 
					for( int i=0; i < project->headers.size(); i++ )
					{
						if( dependency == project->headers[ i ].fileName() )
						{
							found = true;
							break;
						}
					}
					if( !found )
						dependencies.append( dependency );
				}
			}
		}
		inputFile.close();
	}

	return dependencies;
}

QList< QFileInfo > CDependencies::get_files( QString path, const QList< QString >& extensions )
{
	QList< QFileInfo > files_info;

	QDirIterator dirIt( path, QDirIterator::Subdirectories );
	while (dirIt.hasNext())
	{
		dirIt.next();

		if (QFileInfo(dirIt.filePath()).isFile())
		{
			QFileInfo info = QFileInfo(dirIt.filePath());

			for( int i = 0; i < extensions.size(); i++ )
				if( info.suffix() == extensions[ i ] )
					files_info.append( info );
		}
	}
	return files_info;
}
