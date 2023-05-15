#include <SharedMacros.h>
#if STYLE_SHEETS == 1
#include "CustomButton.h"
#include <QEvent>

CustomButton::CustomButton(const QString& text, QWidget* parent) : QPushButton(parent) {
	setText(text);
}

void CustomButton::SetOnHoverEffect(std::unique_ptr<QGraphicsEffect> effect) {
	onHoverEffect = std::move(effect);
	onHoverEffect->setEnabled(false);
	setGraphicsEffect(onHoverEffect.get());
}

bool CustomButton::event(QEvent* event) {
	if (event->type() == QEvent::Enter) { // Mouse hover
		if (onHoverEffect) {
			int xxx = 1;
			//setGraphicsEffect(onHoverEffect);
			onHoverEffect->setEnabled(true);
			int yyy = 1;
		}
	}

	if (event->type() == QEvent::Leave) {
		if (onHoverEffect) {
			int xxx = 1;
			//setGraphicsEffect(nullptr);
			onHoverEffect->setEnabled(false);
			int yyy = 1;
		}
	}

	return QWidget::event(event);
}


#endif