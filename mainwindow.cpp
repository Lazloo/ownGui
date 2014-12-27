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
#include <QFileDialog>
#include "arrow.h"
#include "diagramitem.h"
#include "diagramscene.h"
#include "diagramtextitem.h"
#include "mainwindow.h"
#include <iostream>
#include <QtWidgets>
#include <QPen>
#include <QLineF>
#include <fstream>
#include <sstream>

const int InsertTextButton = 10;

//! [0]
MainWindow::MainWindow():ModelPositions(0,std::vector<double>(0,0)),ModelTypes(0,0)
{
    // In the constructor we call methods to create the widgets and layouts of the example before we create the diagram scene.
    // All the things right below the toolbar
    createActions();
    // The things which can be chosen to be drawn into the scene
    createToolBox();
    //
    createMenus();

    SceneWidth = 800;
    SceneHeight = 600;
    scene = new DiagramScene(itemMenu, this);
    scene->setSceneRect(QRectF(0, 0, SceneWidth, SceneHeight));

    // Draw Boundaries
    penBoundaries = new QPen(Qt::black);
    penBoundaries->setStyle(Qt::SolidLine);
    verticalLineLeft = scene->addLine(QLine(0,0,0,SceneHeight),*penBoundaries);
    verticalLineRight = scene->addLine(QLine(SceneWidth,0,SceneWidth,SceneHeight),*penBoundaries);
    horizontalLineTop = scene->addLine(QLine(0,0,SceneWidth,0),*penBoundaries);
    horizontalLineBottom = scene->addLine(QLine(0,SceneHeight,SceneWidth,SceneHeight),*penBoundaries);




    // We connect to the itemInserted() and textInserted() slots of the diagram scenes as we want to uncheck the buttons in the
    // tool box when an item is inserted. When an item is selected in the scene we receive the itemSelected() signal. We use
    // this to update the widgets that display font properties if the item selected is a DiagramTextItem.
    connect(scene, SIGNAL(itemInserted(DiagramItem*)),
            this, SLOT(itemInserted(DiagramItem*)));
    connect(scene, SIGNAL(itemsInserted(DiagramItem*,std::size_t)),
            this, SLOT(itemsInserted(DiagramItem*,std::size_t)));
//    connect(scene, SIGNAL(itemsInserted(DiagramItem *item, std::size_t nModels)),
//            this, SLOT(itemsInserted(DiagramItem *item, std::size_t nModels)));

    connect(scene, SIGNAL(textInserted(QGraphicsTextItem*)),
            this, SLOT(textInserted(QGraphicsTextItem*)));
    connect(scene, SIGNAL(itemSelected(QGraphicsItem*)),
            this, SLOT(itemSelected(QGraphicsItem*)));
    connect(scene, SIGNAL(checkItemPosition()),
            this, SLOT(checkItemPosition()));
    // The toolbars must be created after the scene as they connect to its signals.
    // (all the things that can be chosen and stay clicked afterwards)
    createToolbars();

    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(toolBox);
    view = new QGraphicsView(scene);
    view->setMaximumSize(QSize(SceneWidth+10, SceneHeight+10));
    layout->addWidget(view);

    QWidget *widget = new QWidget;
    widget->setLayout(layout);

    setCentralWidget(widget);
    setWindowTitle(tr("Diagramscene"));
    setUnifiedTitleAndToolBarOnMac(true);
}
//! [0]

void MainWindow::setBounds(){
//    delete this->grid

    verticalLineLeft->setLine(0,0,0,SceneHeight);
//    verticalLineLeft = new QLine(0,0,0,SceneHeight);
    verticalLineRight->setLine(SceneWidth,0,SceneWidth,SceneHeight);
    horizontalLineTop->setLine(0,0,SceneWidth,0);
    horizontalLineBottom->setLine(0,SceneHeight,SceneWidth,SceneHeight);
}

// In this function we set the QBrush that is used to draw the background of the diagramscene.
// The background can be a grid of squares of blue, gray, or white tiles, or no grid at all.
// We have QPixmaps of the tiles from png files that we create the brush with.
// When one of the buttons in the background tabbed widget item is clicked we change the brush;
// we find out which button it is by checking its text.
void MainWindow::backgroundButtonGroupClicked(QAbstractButton *button)
{
    QList<QAbstractButton *> buttons = backgroundButtonGroup->buttons();
    foreach (QAbstractButton *myButton, buttons) {
        if (myButton != button)
            button->setChecked(false);
    }
    QString text = button->text();
    if (text == tr("Blue Grid"))
        scene->setBackgroundBrush(QPixmap(":/images/background1.png"));
    else if (text == tr("White Grid"))
        scene->setBackgroundBrush(QPixmap(":/images/background2.png"));
    else if (text == tr("Gray Grid"))
        scene->setBackgroundBrush(QPixmap(":/images/background3.png"));
    else
        scene->setBackgroundBrush(QPixmap(":/images/background4.png"));

    scene->update();
    view->update();
}
//! [1]

