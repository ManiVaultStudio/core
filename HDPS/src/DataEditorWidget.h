#ifndef HDPS_DATA_EDITOR_WIDGET_H
#define HDPS_DATA_EDITOR_WIDGET_H

#include <QWidget>

namespace hdps
{

class Core;

namespace gui
{

/**
 * Data editor widget class
 * Widget class for viewing/editing data
 */
class DataEditorWidget : public QWidget
{
public:
    DataEditorWidget(QWidget* parent, Core* core);
            
    void setDataset(const QString& datasetName);

protected:
    Core*       _core;          /** Pointer to core */
    QWidget*    _dataWidget;    /** Current data widget */
};

}
}

#endif // HDPS_DATA_EDITOR_WIDGET_H
