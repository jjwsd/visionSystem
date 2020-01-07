#include "dragbox.h"

#include <QBrush>
#include <QLinearGradient>
#include <QDebug>

#include "math.h"

/**
  *  This box can be re-sized and it can be moved. For moving, we capture
  *  the mouse move events and move the box location accordingly.
  *
  *  To resize the box, we place small indicator boxes on the four corners that give the user
  *  a visual cue to grab with the mouse. The user then drags the corner to stretch
  *  or shrink the box.  The corner grabbers are implemented with the CornerGrabber class.
  *  The CornerGrabber class captures the mouse when the mouse is over the corner's area,
  *  but the StateBox object (which owns the corners) captures and processes the mouse
  *  events on behalf of the CornerGrabbers (because the owner wants to be
  *  resized, not the CornerGrabbers themselves). This is accomplished by installed a scene event filter
  *  on the CornerGrabber objects:
          _corners[0]->installSceneEventFilter(this);
  *
  *
  *
  */

CDragBox::CDragBox(const qreal iWidth, const qreal iHeight, Qt::GlobalColor color, const QPoint movingArea):
    _outterborderColor(color),
    _outterborderPen(),
    _location(0,0),
    _dragStart(0,0),
    _gridSpace(10),
    _width(iWidth),
    _height(iHeight),
    _cornerDragStart(0,0),
    m_XcornerGrabBuffer(iWidth/10), // width size / 10
    m_YcornerGrabBuffer(iHeight/10), // height size / 10
    _drawingWidth(  _width -   m_XcornerGrabBuffer),
    _drawingHeight( _height -  m_YcornerGrabBuffer),
    _drawingOrigenX( m_XcornerGrabBuffer),
    _drawingOrigenY( m_YcornerGrabBuffer),
    m_grabberSize(iWidth/20),
    m_bHorGrabEnable(true),
    m_bVerGrabEnable(true),
    m_movingArea(movingArea)
{

    _outterborderPen.setWidth(10);
    _outterborderPen.setColor(_outterborderColor);

    this->setAcceptHoverEvents(true);
}

QRectF CDragBox::getRectPosBySceneCoord() const
{
    QPointF topLeft = this->mapToScene(QPointF(_drawingOrigenX, _drawingOrigenY));
    QPointF bottomRight = this->mapToScene(QPointF(_drawingWidth, _drawingHeight));
    return QRectF(topLeft, bottomRight);
}



/**
 *  To allow the user to grab the corners to re-size, we need to get a hover
 *  indication. But if the mouse pointer points to the left, then when the mouse
 *  tip is to the left but just outsize the box, we will not get the hover.
 *  So the solution is to tell the graphics scene the box is larger than
 *  what the painter actually paints in. This way when the user gets the mouse
 *  within a few pixels of what appears to be the edge of the box, we get
 *  the hover indication.

 *  So the cornerGrabBuffer is a few pixel wide buffer zone around the outside
 *  edge of the box.
 *
 */
void CDragBox::adjustSize(int x, int y)
{
    _width += x;
    _height += y;

    _drawingWidth =  _width - m_XcornerGrabBuffer;
    _drawingHeight=  _height - m_YcornerGrabBuffer;
}

/**
  * This scene event filter has been registered with all four corner grabber items.
  * When called, a pointer to the sending item is provided along with a generic
  * event.  A dynamic_cast is used to determine if the event type is one of the events
  * we are interrested in.
  */
