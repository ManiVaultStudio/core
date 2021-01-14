#pragma once

#include <QString>
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

    State getState() const;

    void setState(const State& state);

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