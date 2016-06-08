/****************************************************************************
 *   Copyright (C) 2012-2015 by Savoir-Faire Linux                          *
 *   Author : Emmanuel Lepage Vallee <elv1313@gmail.com>                    *
 *                                                                          *
 *   This library is free software; you can redistribute it and/or          *
 *   modify it under the terms of the GNU Lesser General Public             *
 *   License as published by the Free Software Foundation; either           *
 *   version 2.1 of the License, or (at your option) any later version.     *
 *                                                                          *
 *   This library is distributed in the hope that it will be useful,        *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU      *
 *   Lesser General Public License for more details.                        *
 *                                                                          *
 *   You should have received a copy of the GNU General Public License      *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 ***************************************************************************/

#include "dlgaccessibility.h"
#include "klib/kcfg_settings.h"

//Qt
#include <QtWidgets/QStyledItemDelegate>
#include <QtGui/QIcon>

//KDE
#include <KConfigDialog>
#include <klocalizedstring.h>

//Ring
#include <macromodel.h>
#include <macro.h>
#include "mainwindow.h"
#include <conf/account/delegates/categorizeddelegate.h>

///Constructor
DlgAccessibility::DlgAccessibility(KConfigDialog* parent)
 : QWidget(parent),m_Changed(false)
{
   setupUi(this);

#ifdef HAVE_SPEECH
   m_pT2SBG->setEnabled(true);
#endif

   m_pAddTB->setIcon    ( QIcon::fromTheme( QStringLiteral("list-add")    ) );
   m_pRemoveTB->setIcon ( QIcon::fromTheme( QStringLiteral("list-remove") ) );
   m_pInfoIconL->setPixmap(QIcon::fromTheme(QStringLiteral("dialog-information")).pixmap(QSize(24,24)));
   m_pInfoL->setText(i18n("This page allows to create macros that can then be called using keyboard shortcuts or added to the toolbar. To create one, "
   "assign a name and a character sequence. The sequence can be numeric or any character than can be interpreted as one (ex: \"A\" would be interpreted as 2)"));

   //Need to be done before the connection to avoid corruption
   m_pCategoryCBB->setModel(&MacroModel::instance());

   connect(m_pNameLE        , &QLineEdit::textChanged , this,&DlgAccessibility::changed     );
   connect(m_pCategoryCBB->lineEdit()   , &QLineEdit::textChanged ,this,&DlgAccessibility::changed);
   connect(m_pDelaySB       , SIGNAL(valueChanged(int))    , this,SLOT(changed())     );
   connect(m_pSequenceLE    , &QLineEdit::textChanged , this,&DlgAccessibility::changed     );
   connect(m_pDescriptionLE , &QLineEdit::textChanged , this,&DlgAccessibility::changed     );
   connect(m_pAddTB         , &QAbstractButton::clicked            , this,&DlgAccessibility::addMacro    );
   connect(m_pRemoveTB      , &QAbstractButton::clicked            , this,&DlgAccessibility::removeMacro );

   connect(m_pDelaySB       , SIGNAL(valueChanged(int))    , this,SLOT(slotDelaySB(int))      );
   connect(m_pNameLE        , &QLineEdit::textChanged , this,&DlgAccessibility::slotNameLE        );
   connect(m_pCategoryCBB->lineEdit()   , &QLineEdit::textChanged , this,&DlgAccessibility::slotCategoryCBB   );
   connect(m_pSequenceLE    , &QLineEdit::textChanged , this,&DlgAccessibility::slotSequenceLE    );
   connect(m_pDescriptionLE , &QLineEdit::textChanged , this,&DlgAccessibility::slotDescriptionLE );
   connect(m_pShortcuts     , &KShortcutWidget::shortcutChanged , this, &DlgAccessibility::slotShortcut);

   connect(this , SIGNAL(updateButtons()) , parent,SLOT(updateButtons()) );

   connect(&MacroModel::instance(),&MacroModel::selectMacro,this,&DlgAccessibility::selectMacro);
   connect(&MacroModel::instance(),&QAbstractItemModel::layoutChanged,m_pMacroListTV,&QTreeView::expandAll);
   m_pMacroListTV->setModel(&MacroModel::instance());
   m_pMacroListTV->expandAll();
   connect(m_pMacroListTV->selectionModel(),&QItemSelectionModel::currentChanged,&MacroModel::instance(),&MacroModel::setCurrent);

   m_pCategoryDelegate = new CategorizedDelegate(m_pMacroListTV);
   m_pItemDelegate     = new QStyledItemDelegate;
   m_pCategoryDelegate->setChildDelegate(m_pItemDelegate);
   m_pMacroListTV->setItemDelegate(m_pCategoryDelegate);
}

