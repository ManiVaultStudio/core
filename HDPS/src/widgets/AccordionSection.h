#pragma once

#include <QFrame>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QParallelAnimationGroup>
#include <QScrollArea>
#include <QPushButton>
#include <QWidget>

namespace hdps
{

namespace gui
{

/**
 * 
 * Inspired by: https://stackoverflow.com/questions/32476006/how-to-make-an-expandable-collapsable-section-widget-in-qt
 *
 * @author
 */
class AccordionSection : public QWidget
{
    Q_OBJECT

public:
    explicit AccordionSection(QWidget* parent = nullptr);

    QString getName() const;

    QWidget* getWidget();
    void setWidget(QWidget* widget);

public: // Events

    bool eventFilter(QObject* object, QEvent* event);

public: // Expand collapse

    void expand();
    void collapse();

    void setExpanded(const bool& expanded);

    bool isExpanded() const;

private:
    void updateLeftIcon();
    void updateTitleLabel();
    void updateSubtitleLabel();
    void updateRightIcon();
    
signals:
    void expandedChanged(const bool& expanded);

private:
    QVBoxLayout     _mainLayout;
    QFrame          _frame;
    QHBoxLayout     _frameLayout;
    QLabel          _leftIconLabel;
    QLabel          _titleLabel;
    QLabel          _subtitleLabel;
    QLabel          _rightIconLabel;
    QVBoxLayout     _contentLayout;
    QPushButton     _toggleButton;
    QWidget*        _widget;

    static const std::uint32_t ICON_SIZE = 10;
};

}

}