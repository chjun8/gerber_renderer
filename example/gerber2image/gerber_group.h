#pragma once
#include <QString>
#include <memory>
#include <vector>

#include "bound_box.h"


class Gerber;

class GerberGroup {
public:
	GerberGroup(const QString& files);

	BoundBox GetBoundBox() const;
	std::pair<int, int> GetPixelSize(double um_pixel);

	std::vector<std::shared_ptr<Gerber>>::iterator begin();
	std::vector<std::shared_ptr<Gerber>>::iterator end();

private:
	std::vector<std::shared_ptr<Gerber>> gerbers_;
};