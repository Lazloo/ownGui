#ifndef DIAGRAMIMAGE_H
#define DIAGRAMIMAGE_H

#include <QObject>

class diagramImage : public QPixmap
{
    Q_OBJECT
public:
    explicit diagramImage(QObject *parent = 0);


// ------------------------ Gets -----------------------
    std::size_t getModelType(){
        return ModelType;
    }
    std::size_t getMovementType(){
        return MovementType;
    }
    double getMovementDetail(){
        return MovementDetail;
    }
    bool getGravity(){
        return Gravity;
    }
    std::size_t getRelationToMainChara(){
        return RelationToMainChara;
    }
// ------------------------ Sets -----------------------
signals:

public slots:

private:
    std::size_t ModelType;
    std::size_t MovementType;
    double MovementDetail;
    bool Gravity;
    std::size_t RelationToMainChara;
};

#endif // DIAGRAMIMAGE_H
