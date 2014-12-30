#include "diagramimage.h"
#include <iostream>

diagramImage::diagramImage(QObject *parent) :
    QObject(parent)
{
    std::cout<<"Test Initiniliazation"<<std::endl;
}
