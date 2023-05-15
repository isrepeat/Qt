#include <SharedMacros.h>
#if LOADER == 1
#pragma once
#include <QWidget>
#include <QLabel>
#include <QMovie>

class Loader : public QWidget {
	Q_OBJECT
public:
	Loader(const QString& filename, QWidget* parent = nullptr);
	~Loader() = default;

	void Show();
	void Hide();

private:
	void MoveToCenter();

private:
	QMovie* movie;
	QLabel* label;
};
#endif