#pragma once

#include <QStackedWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QGroupBox>

class ScatterplotPlugin;

template<typename SectionWidget>
class ResponsiveSectionWidget : public QStackedWidget
{
public:
    ResponsiveSectionWidget(QWidget* parent) :
        QStackedWidget(parent),
        _sectionWidget(new SectionWidget(this)),
        _popupPushButton(new QPushButton()),
        _popupWidget(new QWidget()),
        _popupGroupBox(new QGroupBox()),
        _popupLayout(new QVBoxLayout()),
        _icon(),
        _text(),
        _tooltip(),
        _simplificationThreshold(500)
    {
        layout()->setMargin(0);

        setFixedHeight(22);

        _popupPushButton->setIconSize(QSize(12, 12));
        _popupPushButton->setFixedSize(QSize(22, 22));

        _popupWidget->setWindowFlags(Qt::Popup);
        _popupWidget->setObjectName("PopupWidget");
        _popupWidget->setStyleSheet("QWidget#PopupWidget { border: 1px solid grey; }");
        _popupWidget->setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum));

        auto layout = new QVBoxLayout();

        layout->setMargin(7);
        layout->addWidget(_popupGroupBox);

        _popupWidget->setLayout(layout);
        _popupGroupBox->setLayout(_popupLayout);

        addWidget(_popupPushButton);
        addWidget(_sectionWidget);
    }

    void initialize(const ScatterplotPlugin& plugin, const std::uint32_t& simplificationThreshold, const QIcon& icon, const QString& sectionTitle, const QString& tooltip)
    {
        const_cast<ScatterplotPlugin&>(plugin).installEventFilter(this);

        _simplificationThreshold = simplificationThreshold;

        _popupPushButton->setIcon(icon);
        _popupPushButton->setToolTip(tooltip);

        _popupGroupBox->setTitle(sectionTitle);

        _sectionWidget->initialize(plugin);

        QObject::connect(_popupPushButton, &QPushButton::clicked, [this]() {
            _popupWidget->show();
        });

        targetWidthChanged(plugin.size().width());
    }

    /** Get simplification threshold */
    std::uint32_t getSimplificationThreshold() const {
        return _simplificationThreshold;
    }

    /** Set simplification threshold */
    void setSimplificationThreshold(const std::uint32_t& simplificationThreshold) {
        _simplificationThreshold = simplificationThreshold;
    }
    
    /** Returns the section widget */
    SectionWidget* getSectionWidget() {
        return _sectionWidget;
    }

public: // Make sure the stacked widget takes on the size of the currently visible widget

    QSize sizeHint() const override {
        return currentWidget()->sizeHint();
    }

    QSize minimumSizeHint() const override {
        return currentWidget()->minimumSizeHint();
    }

public: // Listen to events from the target widget

    /**
     * Listens to the events of \p target
     * @param target Target object to watch for events
     * @param event Event that occurred
     */
    bool eventFilter(QObject* target, QEvent* event) override {
        switch (event->type())
        {
            case QEvent::Resize:
                targetWidthChanged(static_cast<QResizeEvent*>(event)->size().width());
                break;
        }

        return QWidget::eventFilter(target, event);
    };

private:

    /**
     * 
     */
    void targetWidthChanged(const std::uint32_t& targetWidth) {
        const auto compact = targetWidth < _simplificationThreshold;

        if (compact) {
            setCurrentIndex(0);
            removeWidget(_sectionWidget);

            _popupWidget->move(mapToGlobal(_popupPushButton->rect().bottomLeft()) - QPoint(width(), 0));
            _popupLayout->addWidget(_sectionWidget);

            _sectionWidget->show();
        }
        else {
            _popupLayout->removeWidget(_sectionWidget);

            addWidget(_sectionWidget);
            setCurrentIndex(1);
        }
    }

private:
    SectionWidget*      _sectionWidget;
    QPushButton*        _popupPushButton;
    QWidget*            _popupWidget;
    QGroupBox*          _popupGroupBox;
    QVBoxLayout*        _popupLayout;
    QIcon               _icon;
    QString             _text;
    QString             _tooltip;
    std::uint32_t       _simplificationThreshold;
};