bool CDragBox::sceneEventFilter ( QGraphicsItem * watched, QEvent * event )
{
    qDebug() << " QEvent == " + QString::number(event->type());

    CornerGrabber * corner = dynamic_cast<CornerGrabber *>(watched);
    if ( corner == NULL) return false; // not expected to get here

    QGraphicsSceneMouseEvent * mevent = dynamic_cast<QGraphicsSceneMouseEvent*>(event);
    if ( mevent == NULL)
    {
        // this is not one of the mouse events we are interrested in
        return false;
    }


    switch (event->type() )
    {
    // if the mouse went down, record the x,y coords of the press, record it inside the corner object
    case QEvent::GraphicsSceneMousePress:
    {
        corner->setMouseState(CornerGrabber::kMouseDown);
        corner->mouseDownX = mevent->pos().x();
        corner->mouseDownY = mevent->pos().y();
    }
        break;

    case QEvent::GraphicsSceneMouseRelease:
    {
        corner->setMouseState(CornerGrabber::kMouseReleased);

    }
        break;

    case QEvent::GraphicsSceneMouseMove:
    {
        corner->setMouseState(CornerGrabber::kMouseMoving );
    }
        break;

    default:
        // we dont care about the rest of the events
        return false;
        break;
    }


    if ( corner->getMouseState() == CornerGrabber::kMouseMoving )
    {

        qreal x = mevent->pos().x(), y = mevent->pos().y();

        // depending on which corner has been grabbed, we want to move the position
        // of the item as it grows/shrinks accordingly. so we need to eitehr add
        // or subtract the offsets based on which corner this is.

        int XaxisSign = 0;
        int YaxisSign = 0;
        switch( corner->getCorner() )
        {
        case LeftTop:
        {
            XaxisSign = +1;
            YaxisSign = +1;
        }
            break;

        case RightTop:
        {
            XaxisSign = -1;
            YaxisSign = +1;
        }
            break;

        case RightBottom:
        {
            XaxisSign = -1;
            YaxisSign = -1;
        }
            break;

        case LeftBottom:
        {
            XaxisSign = +1;
            YaxisSign = -1;
        }
            break;
        case TopMiddle:
        {
            XaxisSign = 0;
            YaxisSign = +1;
        }
            break;
        case BottomMiddle:
        {
            XaxisSign = 0;
            YaxisSign = -1;
        }
            break;
        case RightMiddle:
        {
            XaxisSign = -1;
            YaxisSign = 0;
        }
            break;
        case LeftMiddle:
        {
            XaxisSign = +1;
            YaxisSign = 0;
        }
            break;
        }
        // if the mouse is being dragged, calculate a new size and also re-position
        // the box to give the appearance of dragging the corner out/in to resize the box

        int xMoved = corner->mouseDownX - x;
        int yMoved = corner->mouseDownY - y;

        int newWidth = _width + ( XaxisSign * xMoved);
        if(newWidth < m_grabberSize * 4)
        {
            newWidth = m_grabberSize * 4;
        }

        int newHeight = _height + (YaxisSign * yMoved) ;
        if(newHeight < m_grabberSize * 4)
        {
            newHeight = m_grabberSize * 4;
        }

        int deltaWidth  =   newWidth - _width ;
        int deltaHeight =   newHeight - _height ;

        adjustSize(  deltaWidth ,   deltaHeight);

        deltaWidth *= (-1);
        deltaHeight *= (-1);

        if ( corner->getCorner() == LeftTop )
        {
            int newXpos = this->pos().x() + deltaWidth;
            int newYpos = this->pos().y() + deltaHeight;
            this->setPos(newXpos, newYpos);
        }
        else if ( corner->getCorner() == RightTop )
        {
            int newYpos = this->pos().y() + deltaHeight;
            this->setPos(this->pos().x(), newYpos);
        }
        else if ( corner->getCorner() == LeftBottom )
        {
            int newXpos = this->pos().x() + deltaWidth;
            this->setPos(newXpos,this->pos().y());
        }
        else if (corner->getCorner() == TopMiddle)
        {
            int newYpos = this->pos().y() + deltaHeight;
            this->setPos(this->pos().x(), newYpos);
        }
        else if (corner->getCorner() == LeftMiddle)
        {
            int newXpos = this->pos().x() + deltaWidth;
            this->setPos(newXpos,this->pos().y());
        }

        if(this->pos().x() + m_XcornerGrabBuffer < 0)
        {
            this->setPos(-m_XcornerGrabBuffer, this->pos().y());
        }

        if(this->pos().y() + m_YcornerGrabBuffer < 0)
        {
            this->setPos(this->pos().x(), -m_YcornerGrabBuffer);
        }

        if(this->pos().x() + _drawingWidth > m_movingArea.x())
        {
            _drawingWidth = m_movingArea.x() - this->pos().x();
        }

        if(this->pos().y() + _drawingHeight > m_movingArea.y())
        {
            _drawingHeight = m_movingArea.y() - this->pos().y();
        }

        setCornerPositions();

        this->update();
    }

    return true;// true => do not send event to watched - we are finished with this event
}



// for supporting moving the box across the scene
void CDragBox::mouseReleaseEvent ( QGraphicsSceneMouseEvent * event )
{
    event->setAccepted(true);
    //_location.setX( ( static_cast<int>(_location.x()) / _gridSpace) * _gridSpace );
    //_location.setY( ( static_cast<int>(_location.y()) / _gridSpace) * _gridSpace );
    //this->setPos(_location);
}


// for supporting moving the box across the scene
void CDragBox::mousePressEvent ( QGraphicsSceneMouseEvent * event )
{
    event->setAccepted(true);
    _dragStart = event->pos();
}

// for supporting moving the box across the scene
void CDragBox::mouseMoveEvent ( QGraphicsSceneMouseEvent * event )
{
    QPointF newPos = event->pos() ;
    _location += (newPos - _dragStart);

    if(_location.x() + m_XcornerGrabBuffer < 0)
    {
        _location.setX(-m_XcornerGrabBuffer);
    }

    if(_location.y() + m_YcornerGrabBuffer < 0)
    {
        _location.setY(-m_YcornerGrabBuffer);
    }

    if(_location.x() + _drawingWidth > m_movingArea.x())
    {
        _location.setX(m_movingArea.x() - _drawingWidth);
    }

    if(_location.y() + _drawingHeight > m_movingArea.y())
    {
        _location.setY(m_movingArea.y() - _drawingHeight);
    }

    this->setPos(_location);
}

