#pragma once

#include "ColorMap.h"

#include <QAbstractListModel>

namespace hdps {

namespace util {

/**
 * Color map model class
 *
 * @author Thomas Kroes
 */
class ColorMapModel : public QAbstractListModel
{
public:

    /**
     * Color map model columns
     * Defines the color map model columns
     */
    enum class Column {
        Preview,                    /** Preview */
        Name,                       /** Name column */
        Image,                      /** Image column */
        NoDimensions,               /** Dimensionality */
        ResourcePath,               /** Resource path */

        Start = Preview,            /** Column start */
        End = ResourcePath          /** Column End */
    };

protected:

    /** Constructor
     * @param parent Parent object
     */
    ColorMapModel(QObject* parent = nullptr);

public: // Inherited MVC

    /**
     * Returns the the number of model columns
     * @param parent Parent index
     */
    int columnCount(const QModelIndex& parent) const override;

    /**
     * Returns the number of color maps in the model
     * @param parent Parent index
     */
    int rowCount(const QModelIndex& parent /* = QModelIndex() */) const override;

    /**
     * Returns model data for the given index
     * @param index Index
     * @param role The data role
     */
    QVariant data(const QModelIndex& index, int role /* = Qt::DisplayRole */) const override;

public: // Miscellaneous

    /** Setups the model data (e.g. loads color maps from resources) */
    void setupModelData();

    /**
     * Returns a color map given a row index
     * @param row Row index
     */
    const ColorMap* getColorMap(const int& row) const;

    static util::ColorMapModel* getGlobalInstance();

private:
    QVector<ColorMap>   _colorMaps;     /** Color maps data */

protected:

    /** Globally available color map model instance */
    static util::ColorMapModel*  globalInstance;
};

}
}
