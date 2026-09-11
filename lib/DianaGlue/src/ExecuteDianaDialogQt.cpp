#include "ExecuteDianaDialogQt.h"
#include "Events.h"
#include "Global.h"
#include "Printer.h"

// qt
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>

namespace {
//
// FIXME: new module for gui stuff?
//
Printer *printer = Printer::instance(Printer::Doc);
} // namespace

CExecuteDianaDialogQt::CExecuteDianaDialogQt(QWidget *pParent) {
  printer->info("qt dlg : new >>");

  m_v_layout = new QVBoxLayout();
  m_h_layout = new QHBoxLayout();

  m_list = new QListWidget(this);
  m_list_w = new QListWidget(this);

  m_btn = new QPushButton("Cancel", this);

  m_label = new QLabel("Messages:", this);
  m_label_w = new QLabel("Warnings:", this);

  m_v_layout->addWidget(m_label);
  m_v_layout->addWidget(m_list);
  m_v_layout->addWidget(m_label_w);
  m_v_layout->addWidget(m_list_w);

  m_h_layout->addStretch();
  m_h_layout->addWidget(m_btn);
  m_h_layout->addStretch();

  m_v_layout->addLayout(m_h_layout);

  setLayout(m_v_layout);
  setMinimumSize(QSize(600, 150));

  setStyleSheet(QString("background-color: %1;").arg(_g->qt_dlg_bg_color()));
  setWindowTitle("Calculating");

  //
  show();
  //
  raise();
  //
  // don't force the focus as it's annoying when you switch to another application to ebe get back to gm
  //
  // activateWindow();
  //

  //
  // TODO: needs to be checked out
  //
  // setAttribute(Qt::WA_DeleteOnClose);

  connect(m_btn, SIGNAL(clicked()), this, SLOT(on_cancel()));
}

CExecuteDianaDialogQt::~CExecuteDianaDialogQt() {

  // for( auto& obj : std::vector<QObject*>{m_list, m_list_w, m_label, m_label_w, m_btn, m_h_layout, m_v_layout} )
  //	delete obj;

  delete m_list;
  delete m_list_w;

  delete m_label;
  delete m_label_w;

  delete m_btn;

  delete m_h_layout;
  delete m_v_layout;

  printer->info("qt dlg : delete <<");
}

//
// FIXME: this makes the app hang, the gui freezes and no actions  can be performed
//
void CExecuteDianaDialogQt::on_cancel() {
  printer->info("qt dlg : --on cancel--");

  //
  // to avoid more than one click
  //
  // disconnect(m_btn, SIGNAL(clicked()), this, SLOT(on_cancel()));

  // m_btn->hide();
  // hide();

  _e->broadcast(CancelRun);

  close();

  //
  // does dlg needs to be closed? better add an extra line with run cancelled message
  //
  // msg({"","","","!!! RUN CANCELLED !!!"},eDianaMsg::Warning);
}

void CExecuteDianaDialogQt::msg(const std::vector<QString> &val, eDianaMsg type_) {
  for (const auto &line : val)
    msg(line, type_);
}

//
// FIXME: who deletes the added items?
//
void CExecuteDianaDialogQt::msg(const QString &val, eDianaMsg type_) {
  printer->debug("qt dlg : msg:%s", val.toStdString().c_str());

  QListWidget *list = type_ == eDianaMsg::Warning || type_ == eDianaMsg::Error ? m_list_w : m_list;

  list->addItem(new QListWidgetItem(val));
  list->scrollToBottom();

  //
  // FIXME: just to test dsa run (too fast so cancel btn can't be pushed)
  //
  /*static int count=0;
  count++;
  if(count==2)
  {
    //count=0;
    on_cancel();
  }*/
  //
  //
  //

  //
  // don't force the focus as it's annoying when you switch to another application to ebe get back to gm
  //
  // activateWindow(); // to send it to front
}

//
// show only if it's not visible
//
void CExecuteDianaDialogQt::show_(bool val) {
  if (val) {
    if (!isVisible())
      show();
  } else
    hide();
}

void CExecuteDianaDialogQt::clear() { m_list->clear(); }