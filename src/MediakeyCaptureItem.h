#pragma once

#include <QDeclarativeItem>


class MediakeyCaptureItemPrivate;

class MediakeyCaptureItem : public QDeclarativeItem
{
    Q_OBJECT

    public:
        MediakeyCaptureItem(QDeclarativeItem *parent = 0);
        void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    signals:
        void volumeDownPressed();
        void volumeUpPressed();

    private:
        MediakeyCaptureItemPrivate *d_ptr;

        friend class MediakeyCaptureItemPrivate;
};
