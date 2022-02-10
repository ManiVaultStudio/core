#pragma once

class QAbstractItemModel;

namespace hdps
{
    // Takes care of calling itemModel.beginResetModel (in its explicit
    // constructor and itemModel.endResetModel (in its destructor).
    class ModelResetter
    {
        QAbstractItemModel* m_model;

    public:
        // Explicitly delete its copy and move member functions.
        ModelResetter(const ModelResetter&) = delete;
        ModelResetter(ModelResetter&&) = delete;
        ModelResetter& operator=(const ModelResetter&) = delete;
        ModelResetter& operator=(ModelResetter&&) = delete;

        explicit ModelResetter(QAbstractItemModel*);
        void EndResetModel();
        ~ModelResetter();
    };
}

