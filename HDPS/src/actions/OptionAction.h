#pragma once

#include "WidgetAction.h"

class QWidget;

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
    public:
        Widget(QWidget* widget, OptionAction* optionAction, const bool& resettable = true);
    };

public:
    OptionAction(QObject* parent, const QString& title = "");

    QWidget* createWidget(QWidget* parent) override;

    QStringList getOptions() const;
    bool hasOptions() const;
    void setOptions(const QStringList& options);

    std::int32_t getCurrentIndex() const;
    void setCurrentIndex(const std::int32_t& currentIndex);

    std::int32_t getDefaultIndex() const;
    void setDefaultIndex(const std::int32_t& defaultIndex);

    bool canReset() const;
    void reset();

    QString getCurrentText() const;
    void setCurrentText(const QString& currentText);

    bool hasSelection() const;

signals:
    void optionsChanged(const QStringList& options);
    void currentIndexChanged(const std::int32_t& currentIndex);
    void defaultIndexChanged(const std::int32_t& defaultIndex);
    void currentTextChanged(const QString& currentText);

protected:
    QStringList     _options;
    std::int32_t    _currentIndex;
    std::int32_t    _defaultIndex;
    QString         _currentText;
};

}
}