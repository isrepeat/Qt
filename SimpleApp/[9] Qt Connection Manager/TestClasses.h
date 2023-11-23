#pragma once
#include <QObject>

struct MovableStruct {
    MovableStruct() 
        : data{1234} 
    {};
    ~MovableStruct() {
    };

private:
    int data;
};


class Sender : public QObject {
    Q_OBJECT;
public:
    Sender() {}
    ~Sender() {}

signals:
    void VoidSignal();
    float FloatSignalWithIntArgs(int);
};


class SenderTemplateNotifier : public QObject {
    Q_OBJECT;
signals:
    void VoidSignal();
    float FloatSignalWithIntArgs(int);
};

template <typename T>
class SenderTemplate : public SenderTemplateNotifier {
public:
    SenderTemplate() {}
    ~SenderTemplate() {}

    void VoidSignal_own() {}

private:
    T someType;
};


class Receiver : public QObject {
    Q_OBJECT;
public:
    Receiver() {}
    ~Receiver() {}

public slots:
    void VoidSlot() {
        int xxx = 9;
    }
};