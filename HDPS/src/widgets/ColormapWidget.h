#pragma once

#include <QWidget>

class QPushButton;
class QString;

class QPropertyAnimation;
class MaterialCheckBoxWidget;

class ColormapWidget : public QWidget
{
    Q_OBJECT
public:
    ColormapWidget(QWidget *parent = 0);
    ~ColormapWidget();

    void setColormap(int colormapIdx, bool isDiscrete);
    int getNumColormaps();

private:
    void initUI();

    void slideIn();
    void slideOut();

private slots:
    void activeColormapClicked();
    void colormapClicked(int id);
    void discreteColormapClicked(int checked);

signals:
    void colormapSelected(int);
    void discreteSelected(bool);

public:
    bool _isOpen;

private:
    const static int NUM_COLOR_MAPS = 15;

    //QGridLayout* _mainLayout;
    QPropertyAnimation* _slideOutAnimation;

    QPushButton* _colormaps[NUM_COLOR_MAPS];
    QPushButton* _activeColormap;
    QString _colormapNames[NUM_COLOR_MAPS];

    QWidget* _leftBGWidget;
    QWidget* _rightBGWidget;

    int _activeColormapIdx;
    bool _isCMDiscrete;

    QString _subWidgetStyles[2];

    MaterialCheckBoxWidget* _discreteColormapCheckbox;
};
