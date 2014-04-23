/***************************************************************************
 *   Copyright (C) 2014 by Savoir-Faire Linux                              *
 *   Author : Emmanuel Lepage Vallee <emmanuel.lepage@savoirfairelinux.com>*
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>. *
 **************************************************************************/
#include "securityprogress.h"

//Qt
#include <QtGui/QPainter>
#include <QtGui/QPalette>
#include <QtGui/QApplication>
#include <QtGui/QLinearGradient>
#include <QtGui/QHBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QToolButton>
#include <QtGui/QListView>
#include <QtGui/QLineEdit>
#include <QtCore/QDebug>

//KDE
#include <KLocale>
#include <KIcon>

class SecurityProgress : public QProgressBar
{
   Q_OBJECT
public:
   explicit SecurityProgress(QWidget* parent = nullptr);
   virtual ~SecurityProgress();

protected:
   //Virtual events
   void paintEvent ( QPaintEvent*  event);

private:
   //Attributes
   QStringList m_Names;
   QList<QColor> m_lColors;
   QList<QColor> m_lAltColors;

};

SecurityProgress::SecurityProgress(QWidget* parent) : QProgressBar(parent)
{
   setMaximum(5);
   setValue(3);
   setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::Fixed ));
   m_Names << "Weak" << "Moderate" << "Good" << "Strong" << "Complete";
   m_lColors << "#0EA02B" << "#6DA00F" << "#CBC910" << "#A05C0F" << "#A02111";
   foreach(const QColor& col,m_lColors) {
      QColor newCol = col;
      newCol.setRed  (newCol.red()  -55);
      newCol.setGreen(newCol.green()-55);
      newCol.setBlue (newCol.blue() -55);
      m_lAltColors << newCol;
   }
}

SecurityProgress::~SecurityProgress()
{
   
}

void SecurityProgress::paintEvent(QPaintEvent* event)
{
   Q_UNUSED(event)
   // Setup
   QPainter painter(this);
   painter.setRenderHint  (QPainter::Antialiasing, true   );
   const QRect rect(2,2,width()-4,height()-4);

   // Draw the background
   QLinearGradient decoGradient1;
   decoGradient1.setStart(rect.topLeft());
   decoGradient1.setFinalStop(rect.bottomLeft());
   QColor bg = QApplication::palette().base().color();
   const char dark = ((bg.red() + bg.green() + bg.blue())/3.0) < 127?1:-1;
   bg.setRed  (bg.red  ()+(35*dark));
   bg.setGreen(bg.green()+(35*dark));
   bg.setBlue (bg.blue ()+(35*dark));
   decoGradient1.setColorAt(0, Qt::transparent);
   decoGradient1.setColorAt(1, bg);
   painter.setBrush(decoGradient1);
   QColor bg2 = bg;
   bg2.setRed  (bg.red  ()+(75*dark));
   bg2.setGreen(bg.green()+(75*dark));
   bg2.setBlue (bg.blue ()+(75*dark));
   painter.setPen(Qt::transparent);
   painter.drawRoundedRect(rect,7,7);

   // Draw the current status
   QLinearGradient decoGradient2;
   decoGradient2.setStart(rect.topLeft());
   decoGradient2.setFinalStop(rect.bottomLeft());
   decoGradient2.setColorAt(0, m_lColors[value()-1]);
   decoGradient2.setColorAt(1, m_lAltColors[value()-1]);
   const QRect progress(rect.x(),rect.y(),rect.width()*(((float)value())/((float)maximum()))-1,rect.height());
   painter.setBrush(decoGradient2);
   painter.drawRoundedRect(progress,7,7);
   painter.drawRect(QRect(progress.x()+progress.width()-10,progress.y(),10,progress.height()));

   // Draw the bars
   const int stepWidth = width()/maximum();
   QPen pen = painter.pen();
   pen.setWidth(1);
//    QFont f = painter.font();
//    f.setBold(true);
//    painter.setFont(f);
   for (int i=0;i<maximum();i++) {
      painter.setOpacity(0.5);
      const int x = stepWidth*(i+1);

      //Draw some separators
      if (i < maximum()-1) {
         pen.setColor(bg2);
         painter.setRenderHint(QPainter::Antialiasing, false);
         painter.setPen(pen);
         painter.drawLine(x,4,x,height()-5);
         pen.setColor(bg);
         painter.setPen(pen);
         painter.drawLine(x+1,4,x+1,height()-5);
      }

      // Draw the labels
      painter.setOpacity(1);
      painter.setPen(QApplication::palette().text().color());
      painter.setRenderHint(QPainter::Antialiasing, true);
      painter.drawText(QRect(x-stepWidth,rect.y(),stepWidth,rect.height()),Qt::AlignVCenter|Qt::AlignHCenter,m_Names[i]);
   }

   // Draw the border
   painter.setBrush(Qt::transparent);
   painter.setOpacity(1);
   painter.setPen(bg2);
   painter.drawRoundedRect(rect,7,7);

}