//! [2]
// This slot is called when a button in buttonGroup is checked. When a button is checked
// the user can click on the graphics view and a DiagramItem of the selected type will be inserted into the DiagramScene.
void MainWindow::buttonGroupClicked(int id)
{
    QList<QAbstractButton *> buttons = buttonGroup->buttons();
    // We must loop through the buttons in the group to uncheck other buttons as only one button is allowed to be checked at a time.
    foreach (QAbstractButton *button, buttons) {
        if (buttonGroup->button(id) != button)
            button->setChecked(false);
    }

    // QButtonGroup assigns an id to each button. We have set the id of each button to the diagram type,
    // as given by DiagramItem::DiagramType that will be inserted into the scene when it is clicked.
    // We can then use the button id when we set the diagram type with setItemType(). In the case of text we assigned
    // an id that has a value that is not in the DiagramType enum.
    if (id == InsertTextButton) {
        scene->setMode(DiagramScene::InsertText);
    } else {
        scene->setItemType(DiagramItem::DiagramType(id));
        scene->setMode(DiagramScene::InsertItem);
    }
}
//! [2]

//! [3]
// This slot deletes the selected item, if any, from the scene. It deletes the arrows first in order to avoid to delete
// them twice. If the item to be deleted is a DiagramItem, we also need to delete arrows connected to it; we don't want
// arrows in the scene that aren't connected to items in both ends.
void MainWindow::deleteItem()
{
    // Initialization
    QList<QGraphicsItem *> itemList = scene->items();
    std::vector<std::size_t> indices(scene->selectedItems().size(),0);
    std::size_t posVec = 0;

    foreach (QGraphicsItem *item, scene->selectedItems()) {

        // Save indices of deleted item in the list. Used further below
        for(std::size_t iItem=0;std::size_t(iItem<itemList.size()-4);iItem++){
            if(itemList[iItem]==item){
                indices[posVec] =iItem;
                posVec++;
            }
        }

         if (item->type() == DiagramItem::Type)
             qgraphicsitem_cast<DiagramItem *>(item)->removeArrows();
         scene->removeItem(item);
         delete item;
     }


    // Bring indices in a defined order
    std::sort (indices.begin(),indices.end());

    // Go in the inverted direction for deleting otherwise you end up messing up the indices
    std::size_t maxSize = ModelTypes.size();
    for(std::size_t iItem=0;iItem<indices.size();iItem++){
        std::size_t indexUsed = maxSize-1-indices[iItem];
        ModelTypes.erase(ModelTypes.begin()+indexUsed);
        MovementType.erase(MovementType.begin()+indexUsed);
        MovementDetails.erase(MovementDetails.begin()+indexUsed);
        Gravity.erase(Gravity.begin()+indexUsed);
        RelationToMainChara.erase(RelationToMainChara.begin()+indexUsed);
        EventIndex.erase(EventIndex.begin()+indexUsed);
    }
}
//! [3]

// This function creates a text-file with all necesaary information in order to recreate the map
void MainWindow::saveFileAs()
{

    QString fileName = QFileDialog::getSaveFileName(this,
         tr("Open File"), "/home", tr("text files (*.txt)"));
    std::ofstream myfile;

    myfile.open (fileName.toStdString(), std::ios::out | std::ios::trunc);

    // Save Size of view
    myfile<<SceneWidth<<"\t"<<SceneHeight<<"\t"<<std::endl;

    QList<QGraphicsItem *> itemList = scene->items();
    // -4 since the last four item are the boundary lines
    std::size_t nItem = static_cast<std::size_t> (itemList.size()-4);
    for(int iItem=0;iItem<nItem;iItem++){
        myfile<<ModelTypes[iItem]<<"\t"<<itemList[nItem-1-iItem]->x()<<"\t"<<itemList[nItem-1-iItem]->y()
             <<"\t"<<MovementType[iItem]<<"\t"<<MovementDetails[iItem]<<"\t"<<Gravity[iItem]
             <<"\t"<<RelationToMainChara[iItem]<<"\t"<<EventIndex[iItem]
             <<"\t"<<std::endl;
    }
    myfile.close();
}


void MainWindow::loadFile(){


    QString fileName = QFileDialog::getOpenFileName(this,
         tr("Open File"), "/home", tr("text files (*.txt)"));

    if(fileName.length()==0){
        return;
    }

    // Clear old Scene
    QList<QGraphicsItem *> itemProto = scene->items();
    for(std::size_t iItem=0;iItem<itemProto.size()-4;iItem++) {
        delete itemProto[iItem];
    }

    std::string line;
    std::size_t nModels = 0;

    // Get number of objects which are saved in the file
    std::ifstream infile(fileName.toStdString());
    while ( std::getline(infile, line) )
       ++nModels;
    nModels=nModels-1;
    std::cout<<"Lines: "<<nModels<<std::endl;
    infile.close();

    ModelTypes.resize(nModels,0);
    ModelPositions.resize(nModels,std::vector<double>(2,0));
    MovementType.resize(nModels,0);
    MovementDetails.resize(nModels,0);
    Gravity.resize(nModels,0);
    RelationToMainChara.resize(nModels,0);
    EventIndex.resize(nModels,0);

    std::size_t lineIndex=0;

    infile.open(fileName.toStdString());
    // First the size of the scene
    std::getline(infile, line);
    std::istringstream iss1(line);
    iss1>> SceneWidth >> SceneHeight;

    while (std::getline(infile, line))
    {
        std::istringstream iss(line);
        if (!(iss >> ModelTypes[lineIndex] >> ModelPositions[lineIndex][0]>>ModelPositions[lineIndex][1])
                     >>MovementType[lineIndex]>>MovementDetails[lineIndex]>>Gravity[lineIndex]
                     >>RelationToMainChara[lineIndex]>>EventIndex[lineIndex]
                ) {
            std::cout<<"break"<<std::endl;
            break;
        } // error
        std::cout<<"ModelType: "<<ModelTypes[lineIndex]<<"\tx: "<<ModelPositions[lineIndex][0]<<"\ty: "<<ModelPositions[lineIndex][1]<<std::endl;
        lineIndex++;
    }

    infile.close();

    scene->addItemsFromList(ModelTypes,ModelPositions);
    setBounds();

}

