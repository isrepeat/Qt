#pragma once
#include <QWidget>
#include <QLabel>
#include <QGridLayout>
#include <QPropertyAnimation>
#include <QTimer>
// PopupWindow - https://evileg.com/ru/post/146/

class PopUpMessage : public QWidget {
    Q_OBJECT

        Q_PROPERTY(float popupOpacity READ getPopupOpacity WRITE setPopupOpacity);

    void setPopupOpacity(float opacity);
    float getPopupOpacity() const;

public:
    PopUpMessage(QWidget* parent = Q_NULLPTR);

public slots:
    void ShowMessage(const QString& text);

private slots:
    void HideAnimation();
    void Hide();

private:
    void paintEvent(QPaintEvent* event) final;

private:
    QLabel label;
    QGridLayout layout;
    QPropertyAnimation animation;
    float popupOpacity;
    QTimer* timer;
};