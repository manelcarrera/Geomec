#include "stdafx.h"

#include "MeshRefinementBoxesHandler.h"
#include "MeshRefinementDlg.h"
#include "MeshRefinementGradientDlg.h"
#include "Printer.h"
#include "util_.h"

#include <QAction>
#include <QCheckBox>
#include <QDoubleValidator>
#include <QGridLayout>
#include <QIntValidator>
#include <QKeyEvent>
#include <QLabel>
#include <QLineEdit>
#include <QMenu>
#include <QMessageBox>
#include <QPixmap>
#include <QPushButton>
// #include <QFontMetrics>

namespace {
const QString IMG_BASE_PATH = QString::fromStdString(util::GetAppPath()) + QString("/img/");

// images
static const QString DELETE_IMG_URL = IMG_BASE_PATH + QString("delete_16.png");
static const QString WARNING_IMG_URL = IMG_BASE_PATH + QString("warning_16.png");
static const QString MODIFIED_IMG_URL = IMG_BASE_PATH + QString("undo_16.png");
static const QString EMPTY_IMG_URL = IMG_BASE_PATH + QString("empty_16.png");
static const QString HEXA_IMG_URL = IMG_BASE_PATH + QString("hexa.png");
static const QString ZOOM_IMG_URL = IMG_BASE_PATH + QString("zoom_16.png");
static const QString ENLARGE_IMG_URL = IMG_BASE_PATH + QString("increase_size.png");
static const QString DECREASE_IMG_URL = IMG_BASE_PATH + QString("minimize.png");
static const QString EDIT_IMG_URL = IMG_BASE_PATH + QString("edit.png");
static const QString ROTATE_IMG_URL = IMG_BASE_PATH + QString("rotate_16.png");
static const QString DONE_IMG_URL = IMG_BASE_PATH + QString("done_16.png");
static const QString OK_IMG_URL = IMG_BASE_PATH + QString("ok_16.png");
static const QString PLANAR_IMG_URL = IMG_BASE_PATH + QString("planar_16.png");
static const QString CENTER_IMG_URL = IMG_BASE_PATH + QString("center_16.png");

// tests
static const QString TEXT_CONFIRM_TITLE = QString("Delete box");
static const QString TEXT_CONFIRM_TEXT = QString("Sure to detele the box?");

static const QString UNDO_TITLE = QString("Undo changes");
static const QString UNDO_TEXT = QString("Sure to undo box changes? (size and position)");

// pop-up context menu
static const QString TEXT_GO = "Zoom to box";
static const QString TEXT_DELETE = "Delete";
static const QString TEXT_NEW = "New";
static const QString TEXT_SHOW_HIDE = "Toggle show/hide";

static const int DIVISIONS = 11;

static const QString REGEXP_EDIT = "^((0?(\\.\\d{0,2}))|1)$";

// coordinates
static const int EDIT_MAX_WIDTH = 70;

// gradient
// Example: 0.99
static const int WIDTH_MAX = 25;
static const int WIDTH_FIXED = 29;
// Example: 0.9
// static const int WIDTH_MAX = 25;
// static const int WIDTH_FIXED = 24;

static const QString RED = "#20FF0000";
static const QString GREEN = "#2000FF00";
static const QString GRAY = "rgba(255, 255, 255, 10)";
static const QString WHITE = "white";
//"transparent" -> Ok ... "#20FF0000": "#" + "80" + "FF0000" (20: transparency level)

void set_btn_img(QPushButton *btn, const QString &img_url, bool is_border = true) {
  btn->setIcon(QIcon(QPixmap(img_url)));
  btn->setIconSize(QSize(16, 16));

  if (!is_border)
    btn->setStyleSheet("border: none");
}

Printer *printer = Printer::instance(Printer::Mr);

} // namespace

///////////////////////////////////////////////////////////////////////////////////////
//
//									CGradientRow
//
///////////////////////////////////////////////////////////////////////////////////////

