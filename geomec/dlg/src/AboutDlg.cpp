#include "stdafx.h"

#include <QDir>
#include <QApplication>
#include <QDesktopServices>

#include "AboutDlg.h"
#include "util_.h"
#include "GeomecDoc.h"
#include "ChangelogDlg.h"
#include "Printer.h"
#include "QUtil.h"
#include "GeomecHelper.h"
#include "VersionNumbers.h"
#include "Environment.h"
#include "SettingsIni.h"
#include "ISettings.h"

namespace
{
	Printer* printer = Printer::instance(Printer::App);

	static const QString IMG_BASE_PATH	= QUtil::url( QString::fromStdString( util::GetAppPath() ), "img" );
	static const QString DIANA_IMG_URL	= QUtil::url( IMG_BASE_PATH, "diana_fea_200.png" );
	static const QString NEW_IMG_URL	= QUtil::url( IMG_BASE_PATH, "new_green_32.png" );
	static const QString DOWN_IMG_URL	= QUtil::url( IMG_BASE_PATH, "down_arrow_16.png" );
	static const QString UP_IMG_URL		= QUtil::url( IMG_BASE_PATH, "up_arrow_16.png" );
	static const QString PDF_IMG_URL	= QUtil::url( IMG_BASE_PATH, "pdf_16.png" );
	static const QString PDF2_IMG_URL	= QUtil::url( IMG_BASE_PATH, "pdf2_16.png" );
	static const QString LIST_IMG_URL	= QUtil::url( IMG_BASE_PATH, "list_16.png" );
	static const QString DONE_IMG_URL	= QUtil::url( IMG_BASE_PATH, "done_16.png" );

	static const QString LF	= "<br>";

	static const int DEFAULT_ICON_SIZE	= 16;

	void set_btn_img( QPushButton* btn, const QString& img_url, int size = DEFAULT_ICON_SIZE, bool is_border = false )
	{
		btn->setIcon( QIcon( QPixmap( img_url ) ) );
		btn->setIconSize( QSize( size, size ) );

		if( !is_border )
			btn->setStyleSheet("border: none");
	}

	void show_( const QBoxLayout& layout, bool show__ )
	{
		for( int i = 0; i < layout.count(); ++i )
		{
			QWidget *widget = layout.itemAt(i)->widget();
			if( widget )
			{
				widget->setVisible( show__ );
			}
			else
			{
				QLayout* layout_ = layout.itemAt( i )->layout();
				for( int j = 0; j < layout_->count(); ++j )
				{
					QWidget* widget_ = layout_->itemAt( j )->widget();
					if( widget_ )
						widget_->setVisible( show__ );
				}
			}
		}
	} // show_
}

QAboutDlg::~QAboutDlg()
{
	for( auto& layout : { &m_v_layout_more, &m_v_layout_more_2 } )
	{
		for( int i = 0; i < layout->count(); i++ )
		{
			QLayout* item_layout = layout->itemAt( i )->layout();
			for( int j = 0; j < item_layout->count(); j++ ) // 2x: logo and text
				item_layout->itemAt( j )->widget();
		}
	}
}

void QAboutDlg::add_texts()
{
	std::vector< eText > texts_v = 
	{
		Working_Dir,
		Gm_Default_Version,
		App_Last_Model_Version,
		Config_File,
		Diapath_Current,
		Diapath_Env,
		Diapath_Config_File,
		Diapath_Force_Local
	};


	for( const auto& id : texts_v )
	{
		QHBoxLayout* item = new QHBoxLayout();

		QPushButton* btn = new QPushButton();
		set_btn_img( btn, DONE_IMG_URL );

		QLabel* lb = new QLabel();
		lb->setText( text( id ) );

		item->addWidget( btn );
		item->addWidget( lb );
		item->addStretch();

		m_v_layout_more.addLayout( item );

		// red if path doesn't exist
		if( id == Diapath_Current || 
			id == Diapath_Env )
		{
			QString path = id == Diapath_Current ? 
				CEnvironment::instance()->get( CEnvironment::DIAPATH ) : 
				CSettingsIni::instance()->env_DIAPATH();

			lb->setStyleSheet( QDir( path ).exists() ? "QLabel { color: black }" : "QLabel { color: red }" );
		}
		// italic for secondary values
		if( id == Diapath_Env || 
			id == Diapath_Config_File || 
			id == Diapath_Force_Local )
		{
			lb->setStyleSheet( lb->styleSheet().append( "QLabel { font: italic }" ) );
		}
	}

}

