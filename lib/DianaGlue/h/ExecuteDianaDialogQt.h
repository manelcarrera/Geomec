#pragma once

#include <QDialog>
#include <QKeyEvent>

#include "IExeDianaDlg.h"

//qt
class QListWidget;
class QVBoxLayout;
class QHBoxLayout;
class QWidget;
class QPushButton;
class QLabel;

//
// custom QDialog to skip ESC keypress
//
class QDlg : public QDialog
{
	Q_OBJECT
public:
	void keyPressEvent(QKeyEvent *e)
	{
		if(e->key() != Qt::Key_Escape)
			QDialog::keyPressEvent(e);
		else {/* skip event */}
	}
};

class CExecuteDianaDialogQt : public QDlg, public IExeDianaDlg
{
	Q_OBJECT

	QVBoxLayout* m_v_layout;
	QHBoxLayout* m_h_layout;
	QListWidget* m_list;
	QListWidget* m_list_w; // warnings
	QPushButton* m_btn;
	QLabel* m_label;
	QLabel* m_label_w; // warnings

public slots:
	void on_cancel();

public:
	CExecuteDianaDialogQt( QWidget* pParent = nullptr );
	~CExecuteDianaDialogQt();

	// for convenience
	void msg(const std::vector<QString>& val, eDianaMsg type_);

	void msg(const QString& val, eDianaMsg type_ = eDianaMsg::Gen);			
	void show_(bool val=true);
	void hide_(){ show_(false); }; // for convenience
	void clear();
};