#pragma once

#include <QDialog>

class ImageCollection;

/**
 * Create subset dialog class
 *
 * Dialog class for controlling data sanitization
 *
  * @author Thomas Kroes
 */
class CreateSubsetDialog : public QDialog
{
public:

    /**
     * Constructor
     * @param imageCollection Image collection
     * @param data Data to sanitize
     * @param parent Parent widget
     */
    SanitizeDataDialog(ImageCollection* imageCollection, std::vector<float>& data, QWidget* parent = nullptr);

    /** Destructor */
    ~SanitizeDataDialog() override = default;

    QSize sizeHint() const override {
        return QSize(400, 0);
    }

protected:

    /** Sanitizes the data (replaces NaN's with zero's) */
    void replaceNans(std::vector<float>& data, const float& replacementValue = 0.0f);
};