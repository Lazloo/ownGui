/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Digia Plc and its Subsidiary(-ies) nor the names
**     of its contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "diagramscene.h"
#include "diagramimage.h"
#include "arrow.h"
#include <iostream>
#include <QTextCursor>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QGraphicsPixmapItem>

//! [0] The scene uses myItemMenu to set the context menu when it creates
//! DiagramItems. We set the default mode to DiagramScene::MoveItem as this
//! gives the default behavior of QGraphicsScene.
DiagramScene::DiagramScene(QMenu *itemMenu, QObject *parent) : QGraphicsScene(parent)
{
    myItemMenu = itemMenu;
    myMode = MoveItem;
    myItemType = DiagramItem::MainChara;
    line = 0;
    textItem = 0;
    myItemColor = Qt::white;
    myTextColor = Qt::black;
    myLineColor = Qt::black;
    EndOfLine = false;

}
//! [0]

//! [1] The isItemChange function returns true if an Arrow item is selected in the scene in which case we want to change its color. When the DiagramScene creates and adds new arrows to the scene it will also use the new color.
void DiagramScene::setLineColor(const QColor &color)
{
    myLineColor = color;
    if (isItemChange(Arrow::Type)) {
        Arrow *item = qgraphicsitem_cast<Arrow *>(selectedItems().first());
        item->setColor(myLineColor);
        update();
    }
}
//! [1]

//! [2]
void DiagramScene::setTextColor(const QColor &color)
{
    myTextColor = color;
    if (isItemChange(DiagramTextItem::Type)) {
        DiagramTextItem *item = qgraphicsitem_cast<DiagramTextItem *>(selectedItems().first());
        item->setDefaultTextColor(myTextColor);
    }
}
//! [2]

//! [3]
void DiagramScene::setItemColor(const QColor &color)
{
    myItemColor = color;
    if (isItemChange(DiagramItem::Type)) {
        DiagramItem *item = qgraphicsitem_cast<DiagramItem *>(selectedItems().first());
        item->setBrush(myItemColor);
    }
}
//! [3]

//! [4]
// Set the font to use for new and selected, if a text item is selected, DiagramTextItems.
void DiagramScene::setFont(const QFont &font)
{
    myFont = font;

    if (isItemChange(DiagramTextItem::Type)) {
        QGraphicsTextItem *item = qgraphicsitem_cast<DiagramTextItem *>(selectedItems().first());
        //At this point the selection can change so the first selected item might not be a DiagramTextItem
        if (item)
            item->setFont(myFont);
    }
}
//! [4]

void DiagramScene::setMode(Mode mode)
{
    myMode = mode;
}

void DiagramScene::setItemType(DiagramItem::DiagramType type)
{
    myItemType = type;
}

//! [5]
// DiagramTextItems emit a signal when they lose focus, which is connected to this slot. We remove the item if it has no text. If not, we would leak memory and confuse the user as the items will be edited when pressed on by the mouse.
void DiagramScene::editorLostFocus(DiagramTextItem *item)
{
    QTextCursor cursor = item->textCursor();
    cursor.clearSelection();
    item->setTextCursor(cursor);

    if (item->toPlainText().isEmpty()) {
        removeItem(item);
        item->deleteLater();
    }
}
//! [5]

//! [6]
// The mousePressEvent() function handles mouse press event's different depending on which mode the DiagramScene is in. We examine its implementation for each mode
void DiagramScene::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    if (mouseEvent->button() != Qt::LeftButton)
        return;

    // Using the constructor of DiagramItem in order to create the Qpixmap
    DiagramItem item = DiagramItem(myItemType, myItemMenu);
    // Create Pixmap Scene Item which must be allocated inorder to stay in the scene even after leaving the context of this function
    QGraphicsPixmapItem * itemImage;
    diagramImage * newImage;