SecurityLevelWidget::SecurityLevelWidget(QWidget* parent) : QWidget(parent),m_pModel(nullptr)
{
   QGridLayout* l = new QGridLayout(this);
   l->setContentsMargins(0,0,0,0);
   l->setSpacing(0);

   m_pLevel = new SecurityProgress(this);
   l->addWidget(m_pLevel,0,0);

   QHBoxLayout* labels = new QHBoxLayout(nullptr);
   l->addLayout(labels,1,0);

   QLabel* infoIcon    = new QLabel(this);
   QLabel* warningIcon = new QLabel(this);
   QLabel* issueIcon   = new QLabel(this);
   QLabel* errorIcon   = new QLabel(this);

   m_pInfoL    = new QLabel(this);
   m_pWarningL = new QLabel(this);
   m_pIssueL   = new QLabel(this);
   m_pErrorL   = new QLabel(this);

   infoIcon->   setPixmap(KIcon("dialog-information").pixmap(QSize(16,16)));
   warningIcon->setPixmap(KIcon("dialog-warning"    ).pixmap(QSize(16,16)));
   issueIcon->  setPixmap(KIcon("task-attempt"      ).pixmap(QSize(16,16)));
   errorIcon->  setPixmap(KIcon("dialog-error"      ).pixmap(QSize(16,16)));

   m_pInfoL   ->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Preferred));
   m_pWarningL->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Preferred));
   m_pIssueL  ->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Preferred));
   m_pErrorL  ->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Preferred));

   labels->addWidget ( errorIcon   );
   labels->addWidget ( m_pErrorL   );
   labels->addWidget ( issueIcon   );
   labels->addWidget ( m_pIssueL   );
   labels->addWidget ( warningIcon );
   labels->addWidget ( m_pWarningL );
   labels->addWidget ( infoIcon    );
   labels->addWidget ( m_pInfoL    );

   m_pView = new QListView(this);
   m_pView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
   m_pView->setWordWrap(true);
   m_pView->setMinimumSize(0,125);
   m_pView->setMaximumSize(99999999,125);
   m_pView->setVisible(false);
   connect(m_pView,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(dblClicked(QModelIndex)));
   l->addWidget(m_pView,2,0);
//    QSize prog = m_pLevel->sizeHint();
//    setMinimumSize(0,prog.height()+125);
   setLayout(l);
   reloadCount();
}

SecurityLevelWidget::~SecurityLevelWidget()
{
   
}

void SecurityLevelWidget::setModel(SecurityValidationModel* model)
{
   m_pView->setModel(model);
   m_pModel = model;
   reloadCount();
   connect(model,SIGNAL(layoutChanged()),this,SLOT(reloadCount()));
}

QListView* SecurityLevelWidget::view() const
{
   return m_pView;
}

void SecurityLevelWidget::mouseReleaseEvent(QMouseEvent * event)
{
   Q_UNUSED(event)
   m_pView->setVisible(!m_pView->isVisible());
}

void SecurityLevelWidget::reloadCount()
{
   if (! m_pModel) return;
   int severity[5] = {0,0,0,0,0};
   foreach(const Flaw* flaw, m_pModel->currentFlaws()) {
      severity[(int)flaw->severity()]++;
   }
   m_pInfoL   ->setText(i18np("%1 tip","%1 tips",severity[(int)SecurityValidationModel::Severity::INFORMATION]));
   m_pWarningL->setText(i18np("%1 warning","%1 warnings",
        severity[(int)SecurityValidationModel::Severity::WARNING      ]
      + severity[(int)SecurityValidationModel::Severity::FATAL_WARNING]
   ));
   m_pIssueL  ->setText(i18np("%1 issue","%1 issues",severity[(int)SecurityValidationModel::Severity::ISSUE]));
   m_pErrorL  ->setText(i18np("%1 error","%1 errors",severity[(int)SecurityValidationModel::Severity::ERROR]));
}