// remove the corner grabbers
void CDragBox::hoverLeaveEvent ( QGraphicsSceneHoverEvent * )
{    
    _outterborderColor = Qt::black;

    for (int var = 0; var < GrabberCnt; ++var) {
        _corners[var]->setParentItem(NULL);
        delete _corners[var];
    }
}

// create the corner grabbers

void CDragBox::hoverEnterEvent ( QGraphicsSceneHoverEvent * )
{
    _outterborderColor = Qt::red;

    if(_drawingWidth < m_grabberSize * 4)
    {
        //m_bHorGrabEnable = false;
    }
    else
    {
        m_bHorGrabEnable = true;
    }

    if(_drawingHeight < m_grabberSize * 4)
    {
        //m_bVerGrabEnable = false;
    }
    else
    {
        m_bVerGrabEnable = true;
    }

    for (int i = 0; i <= LeftBottom; ++i) {
        _corners[i] = new CornerGrabber(this, i, m_grabberSize);
        _corners[i]->installSceneEventFilter(this);
    }

    if(m_bHorGrabEnable)
    {
        _corners[TopMiddle] = new CornerGrabber(this, TopMiddle, m_grabberSize);
        _corners[TopMiddle]->installSceneEventFilter(this);

        _corners[BottomMiddle] = new CornerGrabber(this, BottomMiddle, m_grabberSize);
        _corners[BottomMiddle]->installSceneEventFilter(this);
    }

    if(m_bVerGrabEnable)
    {
        _corners[RightMiddle] = new CornerGrabber(this, RightMiddle, m_grabberSize);
        _corners[RightMiddle]->installSceneEventFilter(this);

        _corners[LeftMiddle] = new CornerGrabber(this, LeftMiddle, m_grabberSize);
        _corners[LeftMiddle]->installSceneEventFilter(this);
    }

    setCornerPositions();
}

void CDragBox::setCornerPositions()
{
    _corners[LeftTop]->setPos(_drawingOrigenX - m_grabberSize/2, _drawingOrigenY - m_grabberSize/2);
    _corners[RightTop]->setPos(_drawingWidth - m_grabberSize/2,  _drawingOrigenY - m_grabberSize/2);
    _corners[RightBottom]->setPos(_drawingWidth - m_grabberSize/2 , _drawingHeight - m_grabberSize/2);
    _corners[LeftBottom]->setPos(_drawingOrigenX - m_grabberSize/2, _drawingHeight - m_grabberSize/2);

    if(m_bHorGrabEnable)
    {
        _corners[TopMiddle]->setPos(_drawingOrigenX + _drawingWidth / 2 - m_grabberSize, _drawingOrigenY - m_grabberSize/2);
        _corners[BottomMiddle]->setPos(_drawingOrigenX + _drawingWidth / 2 - m_grabberSize, _drawingHeight - m_grabberSize/2);
    }

    if(m_bVerGrabEnable)
    {
        _corners[RightMiddle]->setPos(_drawingWidth - m_grabberSize/2, _drawingOrigenY + _drawingHeight / 2 - m_grabberSize/2);
        _corners[LeftMiddle]->setPos(_drawingOrigenX - m_grabberSize/2, _drawingOrigenY + _drawingHeight / 2 - m_grabberSize/2);
    }
}

QRectF CDragBox::boundingRect() const
{
    return QRectF(0,0,_width,_height);
}


// example of a drop shadow effect on a box, using QLinearGradient and two boxes

void CDragBox::paint (QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    /*
     The drop shadow effect will be created by drawing a filled, rounded corner rectangle with a gradient fill.
     Then on top of this will be drawn  filled, rounded corner rectangle, filled with a solid color, and offset such that the gradient filled
     box is only visible below for a few pixels on two edges.

     The total box size is _width by _height. So the top box will start at (0,0) and go to (_width-shadowThickness, _height-shadowThickness),
     while the under box will be offset, and start at (shadowThickness+0, shadowThickness+0) and go to  (_width, _height).
       */

    // for the desired effect, no border will be drawn, and because a brush was set, the drawRoundRect will fill the box with the gradient brush.
    _outterborderPen.setStyle(Qt::DotLine);
    painter->setPen(_outterborderPen);

    QPointF topLeft (_drawingOrigenX,_drawingOrigenY);
    QPointF bottomRight ( _drawingWidth , _drawingHeight);
    QRectF rect (topLeft, bottomRight);

    //painter->drawRoundRect(rect,25,25); // corner radius of 25 pixels
    painter->drawRect(rect);    
}


void CDragBox::mouseMoveEvent(QGraphicsSceneDragDropEvent *event)
{
    event->setAccepted(false);
}

void CDragBox::mousePressEvent(QGraphicsSceneDragDropEvent *event)
{
    event->setAccepted(false);
}
