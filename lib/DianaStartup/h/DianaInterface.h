#pragma once

//own
#include "IListener.h"
#include "IListenerIpc.h"
#include "Heartbeat.h" 
//qt
#include <QString>
#include <QStringList>
//std
#include <map>

class CDianaStartUp;

////////////////////////////////////////////////////
//
//				functions
//
////////////////////////////////////////////////////

namespace gm{
namespace di{

qint64 launch( QString bin, const QStringList& args );
#ifdef KK
bool exe( 
	const QString& bin, 
	QStringList params,
	QString& result );
std::string exec( const char* cmd );
#endif
bool running(const QString &process); 
bool running(const qint64& id);
#ifdef WIN32
bool running_(const qint64& id);
#endif
void kill(const QString &process);
void kill(const qint64& pid);
std::string now_s(const std::string& format = "%Y%m%d%H%M%S");

} // namespace di
} // namespace gm

/////////////////////////////////////////////////////////////////
//
//
//			diana interface (DI)
//
//
/////////////////////////////////////////////////////////////////

class DI : public IListener, public IListenerIpc
{
	HB m_hb; // heartbeat

	int m_hb_timeout;

	CDianaStartUp* m_dsu;

	// qint64 = long long
	qint64 m_diana_app_pid;
	void diana_app_pid(const qint64& pid){m_diana_app_pid=pid;};
	qint64 diana_app_pid(){return m_diana_app_pid;};

	enum eDianaStatus{ Stopped, Running };
	eDianaStatus m_diana_status;
	void diana_status(const eDianaStatus& val){m_diana_status=val;};
	eDianaStatus diana_status(){return m_diana_status;};

	static const char* m_diana_status_cp[];

public:
	enum eKillDianaRes{ DianaNotRunning, DianaKilled, UnableToKillDiana };
	static const char* m_kill_diana_res_cp[];

	static eKillDianaRes kill_diana();

	static eKillDianaRes kill_diana(const qint64& pid_ );

	void hb_timeout( int val ) { m_hb_timeout = val; };

private:
	void on_launch_diana();
	QStringList run_diana_params();

	void on_diana_response(const Cmd& cmd);
	void execute(const Cmd& cmd);

protected:
	virtual void handle(Cmd cmd);
	virtual void received(Cmd cmd);

public:
	std::map< QString, QString > m_var_m; // FFDIR

public:
	DI();
	~DI();
};

