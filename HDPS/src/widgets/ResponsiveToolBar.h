#pragma once

#include "PopupPushButton.h"

#include <QWidget>
#include <QHBoxLayout>

class QFrame;

namespace hdps {

namespace gui {

class ResponsiveToolBar : public QWidget
{
    Q_OBJECT

public:
    enum class WidgetState {
        Undefined = -1,
        Popup,
        Compact,
        Full
    };

public:
    class QSpacer : public QWidget {
    public:
        QSpacer(QWidget* left, QWidget* right);

        bool eventFilter(QObject* target, QEvent* event);

    private:
        void updateState();

    protected:
        QWidget*        _left;
        QWidget*        _right;
        QHBoxLayout*    _layout;
        QFrame*         _verticalLine;
    };

    class SectionWidget;

    class StatefulWidget : public QWidget {
    public:
        StatefulWidget(QWidget* parent, const QString& name) :
            QWidget(parent),
            _name(name),
            _sizeHintWidget(new QWidget()),
            _sizeHints({ QSize(), QSize(), QSize() })
        {
            setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
            setWindowTitle(_name);
            setToolTip(QString("%1 settings").arg(_name));
        }

        void initialize() {
            _sizeHints[0] = getStateSizeHint(WidgetState::Popup);
            _sizeHints[1] = getStateSizeHint(WidgetState::Compact);
            _sizeHints[2] = getStateSizeHint(WidgetState::Full);
        }

    protected:

        virtual QIcon getIcon() const = 0;

        virtual QLayout* getLayout(const WidgetState& state) = 0;

        void setState(const WidgetState& state) {
            if (layout())
                delete layout();

            auto layout = getLayout(state);

            layout->setMargin(0);
            layout->setSpacing(4);

            setLayout(layout);
        }

        std::int32_t getWidth(const WidgetState& state) const {
            switch (state)
            {
                case WidgetState::Compact:
                case WidgetState::Full:
                    return _sizeHints[static_cast<std::int32_t>(state)].width();

                default:
                    break;
            }

            return 0;
        };

    private:
        QSize getStateSizeHint(const WidgetState& state) {
            if (_sizeHintWidget->layout())
                delete _sizeHintWidget->layout();

            _sizeHintWidget->setLayout(getLayout(state));

            return _sizeHintWidget->sizeHint();
        }

    protected:
        QString         _name;
        QWidget*        _sizeHintWidget;
        QList<QSize>    _sizeHints;

        friend class SectionWidget;
    };

    class SectionWidget : public QWidget {
    public:
        SectionWidget(StatefulWidget* statefulWidget, const std::int32_t& priority = 0) :
            _statefulWidget(statefulWidget),
            _state(WidgetState::Undefined),
            _priority(priority),
            _layout(new QHBoxLayout()),
            _popupPushButton(new PopupPushButton())
        {
            //_statefulWidget->setSectionWidget(this);
            _statefulWidget->initialize();
            
            _popupPushButton->setIcon(_statefulWidget->getIcon());
            _popupPushButton->setWidget(_statefulWidget);

            _layout->setMargin(0);
            _layout->addWidget(_popupPushButton);
            _layout->addWidget(_statefulWidget);

            setLayout(_layout);
            setState(WidgetState::Full);
        }

        WidgetState getState() const {
            return _state;
        }

        void setState(const WidgetState& state) {
            if (state == _state)
                return;

            _state = state;

            _statefulWidget->setVisible(_state != WidgetState::Popup);
            _statefulWidget->setState(_state);

            switch (_state)
            {
                case WidgetState::Popup:
                {
                    _popupPushButton->show();
                    _layout->removeWidget(_statefulWidget);
                    break;
                }

                case WidgetState::Compact:
                case WidgetState::Full:
                {
                    _popupPushButton->hide();

                    _layout->addWidget(_statefulWidget);
                    break;
                }

                default:
                    break;
            }

            _statefulWidget->setState(_state);
        }

        bool isPopup() const {
            return _state == WidgetState::Popup;
        }

        void setPopup(const bool& popup) {
            if (popup == isPopup())
                return;

            setState(WidgetState::Popup);
        }

        bool isCompact() const {
            return _state == WidgetState::Compact;
        }

        void setCompact(const bool& compact) {
            if (compact == isCompact())
                return;

            setState(WidgetState::Compact);
        }

        std::int32_t getWidth(const WidgetState& state) const {
            switch (state)
            {
                case WidgetState::Popup:
                    return _popupPushButton->width();

                case WidgetState::Compact:
                case WidgetState::Full:
                    return _statefulWidget->getWidth(state);

                default:
                    break;
            }

            return 0;
        };

    protected:
        StatefulWidget*     _statefulWidget;
        WidgetState         _state;
        std::int32_t        _priority;
        QHBoxLayout*        _layout;
        PopupPushButton*    _popupPushButton;
    };

public:
    ResponsiveToolBar(QWidget* parent = nullptr);

    void setListenWidget(QWidget* listenWidget);

    bool eventFilter(QObject* target, QEvent* event);

    void addStatefulWidget(StatefulWidget* statefulWidget);
    void addStretch(const std::int32_t& stretch = 0);

private:
    void updateLayout(QWidget* widget = nullptr);
    QList<QWidget*> getVisibleWidgets();

private:
    QWidget*                _listenWidget;      /** TODO */
    QHBoxLayout*            _layout;            /** TODO */
    QList<SectionWidget*>   _sectionWidgets;    /** TODO */

public:
    static const std::int32_t LAYOUT_MARGIN;
    static const std::int32_t LAYOUT_SPACING;
};

}
}