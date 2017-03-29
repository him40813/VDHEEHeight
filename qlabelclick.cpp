#include "qlabelclick.h"



void QLabelClick::mousePressEvent( QMouseEvent* ev )
{
    const QPoint p = ev->pos();
    d->draw3DRadius(p);
    std::cout<<"click"<<std::endl;
}

QLabelClick::QLabelClick( QWidget * parent, Qt::WindowFlags f )
    : QLabel( parent, f ) {}

QLabelClick::QLabelClick( const QString& text, QWidget* parent, Qt::WindowFlags f )
    : QLabel( text, parent, f ) {}

void QLabelClick::setDialog(DialogRange *d){
    this->d=d;
}