void MainWindow::setMapSize()
{
    bool ok;
    double sceneWidth = QInputDialog::getDouble(this, tr("QInputDialog::getDouble()"),
                                       tr("Amount:"), SceneWidth, 0, 10000, 2, &ok);
    if (ok)
        SceneWidth=sceneWidth;

    double sceneHeight = QInputDialog::getDouble(this, tr("QInputDialog::getDouble()"),
                                       tr("Amount:"), SceneHeight, 0, 10000, 2, &ok);
    if (ok)
        SceneHeight=sceneHeight;

    view->setSceneRect(QRectF(0, 0, SceneWidth+10, SceneHeight+10));
    scene->setSceneRect(QRectF(0, 0, SceneWidth, SceneHeight));
    setBounds();
}

//! [4]
// The pointerTypeGroup decides whether the scene is in ItemMove or InsertLine mode. This button group is exclusive,
// i.e., only one button is checked at any time. As with the buttonGroup above we have assigned an id to the buttons
// that matches values of the DiagramScene::Mode enum, so that we can use the id to set the correct mode.
void MainWindow::pointerGroupClicked(int)
{
    scene->setMode(DiagramScene::Mode(pointerTypeGroup->checkedId()));
    if(DiagramScene::Mode(pointerTypeGroup->checkedId())==DiagramScene::InsertHorizontalLine){
        scene->setEndOfLine(!(scene->getEndOfLine()));
    };
}
//! [4]

//! [5]
// Several items may collide, i.e., overlap, with each other in the scene. This slot is called when the user requests
// that an item should be placed on top of the items it collides with. QGrapicsItems have a z-value that decides the
// order in which items are stacked in the scene; you can think of it as the z-axis in a 3D coordinate system.
// When items collide the items with higher z-values will be drawn on top of items with lower values. When we bring an
// item to the front we can loop through the items it collides with and set a z-value that is higher than all of them.
void MainWindow::bringToFront()
{
    if (scene->selectedItems().isEmpty())
        return;

    QGraphicsItem *selectedItem = scene->selectedItems().first();
    QList<QGraphicsItem *> overlapItems = selectedItem->collidingItems();

    qreal zValue = 0;
    foreach (QGraphicsItem *item, overlapItems) {
        if (item->zValue() >= zValue && item->type() == DiagramItem::Type)
            zValue = item->zValue() + 0.1;
    }
    selectedItem->setZValue(zValue);
}

//! [5]

//! [6]
// This slot works in the same way as bringToFront() described above, but sets a z-value that is lower than items the
// item that should be send to the back collides with.
void MainWindow::sendToBack()
{
    if (scene->selectedItems().isEmpty())
        return;

    QGraphicsItem *selectedItem = scene->selectedItems().first();
    QList<QGraphicsItem *> overlapItems = selectedItem->collidingItems();

    qreal zValue = 0;
    foreach (QGraphicsItem *item, overlapItems) {
        if (item->zValue() <= zValue && item->type() == DiagramItem::Type)
            zValue = item->zValue() - 0.1;
    }
    selectedItem->setZValue(zValue);
}
//! [6]

void MainWindow::checkItemPosition(){

//    std::cout<<"ItemMoved"<<std::endl;
    bool tooFarRight = true;
    bool tooFarLeft = true;
    bool tooFarUp = true;
    bool tooFarDown = true;
    QList<QGraphicsItem *> list = scene->items();

    QGraphicsItem *item;
    for(int iItem=0;iItem<list.size()-4;iItem++) {
         item = list[iItem];
         if (item->type() == DiagramItem::Type)
             qgraphicsitem_cast<DiagramItem *>(item)->removeArrows();
         tooFarRight = (item->x())<0;
         tooFarLeft = (item->x()+item->sceneBoundingRect().width())>SceneWidth;
         tooFarUp = (item->y())<0;
         tooFarDown = (item->y()+item->sceneBoundingRect().height())>SceneHeight;
         if(tooFarRight||tooFarLeft||tooFarUp||tooFarDown){
             scene->removeItem(item);
             delete item;
         }
     }

//    std::cout<<"Right"<<tooFarRight<<"\tLeft"<<tooFarLeft<<"\tUp"<<tooFarUp<<"\tDown"<<tooFarDown<<std::endl;
}