void SecurityLevelWidget::dblClicked(const QModelIndex& idx)
{
   if (idx.isValid()) {
      m_pModel->currentFlaws()[idx.row()]->slotRequestHighlight();
   }
}

class IssueButton : public QToolButton
{
   Q_OBJECT
public:
   IssueButton(const Flaw* flaw, QWidget* parent);
   const Flaw* m_pFlaw;
public Q_SLOTS:
   void slotHighlightFlaw();
};

IssueButton::IssueButton(const Flaw* flaw, QWidget* parent):
QToolButton(parent),m_pFlaw(flaw)
{
   connect(flaw,SIGNAL(requestHighlight()),this,SLOT(slotHighlightFlaw()));
}

IssuesIcon::IssuesIcon(QWidget* parent) : QWidget(parent),m_pBuddy(nullptr),
m_pLayout(new QHBoxLayout(this)),m_pModel(nullptr)
{
   
}

IssuesIcon::~IssuesIcon()
{
   
}

QWidget* IssuesIcon::buddy() const
{
   return m_pBuddy;
}

void IssueButton::slotHighlightFlaw()
{
   Flaw* flaw = qobject_cast<Flaw*>(sender());
   if (flaw == m_pFlaw) {
      IssuesIcon* par = static_cast<IssuesIcon*>(parentWidget());
      if (par->buddy()) {
         par->buddy()->setFocus(Qt::OtherFocusReason);
      }
      else {
         par->parentWidget()->setFocus(Qt::OtherFocusReason);
      }
   }
}

void IssuesIcon::setModel(SecurityValidationModel* model)
{
   reset();
   m_pModel = model;
}

void IssuesIcon::setBuddy(QWidget* buddy)
{
   m_pBuddy = buddy;
}

void IssuesIcon::addFlaw(const Flaw* flaw)
{
   Q_UNUSED(flaw)
   IssueButton* btn = new IssueButton(flaw,this);
   switch (flaw->severity()) {
      case SecurityValidationModel::Severity::INFORMATION:
         btn->setIcon(KIcon("dialog-information"));
         break;
      case SecurityValidationModel::Severity::WARNING:
      case SecurityValidationModel::Severity::FATAL_WARNING:
         btn->setIcon(KIcon("dialog-warning"));
         break;
      case SecurityValidationModel::Severity::ISSUE:
         btn->setIcon(KIcon("task-attempt"));
         break;
      case SecurityValidationModel::Severity::ERROR:
         btn->setIcon(KIcon("dialog-error"));
         break;
   };

   btn->setToolButtonStyle(Qt::ToolButtonIconOnly);
   btn->setStyleSheet("border:0px;background-color:transparent;margin:0px;padding:0px;");
   m_pLayout->addWidget(btn);
   connect(flaw,SIGNAL(solved()),this,SLOT(slotSolved()));
   connect(btn,SIGNAL(clicked()),this,SLOT(slotFlawClicked()));

   QLineEdit* le = qobject_cast<QLineEdit*>(parentWidget());
   if (le) {
      static const int margin = style()->pixelMetric(QStyle::PM_FocusFrameHMargin)/2;
      const QSize s = sizeHint();
      resize(s.width(),le->height()-2*margin);
      move(le->width()-s.width(),margin);
   }
}

void IssuesIcon::slotFlawClicked()
{
   IssueButton* btn = qobject_cast<IssueButton*>(sender());
   if (btn) {
      selectFlaw(m_pModel->getIndex(btn->m_pFlaw));
   }
}

void IssuesIcon::setupForLineEdit(QLineEdit* le)
{
   le->installEventFilter(this);
}

void IssuesIcon::reset()
{
   while (layout()->count()) {
      QWidget* w = layout()->takeAt(0)->widget();
      if (w) {
         delete w;
      }
   }
}

bool IssuesIcon::eventFilter(QObject *obj, QEvent *event)
{
   QLineEdit* le = qobject_cast<QLineEdit*>(obj);
   if (le && event->type() == QEvent::Resize) {
      static const int margin = style()->pixelMetric(QStyle::PM_FocusFrameHMargin)/2;
      const QSize s = sizeHint();
      resize(s.width(),le->height()-2*margin);
      move(le->width()-s.width(),margin);
   }
   return false;
}


void IssuesIcon::slotSolved()
{
   Flaw* f = qobject_cast<Flaw*>(sender());
   if (f) {
      //TODO
   }
}

#include "moc_securityprogress.cpp"
#include "securityprogress.moc"
