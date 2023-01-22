#include "StartPageActionDelegate.h"
#include "StartPageActionDelegateEditorWidget.h"

#include <QCoreApplication>

#ifdef _DEBUG
    #define START_PAGE_ACTION_DELEGATE_VERBOSE
#endif

StartPageActionDelegate::StartPageActionDelegate(QObject* parent /*= nullptr*/) :
    QStyledItemDelegate(parent)
{
}

QSize StartPageActionDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    QScopedPointer<StartPageActionDelegateEditorWidget> widget(new StartPageActionDelegateEditorWidget);

    widget->setAttribute(Qt::WA_DontShowOnScreen);

    QCoreApplication::processEvents();

    return QSize(option.rect.width(), widget->sizeHint().height());
}

QWidget* StartPageActionDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    Q_UNUSED(option)
    Q_UNUSED(index)

    return new StartPageActionDelegateEditorWidget(parent);
}

void StartPageActionDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
    qobject_cast<StartPageActionDelegateEditorWidget*>(editor)->setEditorData(index);
}

void StartPageActionDelegate::updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    Q_UNUSED(index)

    editor->setGeometry(option.rect);
}