CGradientRow::CGradientRow(CMeshRefinementBoxesHandler *boxes_handler, const QString &box_name, QWidget *parent)
    : QFrame(parent), m_boxes_handler(boxes_handler), m_edit_dlg(nullptr) {
  ///////////////////////////////////// ui

  m_layout = new QHBoxLayout();
  m_img_label = new QLabel();
  m_warning_btn = new QPushButton();
  m_modified = new QPushButton();
  m_zoom = new QPushButton();
  m_name_edit = new QLineEdit();
  m_checkbox = new QCheckBox();
  m_values = new CGradientValues(this);
  m_delete_btn = new QPushButton();
  m_enlarge = new QPushButton();
  m_decrease = new QPushButton();
  m_edit_btn = new QPushButton();
  m_type_btn = new QPushButton();
  // m_rotate_btn	= new QPushButton();

  m_checkbox->setFocus(); // to the frame: doesn't remove the focus from delete_btn

  m_img_label->setPixmap(QPixmap::fromImage(QImage(HEXA_IMG_URL)));
  m_img_label->adjustSize();

  m_name = box_name;

  m_name_edit->setText(m_name);
  // m_name_edit->setStyleSheet("QLineEdit { border: none; background-color: rgba(255, 255, 255, 10); }");
  // m_name_edit->setStyleSheet("QLineEdit { width: 80px; background-color: rgba(255, 255, 255, 10); }");
  m_name_edit->setStyleSheet("QLineEdit { width: 80px; }");

  bool is_check = !boxes_handler->data_m().contains(box_name) || // new
                  boxes_handler->is_visible(box_name);           // existing one

  m_checkbox->setChecked(is_check);

  bool exists = boxes_handler->data_m().contains(box_name);
  m_values->gradient(exists ? boxes_handler->data_m()[box_name].Profile() : std::vector<RelativeSize>{});

  m_type = exists ? boxes_handler->data_m()[box_name].GradientType() : RefinementBox::PlanarZ;

  std::vector<std::pair<QPushButton *, const QString &>> btns_v = {
      {m_delete_btn, DELETE_IMG_URL}, {m_warning_btn, OK_IMG_URL},
      {m_modified, DONE_IMG_URL},     {m_zoom, ZOOM_IMG_URL},
      {m_enlarge, ENLARGE_IMG_URL},   {m_decrease, DECREASE_IMG_URL},
      {m_edit_btn, EDIT_IMG_URL},     {m_type_btn, m_type == RefinementBox::PlanarZ ? PLANAR_IMG_URL : CENTER_IMG_URL},
      /*{ m_rotate_btn,	ROTATE_IMG_URL }*/};

  for (auto btn : btns_v)
    set_btn_img(btn.first, btn.second, btn.first != m_warning_btn);

  std::vector<QWidget *> wgt_v = {m_checkbox, m_delete_btn, m_warning_btn, m_modified, m_zoom, m_name_edit, m_enlarge,
                                  m_decrease, m_edit_btn, m_type_btn,
                                  // m_rotate_btn,
                                  m_values};

  for (auto const &wgt : wgt_v)
    m_layout->addWidget(wgt);

  setLayout(m_layout);

  m_layout->setMargin(0); // it works

  ///////////////////////////////////// context menu -> it works

  // uncomment if needed
  // setContextMenuPolicy( Qt::CustomContextMenu );
  // connect( this, SIGNAL( customContextMenuRequested( const QPoint & ) ), this, SLOT( on_custom_context_menu( const
  // QPoint &) ) );

  m_menu = new QMenu();

  std::vector<std::pair<eActions, const QString &>> actions_v = {
      {New, TEXT_NEW}, {Delete, TEXT_DELETE}, {Go, TEXT_GO}, {Show_Hide, TEXT_SHOW_HIDE}};

  for (auto a : actions_v) {
    m_actions[a.first] = new QAction(a.second, this);
    m_menu->addAction(m_actions[a.first]);

    int a_ = a.first;
    connect(m_actions[a_], &QAction::triggered, this, [this, a_] { action((eActions)a_); });
  }

  ///////////////////////////////////// widgets actions
  // internal
  connect(m_delete_btn, SIGNAL(clicked()), this, SLOT(on_delete()));
  connect(m_modified, SIGNAL(clicked()), this, SLOT(on_undo()));
  connect(m_zoom, SIGNAL(clicked()), this, SLOT(on_zoom_to_box()));
  connect(m_checkbox, SIGNAL(stateChanged(int)), this, SLOT(on_checkbox(int)));
  connect(m_name_edit, SIGNAL(editingFinished()), this, SLOT(on_rename()));
  connect(m_enlarge, SIGNAL(clicked()), this, SLOT(on_size_up()));
  connect(m_decrease, SIGNAL(clicked()), this, SLOT(on_size_down()));
  connect(m_edit_btn, SIGNAL(clicked()), this, SLOT(on_edit()));
  connect(m_type_btn, SIGNAL(clicked()), this, SLOT(on_toggle_type()));
  // connect( m_rotate_btn,	SIGNAL( clicked() ),			this, SLOT( on_rotate() ) );
  connect(m_warning_btn, SIGNAL(clicked()), this, SLOT(on_show_warning()));

  // to parent: dlg
  connect(this, SIGNAL(remove(const QString &)), parent, SLOT(on_remove(const QString &)));
  connect(this, SIGNAL(add()), parent, SLOT(on_add()));
  connect(this, SIGNAL(checkbox_change(const QString &, int)), parent,
          SLOT(on_box_checkbox_change(const QString &, int)));
  connect(this, SIGNAL(zoom_to_box_request(const QString &)), parent, SLOT(on_zoom_to_box_request(const QString &)));
  connect(this, SIGNAL(resize(const QString &, int)), parent,
          SLOT(on_resize(const QString &, int))); // name ... up / down
  connect(this, SIGNAL(show_warning_evt(const QString &)), parent, SLOT(on_show_warning(const QString &))); // name

  // on mouse over help
  m_delete_btn->setToolTip("Delete the box");
  m_modified->setToolTip("Revert changes on box size and position");
  m_zoom->setToolTip("Zoom to the box");
  m_warning_btn->setToolTip("Gradient values not correct, value at center or at edge not informed");
  m_checkbox->setToolTip("Show/hide box in the scene. If hidden, mesh in the box region is not refined");
  m_name_edit->setToolTip("Box name. It can be edited");
  m_enlarge->setToolTip("Enlarge box x10 times");
  m_decrease->setToolTip("Decrease box size x10 times");
  m_edit_btn->setToolTip("Set box size and position");
  m_type_btn->setToolTip("Toogle type: planar/center");
  // m_rotate_btn->setToolTip("Show rotor to rotate box");

  on_checkbox(m_checkbox->isChecked() ? Qt::Checked : Qt::Unchecked);
}

