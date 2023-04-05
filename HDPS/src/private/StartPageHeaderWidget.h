#pragma once

#include <QVBoxLayout>
#include <QLabel>

/**
 * Header widget class
 *
 * Widget class for header with logo.
 *
 * @author Thomas Kroes
 */
class StartPageHeaderWidget : public QWidget
{
public:

    /**
     * Construct with \p parent widget
     * @param parent Pointer to parent widget
     */
    StartPageHeaderWidget(QWidget* parent = nullptr);

protected:
    void resizeEvent(QResizeEvent* event) override;
    void showEvent(QShowEvent* event) override;

private:
    void resizeIcon(const QSize& newSize);

protected:
    QVBoxLayout     _layout;        /** Main layout */
    QLabel          _headerLabel;   /** Header label */

private:
    QString         _iconName;      /** Name of background icon */
    int             _previousHeight;/** Name of background icon */
};