//! [7]
// This slot is called from the DiagramScene when an item has been added to the scene. We set the mode of the scene back
// to the mode before the item was inserted, which is ItemMove or InsertText depending on which button is checked in the
// pointerTypeGroup. We must also uncheck the button in the in the buttonGroup.
void MainWindow::itemInserted(DiagramItem *item)
{
    scene->setMode(DiagramScene::Mode(3));
    buttonGroup->button(int(item->diagramType()))->setChecked(false);

    switch (item->diagramType()) {
    // Archer
    case DiagramItem::DiagramType(0):
        MovementType.push_back(1);
        MovementDetails.push_back(0);
        Gravity.push_back(1);
        RelationToMainChara.push_back(2);
        EventIndex.push_back(0);
        break;
    // Cat Girl
    case DiagramItem::DiagramType(1):
        MovementType.push_back(1);
        MovementDetails.push_back(2);
        Gravity.push_back(1);
        RelationToMainChara.push_back(2);
        EventIndex.push_back(0);
        break;
    // Monster
    case DiagramItem::DiagramType(2):
        MovementType.push_back(1);
        MovementDetails.push_back(2);
        Gravity.push_back(1);
        RelationToMainChara.push_back(1);
        EventIndex.push_back(0);
        break;
    // Tree
    case DiagramItem::DiagramType(3):
        MovementType.push_back(0);
        MovementDetails.push_back(0);
        Gravity.push_back(0);
        RelationToMainChara.push_back(0);
        EventIndex.push_back(0);
    default:
        break;
    }

    // Update lists
    ModelTypes.push_back(item->diagramType());
}

void MainWindow::itemsInserted(DiagramItem *item, std::size_t nModels){
//    scene->setMode(DiagramScene::Mode(3));
//    buttonGroup->button(int(item->diagramType()))->setChecked(false);

    for(std::size_t iModel=0;iModel<nModels;iModel++){
        switch (item->diagramType()) {
        // Archer
        case DiagramItem::DiagramType(0):
            MovementType.push_back(1);
            MovementDetails.push_back(0);
            Gravity.push_back(1);
            RelationToMainChara.push_back(2);
            EventIndex.push_back(0);
            break;
        // Cat Girl
        case DiagramItem::DiagramType(1):
            MovementType.push_back(1);
            MovementDetails.push_back(2);
            Gravity.push_back(1);
            RelationToMainChara.push_back(2);
            EventIndex.push_back(0);
            break;
        // Monster
        case DiagramItem::DiagramType(2):
            MovementType.push_back(1);
            MovementDetails.push_back(2);
            Gravity.push_back(1);
            RelationToMainChara.push_back(1);
            EventIndex.push_back(0);
            break;
        // Tree
        case DiagramItem::DiagramType(3):
            MovementType.push_back(0);
            MovementDetails.push_back(0);
            Gravity.push_back(0);
            RelationToMainChara.push_back(0);
            EventIndex.push_back(0);
        default:
            break;
        }

        // Update lists
        ModelTypes.push_back(item->diagramType());
    }
}
//! [7]

//! [8]
// We simply set the mode of the scene back to the mode it had before the text was inserted.
void MainWindow::textInserted(QGraphicsTextItem *)
{
//    buttonGroup->button(InsertTextButton)->setChecked(false);
    scene->setMode(DiagramScene::Mode(3));
}
//! [8]

//! [9]
// When the user requests a font change, by using one of the widgets in the fontToolBar, we create a new QFont object and set
// its properties to match the state of the widgets. This is done in handleFontChange(), so we simply call that slot.
void MainWindow::currentFontChanged(const QFont &)
{
    handleFontChange();
}
//! [9]

//! [10] When the user requests a font change, by using one of the widgets in
//the fontToolBar, we create a new QFont object and set its properties to match
//the state of the widgets. This is done in handleFontChange(), so we simply
//call that slot.
void MainWindow::fontSizeChanged(const QString &)
{
    handleFontChange();
}
//! [10]

//! [11]
// The user can increase or decrease the scale, with the sceneScaleCombo, the scene is drawn in. It is not the scene itself
// that changes its scale, but only the view.
void MainWindow::sceneScaleChanged(const QString &scale)
{
    double newScale = scale.left(scale.indexOf(tr("%"))).toDouble() / 100.0;
    QMatrix oldMatrix = view->matrix();
    view->resetMatrix();
    view->translate(oldMatrix.dx(), oldMatrix.dy());
    view->scale(newScale, newScale);
}
//! [11]

//! [12]
// This slot is called when an item in the drop-down menu of the fontColorToolButton is pressed. We need to change the icon
// on the button to the color of the selected QAction. We keep a pointer to the selected action in textAction.
// It is in textButtonTriggered() we change the text color to the color of textAction, so we call that slot.
void MainWindow::textColorChanged()
{
    textAction = qobject_cast<QAction *>(sender());
    fontColorToolButton->setIcon(createColorToolButtonIcon(
                                     ":/images/textpointer.png",
                                     qvariant_cast<QColor>(textAction->data())));
    textButtonTriggered();
}

//! [12]

//! [13]
// This slot handles requests for changing the color of DiagramItems in the same manner as textColorChanged() does for DiagramTextItems.
void MainWindow::itemColorChanged()
{
    fillAction = qobject_cast<QAction *>(sender());
    fillColorToolButton->setIcon(createColorToolButtonIcon(
                                     ":/images/floodfill.png",
                                     qvariant_cast<QColor>(fillAction->data())));
    fillButtonTriggered();
}
//! [13]

