#pragma once
#include <QGraphicsDropShadowEffect>
#include <QGraphicsEffect>
#include <QPushButton>

class CustomButton : public QPushButton {
    Q_OBJECT

public:
    CustomButton(const QString& text = "", QWidget* parent = 0);
    void SetOnHoverEffect(std::unique_ptr<QGraphicsEffect> effect);


private:
    bool event(QEvent* event) override;

private:
    std::unique_ptr<QGraphicsEffect> onHoverEffect;
};