void CGradientRow::show_btns(bool val) {
  std::vector<QWidget *> btns_v = {m_delete_btn, m_warning_btn, m_modified, m_zoom, m_enlarge,
                                   m_decrease,   m_edit_btn,    m_type_btn
                                   /*m_rotate_btn*/};

  for (auto const &btn : btns_v) {
    if (val)
      btn->show();
    else
      btn->hide();
  }
}

CGradientRow::~CGradientRow() {
  // TODO: delete eveything, not only a few

  delete m_layout;
  delete m_img_label;
  delete m_warning_btn;
  delete m_modified;
  delete m_zoom;
  delete m_name_edit;
  delete m_checkbox;
  delete m_values;
  delete m_delete_btn;
  delete m_enlarge;
  delete m_decrease;
  delete m_edit_btn;
  // delete m_rotate_btn;
}

void CGradientRow::on_toggle_type() {
  m_type = m_type == RefinementBox::PlanarZ ? RefinementBox::CenterPoint : RefinementBox::PlanarZ;
  set_btn_img(m_type_btn, m_type == RefinementBox::PlanarZ ? PLANAR_IMG_URL : CENTER_IMG_URL, true);
}

void CGradientRow::on_size_up() { emit resize(m_name, 0); }
void CGradientRow::on_size_down() { emit resize(m_name, 1); }

// everything in the dlg cames in rw coords
void CGradientRow::on_apply() {
  // pos and size on open dlg, OIV coordinates
  T3da pos_0 = m_edit_dlg->pos_ini();
  T3da size_0 = m_edit_dlg->size_ini();

  T3da pos = m_edit_dlg->pos();
  T3da size = m_edit_dlg->size();

  if (pos_0 != pos)
    m_boxes_handler->pos(m_name, RBox::rw_to_oiv(pos, RBox::Pos));

  if (size_0 != size)
    m_boxes_handler->size(m_name, RBox::rw_to_oiv(size, RBox::Size));
}

void CGradientRow::on_ok() {
  on_apply();

  // delete
  m_edit_dlg->accept();
  delete m_edit_dlg;
  m_edit_dlg = nullptr;
}