//! [14]
// This slot handles requests for changing the color of Arrows in the same manner that textColorChanged() does it for DiagramTextItems.
void MainWindow::lineColorChanged()
{
    lineAction = qobject_cast<QAction *>(sender());
    lineColorToolButton->setIcon(createColorToolButtonIcon(
                                     ":/images/linecolor.png",
                                     qvariant_cast<QColor>(lineAction->data())));
    lineButtonTriggered();
}
//! [14]

//! [15]
// textAction points to the QAction of the currently selected menu item in the fontColorToolButton's color drop-down menu.
// We have set the data of the action to the QColor the action represents, so we can simply fetch this when we set the color
// of text with setTextColor().
void MainWindow::textButtonTriggered()
{
    scene->setTextColor(qvariant_cast<QColor>(textAction->data()));
}
//! [15]

//! [16]
// fillAction points to the selected menu item in the drop-down menu of fillColorToolButton().
// We can therefore use the data of this action when we set the item color with setItemColor().
void MainWindow::fillButtonTriggered()
{
    scene->setItemColor(qvariant_cast<QColor>(fillAction->data()));
}
//! [16]

//! [17]
// lineAction point to the selected item in the drop-down menu of lineColorToolButton.
// We use its data when we set the arrow color with setLineColor().
void MainWindow::lineButtonTriggered()
{
    scene->setLineColor(qvariant_cast<QColor>(lineAction->data()));
}
//! [17]

//! [18]
// handleFontChange() is called when any of the widgets that show font properties changes. We create a new QFont object and
// set its properties based on the widgets. We then call the setFont() function of DiagramScene; it is the scene that set the
// font of the DiagramTextItems it manages.
void MainWindow::handleFontChange()
{
    QFont font = fontCombo->currentFont();
    font.setPointSize(fontSizeCombo->currentText().toInt());
    font.setWeight(boldAction->isChecked() ? QFont::Bold : QFont::Normal);
    font.setItalic(italicAction->isChecked());
    font.setUnderline(underlineAction->isChecked());

    scene->setFont(font);
}
//! [18]

//! [19]
// This slot is called when an item in the DiagramScene is selected. In the case of this example it is only text items
// that emit signals when they are selected, so we do not need to check what kind of graphics item is.
// We set the state of the widgets to match the properties of the font of the selected text item.
void MainWindow::itemSelected(QGraphicsItem *item)
{
    DiagramTextItem *textItem =
    qgraphicsitem_cast<DiagramTextItem *>(item);

    QFont font = textItem->font();
    fontCombo->setCurrentFont(font);
    fontSizeCombo->setEditText(QString().setNum(font.pointSize()));
    boldAction->setChecked(font.weight() == QFont::Bold);
    italicAction->setChecked(font.italic());
    underlineAction->setChecked(font.underline());
}
//! [19]

//! [20]
// This slot displays an about box for the example when the user selects the about menu item from the help menu.
void MainWindow::about()
{
    QMessageBox::about(this, tr("About Diagram Scene"),
                       tr("The <b>Diagram Scene</b> example shows "
                          "use of the graphics framework."));
}
//! [20]

//! [21]

