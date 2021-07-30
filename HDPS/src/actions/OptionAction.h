#pragma once

#include "WidgetAction.h"

#include <QStringListModel>

class QWidget;
class QComboBox;
class QPushButton;
class QAbstractListModel;

namespace hdps {

namespace gui {

/**
 * Option widget action class
 *
 * Stores options and creates widgets to interact with these
 *
 * @author Thomas Kroes
 */
class OptionAction : public WidgetAction
{
    Q_OBJECT

public:

    class Widget : public WidgetAction::Widget {
    protected:
        Widget(QWidget* parent, OptionAction* optionAction);

    public:
        QHBoxLayout* getLayout() { return _layout; }
        QComboBox* getComboBox() { return _comboBox; }
        QPushButton* getResetPushButton() { return _resetPushButton; }

    protected:
        QHBoxLayout*    _layout;
        QComboBox*      _comboBox;
        QPushButton*    _resetPushButton;

        friend class OptionAction;
    };

protected:
    QWidget* getWidget(QWidget* parent, const Widget::State& state = Widget::State::Standard) {
        return new OptionAction::Widget(parent, this);
    };

public:
    OptionAction(QObject* parent, const QString& title = "", const QStringList& options = QStringList(), const QString& currentOption = "", const QString& defaultOption = "");

    QStringList getOptions() const;
    bool hasOptions() const;
    void setOptions(const QStringList& options);

    void setCustomListModel(QAbstractListModel* listModel);
    bool hasCustomListModel() const;

    std::int32_t getCurrentIndex() const;
    void setCurrentIndex(const std::int32_t& currentIndex);

    std::int32_t getDefaultIndex() const;
    void setDefaultIndex(const std::int32_t& defaultIndex);

    QString getDefaultText() const;
    void setDefaultText(const QString& defaultText);

    bool canReset() const;
    void reset();

    QString getCurrentText() const;
    void setCurrentText(const QString& currentText);

    bool hasSelection() const;

    const QAbstractListModel* getModel() const;;

signals:
    void optionsChanged(const QStringList& options);
    void customListModelChanged(QAbstractListModel* customListModel);
    void currentIndexChanged(const std::int32_t& currentIndex);
    void defaultIndexChanged(const std::int32_t& defaultIndex);
    void currentTextChanged(const QString& currentText);

protected:
    QStringListModel        _defaultModel;          /** Default simple string list model */
    QAbstractListModel*     _customListModel;       /** Custom list model for enriched (combobox) ui */
    std::int32_t            _currentIndex;          /** Currently selected index */
    std::int32_t            _defaultIndex;          /** Default index */
};

}
}