//    DiagramImage * itemImage;
    // Vector containing the poistion of the model. Needed for the emitting
    std::vector<std::vector<double>> posVec(1,std::vector<double>(2,0));
    switch (myMode) {
        // We simply create a new DiagramItem and add it to the scene at the position the mouse was pressed. Note that the origin of its
        // local coordinate system will be under the mouse pointer position.
        case InsertItem:


//            std::cout<<"Test:"<<objectImage.getGravity()<<std::endl;

            // Define QGraphicsPixmapItem
            itemImage = new QGraphicsPixmapItem(item.image());
//            itemImage->
            // Move Item to the current mouse position and center it afterwards
            itemImage->setPos(mouseEvent->scenePos());
            itemImage->setPos(itemImage->x()-(itemImage->sceneBoundingRect()).height()/2
                              ,itemImage->y()-(itemImage->sceneBoundingRect()).width()/2);
            itemImage->setVisible(true);
            itemImage->setFlag(QGraphicsItem::ItemIsSelectable, true);
            itemImage->setFlag(QGraphicsItem::ItemIsMovable, true);
            itemImage->setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
//            bool tooFarRight = (itemImage->y()+itemImage->sceneBoundingRect()).width())>;
            // Add Item to the current scene
            addItem(itemImage);

            newImage = new diagramImage(itemImage);

            // Emiting does only make sure to return to the old state before clicking
//            item.setPos(itemImage->x(),itemImage->y());
            posVec[0][0] = itemImage->x();
            posVec[0][1] = itemImage->y();

//            emit itemsInserted(&item,1,posVec);
            emit itemsInserted(newImage,item.diagramType());
            break;
        case InsertHorizontalLine:
//            line = new QGraphicsLineItem(QLineF(mouseEvent->scenePos(),
//                                        mouseEvent->scenePos()));
//            line->setPen(QPen(myLineColor, 2));
//            addItem(line);
            break;
//! [7] //! [8]
        // The DiagramTextItem is editable when the Qt::TextEditorInteraction flag is set, else it is movable by the mouse. We always want the text to be drawn on top of the other items in the scene, so we set the value to a number higher than other items in the scene.
        case InsertText:
            textItem = new DiagramTextItem();
            textItem->setFont(myFont);
            textItem->setTextInteractionFlags(Qt::TextEditorInteraction);
            textItem->setZValue(1000.0);
            connect(textItem, SIGNAL(lostFocus(DiagramTextItem*)),
                    this, SLOT(editorLostFocus(DiagramTextItem*)));
            connect(textItem, SIGNAL(selectedChange(QGraphicsItem*)),
                    this, SIGNAL(itemSelected(QGraphicsItem*)));
            addItem(textItem);
            textItem->setDefaultTextColor(myTextColor);
            textItem->setPos(mouseEvent->scenePos());
            emit textInserted(textItem);
//! [8] //! [9]
        case MoveItem:

    default:
        ;
    }
    // We are in MoveItem mode if we get to the default switch; we can then call the QGraphicsScene implementation, which handles movement of items with the mouse. We make this call even if we are in another mode making it possible to add an item and then keep the mouse button pressed down and start moving the item. In the case of text items, this is not possible as they do not propagate mouse events when they are editable.
    QGraphicsScene::mousePressEvent(mouseEvent);
    emit checkItemPosition();
//    if (myMode==MoveItem){emit checkItemPosition();}
}
//! [9]

void DiagramScene::addItemsFromList(const std::vector<int> &modelTypes,const std::vector<std::vector<double>> &modelPositions){

    DiagramItem *item;
    QGraphicsPixmapItem *itemImage;
    diagramImage * newImage;
    for(std::size_t iItem=0;iItem<modelTypes.size();iItem++){

        // Using the constructor of DiagramItem in order to create the Qpixmap
        item = &(DiagramItem(DiagramItem::DiagramType(modelTypes[iItem]), myItemMenu));

        // Define QGraphicsPixmapItem
        itemImage = new QGraphicsPixmapItem(item->image());

        // Move Item to the current mouse position and center it afterwards
        itemImage->setPos(QPointF(modelPositions[iItem][0],modelPositions[iItem][1]));
//        std::cout<<"Type: "<<DiagramItem::DiagramType(modelTypes[iItem])<<"\tx: "<<modelPositions[iItem][0]<<"\ty: "<<modelPositions[iItem][1]<<std::endl;

        itemImage->setVisible(true);
        itemImage->setFlag(QGraphicsItem::ItemIsSelectable, true);
        itemImage->setFlag(QGraphicsItem::ItemIsMovable, true);
        itemImage->setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);

        // Add Item to the current scene
        addItem(itemImage);
        newImage = new diagramImage(itemImage);

        emit itemsInserted(newImage,item->diagramType());
    }
}

