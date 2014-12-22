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

#include "diagramitem.h"
#include "arrow.h"

#include <QGraphicsScene>
#include <QGraphicsSceneContextMenuEvent>
#include <QMenu>
#include <QPainter>
#include <QGraphicsPixmapItem>
//! [0]

// In the constructor we create the items polygon according to diagramType. QGraphicsItems are not movable or selectable by default, so we must set these properties.
DiagramItem::DiagramItem(DiagramType diagramType, QMenu *contextMenu,
             QGraphicsItem *parent)
    : QGraphicsPolygonItem(parent)
{
    myDiagramType = diagramType;
    myContextMenu = contextMenu;

//    QPainterPath path;
    switch (myDiagramType) {
        case MainChara:
            path = ":/images/MainChara.png";
            break;
        case Monster:
            path = ":/images/CatGirt.png";
            break;
        case CatGirl:
            path = ":/images/Monster.png";
            break;
        case Tree:
            path = ":/images/Baum_1.png";
            break;
        default:
            myPolygon << QPointF(-120, -80) << QPointF(-70, 80)
                              << QPointF(120, 80) << QPointF(70, -80)
                              << QPointF(-120, -80);
//            fputs ("error opening file\n",stderr);
//            std::abort();
            break;
    }
//    setPolygon(myPolygon);
    setFlag(QGraphicsItem::ItemIsMovable, true);
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
}
//! [0]

//! [1]
// removeArrow() is used to remove Arrow items when they or DiagramItems they are connected to are removed from the scene.
void DiagramItem::removeArrow(Arrow *arrow)
{
    int index = arrows.indexOf(arrow);

    if (index != -1)
        arrows.removeAt(index);
}
//! [1]

//! [2]
// This function is called when the item is removed from the scene and removes all arrows that are connected to this item. The arrow must be removed from the arrows list of both its start and end item.
void DiagramItem::removeArrows()
{
    foreach (Arrow *arrow, arrows) {
        arrow->startItem()->removeArrow(arrow);
        arrow->endItem()->removeArrow(arrow);
        scene()->removeItem(arrow);
        delete arrow;
    }
}
//! [2]

//! [3]
// This function simply adds the arrow to the items arrows list.
void DiagramItem::addArrow(Arrow *arrow)
{
    arrows.append(arrow);
}
//! [3]

//! [4]
// This function draws the polygon of the item onto a QPixmap. In this example we use this to create icons for the tool buttons in the tool box.
QPixmap DiagramItem::image() const
{
    // Add off-screen image representation that can be used as a paint device
    QPixmap pixmap(84, 111);
    pixmap.fill(Qt::transparent);
    // The QPainter class performs low-level painting on widgets and other paint devices
    QPainter painter(&pixmap);
    // Load Image file by constructing
    switch (myDiagramType) {
    case MainChara:
        break;
    default:
        break;
    }
    QPixmap image(path);
//    QPixmap image(":/images/Monster.png");
    // Draw icon centred horizontally on button.
    QRect target(0, 0, pixmap.width(), pixmap.height());
    QRect source(0, 0, 400, 500);
    painter.drawPixmap(target, image, source);

    return pixmap;
}
//! [4]

//! [5]
// We show the context menu. As right mouse clicks, which shows the menu, don't select items by default we set the item selected with setSelected(). This is necessary since an item must be selected to change its elevation with the bringToFront and sendToBack actions.
void DiagramItem::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    scene()->clearSelection();
    setSelected(true);
    myContextMenu->exec(event->screenPos());
}
//! [5]

//! [6]

// If the item has moved, we need to update the positions of the arrows connected to it. The implementation of QGraphicsItem does nothing, so we just return value.
QVariant DiagramItem::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if (change == QGraphicsItem::ItemPositionChange) {
        foreach (Arrow *arrow, arrows) {
            arrow->updatePosition();
        }
    }

    return value;
}
//! [6]
