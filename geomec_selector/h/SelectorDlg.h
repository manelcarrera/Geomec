#pragma once

#include <QDialog>
#include <QButtonGroup>
#include <QGridLayout>
#include <QCheckBox>
#include <QPushButton>
#include <QRadioButton>
#include <QHBoxLayout>

class QMenu;

class QStringList;

class QRadioButton_Custom;

//////////////////////////////////////////////////////////////////////////
//
//                      CSelectorDlg
//
//////////////////////////////////////////////////////////////////////////

class CSelectorDlg : public QDialog
{
	Q_OBJECT

	QGridLayout m_grid_layout;
	QHBoxLayout m_h_layout_btns;
	QHBoxLayout m_h_layout_btns_left;

	QCheckBox m_cb;
	QButtonGroup m_rb_group;

	// btns
	QPushButton m_btn_open;
	QPushButton m_btn_less;
	QPushButton m_btn_more;
	QPushButton m_btn_pdf;
	QPushButton m_btn_config;
	QPushButton m_btn_logs;
	QPushButton m_btn_ini;
	QPushButton m_btn_logs_folder;
	QPushButton m_btn_logs_config;
	QPushButton m_btn_python;

	std::vector< QString > m_versions_v;
	std::vector< QString > m_paths_v;

	bool m_b_delete; // delete actions in dlg enable/disable: context menu + icon

private:
	bool delete__( int id );
	void update_style();
	void on_open( const QString& url );

public slots:
	void on_more();
	void on_less();
	void on_pdf();
	void on_config();
	void on_logs();
	void on_ini_file();

	void on_version_ini_file();
	void on_version_folder();
	void on_remove();
	void on_version_logs();
	void on_logs_folder();
	void on_logs_config();
	void on_python();

public:
	CSelectorDlg( 
		const std::pair< 
			std::vector< QString >, 
			std::vector< QString > >& versions_p );

	~CSelectorDlg();

	bool check();
	int id();

	bool delete_( QRadioButton_Custom* rb );
	bool set_as_default( QRadioButton_Custom* rb );

	bool delete_config_file( int id );
};

//////////////////////////////////////////////////////////////////////////
//
//                      QRadioButton_Custom
//
//////////////////////////////////////////////////////////////////////////

class QRadioButton_Custom : public QRadioButton
{
	Q_OBJECT

private:
	QMenu* m_menu;

private slots:
	void on_custom_context_menu( const QPoint &point );
	void on_delete();
	void on_set_as_default();

public:
	QRadioButton_Custom( 
		const QString& caption, 
		QWidget* parent = nullptr );
	~QRadioButton_Custom();
};

//////////////////////////////////////////////////////////////////////////
//
//                      QPushButton_Custom
//
//////////////////////////////////////////////////////////////////////////

class QPushButton_Custom : public QPushButton
{
	Q_OBJECT

public:
	enum eType_
	{
		Version_Config_File,
		Installation_Folder,
		Remove_Version,
		Version_Log_File
	};

private:
	QMenu* m_menu;
	eType_ m_type;
	int m_id;
	bool m_has_context_menu;

private slots:
	void on_custom_context_menu( const QPoint &point );
	void on_delete(); // version config file

public:
	QPushButton_Custom( 
		eType_ type_,
		const int id, 
		QWidget* parent = nullptr,
		bool has_context_menu = false );

	~QPushButton_Custom();

	int id(){ return m_id; }

	void has_context_menu( bool val ){ m_has_context_menu = val; };
};