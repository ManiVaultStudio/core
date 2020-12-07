#pragma once

#include <memory>

#include <QWidget>
#include <QMap>

/**
 * TODO
 *
 * @author Thomas Kroes
 */
class SelectionTool : public QObject
{
public:

    /** Types */
    enum class Type
    {
        None = -1,		/** Not set */
        Rectangle,		/** Select points within a rectangle */
        Brush,			/** A brush is used the paint the selection */
        Lasso,			/** A lasso tool is used to enclose points */
        Polygon		    /** Select points in the interior of a polygon */
    };

    /** Maps type name to type enum and vice versa */
    static QMap<QString, Type> const types;

    /** Get string representation of type enum */
    static QString getTypeName(const Type& type) {
        return types.key(type);
    }

    /** Get enum representation from type name */
    static Type getTypeEnum(const QString& typeName) {
        return types[typeName];
    }

    /** Modifiers */
    enum class Modifier
    {
        Replace,        /** Replace selection */
        Add,            /** Add to selection */
        Remove,         /** Remove from selection */
        All,            /** Select all */
        None,           /** Deselect all points */
        Invert          /** Invert point selection */
    };

    /** Maps modifier name to modifier enum and vice versa */
    static QMap<QString, Modifier> const modifiers;

    /** Get string representation of modifier enum */
    static QString getModifierName(const Modifier& modifier) {
        return modifiers.key(modifier);
    }

    /** Get enum representation from modifier name */
    static Modifier getModifierEnum(const QString& modifierName) {
        return modifiers[modifierName];
    }

public: // Construction/destruction

    SelectionTool(QObject* parent);

public: // Getters/setters

    Type getSelectionType() const { return _type; }
    void setSelectionType(const Type& selectionType) { _type = selectionType; }

    Modifier getModifier() const { return _modifier; }
    void setModifier(const Modifier& modifier) { _modifier = modifier; }

    float getBrushRadius() const { return _brushRadius; }
    void setBrushRadius(const float& brushRadius) { _brushRadius = brushRadius; }

public: // Event handling

    bool eventFilter(QObject* target, QEvent* event) override;

protected:
    Type                _type;              /** Current selection type */
    Modifier            _modifier;          /** Selection modifier */
    float               _brushRadius;       /** Brush radius */
    QVector<QPoint>     _mousePositions;    /** Recorded mouse positions */
    int                 _mouseButtons;      /** State of the left, middle and right mouse buttons */

    static constexpr float MIN_BRUSH_RADIUS = 1.0f;         /** Minimum brush radius */
    static constexpr float MAX_BRUSH_RADIUS = 1000.0f;      /** Maximum brush radius */
    static constexpr float DEFAULT_BRUSH_RADIUS = 10.0f;    /** Default brush radius */

};