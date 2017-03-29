
#include <QWidget>
#include <QLabel>
#include <QMouseEvent>
#include <QDialog>
#include "dialogrange.h"

class QLabelClick : public QLabel
{
Q_OBJECT
signals:
    void mousePressed( const QPoint& );

public:
    QLabelClick( QWidget* parent = 0, Qt::WindowFlags f = 0 );
    QLabelClick( const QString& text, QWidget* parent = 0, Qt::WindowFlags f = 0 );
    void setDialog(DialogRange *d);
    void mousePressEvent( QMouseEvent* ev );
    DialogRange *d;
};
