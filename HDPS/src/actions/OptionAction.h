#pragma once

#include "WidgetAction.h"

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
    OptionAction(QObject* parent, const QString& title = "");

    QStringList getOptions() const;
    bool hasOptions() const;
    void setOptions(const QStringList& options);
    QAbstractListModel* getModel();
    void setModel(QAbstractListModel* listModel);
    bool hasModel() const;

    std::int32_t getCurrentIndex() const;
    void setCurrentIndex(const std::int32_t& currentIndex);

    std::int32_t getDefaultIndex() const;
    void setDefaultIndex(const std::int32_t& defaultIndex);

    bool canReset() const;
    void reset();
    void clearOptions();

    QString getCurrentText() const;
    void setCurrentText(const QString& currentText);

    bool hasSelection() const;

    OptionAction& operator= (const OptionAction& other)
    {
        WidgetAction::operator=(other);

        _options        = other._options;
        _model          = other._model;
        _currentIndex   = other._currentIndex;
        _defaultIndex   = other._defaultIndex;
        _currentText    = other._currentText;

        return *this;
    }

signals:
    void optionsChanged(const QStringList& options);
    void modelChanged(QAbstractListModel* listModel);
    void currentIndexChanged(const std::int32_t& currentIndex);
    void defaultIndexChanged(const std::int32_t& defaultIndex);
    void currentTextChanged(const QString& currentText);

protected:
    QStringList             _options;
    QAbstractListModel*     _model;
    std::int32_t            _currentIndex;
    std::int32_t            _defaultIndex;
    QString                 _currentText;
};

}
}