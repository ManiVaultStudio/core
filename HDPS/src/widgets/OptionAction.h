#pragma once

#include "WidgetAction.h"

#include <QHBoxLayout>
#include <QComboBox>

class QWidget;

namespace hdps {

namespace gui {

class OptionAction : public WidgetAction
{
    Q_OBJECT

public:
    class Widget : public WidgetAction::Widget {
    public:
        Widget(QWidget* widget, OptionAction* optionAction);

    private:
        QHBoxLayout     _layout;
        QComboBox       _comboBox;
    };

public:
    OptionAction(QObject* parent, const QString& title = "");

    QWidget* createWidget(QWidget* parent) override;

    QStringList getOptions() const;
    void setOptions(const QStringList& options);

    std::uint32_t getCurrentIndex() const;
    void setCurrentIndex(const std::uint32_t& currentIndex);

    QString getCurrentText() const;
    void setCurrentText(const QString& currentText);

signals:
    void optionsChanged(const QStringList& options);
    void currentIndexChanged(const std::uint32_t& currentIndex);
    void currentTextChanged(const QString& currentText);

protected:
    QStringList     _options;
    std::uint32_t   _currentIndex;
    QString         _currentText;
};

}
}