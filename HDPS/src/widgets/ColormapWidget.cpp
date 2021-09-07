#include "ColormapWidget.h"

#include <QString>
#include <QPushButton>
#include <QLabel>

#include <QPropertyAnimation>
#include <QSignalMapper>

ColormapWidget::ColormapWidget(QWidget *parent) :
    QWidget(parent),
    _isOpen(false),
    _slideOutAnimation(nullptr),
    _activeColormap(nullptr),
    _leftBGWidget(nullptr),
    _rightBGWidget(nullptr),
    _activeColormapIdx(0),
    _isCMDiscrete(false),
    _discreteColormapCheckbox(nullptr)
{
    for (int i = 0; i < NUM_COLOR_MAPS; i++)
    {
        _colormaps[i] = NULL;
    }

    _colormapNames[0] = ":/colormaps/Spectral_vert";

    _colormapNames[1] = ":/colormaps/RdYlBu_vert";
    _colormapNames[2] = ":/colormaps/PiYG_vert";
    _colormapNames[3] = ":/colormaps/PuOr_vert";

    _colormapNames[4] = ":/colormaps/Magma_vert";
    _colormapNames[5] = ":/colormaps/Plasma_vert";
    _colormapNames[6] = ":/colormaps/Viridis_vert";

    _colormapNames[7] = ":/colormaps/RdPu_vert";
    _colormapNames[8] = ":/colormaps/YlGnBu_vert";
    _colormapNames[9] = ":/colormaps/GnBu_vert";
    _colormapNames[10] = ":/colormaps/BuPu_vert";
    _colormapNames[11] = ":/colormaps/YlOrRd_vert";
    _colormapNames[12] = ":/colormaps/Reds_vert";

    _colormapNames[13] = ":/colormaps/qualitative_vert";
    _colormapNames[14] = ":/colormaps/Q_BlGrRd_vert";

    _subWidgetStyles[0] = "QWidget{border: 1px solid rgba(200, 200, 200, 1.0); background-color: rgba(255, 255, 255, 0.02); padding: 0px; margin: 0px;}";
    _subWidgetStyles[0] += "QLabel{border: none; background-color: transparent; text-align: right;}";
    _subWidgetStyles[0] += "QLabel{color: black;}";

    _subWidgetStyles[1] = "QWidget{border: 1px solid rgba(35, 35, 35, 1.0); background-color: rgba(255, 255, 255, 0.02); padding: 0px; margin: 0px;}";
    _subWidgetStyles[1] += "QLabel{border: none; background-color: transparent; text-align: right;}";
    _subWidgetStyles[1] += "QLabel{color: white;}";

    initUI();
}

ColormapWidget::~ColormapWidget()
{

}