void MainWindow::createToolBox()
{
    // This part of the function sets up the tabbed widget item that contains the flowchart shapes. An exclusive QButtonGroup
    // always keeps one button checked; we want the group to allow all buttons to be unchecked. We still use a button group
    // since we can associate user data, which we use to store the diagram type, with each button.
    buttonGroup = new QButtonGroup(this);
    buttonGroup->setExclusive(false);
    connect(buttonGroup, SIGNAL(buttonClicked(int)),
            this, SLOT(buttonGroupClicked(int)));
    QGridLayout *layout = new QGridLayout;
    // The createCellWidget() function sets up the buttons in the tabbed widget item and is examined later.
    layout->addWidget(createCellWidget(tr("MainChara"), DiagramItem::MainChara), 0, 0);
    layout->addWidget(createCellWidget(tr("Monster"), DiagramItem::Monster),0, 1);
    layout->addWidget(createCellWidget(tr("CatGirl"), DiagramItem::CatGirl), 1, 0);
    layout->addWidget(createCellWidget(tr("Tree"), DiagramItem::Tree), 1, 1);

    // Not Commented -----------------------------------------------------
//    QToolButton *textButton = new QToolButton;
//    textButton->setCheckable(true);
//    buttonGroup->addButton(textButton, InsertTextButton);
//    textButton->setIcon(QIcon(QPixmap(":/images/Rechts_Laufen_Bogen.png")));
//    textButton->setIconSize(QSize(50, 50));
//    QGridLayout *textLayout = new QGridLayout;
//    textLayout->addWidget(textButton, 0, 0, Qt::AlignHCenter);
//    textLayout->addWidget(new QLabel(tr("Text")), 1, 0, Qt::AlignCenter);
//    QWidget *textWidget = new QWidget;
//    textWidget->setLayout(textLayout);
//    layout->addWidget(textWidget, 1, 1);

    layout->setRowStretch(3, 10);
    layout->setColumnStretch(2, 10);

    QWidget *itemWidget = new QWidget;
    itemWidget->setLayout(layout);

    // #########################
    QToolButton *mainCharaButton = new QToolButton;
    mainCharaButton->setCheckable(true);
    buttonGroup->addButton(mainCharaButton, InsertTextButton);
    mainCharaButton->setIcon(QIcon(QPixmap(":/images/Rechts_Laufen_Bogen.png")));
//    mainCharaButton->setIconSize(QSize(50, 50));
//    QGridLayout *mainCharaLayout = new QGridLayout;
//    mainCharaLayout->addWidget(mainCharaButton, 0, 0, Qt::AlignHCenter);
//    mainCharaLayout->addWidget(new QLabel(tr("MainCharacter")), 1, 0, Qt::AlignCenter);
//    QWidget *mainCharaWidget = new QWidget;
//    mainCharaWidget->setLayout(mainCharaLayout);
//    layout->addWidget(mainCharaWidget, 1, 1);

//    layout->setRowStretch(3, 10);
//    layout->setColumnStretch(2, 10);

//    QWidget *itemMainCharaWidget = new QWidget;
//    itemMainCharaWidget->setLayout(layout);

    // #########################


    backgroundButtonGroup = new QButtonGroup(this);
    connect(backgroundButtonGroup, SIGNAL(buttonClicked(QAbstractButton*)),
            this, SLOT(backgroundButtonGroupClicked(QAbstractButton*)));

    QGridLayout *backgroundLayout = new QGridLayout;
    backgroundLayout->addWidget(createBackgroundCellWidget(tr("Blue Grid"),
                                                           ":/images/background1.png"), 0, 0);
    backgroundLayout->addWidget(createBackgroundCellWidget(tr("White Grid"),
                                                           ":/images/background2.png"), 0, 1);
    backgroundLayout->addWidget(createBackgroundCellWidget(tr("Gray Grid"),
                                                           ":/images/background3.png"), 1, 0);
    backgroundLayout->addWidget(createBackgroundCellWidget(tr("No Grid"),
                                                           ":/images/background4.png"), 1, 1);

    backgroundLayout->setRowStretch(2, 10);
    backgroundLayout->setColumnStretch(2, 10);

    QWidget *backgroundWidget = new QWidget;
    backgroundWidget->setLayout(backgroundLayout);
    // Not Commented -----------------------------------------------------

    // We set the preferred size of the toolbox as its maximum. This way, more space is given to the graphics view.
    toolBox = new QToolBox;
    toolBox->setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Ignored));
    toolBox->setMinimumWidth(itemWidget->sizeHint().width());
    toolBox->addItem(itemWidget, tr("Basic Flowchart Shapes"));
    toolBox->addItem(backgroundWidget, tr("Backgrounds"));
}
//! [22]

//! [23]
void MainWindow::createActions()
{
    // Create actions

    // Create button and the text if the mouse shows on it
    toFrontAction = new QAction(QIcon(":/images/bringtofront.png"),
                                tr("Bring to &Front"), this);
    // Set the shortcut
    toFrontAction->setShortcut(tr("Ctrl+F"));
    //
    toFrontAction->setStatusTip(tr("Bring item to front"));
    // Anytime the button is click (triggered) bringToFront is called
    connect(toFrontAction, SIGNAL(triggered()), this, SLOT(bringToFront()));

    sendBackAction = new QAction(QIcon(":/images/sendtoback.png"), tr("Send to &Back"), this);
    sendBackAction->setShortcut(tr("Ctrl+B"));
    sendBackAction->setStatusTip(tr("Send item to back"));
    connect(sendBackAction, SIGNAL(triggered()), this, SLOT(sendToBack()));

    deleteAction = new QAction(QIcon(":/images/delete.png"), tr("&Delete"), this);
    deleteAction->setShortcut(tr("Delete"));
    deleteAction->setStatusTip(tr("Delete item from diagram"));
    connect(deleteAction, SIGNAL(triggered()), this, SLOT(deleteItem()));

    exitAction = new QAction(tr("E&xit"), this);
    exitAction->setShortcuts(QKeySequence::Quit);
    exitAction->setStatusTip(tr("Quit Scenediagram"));
    connect(exitAction, SIGNAL(triggered()), this, SLOT(close()));

    saveFileAction = new QAction(tr("Save as"), this);
    saveFileAction->setShortcut(tr("Ctrl+S"));
    saveFileAction->setStatusTip(tr("Save Level"));
    connect(saveFileAction, SIGNAL(triggered()), this, SLOT(saveFileAs()));

    loadFileAction = new QAction(tr("Load"), this);
    loadFileAction->setShortcut(tr("Ctrl+O"));
    loadFileAction->setStatusTip(tr("Load Level"));
    connect(loadFileAction, SIGNAL(triggered()), this, SLOT(loadFile()));

    setMapSizeAction = new QAction(tr("Set Map Size"), this);
    setMapSizeAction->setShortcut(tr("Ctrl+M"));
    setMapSizeAction->setStatusTip(tr("Set the map size to a user defined value"));
    connect(setMapSizeAction, SIGNAL(triggered()), this, SLOT(setMapSize()));

    boldAction = new QAction(tr("Bold"), this);
    boldAction->setCheckable(true);
    QPixmap pixmap(":/images/bold.png");
    boldAction->setIcon(QIcon(pixmap));
    boldAction->setShortcut(tr("Ctrl+B"));
    connect(boldAction, SIGNAL(triggered()), this, SLOT(handleFontChange()));

    italicAction = new QAction(QIcon(":/images/italic.png"), tr("Italic"), this);
    italicAction->setCheckable(true);
    italicAction->setShortcut(tr("Ctrl+I"));
    connect(italicAction, SIGNAL(triggered()), this, SLOT(handleFontChange()));

    underlineAction = new QAction(QIcon(":/images/underline.png"), tr("Underline"), this);
    underlineAction->setCheckable(true);
    underlineAction->setShortcut(tr("Ctrl+U"));
    connect(underlineAction, SIGNAL(triggered()), this, SLOT(handleFontChange()));

    aboutAction = new QAction(tr("A&bout"), this);
    aboutAction->setShortcut(tr("Ctrl+B"));
    connect(aboutAction, SIGNAL(triggered()), this, SLOT(about()));
}