//	everything in the dlg cames in rw coords
//	values passed to handler needs to be converted back to oiv coords
void CGradientRow::on_cancel() {
  T3da pos_0 = m_edit_dlg->pos_ini();
  T3da size_0 = m_edit_dlg->size_ini();

  T3da pos = m_edit_dlg->pos();
  T3da size = m_edit_dlg->size();

  if (pos_0 != pos)
    m_boxes_handler->pos(m_name, RBox::rw_to_oiv(pos_0, RBox::Pos));

  if (size_0 != size)
    m_boxes_handler->size(m_name, RBox::rw_to_oiv(size_0, RBox::Size));

  // delete
  m_edit_dlg->reject();
  delete m_edit_dlg;
  m_edit_dlg = nullptr;
}

void CGradientRow::on_edit() {
  if (m_edit_dlg == nullptr) {
    T3da pos_0 = m_boxes_handler->pos(m_name);
    T3da size_0 = m_boxes_handler->size(m_name);

    m_edit_dlg = new CBoxEdit(pos_0, size_0, this);
    m_edit_dlg->show();
  } else {
    // msg dlg already open
    // or bring to front
  }
}

// to avoid delete popup on clicking enter after editing box name
void CGradientRow::keyPressEvent(QKeyEvent *e) {
  switch (e->key()) {
  case Qt::Key_Return:
  case Qt::Key_Enter:
    break;
  default:
    QFrame::keyPressEvent(e);
  }
}

// this is called twice but second call is filtered, it may be solved in a better way
// it's also called for other line_edits not edited: box_1 -> box_1_ and box_2 -> box_2 ( box_2 doesn't really chages )
void CGradientRow::on_rename() {
  QString new_name = m_name_edit->text();
  if (m_name != new_name) {
    m_boxes_handler->rename(m_name, new_name);
    m_name = new_name;
  }
}

void CGradientRow::on_rotate() { m_boxes_handler->rotate(m_name); }

void CGradientRow::on_undo() {
  /*QMessageBox::StandardButton res = QMessageBox::critical(
    nullptr,
    UNDO_TITLE,
    UNDO_TEXT,
    QMessageBox::Yes | QMessageBox::No );

  if( res == QMessageBox::Yes )*/
  {
    m_boxes_handler->undo(m_name);
    update_box_modified(false); // icon
  }
}

void CGradientRow::on_delete() {
  QMessageBox::StandardButton res =
      QMessageBox::critical(nullptr, TEXT_CONFIRM_TITLE, TEXT_CONFIRM_TEXT, QMessageBox::Yes | QMessageBox::No);

  if (res == QMessageBox::Yes)
    emit remove(m_name);
}

void CGradientRow::on_checkbox(int state) {
  m_boxes_handler->show(m_name, state == Qt::Checked);

  if (state == Qt::Checked)
    m_values->show();
  else
    m_values->hide();

  show_btns(state == Qt::Checked);

  emit checkbox_change(m_name, state);
}

void CGradientRow::on_custom_context_menu(const QPoint &point) { m_menu->exec(mapToGlobal(point)); }

void CGradientRow::on_zoom_to_box() {
  emit zoom_to_box_request(m_name); // notify dlg
}

void CGradientRow::on_new() { emit add(); }

void CGradientRow::on_update_warning() {
  MeshRefinementDlg *dlg = (MeshRefinementDlg *)parentWidget();

  // boxes check
  bool res1 = dlg->check_scope(m_name);

  std::vector<QString> over_v;
  bool res2 = dlg->check_overlap(m_name, over_v);

  bool res3 = m_values->gradient_check();

  update_warning(res1 && res2 && res3);
}

void CGradientRow::update_warning(bool val) { set_btn_img(m_warning_btn, val ? OK_IMG_URL : WARNING_IMG_URL, false); }

void CGradientRow::update_box_modified(bool val) {
  // undo
  m_modified->setIcon(QIcon(QPixmap::fromImage(QImage(val ? MODIFIED_IMG_URL : DONE_IMG_URL))));

  // update warning icon
  on_update_warning();

  // coordinates dlg
  if (m_edit_dlg) {
    T3da pos_0 = m_boxes_handler->pos(m_name);
    T3da size_0 = m_boxes_handler->size(m_name);

    m_edit_dlg->update(pos_0, size_0);
  }
}

TGradientV CGradientRow::gradient() { return m_values->gradient(); }

