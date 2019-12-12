#ifndef USERRECTITEM_H
#define USERRECTITEM_H

#include <QGraphicsRectItem>
#include <QPen>

class UserRectItem : public QGraphicsRectItem
{
    enum HorDirection {
        HorNone = 0,
        Left = 1,
        Right = 2,
        HorCenter = 3,
    };

    enum VerDirection {
        VerNone = 0,
        Top = 1,
        Bottom = 2,
        VerCenter =3,
    };

public:
    UserRectItem(QGraphicsItem *parent = 0);
    UserRectItem(const QRectF &rect, QGraphicsItem *parent = 0);

    void SetRectItem(const QRectF & rect);
    void SetLineColor(Qt::GlobalColor color);
    void SetLineWidth(const int iLineWidth);
    bool IsEmpty() const;

    enum HorDirection m_eHorDirection;
    enum VerDirection m_eVerDirection;

    float m_fHorDist;
    float m_fVerDist;

    QRectF getRectPosBySceneCoord() const;

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent * event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent * event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent * event);

    void hoverEnterEvent(QGraphicsSceneHoverEvent * event);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent * event);
    void hoverMoveEvent(QGraphicsSceneHoverEvent * event);

    void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0);

private:
    void find_model_resize_direction(const QPointF& pos);
    bool is_model_resize_mode() const;
    bool is_model_move_mode() const;
    void set_mouse_cursor(QGraphicsSceneHoverEvent *event);
    QRectF get_boarder_rect();
    QPointF m_pt;
    bool bDragMode;

    QColor m_lineColor;
    QPen   m_Pen;
    int m_iLineWidth;
};

#endif // USERRECTITEM_H