///Destructor
DlgAccessibility::~DlgAccessibility()
{
   m_pCategoryDelegate->setChildDelegate(nullptr);
   delete m_pItemDelegate;
   delete m_pCategoryDelegate;
}

///Save
void DlgAccessibility::updateSettings()
{
   MacroModel::instance().save();
   m_Changed = false;
   emit updateButtons();
}

///Load
void DlgAccessibility::updateWidgets()
{
   
}

void DlgAccessibility::changed()
{
   m_Changed = true;
   emit updateButtons();
}

bool DlgAccessibility::hasChanged()
{
   return m_Changed;
}

void DlgAccessibility::addMacro()
{
   Macro* ret = MacroModel::instance().newMacro();
   if (ret) {
      m_pMacroFrm->setEnabled(true);
   }
}

void DlgAccessibility::removeMacro()
{
   MacroModel::instance().removeMacro(m_pMacroListTV->selectionModel()->currentIndex());
}

void DlgAccessibility::selectMacro(Macro* macro)
{
   if (macro) {
      m_pMacroListTV->expandAll();
      m_pNameLE->setText(macro->name());

      const QModelIndex& idx = macro->index();

      if (idx.parent().isValid())
         m_pCategoryCBB->setCurrentIndex(idx.parent().row());
      else
         m_pCategoryCBB->lineEdit()->setText(macro->category());

      m_pDelaySB->setValue(macro->delay());
      m_pSequenceLE->setText(macro->sequence());
      m_pDescriptionLE->setText(macro->description());

      // Load the shortcut
      QAction* a = qvariant_cast<QAction*>(macro->action());

      if (a) {
         static KActionCollection* c = MainWindow::app()->actionCollection();
         m_pShortcuts->setCheckActionCollections({c});
         m_pShortcuts->setShortcut({a->shortcut()});
      }

      m_pMacroFrm->setEnabled(true);
      m_pNameLE->selectAll();
      m_pNameLE->setFocus();
      QModelIndex newIdx = macro->index();
      if (newIdx != m_pMacroListTV->selectionModel()->currentIndex()) {
         m_pMacroListTV->selectionModel()->setCurrentIndex(newIdx,QItemSelectionModel::ClearAndSelect);
      }
   }
}

//Widget change
void DlgAccessibility::slotNameLE(const QString& newText)
{
   Macro* current = MacroModel::instance().getCurrentMacro();
   if (current) {
      current->setName(newText);
   }
}

void DlgAccessibility::slotCategoryCBB(const QString& newText)
{
   Macro* current = MacroModel::instance().getCurrentMacro();
   if (current) {
      current->setCategory(newText);
   }
}

void DlgAccessibility::slotDelaySB(int newValue)
{
   Macro* current = MacroModel::instance().getCurrentMacro();
   if (current) {
      current->setDelay(newValue);
   }
}

void DlgAccessibility::slotSequenceLE(const QString& newText)
{
   Macro* current = MacroModel::instance().getCurrentMacro();
   if (current) {
      current->setSequence(newText);
   }
}

void DlgAccessibility::slotDescriptionLE(const QString& newText)
{
   Macro* current = MacroModel::instance().getCurrentMacro();
   if (current) {
      current->setDescription(newText);
   }
}

void DlgAccessibility::slotShortcut(const QList<QKeySequence>& cut)
{
   Macro* current = MacroModel::instance().getCurrentMacro();
   if (current && qvariant_cast<QAction*>(current->action()) && cut.size())
      qvariant_cast<QAction*>(current->action())->setShortcut(cut[0]);
}