void QAboutDlg::add_pdfs()
{
	QDir dir( QString::fromStdString( util::GetAppPath() ), "*.pdf", QDir::Name, QDir::Files | QDir::NoDot | QDir::NoDotDot ); // ????
	QStringList files = dir.entryList();

	int i=0;
	for( const auto& file : files )
	{
		QHBoxLayout* item = new QHBoxLayout();

		QPushButton* btn1 = new QPushButton();
		set_btn_img( btn1, PDF_IMG_URL );

		QPushButton* btn2 = new QPushButton();
		btn2->setText( file );
		btn2->setStyleSheet("border: none");

		item->addWidget( btn1 );
		item->addWidget( btn2 );
		item->addStretch();

		connect( btn2,  &QAbstractButton::clicked, this, [this,i]{ on_open_pdf( i ); });

		m_v_layout_more_2.addLayout( item );
		i++;
	}

}

QString QAboutDlg::text( eText type_ )
{
	QString res;
	switch( type_ )
	{
		case Version:
		{
			QString v = gm::doc_::version( gm::doc_::App );
			//QString v = GetGeomecDoc()->version( CGeomecDoc::App );
			QString line1 = "Geomec Version: " + v.replace("_"," ") + " (64 bits Edition)";
			QString line2 = QString::asprintf("Copyright %c 2000 - 2020", 0xA9);
			res = line1 + LF +  line2;
			break;
		}
		case Working_Dir:
		{
			res = "Working dir: " + QString::fromStdString( util::GetAppPath() ).replace("/","\\");
			break;
		}
		case Gm_Default_Version:
		{
			res = "Default version: " + gm::helper::version::read();
			break;
		}
		case App_Last_Model_Version:
		{
			res = QString::asprintf("Last model version allowed: %d.%d.%d", VERSION_MAJOR, VERSION_MINOR, VERSION_REVISION );
			break;
		}
		case Config_File:
		{
			std::pair< std::string, std::string > url = util::split_url( CSettingsIni::instance()->usr_url().toStdString() );
			res = "Config file: " + QString::fromStdString( url.second );
			break;
		}
		case Diapath_Current:
		{
			res = "DIAPATH (Current): " + CEnvironment::instance()->get( CEnvironment::DIAPATH );
			break;
		}
		case Diapath_Env:
		{
			res = "DIAPATH (Env): " + CSettingsIni::instance()->env_DIAPATH();
			break;
		}
		case Diapath_Config_File:
		{
			res = "DIAPATH (Config file): " + ISettings::instance()->getProfileString( "Paths", CEnvironment::DIAPATH );
			break;
		}
		case Diapath_Force_Local:
		{
			res = "Force diana local: " + ISettings::instance()->getProfileString( "Paths", "DianaLocal" );
			break;
		}
	}
	return res;
}