void ColormapWidget::initUI()
{
    int widthLeft = 64;
    int widthCenter = 14;
    int WidthRight = NUM_COLOR_MAPS * 36;
    int height = 178;
    int rightHeight = 212;

    setFixedSize(widthLeft + widthCenter + WidthRight, rightHeight);

    _leftBGWidget = new QWidget(this);
    _leftBGWidget->setFixedSize(widthLeft, height);

    QWidget* centerBGWidget = new QWidget(this);
    centerBGWidget->setFixedSize(widthCenter, height);
    centerBGWidget->move(widthLeft, 0);

    _rightBGWidget = new QWidget(this);
    _rightBGWidget->setFixedSize(WidthRight, rightHeight);
    _rightBGWidget->move(widthLeft + widthCenter, 0);

    _leftBGWidget->setStyleSheet(_subWidgetStyles[0]);
    _rightBGWidget->setStyleSheet(_subWidgetStyles[0]);

    QLabel* lowLabel = new QLabel("low");
    lowLabel->setParent(_leftBGWidget);
    lowLabel->setFixedWidth(30);
    lowLabel->setAlignment(Qt::AlignRight);
    lowLabel->move(0, 158);
    QLabel* highLabel = new QLabel("high");
    highLabel->setParent(_leftBGWidget);
    highLabel->setFixedWidth(30);
    highLabel->setAlignment(Qt::AlignRight);
    highLabel->move(0, 5);

    QString discrete = _isCMDiscrete ? "_disc" : "";

    _activeColormap = new QPushButton(_leftBGWidget);
    QString buttonStyle = "QPushButton{background-color: transparent; background-image: url(" + _colormapNames[_activeColormapIdx] + discrete + ".png);}";
    _activeColormap->setFixedSize(24, 168);
    _activeColormap->move(35, 5);
    _activeColormap->setStyleSheet(buttonStyle);

    QSignalMapper* signalMapper = new QSignalMapper(this);
    for (int i = 0; i < NUM_COLOR_MAPS; i++)
    {
        buttonStyle = "QPushButton{background-color: transparent; background-image: url(" + _colormapNames[i] + discrete + ".png);}";

        _colormaps[i] = new QPushButton(_rightBGWidget);
        _colormaps[i]->setFixedSize(24, 168);
        _colormaps[i]->move(5 + i * 36, 5);
        _colormaps[i]->setStyleSheet(buttonStyle);
        QObject::connect(_colormaps[i], &QPushButton::released, signalMapper, static_cast<void (QSignalMapper::*)()>(&QSignalMapper::map));
        signalMapper->setMapping(_colormaps[i], i);
    }
    QObject::connect(_activeColormap, &QPushButton::released, this, &ColormapWidget::activeColormapClicked);
    QObject::connect(signalMapper, static_cast<void (QSignalMapper::*)(int)>(&QSignalMapper::mapped), this, &ColormapWidget::colormapClicked);

    _discreteColormapCheckbox = new QCheckBox("Discrete Colormap", _rightBGWidget);
    _discreteColormapCheckbox->setChecked(false);
    _discreteColormapCheckbox->move(5, 180);
    QObject::connect(_discreteColormapCheckbox, &QCheckBox::stateChanged, this, &ColormapWidget::discreteColormapClicked);

    _slideOutAnimation = new QPropertyAnimation(this, "geometry");
    _slideOutAnimation->setDuration(500);
    _slideOutAnimation->setEasingCurve(QEasingCurve::InOutCubic);
}

QString ColormapWidget::getActiveColormap()
{
    return _colormapNames[_activeColormapIdx];
}

void ColormapWidget::setColormap(int colormapIdx, bool isDiscrete)
{
    QString discrete = isDiscrete ? "_disc" : "";

    _activeColormapIdx = colormapIdx;
    _activeColormap->setStyleSheet("QPushButton{background-color: transparent; background-image: url(" + _colormapNames[_activeColormapIdx] + discrete + ".png);}");

    if (isDiscrete != _isCMDiscrete)
    {
        _isCMDiscrete = isDiscrete;
        _discreteColormapCheckbox->setChecked(_isCMDiscrete);
        for (int i = 0; i < NUM_COLOR_MAPS; i++)
        {
            _colormaps[i]->setStyleSheet("QPushButton{background-color: transparent; background-image: url(" + _colormapNames[i] + discrete + ".png);}");
        }
    }
}

void ColormapWidget::slideIn()
{
    _slideOutAnimation->setDuration(500);
    _slideOutAnimation->setStartValue(QRect(x(), y(), width(), height()));
    _slideOutAnimation->setEndValue(QRect(x() - (NUM_COLOR_MAPS * 36 + 15), y(), width(), height()));

    _slideOutAnimation->start();

    _isOpen = true;

    //_parent->update();
}

void ColormapWidget::slideOut()
{
    _slideOutAnimation->setDuration(500);
    _slideOutAnimation->setStartValue(QRect(x(), y(), width(), height()));
    _slideOutAnimation->setEndValue(QRect(x() + (NUM_COLOR_MAPS * 36 + 15), y(), width(), height()));

    _slideOutAnimation->start();

    _isOpen = false;

    ////_parent->update();
}

void ColormapWidget::activeColormapClicked()
{
    if (_isOpen) { slideOut(); }
    else { slideIn(); }
}

void ColormapWidget::colormapClicked(int id)
{

    setColormap(id, _isCMDiscrete);

    emit colormapSelected(_colormapNames[_activeColormapIdx]);

    slideOut();
}

void ColormapWidget::discreteColormapClicked(int checked)
{
    setColormap(_activeColormapIdx, checked != 0);

    emit discreteSelected(_isCMDiscrete);
}
