#include "PopUpMessage.h"
#include <QDesktopWidget>
#include <QApplication>
#include <QPainter>
#include <QDebug>
#include <Windows.h>
#include <thread>


AnimationTwoPass::AnimationTwoPass(QObject* targetObject, QString propertyName, QString selfName)
    : timer{ new QTimer(this) }
    , selfName{ selfName }
    , animationPass{ 0 }
{
    animation.setTargetObject(targetObject);
    animation.setPropertyName(propertyName.toUtf8());
    connect(&animation, &QAbstractAnimation::finished, this, [this] {
        if (--animationPass == 0) {
            emit Finished(this->selfName);
        }
        });
}


void AnimationTwoPass::StartAnimation(float startValue, float endValue, int durationShowMs, int durationIdleMs, int durationHideMs) {
    if (animationPass.exchange(AnimationTwoPass::PASS_COUNT))
        return; // return if previous value != 0

    disconnect(timer, &QTimer::timeout, nullptr, nullptr);
    connect(timer, &QTimer::timeout, this, [=] {
        timer->stop();
        animation.setDuration(durationHideMs);
        animation.setStartValue(endValue);
        animation.setEndValue(startValue);
        animation.start();
        });

    animation.setDuration(durationShowMs);
    animation.setStartValue(startValue);
    animation.setEndValue(endValue);
    animation.start();

    timer->start(durationShowMs + durationIdleMs);
}

bool AnimationTwoPass::IsFinished() {
    return animationPass == 0;
}





PopUpMessage::PopUpMessage(QString selfName, QWidget* parent)
    : QWidget(parent)
    , selfName{ selfName }
    , animation { this, "popupOpacity", selfName + "_[anim]" }
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::Tool | Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_ShowWithoutActivating);

    label.setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    label.setStyleSheet("QLabel { color : white; "
        "margin-top: 6px;"
        "margin-bottom: 6px;"
        "margin-left: 10px;"
        "margin-right: 10px; }");

    layout.addWidget(&label, 0, 0);
    setLayout(&layout);

    connect(&animation, &AnimationTwoPass::Finished, this, &PopUpMessage::Hide);

    QObject::connect(qApp, &QCoreApplication::aboutToQuit, this, [this] {
        QWidget::hide(); // widget must be hidden when app event loop was finished but animation is not finished
        });
}


void PopUpMessage::ShowMessage(const QString& text, int id) {
    label.setText(text);
    adjustSize();

    auto padding = 15;
    auto offsetBetweenY = 15;

    auto desktopGeometry = QApplication::desktop()->availableGeometry();
    auto startX = desktopGeometry.x() + (desktopGeometry.width() - width()) - padding;
    auto startY = desktopGeometry.y() + (desktopGeometry.height() - height()) - (padding + id * (height() + offsetBetweenY));
    setGeometry(startX, startY, width(), height());
    
    setWindowOpacity(0.0);

    QWidget::show();
    animation.StartAnimation(0.0, 1.0, 150, 3000, 1000);
    //animation.StartAnimation(0.0, 1.0, 1000, 1000, 1000); // total animation duration = 2s
}

bool PopUpMessage::IsFinished() {
    return animation.IsFinished();
}

void PopUpMessage::Hide(QString animationName) {
    QWidget::hide();
}

void PopUpMessage::SetPopupOpacity(float opacity) {
    popupOpacity = opacity;
    setWindowOpacity(opacity);
}

float PopUpMessage::GetPopupOpacity() const {
    return popupOpacity;
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







PopUpMessager& PopUpMessager::GetInstance() {
    static PopUpMessager instance;
    return instance;
}

PopUpMessager::PopUpMessager(QObject* parent)
    : QObject(parent)
    , popUpMessages{ CreatePopUpMessages(5) }
{
}

std::deque<PopUpMessage> PopUpMessager::CreatePopUpMessages(int count) {
    std::deque<PopUpMessage> popUpMessagesTmp;
    for (int i = 0; i < count; i++) {
        popUpMessagesTmp.emplace_back(QString("popUpMessage_%1").arg(i), nullptr);
    }
    return popUpMessagesTmp;
}


void PopUpMessager::ShowMessage(const QString& text) {
    auto& _this = GetInstance();

    int messageId = 0; // id after last not hidden message
    for (int i = 0; i < _this.popUpMessages.size(); i++) {
        bool isHidden = _this.popUpMessages[i].isHidden();
        bool isFinished = _this.popUpMessages[i].IsFinished();
        if (!_this.popUpMessages[i].IsFinished()) {
            messageId = i + 1;
        }
    }

    if (messageId < _this.popUpMessages.size()) {
        _this.popUpMessages[messageId].ShowMessage(text, messageId);
    }
}