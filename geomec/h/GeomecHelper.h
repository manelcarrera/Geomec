#pragma once

class ChangelogDlg;

namespace gm{
	namespace helper{
		namespace version{
			static const std::string KEY = "version";
			static const std::string CONFIG_FILE = std::string( getenv("APPDATA") ) + "/" + "Geomec" + "/" + "selector.xml";

			void unset(); // selector.xml
			QString read(); // from selector.xml
			void set( const QString& version ); // to selector.xml
		}// version

		ChangelogDlg* changelog( std::string release );
		void plugins();
		void add_default_version_menu_item();
		void qdebug_msg_handler( 
			QtMsgType type, 
			const QMessageLogContext & context, 
			const QString & msg );
	} // namespace helper
} //namespace gm
