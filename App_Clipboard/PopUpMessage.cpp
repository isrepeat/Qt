#include "PopUpMessage.h"
#include <QPainter>
#include <QApplication>
#include <QDesktopWidget>
#include <QDebug>


PopUpMessage::PopUpMessage(QWidget* parent) : QWidget(parent) {
    setWindowFlags(Qt::FramelessWindowHint | Qt::Tool | Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_ShowWithoutActivating);

    animation.setTargetObject(this);
    animation.setPropertyName("popupOpacity");
    connect(&animation, &QAbstractAnimation::finished, this, &PopUpMessage::Hide);

    label.setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    label.setStyleSheet("QLabel { color : white; "
        "margin-top: 6px;"
        "margin-bottom: 6px;"
        "margin-left: 10px;"
        "margin-right: 10px; }");

    layout.addWidget(&label, 0, 0);
    setLayout(&layout);

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &PopUpMessage::HideAnimation);
}

void PopUpMessage::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QRect roundedRect;
    roundedRect.setX(rect().x() + 5);
    roundedRect.setY(rect().y() + 5);
    roundedRect.setWidth(rect().width() - 10);
    roundedRect.setHeight(rect().height() - 10);

    painter.setBrush(QBrush(QColor(0, 0, 0, 180)));
    painter.setPen(Qt::NoPen);

    painter.drawRoundedRect(roundedRect, 10, 10);
}


void PopUpMessage::ShowMessage(const QString& text) {
    label.setText(text);
    adjustSize();

    setWindowOpacity(0.0);

    animation.setDuration(150);
    animation.setStartValue(0.0);
    animation.setEndValue(1.0);

    setGeometry(QApplication::desktop()->availableGeometry().width() - 14 - width() + QApplication::desktop()->availableGeometry().x(),
        QApplication::desktop()->availableGeometry().height() - 14 - height() + QApplication::desktop()->availableGeometry().y(),
        width(),
        height());

    QWidget::show();

    animation.start();
    timer->start(3000);
}

void PopUpMessage::HideAnimation() {
    timer->stop();
    animation.setDuration(1000);
    animation.setStartValue(1.0);
    animation.setEndValue(0.0);
    animation.start();
}

void PopUpMessage::Hide() {
    if (getPopupOpacity() == 0.0) {
        QWidget::hide();
    }
}

void PopUpMessage::setPopupOpacity(float opacity) {
    popupOpacity = opacity;
    setWindowOpacity(opacity);
}

float PopUpMessage::getPopupOpacity() const {
    return popupOpacity;
}