bool CGradientRow::is_values_visible() { return m_checkbox->isVisible(); };

void CGradientRow::show_values(bool val) { on_checkbox(val ? Qt::Checked : Qt::Unchecked); };

void CGradientRow::action(eActions action_) {
  switch (action_) {
  case New:
    on_new();
    break;
  case Delete:
    on_delete();
    break;
  case Go:
    on_zoom_to_box();
    break;

  default:
    break;
  }
}

void CGradientRow::on_show_warning() {
  // bool res = ( (MeshRefinementDlg*)parentWidget() )->check( m_name, true );
  emit show_warning_evt(m_name);
}

///////////////////////////////////////////////////////////////////////////////////////
//
//									CGradientValues
//
///////////////////////////////////////////////////////////////////////////////////////

namespace {

QString calculate_bgcolor(QLineEdit *e, int index) // index not needed, dynamic_cast can be done
{
  QString color;
  if (index == 0) {
    bool val = e->text().isEmpty() || e->text().toDouble() == 0;

    color = val ? RED : GREEN;
  } else if (index == DIVISIONS - 1)
    color = GRAY;
  else
    color = e->text().isEmpty() ? WHITE : GREEN;

  return color;
}

bool check(QLineEdit *e, int index) {
  bool val = true;
  if (index == 0) {
    val = !e->text().isEmpty() && e->text().toDouble() != 0;
  }
  return val;
}

void set_bgcolor(QLineEdit *e, const QString &color) {
  e->setStyleSheet(QString::asprintf("background-color: %s", color.toStdString().c_str()));
}

} // namespace

// TODO: This is not well implemented, needs to be redone
// reads gradient from the GUI
TGradientV CGradientValues::gradient() {
  TGradientV gradient_v;

  int i = -1;
  for (auto edit : m_edits_v) {
    i++;
    if (edit->text().isEmpty())
      continue;

    double size_ = edit->text().toDouble();

    // better do transformation for the values sent to the mesher but not to load/save
    gradient_v.push_back(RelativeSize(i, size_));
    printer->debug("gradient : get : [d:%d s:%.2f]", i, size_);
  }

  return gradient_v;
}

// TODO: This is not well implemented, needs to be redone
// 1) sets gradient into the GUI:
// -vaues
// -bgcolor
// 2) sends warning event when needed
void CGradientValues::gradient(const TGradientV &gradient_v) {
  for (auto g : gradient_v) {
    int d_ = g.Distance();
    double s_ = g.Size();

    printer->debug("gradient : set : [d:%d s:%.2f]", d_, s_);

    int index = d_;

    QLineEdit *e = m_edits_v[index];

    e->setText(QString::number(s_));

    QString color = calculate_bgcolor(e, index);
    set_bgcolor(e, color);
  }
  // show warning icon when nedeed
  emit update_warning_evt();
}

bool CGradientValues::gradient_check() {
  int i = 0;
  for (auto e : m_edits_v)
    if (!check(e, i++))
      return false;
  return true;
}

CGradientValues::~CGradientValues() {
  for (auto &e : m_edits_v) {
    delete e->validator();
    delete e;
  }
}

CGradientValues::CGradientValues(QWidget *parent) : QFrame(parent) {
  setSizePolicy(QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Fixed);

  QGridLayout *layout_1 = new QGridLayout(this);

  m_edits_v.reserve(DIVISIONS);

  int row = 0;
  for (int i = 0; i < DIVISIONS; i++) {
    QLineEdit *edit;

    if (i == 0)
      edit = new QLineEdit_Center(this);
    else if (i == DIVISIONS - 1)
      edit = new QLineEdit();
    else
      edit = new QLineEdit_Others(this);

    edit->setAlignment(Qt::AlignHCenter);

    QRegularExpression re(REGEXP_EDIT);
    QRegularExpressionValidator *v = new QRegularExpressionValidator(re, 0);

    edit->setValidator(v);

    layout_1->addWidget(edit, row + 1, i);

    edit->setMaximumWidth(WIDTH_MAX);
    edit->setFixedWidth(WIDTH_FIXED);

    if (i == DIVISIONS - 1) {
      edit->setReadOnly(true);
      set_bgcolor(edit, GRAY);
      edit->setText("1");
    }

    // default values in center and border
    if (i == 0) {
      set_bgcolor(edit, GREEN);
      edit->setText("0.5");
    }

    m_edits_v.push_back(edit);
    m_index_m[edit] = i;

    edit->editingFinished(); // in case is a new one and not set_gradient is called later on : bg colors need to be set
                             // and warnig icon shown
  }

  layout_1->setMargin(0);

  connect(this, SIGNAL(update_warning_evt()), parent, SLOT(on_update_warning()));
}