//! [24]
void MainWindow::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(exitAction);
    fileMenu->addAction(saveFileAction);
    fileMenu->addAction(loadFileAction);
    fileMenu->addAction(setMapSizeAction);

    itemMenu = menuBar()->addMenu(tr("&Item"));
    itemMenu->addAction(deleteAction);
    itemMenu->addSeparator();
    itemMenu->addAction(toFrontAction);
    itemMenu->addAction(sendBackAction);

    aboutMenu = menuBar()->addMenu(tr("&Help"));
    aboutMenu->addAction(aboutAction);
}
//! [24]

//! [25]

void MainWindow::createToolbars()
{
//! [25]
    editToolBar = addToolBar(tr("Edit"));
    editToolBar->addAction(deleteAction);
    editToolBar->addAction(toFrontAction);
    editToolBar->addAction(sendBackAction);

//    fontCombo = new QFontComboBox();
//    connect(fontCombo, SIGNAL(currentFontChanged(QFont)),
//            this, SLOT(currentFontChanged(QFont)));

//    fontSizeCombo = new QComboBox;
//    fontSizeCombo->setEditable(true);
//    for (int i = 8; i < 30; i = i + 2)
//        fontSizeCombo->addItem(QString().setNum(i));
//    QIntValidator *validator = new QIntValidator(2, 64, this);
//    fontSizeCombo->setValidator(validator);
//    connect(fontSizeCombo, SIGNAL(currentIndexChanged(QString)),
//            this, SLOT(fontSizeChanged(QString)));

//    fontColorToolButton = new QToolButton;
//    fontColorToolButton->setPopupMode(QToolButton::MenuButtonPopup);
//    fontColorToolButton->setMenu(createColorMenu(SLOT(textColorChanged()), Qt::black));
//    textAction = fontColorToolButton->menu()->defaultAction();
//    fontColorToolButton->setIcon(createColorToolButtonIcon(":/images/textpointer.png", Qt::black));
//    fontColorToolButton->setAutoFillBackground(true);
//    connect(fontColorToolButton, SIGNAL(clicked()),
//            this, SLOT(textButtonTriggered()));

//! [26]
//    // This button lets the user select a color for the diagram items.
//    fillColorToolButton = new QToolButton;
//    fillColorToolButton->setPopupMode(QToolButton::MenuButtonPopup);
//    // We set the menu of the tool button with setMenu(). We need the fillAction QAction object to always be pointing
//    // to the selected action of the menu.
//    //  The menu is created with the createColorMenu() function and, as we shall see later, contains one menu item for
//    // each color that the items can have.
//    fillColorToolButton->setMenu(createColorMenu(SLOT(itemColorChanged()), Qt::white));
//    fillAction = fillColorToolButton->menu()->defaultAction();
//    fillColorToolButton->setIcon(createColorToolButtonIcon(
//                                     ":/images/floodfill.png", Qt::white));
//    // When the user presses the button, which trigger the clicked() signal, we can set the color of the selected item
//    // to the color of fillAction. It is with createColorToolButtonIcon() we create the icon for the button.
//    connect(fillColorToolButton, SIGNAL(clicked()),
//            this, SLOT(fillButtonTriggered()));


//    lineColorToolButton = new QToolButton;
//    lineColorToolButton->setPopupMode(QToolButton::MenuButtonPopup);
//    lineColorToolButton->setMenu(createColorMenu(SLOT(lineColorChanged()), Qt::black));
//    lineAction = lineColorToolButton->menu()->defaultAction();
//    lineColorToolButton->setIcon(createColorToolButtonIcon(
//                                     ":/images/linecolor.png", Qt::black));
//    connect(lineColorToolButton, SIGNAL(clicked()),
//            this, SLOT(lineButtonTriggered()));

//    textToolBar = addToolBar(tr("Font"));
//    textToolBar->addWidget(fontCombo);
//    textToolBar->addWidget(fontSizeCombo);
//    textToolBar->addAction(boldAction);
//    textToolBar->addAction(italicAction);
//    textToolBar->addAction(underlineAction);

//    colorToolBar = addToolBar(tr("Color"));
//    colorToolBar->addWidget(fontColorToolButton);
//    colorToolBar->addWidget(fillColorToolButton);
//    colorToolBar->addWidget(lineColorToolButton);

    QToolButton *pointerButton = new QToolButton;
    pointerButton->setCheckable(true);
    pointerButton->setChecked(true);
    pointerButton->setIcon(QIcon(":/images/pointer.png"));

    QToolButton *linePointerButton = new QToolButton;
    linePointerButton->setCheckable(true);
    linePointerButton->setIcon(QIcon(":/images/linepointer.png"));

    pointerTypeGroup = new QButtonGroup(this);
    pointerTypeGroup->addButton(pointerButton, int(DiagramScene::MoveItem));
    pointerTypeGroup->addButton(linePointerButton, int(DiagramScene::InsertHorizontalLine));
    connect(pointerTypeGroup, SIGNAL(buttonClicked(int)),
            this, SLOT(pointerGroupClicked(int)));

    sceneScaleCombo = new QComboBox;
    QStringList scales;
    scales << tr("50%") << tr("75%") << tr("100%") << tr("125%") << tr("150%");
    sceneScaleCombo->addItems(scales);
    sceneScaleCombo->setCurrentIndex(2);
    connect(sceneScaleCombo, SIGNAL(currentIndexChanged(QString)),
            this, SLOT(sceneScaleChanged(QString)));

    pointerToolbar = addToolBar(tr("Pointer type"));
    pointerToolbar->addWidget(pointerButton);
    pointerToolbar->addWidget(linePointerButton);
    pointerToolbar->addWidget(sceneScaleCombo);
//! [27]
}

