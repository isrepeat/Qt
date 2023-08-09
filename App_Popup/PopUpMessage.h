#pragma once
#include <QWidget>
#include <QLabel>
#include <QGridLayout>
#include <QPropertyAnimation>
#include <QTimer>
#include <condition_variable>
#include <Windows.h>
// orig PopupWindow - https://evileg.com/ru/post/146/


#define LOG_FIELD_DESTRUCTION(ClassName, CustomField)													\
    struct ClassName##CustomField {																		\
        ~ClassName##CustomField() {																		\
            OutputDebugStringA("===== ~" #ClassName #CustomField " destroyed ===== \n");				\
        }																								\
    } ClassName##CustomField##_Instance;



#define SINGLETON

class PopUpMessage : public QWidget {
private:
    Q_OBJECT
    Q_PROPERTY(float popupOpacity READ getPopupOpacity WRITE setPopupOpacity);

#ifdef SINGLETON
private:
    static PopUpMessage& GetInstance();
    PopUpMessage(QWidget* parent = nullptr);
#else
public:
    PopUpMessage(QWidget* parent = nullptr);
#endif

public:
    ~PopUpMessage();

#ifdef SINGLETON
    static void ShowMessage(const QString& text);
#else
    void ShowMessage(const QString& text);
#endif

private slots:
    void HideAnimation();
    void Hide();

private:
    void ShowMessageInternal(const QString& text);
    void setPopupOpacity(float opacity);
    float getPopupOpacity() const;

    void paintEvent(QPaintEvent* event) final;

private:
    QLabel label;
    QGridLayout layout;

    QTimer* timer;
    QPropertyAnimation animation;
    float popupOpacity;
};