#pragma once
#include <QWidget>
#include <QLabel>
#include <QGridLayout>
#include <QPropertyAnimation>
#include <QTimer>
#include <condition_variable>
#include <Windows.h>
#include <deque>
// orig PopupWindow - https://evileg.com/ru/post/146/

namespace H {
    template<class T, class ContainerT>
    class fixed_container : private ContainerT {
    public:
        using Base = ContainerT;
        using Base::Base;

        using Base::size;
        using Base::begin;
        using Base::end;
        using Base::operator[];

        fixed_container(ContainerT&& other)
            : ContainerT(std::move(other))
        {}
    };

    template<class T>
    using fixed_deque = fixed_container<T, std::deque<T>>;
}


class AnimationTwoPass : public QObject {
private:
    Q_OBJECT
    static const int PASS_COUNT = 2;

public:
    AnimationTwoPass(QObject* targetObject, QString propertyName, QString selfName);
    ~AnimationTwoPass() = default;

    void StartAnimation(float startValue, float endValue, int durationShowMs, int durationIdleMs, int durationHideMs);
    bool IsFinished();

signals:
    void Finished(QString selfName);

private:
    QTimer* timer;
    QString selfName;
    QPropertyAnimation animation;

    std::atomic<int> animationPass;
};



class PopUpMessage : public QWidget {
private:
    Q_OBJECT
    Q_PROPERTY(float popupOpacity READ GetPopupOpacity WRITE SetPopupOpacity);

public:
    PopUpMessage(QString selfName, QWidget* parent = nullptr);
    ~PopUpMessage() = default;

    void ShowMessage(const QString& text, int id = 0);
    bool IsFinished();

private slots:
    void Hide(QString animationName);

private:
    void SetPopupOpacity(float opacity);
    float GetPopupOpacity() const;

    void paintEvent(QPaintEvent* event) final;

private:
    QLabel label;
    QString selfName;
    QGridLayout layout;
    AnimationTwoPass animation;

    float popupOpacity;
};



class PopUpMessager : public QObject {
private:
    Q_OBJECT

    static PopUpMessager& GetInstance();
    PopUpMessager(QObject* parent = nullptr);
public:
    ~PopUpMessager() = default;

    static void ShowMessage(const QString& text);

private:
    std::deque<PopUpMessage> CreatePopUpMessages(int count);

private:
    H::fixed_deque<PopUpMessage> popUpMessages;
    //std::deque<PopUpMessage> popUpMessages;
};