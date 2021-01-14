#pragma once

#include <QString>
#include <QSize>
#include <QVBoxLayout>
#include <QGroupBox>

class WidgetStateMixin
{
public:

    enum class State {
        Undefined = -1,
        Popup,
        Compact,
        Full
    };

public:
    WidgetStateMixin(const QString& title);
    
public: // State management

    virtual State getState(const QSize& sourceWidgetSize) const = 0;
    void setState(const State& state);

protected:

    virtual void updateState() = 0;

public:

    QString getTitle() const;

    template<typename GroupBoxLayoutType>
    std::pair<QVBoxLayout*, GroupBoxLayoutType*> getPopupLayout(QWidget* parent) {
        auto popupLayout    = new QVBoxLayout();
        auto groupBox       = new QGroupBox(parent);
        auto groupBoxLayout = new GroupBoxLayoutType();

        groupBox->setTitle(getTitle());
        groupBox->setLayout(groupBoxLayout);

        popupLayout->addWidget(groupBox);

        return std::pair<QVBoxLayout*, GroupBoxLayoutType*>(popupLayout, groupBoxLayout);
    }

protected:
    State       _state;
    QString     _title;
};