///////////////////////////////////////////////////////////////////////////////////////
//
//									QLineEdit_Center
//
///////////////////////////////////////////////////////////////////////////////////////

void QLineEdit_Center::focusOutEvent(QFocusEvent *e) {
  bool res = !text().isEmpty() && text().toDouble() != 0;

  set_bgcolor(this, res ? GREEN : RED);

  emit((CGradientValues *)parentWidget())->update_warning_evt();

  QLineEdit::focusOutEvent(e);
};

///////////////////////////////////////////////////////////////////////////////////////
//
//									QLineEdit_Others
//
///////////////////////////////////////////////////////////////////////////////////////

void QLineEdit_Others::focusOutEvent(QFocusEvent *e) {
  set_bgcolor(this, !text().isEmpty() ? GREEN : WHITE);
  QLineEdit::focusOutEvent(e);
};

///////////////////////////////////////////////////////////////////////////////////////
//
//									CGradientValuesHeader
//
///////////////////////////////////////////////////////////////////////////////////////

CGradientValuesHeader::~CGradientValuesHeader() {
  for (int i = 0; i < m_layout->count(); i++) {
    QLayoutItem *const item = m_layout->itemAt(i);
    if (dynamic_cast<QWidgetItem *>(item))
      delete item->widget();
  }
  delete m_layout;
}

CGradientValuesHeader::CGradientValuesHeader(QWidget *parent) {
  setSizePolicy(QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Fixed);

  m_layout = new QGridLayout(this); // TODO: To be deleted

  int row = 0;
  QLabel *label_h = new QLabel("[center]");
  m_layout->addWidget(label_h, 0, 0, 1, 5, Qt::AlignLeft); // from_row, from_column, row_span, column_span

  QLabel *label_h_2 = new QLabel("[edge]");
  m_layout->addWidget(label_h_2, 0, 5, 1, 6, Qt::AlignRight);

  row = 1;
  for (int i = 0; i < DIVISIONS; i++) {
    QLabel *label = new QLabel(QString::number(i));
    label->setAlignment(Qt::AlignCenter);

    label->setMaximumWidth(WIDTH_MAX);
    label->setFixedWidth(WIDTH_FIXED);

    m_layout->addWidget(label, row, i, Qt::AlignHCenter);
  }
  m_layout->setMargin(0);

  label_h->setToolTip("Center of the refinement box");
  label_h_2->setToolTip("Border of the refinement box");
}

///////////////////////////////////////////////////////////////////////////////////////
//
//									CBoxEdit
//
///////////////////////////////////////////////////////////////////////////////////////

CBoxEdit::~CBoxEdit() {
  for (int i = 0; i < m_layout->count(); i++) {
    QLayoutItem *const item = m_layout->itemAt(i);
    if (dynamic_cast<QWidgetItem *>(item))
      delete item->widget();
  }
  delete m_layout;

  delete m_validator_pos;
  delete m_validator_size;
}

// rw coordinates
T3da CBoxEdit::pos() { return T3da{m_px->text().toDouble(), m_py->text().toDouble(), m_pz->text().toDouble()}; }

// rw coordinates
T3da CBoxEdit::size() { return T3da{m_sx->text().toDouble(), m_sy->text().toDouble(), m_sz->text().toDouble()}; }

// rw coordinates
T3da CBoxEdit::pos_ini() { return m_pos_ini; }

// rw coordinates
T3da CBoxEdit::size_ini() { return m_size_ini; }

void CBoxEdit::closeEvent(QCloseEvent *event) { emit close(); }

void CBoxEdit::update(T3da pos_, T3da size_) {
  T3da pos = RBox::oiv_to_rw(pos_, RBox::Pos);
  T3da size = RBox::oiv_to_rw(size_, RBox::Size);

  m_pos_ini = pos;
  m_size_ini = size;

  // pos
  int i = 0;
  for (auto &field : {m_px, m_py, m_pz})
    field->setText(QString::number(pos[i++]));

  // size
  i = 0;
  for (auto &field : {m_sx, m_sy, m_sz})
    field->setText(QString::number(size[i++]));
}

