#include "gerber_group.h"
#include "gerber/gerber.h"
#include <QStringList>


GerberGroup::GerberGroup(const QString& files)
{
	auto file_list = files.split(',', Qt::SkipEmptyParts);

	for (const auto file : file_list) {
		gerbers_.push_back(std::make_shared<Gerber>(file.toLocal8Bit().toStdString()));
	}
}

BoundBox GerberGroup::GetBoundBox() const
{
	BoundBox box;
	for (const auto gerber : gerbers_) {
		box.UpdateBox(gerber->GetBBox());
	}

	return box;
}

std::vector<std::shared_ptr<Gerber>>::iterator GerberGroup::end()
{
	return gerbers_.end();
}

std::vector<std::shared_ptr<Gerber>>::iterator GerberGroup::begin()
{
	return gerbers_.begin();
}

std::pair<int, int> GerberGroup::GetPixelSize(double um_pixel) {
	if (gerbers_.empty()) {
		return { 0 ,0 };
	}

	const auto box = GetBoundBox();
	auto width = box.Right() - box.Left();
	auto height = box.Top() - box.Bottom();
	if (gerbers_[0]->Unit() == GERBER_UNIT::guInches) {
		width *= 25.4;
		height *= 25.4;
	}

	return std::make_pair(width * 1000 / um_pixel, height * 1000 / um_pixel);
}
