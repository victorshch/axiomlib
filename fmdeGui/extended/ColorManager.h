/****************************************************************************
*			ColorManager
*
*	Description:
*	Author:		wictor
*	History:	
*
****************************************************************************/


#ifndef COLORMANAGER_H_
#define COLORMANAGER_H_

#include <QColor>

class ColorManager {
	QColor currentColor;
	static const unsigned int initialColor = 0;
	unsigned int hueStep;
public:

	ColorManager(int numColors = 20) {
		hueStep = 17;
		currentColor = QColor::fromHsv(0, 255, 255, 255);
	}

	QColor getColor() {
		QColor result = currentColor;
		currentColor.setHsv(currentColor.hue() + hueStep, 
							currentColor.saturation(), 
							currentColor.value());
		return result.toRgb();
	}
};

#endif /* COLORMANAGER_H_ */
