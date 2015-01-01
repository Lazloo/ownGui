#ifndef DIAGRAMIMAGE_H
#define DIAGRAMIMAGE_H
#include <iostream>
#include <QGraphicsPixmapItem>

class diagramImage
{
public:
    diagramImage(QGraphicsPixmapItem *itemImage){
//        std::cout<<"Test4"<<std::endl;
        ModelType = 0;
        MovementType = 0;
        MovementDetail = 0;
        Gravity = 0;
        RelationToMainChara = 0;
        ItemImage = itemImage;
    }
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

    std::size_t getEventIndex(){
        return EventIndex;
    }

    QGraphicsPixmapItem* getItemImage(){
        return ItemImage;
    }
// ------------------------ Sets -----------------------
    void setModelType(std::size_t modelType){
        ModelType = modelType;
    }

    void setMovementType(std::size_t movementType){
        MovementType = movementType;
    }

    void setMovementDetail(double movementDetail){
        MovementDetail = movementDetail;
    }

    void setGravity(bool gravity){
        Gravity = gravity;
    }

    void setRelationToMainChara(std::size_t relationToMainChara){
        RelationToMainChara = relationToMainChara;
    }

    void setEventIndex(std::size_t eventIndex){
        EventIndex = eventIndex;
    }

    void setItemImage(QGraphicsPixmapItem * itemImage){
        ItemImage = itemImage;
    }

private:
    std::size_t ModelType;
    std::size_t MovementType;
    double MovementDetail;
    bool Gravity;
    std::size_t RelationToMainChara;
    std::size_t EventIndex;
    QGraphicsPixmapItem * ItemImage;
};

#endif // DIAGRAMIMAGE_H
