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
    myItemType = DiagramItem::Step;
    line = 0;
    textItem = 0;
    myItemColor = Qt::white;
    myTextColor = Qt::black;
    myLineColor = Qt::black;
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
    switch (myMode) {
        // We simply create a new DiagramItem and add it to the scene at the position the mouse was pressed. Note that the origin of its
        // local coordinate system will be under the mouse pointer position.
        case InsertItem:
            // Define QGraphicsPixmapItem
            itemImage = new QGraphicsPixmapItem(item.image());
            // Add Item to the current scene
            addItem(itemImage);
            // Move Item to the current mouse position and center it afterwards
            itemImage->setPos(mouseEvent->scenePos());
            itemImage->setPos(itemImage->x()-(itemImage->sceneBoundingRect()).height()/2
                              ,itemImage->y()-(itemImage->sceneBoundingRect()).width()/2);
            itemImage->setVisible(true);
            itemImage->setFlag(QGraphicsItem::ItemIsSelectable, true);
            itemImage->setFlag(QGraphicsItem::ItemIsMovable, true);
            itemImage->setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
            // Emiting does only make sure to return to the old state before clicking
            emit itemInserted(&item);
            break;
//! [6] //! [7]
        // The user adds Arrows to the scene by stretching a line between the items the arrow should connect. The start of the line is fixed in the place the user clicked the mouse and the end follows the mouse pointer as long as the button is held down. When the user releases the mouse button an Arrow will be added to the scene if there is a DiagramItem under the start and end of the line. We will see how this is implemented later; here we simply add the line.
        case InsertLine:
            line = new QGraphicsLineItem(QLineF(mouseEvent->scenePos(),
                                        mouseEvent->scenePos()));
            line->setPen(QPen(myLineColor, 2));
            addItem(line);
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
    default:
        ;
    }
    // We are in MoveItem mode if we get to the default switch; we can then call the QGraphicsScene implementation, which handles movement of items with the mouse. We make this call even if we are in another mode making it possible to add an item and then keep the mouse button pressed down and start moving the item. In the case of text items, this is not possible as they do not propagate mouse events when they are editable.
    QGraphicsScene::mousePressEvent(mouseEvent);
}
//! [9]

//! [10]
void DiagramScene::mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    if (myMode == InsertLine && line != 0) {
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
    // First we need to get the items (if any) under the line's start and end points. The line itself is the first item at these points, so we remove it from the lists. As a precaution, we check if the lists are empty, but this should never happen.
    if (line != 0 && myMode == InsertLine) {
        QList<QGraphicsItem *> startItems = items(line->line().p1());
        if (startItems.count() && startItems.first() == line)
            startItems.removeFirst();
        QList<QGraphicsItem *> endItems = items(line->line().p2());
        if (endItems.count() && endItems.first() == line)
            endItems.removeFirst();

        removeItem(line);
        delete line;
//! [11] //! [12]
        // Now we check if there are two different DiagramItems under the lines start and end points. If there are we can create an Arrow with the two items. The arrow is then added to each item and finally the scene. The arrow must be updated to adjust its start and end points to the items. We set the z-value of the arrow to -1000.0 because we always want it to be drawn under the items.
        if (startItems.count() > 0 && endItems.count() > 0 &&
            startItems.first()->type() == DiagramItem::Type &&
            endItems.first()->type() == DiagramItem::Type &&
            startItems.first() != endItems.first()) {
            DiagramItem *startItem = qgraphicsitem_cast<DiagramItem *>(startItems.first());
            DiagramItem *endItem = qgraphicsitem_cast<DiagramItem *>(endItems.first());
            Arrow *arrow = new Arrow(startItem, endItem);
            arrow->setColor(myLineColor);
            startItem->addArrow(arrow);
            endItem->addArrow(arrow);
            arrow->setZValue(-1000.0);
            addItem(arrow);
            arrow->updatePosition();
        }
    }
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