// TODO: don't forget to release memory in the destructor !!!
QAboutDlg::QAboutDlg( QDialog* parent )
{
	setWindowTitle( "About Geomec" );

	// imgs
	bool border = false;
	set_btn_img( &m_btn_logo,	DIANA_IMG_URL, 64 );
	set_btn_img( &m_btn_new,	NEW_IMG_URL, 32 );
	set_btn_img( &m_btn_down,	DOWN_IMG_URL );
	set_btn_img( &m_btn_down_2,	DOWN_IMG_URL );
	set_btn_img( &m_btn_up,		UP_IMG_URL );
	set_btn_img( &m_btn_up_2,		UP_IMG_URL );
	set_btn_img( &m_btn_menu_pdf,	PDF2_IMG_URL );
	set_btn_img( &m_btn_menu_var,	LIST_IMG_URL );

	m_btn_new2.setText( "What's new?" );
	m_btn_new2.setStyleSheet("border: none");

	m_h_layout_up.addWidget( &m_btn_logo );
	m_h_layout_up.addLayout( &m_v_layout_up_right );
	m_v_layout_up_right.addWidget( &m_lb_version );
	
	m_v_layout_up_right.addLayout( &m_h_layout_new );
	m_h_layout_new.addWidget( &m_btn_new );
	m_h_layout_new.addWidget( &m_btn_new2 );
	m_h_layout_new.addStretch();

	m_v_layout.addLayout( &m_h_layout_up );

	m_v_layout.addLayout( &m_h_layout_arrows );

	m_h_layout_arrows.addWidget( &m_btn_menu_var );
	m_h_layout_arrows.addWidget( &m_btn_down );
	m_h_layout_arrows.addWidget( &m_btn_up );
	m_h_layout_arrows.addStretch();

	m_v_layout.addLayout( &m_v_layout_more );

	m_h_layout_arrows_2.addWidget( &m_btn_menu_pdf );
	m_h_layout_arrows_2.addWidget( &m_btn_down_2 );
	m_h_layout_arrows_2.addWidget( &m_btn_up_2 );
	m_h_layout_arrows_2.addStretch();

	m_v_layout.addLayout( &m_h_layout_arrows_2 );

	m_v_layout.addLayout( &m_v_layout_more_2 );

	m_v_layout.addStretch();

	setLayout( &m_v_layout );

	// data to controls 
	m_lb_version.setText( text( Version ) );
	QDir dir( QString::fromStdString( util::GetAppPath() ), "*.pdf", QDir::Name, QDir::Files | QDir::NoDot | QDir::NoDotDot ); // ????
	m_pdfs = dir.entryList();
	add_texts();
	add_pdfs();
	//

	connect( &m_btn_new,	SIGNAL( clicked() ), this, SLOT( on_changelog() ) );
	connect( &m_btn_new2,	SIGNAL( clicked() ), this, SLOT( on_changelog() ) );

	connect( &m_btn_down,	SIGNAL( clicked() ), this, SLOT( on_more() ) );
	connect( &m_btn_up,		SIGNAL( clicked() ), this, SLOT( on_less() ) );

	connect( &m_btn_down_2,	SIGNAL( clicked() ), this, SLOT( on_more_2() ) );
	connect( &m_btn_up_2,	SIGNAL( clicked() ), this, SLOT( on_less_2() ) );

	on_less();
	on_less_2();

	// don't remove, needed
	QApplication::processEvents();
	adjustSize();
}

void QAboutDlg::on_open_pdf( int i )
{
	QDesktopServices::openUrl( QUrl::fromLocalFile( m_pdfs.at( i ) ) );
}

void QAboutDlg::on_more()
{
	show_( m_v_layout_more, true );
	m_btn_down.hide();
	m_btn_up.show();

	QApplication::processEvents();
	adjustSize();
}

void QAboutDlg::on_less()
{
	show_( m_v_layout_more, false );
	m_btn_down.show();
	m_btn_up.hide();

	QApplication::processEvents();
	adjustSize();
}

void QAboutDlg::on_more_2()
{
	show_( m_v_layout_more_2, true );
	m_btn_down_2.hide();
	m_btn_up_2.show();

	QApplication::processEvents();
	adjustSize();
}

void QAboutDlg::on_less_2()
{
	show_( m_v_layout_more_2, false );
	m_btn_down_2.show();
	m_btn_up_2.hide();

	QApplication::processEvents();
	adjustSize();
}



void QAboutDlg::on_changelog()
{
	ChangelogDlg dlg;

	if( dlg.verify() )
		dlg.exec();
	else
	{
		AfxMessageBox( "No changelog data found", MB_ICONERROR );
		printer->info( "No changelog data found" );
	}
}
