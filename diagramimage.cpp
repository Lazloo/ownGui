#include "diagramimage.h"
#include <QGraphicsScene>
#include <QGraphicsSceneContextMenuEvent>
#include <QMenu>
#include <QPainter>

// In the constructor we create the items polygon according to diagramType. QGraphicsItems are not movable or selectable by default, so we must set these properties.
DiagramImage::DiagramImage(DiagramType diagramType, QMenu *contextMenu,
             QGraphicsItem *parent)
    : QGraphicsPolygonItem(parent)
{
    myDiagramType = diagramType;
    myContextMenu = contextMenu;


    switch (myDiagramType) {
        case StartEnd:
//            QPixmap image("/images/Rechts_Laufen_Bogen.png");
            break;
        case Conditional:
//            myPolygon << QPointF(-100, 0) << QPointF(0, 100)
//                      << QPointF(100, 0) << QPointF(0, -100)
//                      << QPointF(-100, 0);
            break;
        case Step:
//            myPolygon << QPointF(-100, -100) << QPointF(100, -100)
//                      << QPointF(100, 100) << QPointF(-100, 100)
//                      << QPointF(-100, -100);
            break;
        default:
//            myPolygon << QPointF(-120, -80) << QPointF(-70, 80)
//                      << QPointF(120, 80) << QPointF(70, -80)
//                      << QPointF(-120, -80);
            break;
    }
    QPixmap image("/images/Rechts_Laufen_Bogen.png");
//    QPainter painter(&pixmap);
    // Load Image file by constructing
//    QPixmap image(imageFile);
//    QPixmap image("images/Rechts_Laufen_Bogen.png");
    // Draw icon centred horizontally on button.
    QRect target(0, 0, 40, 50);
    QRect source(0, 0, 400, 500);
    fillRect(QRect(0, 50, 50, 80), color);
    drawPixmap(target, image, source);

    setFlag(QGraphicsItem::ItemIsMovable, true);
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
}
//! [0]

//! [1]
// removeArrow() is used to remove Arrow items when they or DiagramItems they are connected to are removed from the scene.
void DiagramItem::removeArrow(Arrow *arrow)
{
//    int index = arrows.indexOf(arrow);

//    if (index != -1)
//        arrows.removeAt(index);
}
//! [1]

//! [2]
// This function is called when the item is removed from the scene and removes all arrows that are connected to this item. The arrow must be removed from the arrows list of both its start and end item.
void DiagramItem::removeArrows()
{
//    foreach (Arrow *arrow, arrows) {
//        arrow->startItem()->removeArrow(arrow);
//        arrow->endItem()->removeArrow(arrow);
//        scene()->removeItem(arrow);
//        delete arrow;
//    }
}
//! [2]

//! [3]
// This function simply adds the arrow to the items arrows list.
void DiagramItem::addArrow(Arrow *arrow)
{
//    arrows.append(arrow);
}
//! [3]

//! [4]
// This function draws the polygon of the item onto a QPixmap. In this example we use this to create icons for the tool buttons in the tool box.
QPixmap DiagramItem::image() const
{
    QPixmap pixmap(250, 250);
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    painter.setPen(QPen(Qt::black, 8));
    painter.translate(125, 125);
    painter.drawPolyline(myPolygon);

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