//! [10]
void DiagramScene::mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    if (myMode == InsertHorizontalLine && line != 0) {
        QLineF newLine(line->line().p1(), mouseEvent->scenePos());
        line->setLine(newLine);
    } else if (myMode == MoveItem) {
        QGraphicsScene::mouseMoveEvent(mouseEvent);
    }
}
//! [10]

//! [11]
// In the mouseReleaseEvent() function we need to check if an arrow should be added to the scene
void DiagramScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
//    std::cout<<"Mouse Event: "<<myMode<<"\tcheck: "<<InsertVerticalLine<<"\tEndOfLine: "<<EndOfLine<<std::endl;

    // Get Position of start and end of the model line
    if ((myMode == InsertHorizontalLine||myMode == InsertVerticalLine)&&(!EndOfLine)) {
        std::cout<<"Start Point: "<<std::endl;
        startPoint = QPointF(mouseEvent->scenePos());
    }

    if ((myMode == InsertHorizontalLine||myMode == InsertVerticalLine)&&(EndOfLine)) {
        std::cout<<"End Point: "<<(myMode == InsertHorizontalLine||myMode == InsertVerticalLine)<<
                " - "<<EndOfLine<<std::endl;
        endPoint = QPointF(mouseEvent->scenePos());

        // Create Item whih contains a prototype of the item which shall be inserted
        DiagramItem item = DiagramItem(myItemType, myItemMenu);
        QGraphicsPixmapItem image(item.image());
        double width = image.sceneBoundingRect().width();
        double height = image.sceneBoundingRect().height();

        // Center Images with respect to the mouse pointer
        startPoint.setX(startPoint.x()-width/2);
        startPoint.setY(startPoint.y()-height/2);
        endPoint.setX(endPoint.x()-width/2);
        endPoint.setY(endPoint.y()-height/2);

        std::size_t nModels = 0;
        switch (myMode) {
        case InsertHorizontalLine:
             nModels= ceil(abs(startPoint.x()-endPoint.x())/width);
             std::cout<<"nModels1 : "<<nModels<<std::endl;
            break;
        case InsertVerticalLine:
            nModels = ceil(abs(startPoint.y()-endPoint.y())/height);
            std::cout<<"nModels2 : "<<nModels<<std::endl;
            break;
        default:
            std::cout<<"nModels3 : "<<nModels<<std::endl;
            break;
        }


        std::vector<int> modelTypes(nModels,myItemType);
        std::vector<std::vector<double>> modelPositions(nModels,std::vector<double>(2,0));

        int signDistance = 0;
        for(std::size_t iModel=0;iModel<nModels;iModel++){
            if(myMode == InsertHorizontalLine){
                signDistance = 1 - 2*static_cast<unsigned>((startPoint.x()-endPoint.x())>0);
                modelPositions[iModel][0] = startPoint.x() + double(signDistance)*double(iModel)*double(width);
                modelPositions[iModel][1] = startPoint.y();
            }
            else{
                signDistance = 1 - 2*static_cast<unsigned>((startPoint.y()-endPoint.y())>0);
                modelPositions[iModel][0] = startPoint.x();
                modelPositions[iModel][1] = startPoint.y()+ double(signDistance)*double(iModel)*double(height);
            }
        }

        // Add the items to the scenes
        addItemsFromList(modelTypes,modelPositions);
//        emit itemsInserted(&item,nModels,modelPositions);
    }
    if(myMode == InsertHorizontalLine||myMode == InsertVerticalLine){
        EndOfLine = !EndOfLine;
    }

//    std::cout<<"startPoint: "<<startPoint.x()<<"\t-"<<startPoint.y()<<std::endl;

//! [12] //! [13]
    line = 0;
    QGraphicsScene::mouseReleaseEvent(mouseEvent);
}
//! [13]

//! [14]
// The scene has single selection, i.e., only one item can be selected at any given time. The foreach will then loop one time with the selected item or none if no item is selected. isItemChange() is used to check whether a selected item exists and also is of the specified diagram type.
bool DiagramScene::isItemChange(int type)
{
    foreach (QGraphicsItem *item, selectedItems()) {
        if (item->type() == type)
            return true;
    }
    return false;
}
//! [14]
