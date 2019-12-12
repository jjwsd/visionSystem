#include "userrectitem.h"

#include <QApplication>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QtMath>
#include <QDebug>

#define DEBUG_MODE 0

UserRectItem::UserRectItem(QGraphicsItem *parent):QGraphicsRectItem(parent),m_lineColor(Qt::green),m_iLineWidth(10)
{
    this->setFlag(QGraphicsItem::ItemSendsGeometryChanges);
    this->setAcceptHoverEvents(true);
    bDragMode = false;

    m_Pen.setStyle(Qt::DashDotLine);
    m_Pen.setWidth(m_iLineWidth);
    m_Pen.setCapStyle(Qt::RoundCap);
    m_Pen.setJoinStyle(Qt::RoundJoin);
    m_Pen.setColor(m_lineColor);    
}

UserRectItem::UserRectItem(const QRectF &rect, QGraphicsItem *parent):QGraphicsRectItem(rect, parent)
  ,m_lineColor(Qt::green),m_iLineWidth(10)
{
    this->setRect(rect);
    this->setFlag(QGraphicsItem::ItemSendsGeometryChanges);
    this->setAcceptHoverEvents(true);
    bDragMode = false;

    m_Pen.setStyle(Qt::DashDotLine);
    m_Pen.setWidth(m_iLineWidth);
    m_Pen.setCapStyle(Qt::RoundCap);
    m_Pen.setJoinStyle(Qt::RoundJoin);
    m_Pen.setColor(m_lineColor);
}

void UserRectItem::SetRectItem(const QRectF &rect)
{
    this->setRect(rect);
}

void UserRectItem::SetLineColor(Qt::GlobalColor color)
{
    m_lineColor = color;
    m_Pen.setColor(m_lineColor);
}

void UserRectItem::SetLineWidth(const int iLineWidth)
{
    m_iLineWidth = iLineWidth;
    m_Pen.setWidth(m_iLineWidth);
}

bool UserRectItem::IsEmpty() const
{
    return this->rect().isEmpty();
}

QRectF UserRectItem::getRectPosBySceneCoord() const
{
    QPointF topLeft = this->mapToScene(this->rect().topLeft());
    QPointF bottomRight = this->mapToScene(this->rect().bottomRight());
    return QRectF(topLeft, bottomRight);
}

void UserRectItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{    
    find_model_resize_direction(event->pos());
    if(is_model_resize_mode() == false)
    {
        event->ignore();
    }
}

void UserRectItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if(is_model_resize_mode() == false)
    {
        event->ignore();
    }
    else if(is_model_move_mode() == true)
    {
        if(bDragMode == false)
        {
            m_pt = event->pos();            
            bDragMode = true;
#if DEBUG_MODE
            QPointF eventPos = this->mapToScene(event->pos());
            QPointF rectPos = this->mapToScene(rect().topLeft());
            QPointF diffPos = this->mapToScene(m_pt);

            qDebug() << "";
            qDebug() <<"<Local> ClickPos (X = "<< event->pos().x() << ", Y = " << event->pos().y() << ")";
            qDebug() << "<Scene> ClickPos (X = " << eventPos.x() << ", Y = " << eventPos.y() << ")";

            qDebug() <<"<Local> CurRect (X = "<< this->rect().topLeft().x() << ", Y = " << this->rect().topLeft().y() << ")";
            qDebug() << "<Scene> CurRect (X = " << rectPos.x() << ", Y = " << rectPos.y() << ")";

            qDebug() <<"<Local> Diff (X = "<< m_pt.x() << ", Y = " << m_pt.y() << ")";
            qDebug() << "<Scene> Diff (X = " << diffPos.x() << ", Y = " << diffPos.y() << ")";
#endif
            this->prepareGeometryChange();            
            this->setPos(0,0);
        }
        else
        {
            this->prepareGeometryChange();
            this->setPos(this->mapToScene(event->pos() -m_pt));
        }
#if DEBUG_MODE
        QPointF tempScene = this->mapToScene(this->rect().topLeft());
        qDebug() << "[Local]rectPos (X = " << this->rect().left() << ", Y = " << this->rect().top()<< ")";
        qDebug() << "[Scene]rectPos (X = " <<  tempScene.x() << ", Y = " << tempScene.y() << ")";
#endif
    }
    else
    {
        qreal left = this->rect().left();
        qreal right = left + this->rect().width();
        qreal top   = this->rect().top();
        qreal bottom = top + this->rect().height();

#if DEBUG_MODE
        qDebug() << "1. ========================";
        qDebug() << "left = " << left << " \\ widht = " << right-left ;
        qDebug() << "1. End ====================";
#endif

        if(m_eHorDirection == Left)
        {
            left = event->pos().x() + this->m_fHorDist;
        } else if (m_eHorDirection == Right)
        {
            right = event->pos().x() + this->m_fHorDist;
        }

        if (m_eVerDirection == Top)
        {
            top = event->pos().y() + this->m_fVerDist;
        } else if (m_eVerDirection == Bottom)
        {
           bottom = event->pos().y() + this->m_fVerDist;
        }

        QRectF newRect(left, top, right - left, bottom - top);
#if DEBUG_MODE
        qDebug() << "2 ===================================";
        qDebug() << "2 newRect xPos =" << newRect.left()
                 << " \\  newRect width = " << newRect.width();
        qDebug() << "End =================================";
#endif

        if (newRect != this->rect())
        {
            this->prepareGeometryChange();
            QPointF temp = this->mapToParent(newRect.topLeft() - this->rect().topLeft());
#if DEBUG_MODE
            qDebug() << "3 =================================";
            qDebug() << "3 maptoParent x Offset = " << temp.x();
            qDebug() << "  real x Offset " << (newRect.topLeft() - this->rect().topLeft()).x();
            qDebug() << "3 End =============================";

            qDebug() << "4 =================================";
            qDebug() << "4 cur X = " << this->scenePos().x();
            qDebug() << "4 cur Widht = " << this->scenePos().y();
#endif
            this->setPos(temp);
#if DEBUG_MODE
            qDebug() << "5 =================================";
            qDebug() << "5 cur X = " << this->scenePos().x();
            qDebug() << "5 cur Widht = " << this->scenePos().y();

            qDebug() << "6 =================================";
            qDebug() << "6 new X = " << newRect.x();
            qDebug() << "6 new Widht = " << newRect.width();
            qDebug() << "6 End =============================";
#endif
            newRect.translate(this->rect().topLeft() - newRect.topLeft());

#if DEBUG_MODE
            qDebug() << "7 =================================";
            qDebug() << "7 new X = " << newRect.x();
            qDebug() << "7 new Widht = " << newRect.width();
            qDebug() << "7 End =============================";
#endif
            this->setRect(newRect);

            QPointF temp3 = this->mapFromScene(newRect.topLeft());
            QPointF temp4 = this->mapFromScene(newRect.bottomRight());
            QPointF temp5 = this->mapToScene(newRect.topLeft());
            QPointF temp6 = this->mapToScene(newRect.bottomRight());

#if DEBUG_MODE
            qDebug() << "mapFromScene() = " << temp3.x()
                     << " \\ width = " << temp4.x()- temp3.x();

            qDebug() << "mapToScene() = " << temp5.x()
                     << " \\ width = " << temp6.x()- temp5.x();
#endif

        }
    }
}

void UserRectItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    bDragMode = false;
}

void UserRectItem::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{    
    find_model_resize_direction(event->pos());
    set_mouse_cursor(event);
}

void UserRectItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    m_eHorDirection = HorNone;
    m_eVerDirection = VerNone;
    QApplication::restoreOverrideCursor();
}

void UserRectItem::hoverMoveEvent(QGraphicsSceneHoverEvent *event)
{
    find_model_resize_direction(event->pos());
    set_mouse_cursor(event);
}

void UserRectItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    QPen oldPen = painter->pen();
    QBrush oldBrush = painter->brush();

    painter->setPen(m_Pen);
    painter->drawRect(this->rect());

    painter->setPen(oldPen);
    painter->setBrush(oldBrush);
}

void UserRectItem::find_model_resize_direction(const QPointF& pos)
{
    QRectF outRect = get_boarder_rect();

    if(pos.x() < outRect.left())
    {
        m_eHorDirection = Left;
        m_fHorDist = this->rect().left() - pos.x();
    }
    else if(pos.x() > outRect.right())
    {        
        m_eHorDirection = Right;
        m_fHorDist = this->rect().right() - pos.x();
    }
    else if(pos.x() >= outRect.left() && pos.x() <= outRect.right())
    {
        m_eHorDirection = HorCenter;
    }
    else
    {
        m_eHorDirection = HorNone;
    }

    if(pos.y() < outRect.top())
    {
        m_eVerDirection = Top;
        m_fVerDist = this->rect().top() - pos.y();
    }
    else if(pos.y() > outRect.bottom())
    {
        m_eVerDirection = Bottom;
        m_fVerDist = this->rect().bottom() - pos.y();
    }
    else if(pos.y() >= outRect.top() && pos.y() <= outRect.bottom())
    {
        m_eVerDirection = VerCenter;
    }
    else
    {
        m_eVerDirection = VerNone;
    }
}

bool UserRectItem::is_model_resize_mode() const
{
    return (this->m_eHorDirection != HorNone || this->m_eHorDirection != VerNone);
}

bool UserRectItem::is_model_move_mode() const
{
    return (this->m_eHorDirection == HorCenter && this->m_eVerDirection == VerCenter);
}

void UserRectItem::set_mouse_cursor(QGraphicsSceneHoverEvent *event)
{
    if(get_boarder_rect().contains(event->pos()))
    {
        QApplication::restoreOverrideCursor();
        QApplication::setOverrideCursor(Qt::SizeAllCursor);
    }
    else
    {
        QCursor tmpCursor = Qt::SizeAllCursor;

        if(m_eHorDirection != HorNone && m_eHorDirection != HorCenter)
        {
            tmpCursor = Qt::SizeHorCursor;
        }

        if(m_eVerDirection != VerNone && m_eVerDirection != VerCenter)
        {
            tmpCursor = Qt::SizeVerCursor;
        }

        if ((m_eVerDirection == Top && m_eHorDirection == Left) ||
            (m_eVerDirection == Bottom && m_eHorDirection == Right) )
        {
            tmpCursor = Qt::SizeFDiagCursor;
        }

        if ((m_eVerDirection == Top && m_eHorDirection == Right) ||
            (m_eVerDirection == Bottom && m_eHorDirection == Left))
        {
            tmpCursor = Qt::SizeBDiagCursor;
        }

        QApplication::restoreOverrideCursor();
        QApplication::setOverrideCursor(tmpCursor);
    }
}

QRectF UserRectItem::get_boarder_rect()
{
    QRectF rect = this->rect();
    return rect.adjusted(50, 50, -50, -50);
}

