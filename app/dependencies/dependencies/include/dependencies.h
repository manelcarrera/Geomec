#if !defined(Dependencies_h)
#define Dependencies_h

#include <QtCore/QList>
#include <QtCore/QString>
#include <QtCore/QMap>
#include <QtCore/QFileInfo>

//listen
//#include <QtNetwork/QTcpServer>
//#include <QtNetwork/QTcpSocket>


#include <QtCore/QObject>


static const QString DefaultProjetPath = "C:\\dev\\Geomec\\trunk";


class CDependencies : QObject
{
	/*Q_OBJECT

public slots:

	void accept_connection();*/
	void handle_command();

public:

	QList< QString > m_parsing_errors; //FIXME: -> private

	enum DataType
	{
		Headers,
		Dependencies,
		DependentProjects,
		DependenciesnNotFound
	};

	static const int PRINT_PROJECT; 
	static const int PRINT_LIB; 
	static const int PRINT_NOT_FOUND; 
	static const int PRINT_ALL; 

	class CProject
	{
	public:

		enum ProjectsType
		{
			Dependent,
			LikedBy,
			DependenciesNotFound
		};

		QString name;
		QString path;
		QList< QString > dependencies;
		QList< QFileInfo > headers;
		QList< QFileInfo > sources;
	
		QList< QString > dependent_projects;
		QList< int > dependencies_not_found;

		QList< QString > linked_by;

		bool is_external;

		CProject(	QString _name = "", 
					QString _path = "", 
					bool _is_external = false ) 
		:	name(			_name ), 
			path(			_path ), 
			is_external(	_is_external )
		{
		};
	};

	CDependencies();
	virtual ~CDependencies();

	void run( const QString& path = DefaultProjetPath, const QList< QString >& exclude_projects = QList< QString >() );
	void clean();

	void print_results( const int what_to_print = CDependencies::PRINT_ALL );
	
	void new_print_results( CDependencies::CProject::ProjectsType _type = CDependencies::CProject::ProjectsType::Dependent );

	void print_data( const QString& project_name, CDependencies::DataType data_type );

	QString find_dependent_project( QString& dependency );

	void listen();

private:

	QMap< QString, CDependencies::CProject > m_projects;

	//QTcpServer m_tcp_server;
    //QTcpSocket* m_tcp_server_connection;


private:

	void CDependencies::find_projects( const QString& path, const QList< QString >& exclude_projects = QList< QString >() );
	void find_source_files();
	void find_dependencies();
	void find_dependent_projects();
	void add_external_projects();

	QString get_included_file( QString& _line ); // parser of the lines that contain the reserved word "#include"
	QList< QString > get_dependencies( QString header, const QString& project_name );
	QList< QFileInfo > get_files( QString path, const QList< QString >& extensions );

	QList< QString > find_linked_by( const QString& project_name );
	void set_linked_by();

	int get_max( const QList< QPair< QString, int > >& list );

	QList< QPair< QString, int > > get_number_of_projects( CDependencies::CProject::ProjectsType _type = CDependencies::CProject::ProjectsType::Dependent );

};



#endif //!defined(Dependencies_h)
