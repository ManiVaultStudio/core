#pragma once

#include "OptionAction.h"

#include <QSortFilterProxyModel>

namespace hdps {

namespace gui {

/**
 * Color map action class
 *
 * Extended option action for color map selection and management
 *
 * @author Thomas Kroes
 */
class ColorMapAction : public OptionAction
{
    Q_OBJECT

public:

    /**
     * Color map class
     *
     * Primarily a container for color map related parameters (e.g. name, resource path)
     * This model can contain 1D and 2D color maps (perhaps in the future also 3D)
     *
     * @author Thomas Kroes
     */
    class ColorMap {

    public: // Enumerations

        /**
         * Color map type
         * Defines the color map types
         */
        enum class Type {
            ZeroDimensional,    /** Zero-dimensional color map (not in use at the moment) */
            OneDimensional,     /** One-dimensional color map */
            TwoDimensional      /** Two-dimensional color map */
        };

    public: // Construction/destruction

        /** (Default) constructor
         * @param name Name of the color map
         * @param resourcePath Resource path of the color map image
         * @param name Color map type
         * @param image Color map image
         */
        ColorMap(const QString& name = "", const QString& resourcePath = "", const Type& type = Type::OneDimensional, const QImage& image = QImage());

    public: // Getters

        /** Returns the color map name */
        QString getName() const;

        /** Returns the resource path */
        QString getResourcePath() const;

        /** Returns the color map type */
        Type getType() const;

        /** Returns the color map image */
        QImage getImage() const;

        /** Returns the number of dimensions */
        int getNoDimensions() const;

    private:
        const QString       _name;              /** Name in the user interface */
        const QString       _resourcePath;      /** Resource path of the color image */
        const Type          _type;              /** Color map type */
        const QImage        _image;             /** Color map image */
    };

    /**
     * Color map model class
     *
     * Provides a central place where color maps are stored
     * This model can contain 1D and 2D color maps (perhaps in the future also 3D)
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

            Start = Preview,          /** Column start */
            End = ResourcePath      /** Column End */
        };

    public:

        /** Constructor */
        ColorMapModel(QObject* parent, const ColorMap::Type& type);

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

    private:
        QVector<ColorMap>   _colorMaps;     /** Color maps data */
    };

public:

    /**
     * Color map filter model class
     *
     * A class for filtering out 1D or 2D color maps from the color map model
     *
     * @author Thomas Kroes
     */
    class ColorMapFilterModel : public QSortFilterProxyModel {
    public:

        /** Constructor */
        ColorMapFilterModel(QObject *parent, const ColorMap::Type& type = ColorMap::Type::OneDimensional) :
            QSortFilterProxyModel(parent),
            _type(type)
        {
        }

        /**
         * Returns whether a give row with give parent is filtered out (false) or in (true)
         * @param row Row index
         * @param parent Parent index
         */
        bool filterAcceptsRow(int row, const QModelIndex& parent) const override
        {
            const auto index = sourceModel()->index(row, static_cast<int>(ColorMapModel::Column::NoDimensions));
            return static_cast<int>(_type) == sourceModel()->data(index, Qt::EditRole).toInt();
        }

        /** Returns the type of color map */
        ColorMap::Type type() const {
            return _type;
        }

        /**
         * Sets the type of color map
         * @param type Type of color map (e.g. 1D, 2D)
         */
        void setType(const ColorMap::Type& type) {
            if (type == _type)
                return;

            _type = type;
            invalidateFilter();
        }

    private:
        ColorMap::Type      _type;      /** Type of color map (e.g. 1D, 2D) */
    };

public:

    /**
     * Constructor
     * @param parent Pointer to parent object
     * @param title Title of the action
     * @param colorMap Current color map
     * @param defaultColorMap Default color map
     */
    ColorMapAction(QObject* parent, const QString& title = "", const ColorMap::Type& colorMapType = ColorMap::Type::OneDimensional, const QString& colorMap = "", const QString& defaultColorMap = "");

    /**
     * Initialize the color map action
     * @param colorMap Current color map
     * @param defaultColorMap Default color map
     */
    void initialize(const QString& colorMap = "", const QString& defaultColorMap = "");

public: // Option action wrappers

    /** Gets the current color map name */
    QString getColorMap() const;

    /**
     * Sets the current color map name
     * @param colorMap Name of the current color map
     */
    void setColorMap(const QString& colorMap);

    /** Gets the default color map name */
    QString getDefaultColorMap() const;

    /**
     * Sets the default color map name
     * @param defaultColorMap Name of the default color map
     */
    void setDefaultColorMap(const QString& defaultColorMap);

signals:

    /**
     * Signals that the current color map name changed
     * @param colorMap Current color map name
     */
    void colorMapChanged(const QString& colorMap);

    /**
     * Signals that the default color map name changed
     * @param defaultColorMap Default color map name
     */
    void defaultColorMapChanged(const QString& defaultColorMap);

protected:
    ColorMapFilterModel     _filteredColorMapModel;     /** The filtered color map model (contains either 1D or 2D color maps) */

protected:
    static ColorMapModel colorMapModel;
};

}
}