// context saved in rw coordinates
// TODO: copy / paste doesn't work: nothing to do with the validator
CBoxEdit::CBoxEdit(T3da pos_, T3da size_, QWidget *parent) {
  setWindowTitle(static_cast<CGradientRow *>(parent)->name());

  T3da pos = RBox::oiv_to_rw(pos_, RBox::Pos);
  T3da size = RBox::oiv_to_rw(size_, RBox::Size);

  // save context: rw coordinates
  m_pos_ini = pos;
  m_size_ini = size;

  setSizePolicy(QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Fixed);

  // 2) Create controls

  m_layout = new QGridLayout(this); // TODO: To be deleted

  // labels
  QLabel *l_p = new QLabel("Center:");
  QLabel *l_s = new QLabel("Size:");

  QLabel *l_x = new QLabel("N"); // x
  QLabel *l_y = new QLabel("E"); // y
  QLabel *l_z = new QLabel("D"); // z

  // pos
  m_px = new QLineEdit(QString::number(pos[0]));
  m_py = new QLineEdit(QString::number(pos[1]));
  m_pz = new QLineEdit(QString::number(pos[2]));

  // size
  m_sx = new QLineEdit(QString::number(size[0]));
  m_sy = new QLineEdit(QString::number(size[1]));
  m_sz = new QLineEdit(QString::number(size[2]));

  // Too big: 27 pixels, so better don't calculate teh QLieEdit width this way
  // int max_width_per_char = QFontMetrics( le_sx->font() ).maxWidth();

  // btns
  QPushButton *btn_apply = new QPushButton("Apply");
  QPushButton *btn_ok = new QPushButton("Ok");
  QPushButton *btn_cancel = new QPushButton("Cancel");

  // 2) Layout

  int row = 0;
  // header: N, E, D
  int i = 1;
  for (auto &field : {l_x, l_y, l_z})
    m_layout->addWidget(field, row, i++, Qt::AlignHCenter);

  // position
  row = 1;
  i = 0;
  for (auto &field : std::vector<QWidget *>{l_p, m_px, m_py, m_pz})
    m_layout->addWidget(field, row, i++, Qt::AlignHCenter);

  // size
  row = 2;
  i = 0;
  for (auto &field : std::vector<QWidget *>{l_s, m_sx, m_sy, m_sz})
    m_layout->addWidget(field, row, i++, Qt::AlignHCenter);

  // footer: buttons
  row = 3;
  i = 1;
  for (auto &field : {btn_apply, btn_ok, btn_cancel})
    m_layout->addWidget(field, row, i++, Qt::AlignHCenter);

  // m_layout->setMargin( 2 );

  // 3) Validators
  // 3a) validator: size
  // FIXME: validator doesn't work
  double bottom = 0;
  double top = 1000 * 1000 * 1000;
  m_validator_size = new QDoubleValidator(bottom, top, 2, this);
  for (auto &le : {m_sx, m_sy, m_sz})
    le->setValidator(m_validator_size);

  // 3b) validator: position
  bottom = -1000 * 1000 * 1000;
  top = 1000 * 1000 * 1000;
  m_validator_pos = new QDoubleValidator(bottom, top, 2, this);
  for (auto &le : {m_px, m_py, m_pz})
    le->setValidator(m_validator_pos);

  // 4)Geometry
  // Edit fields geometry: size and position
  for (auto &le : {m_sx, m_sy, m_sz, m_px, m_py, m_pz}) {
    // le->setMaxLength( 50 );
    le->setMaximumWidth(EDIT_MAX_WIDTH);
    le->setFixedWidth(EDIT_MAX_WIDTH);
  }

  // 5) Events

  // 5a) parent
  connect(btn_apply, SIGNAL(clicked()), parent, SLOT(on_apply()));
  connect(btn_ok, SIGNAL(clicked()), parent, SLOT(on_ok()));

  //	only restores values modified from the dlg
  //	if having the dlg opened, coords are modified from the OIV scene, then dlg ini values are updated
  connect(btn_cancel, SIGNAL(clicked()), parent, SLOT(on_cancel()));

  // 5b) self
  connect(this, SIGNAL(close()), parent, SLOT(on_cancel()));
}