//! [27]

//! [28]
// This function creates QWidgets containing a tool button and a label. The widgets created with
// this function are used for the background tabbed widget item in the tool box.
QWidget *MainWindow::createBackgroundCellWidget(const QString &text, const QString &image)
{
    QToolButton *button = new QToolButton;
    button->setText(text);
    button->setIcon(QIcon(image));
    button->setIconSize(QSize(50, 50));
    button->setCheckable(true);
    backgroundButtonGroup->addButton(button);

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(button, 0, 0, Qt::AlignHCenter);
    layout->addWidget(new QLabel(text), 1, 0, Qt::AlignCenter);

    QWidget *widget = new QWidget;
    widget->setLayout(layout);

    return widget;
}
//! [28]

//! [29]

// This function returns a QWidget containing a QToolButton with an image of one of the DiagramItems, i.e., flowchart shapes.
// The image is created by the DiagramItem through the image() function.
// The widgets created with this function is used in the tool box.
QWidget *MainWindow::createCellWidget(const QString &text, DiagramItem::DiagramType type)
{
    // The image is created by the DiagramItem through the image() function.
    DiagramItem item(type, itemMenu);
    QIcon icon(item.image());

    // The QButtonGroup class lets us attach an id (int) with each button; we store the diagram's type,
    // i.e., the DiagramItem::DiagramType enum.
    QToolButton *button = new QToolButton;
    button->setIcon(icon);
    button->setIconSize(QSize(50, 50));
    button->setCheckable(true);
    buttonGroup->addButton(button, int(type));

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(button, 0, 0, Qt::AlignHCenter);
    layout->addWidget(new QLabel(text), 1, 0, Qt::AlignCenter);

    QWidget *widget = new QWidget;
    widget->setLayout(layout);

    return widget;
}
//! [29]

// This function creates a color menu that is used as the drop-down menu for the tool buttons
// in the colorToolBar. We create an action for each color that we add to the menu. We fetch
// the actions data when we set the color of items, lines, and text.
QMenu *MainWindow::createColorMenu(const char *slot, QColor defaultColor)
{
    QList<QColor> colors;
    colors << Qt::black << Qt::white << Qt::red << Qt::blue << Qt::yellow;
    QStringList names;
    names << tr("black") << tr("white") << tr("red") << tr("blue")
          << tr("yellow");

    QMenu *colorMenu = new QMenu(this);
    for (int i = 0; i < colors.count(); ++i) {
        QAction *action = new QAction(names.at(i), this);
        action->setData(colors.at(i));
        action->setIcon(createColorIcon(colors.at(i)));
        connect(action, SIGNAL(triggered()), this, slot);
        colorMenu->addAction(action);
        if (colors.at(i) == defaultColor)
            colorMenu->setDefaultAction(action);
    }
    return colorMenu;
}
//! [30]

//! [31]
// This function is used to create the QIcon of the fillColorToolButton, fontColorToolButton, and lineColorToolButton.
// The imageFile string is either the text, flood-fill, or line symbol that is used for the buttons. Beneath the image
// we draw a filled rectangle using color.
QIcon MainWindow::createColorToolButtonIcon(const QString &imageFile, QColor color)
{
    QPixmap pixmap(50, 80);
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    // Load Image file by constructing
    QPixmap image(imageFile);
//    QPixmap image("images/Rechts_Laufen_Bogen.png");
    // Draw icon centred horizontally on button.
    QRect target(0, 0, 42, 43);
    QRect source(0, 0, 42, 43);
    painter.fillRect(QRect(0, 60, 50, 80), color);
    painter.drawPixmap(target, image, source);

    return QIcon(pixmap);
}
//! [31]

//! [32]
// This function creates an icon with a filled rectangle in the color of color. It is used for creating icons for the
// color menus in the fillColorToolButton, fontColorToolButton, and lineColorToolButton
QIcon MainWindow::createColorIcon(QColor color)
{
    QPixmap pixmap(20, 20);
    QPainter painter(&pixmap);
    painter.setPen(Qt::NoPen);
    painter.fillRect(QRect(0, 0, 20, 20), color);

    return QIcon(pixmap);
}
//! [32]
