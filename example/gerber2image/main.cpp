#include <QApplication>
#include <QFileDialog>
#include <QIcon>
#include <QBitmap>

#include "gerber_renderer.h"
#include "engine/qt_engine.h"
#include "gerber_group.h"

#include <gflags/gflags.h>


DEFINE_string(gerber_files, "", "The path of gerber files you want to export.If there are more than one file, separate them with ','.");
DEFINE_string(output_path, "", "Output path of rendered image files");
DEFINE_double(um_pixel, 5, "How much um/pixel.Default value is 5um/pixel");


QString GetExportFileName(std::shared_ptr<Gerber> gerber, int i, int j) {
	auto file_name = QString(gerber->FileName().c_str()).split('/').last();
	auto image_file = QString(FLAGS_output_path.c_str()) + file_name + '_' + QString("%1").arg(i) + '_' + QString("%1").arg(j) + ".bmp";
	return image_file;
}

std::pair<int, int> GetPiecePixelSize(const std::pair<int, int>& origin_pix_size, int width) {
	if (origin_pix_size.first > origin_pix_size.second) {
		return { width , width * origin_pix_size.second / origin_pix_size.first };
	}

	return { width * origin_pix_size.first / origin_pix_size.second, width };
}

void ExportGerber(std::shared_ptr<Gerber> gerber, const BoundBox& box, const std::pair<int, int>& piece_pix_size, const std::pair<int, int>& total_pix_size)
{
	auto image = std::make_unique<QBitmap>(piece_pix_size.first, piece_pix_size.second);
	auto engine = std::make_unique<QtEngine>(image.get(), box, BoundBox(0.0, 0.0, 0.0, 0.0));
	GerberRender render(engine.get());

	int y_piece_cnt = (total_pix_size.second - 1) / piece_pix_size.second + 1;
	int x_piece_cnt = (total_pix_size.first - 1) / piece_pix_size.first + 1;

	engine->Scale(std::max(x_piece_cnt, y_piece_cnt) - 1);

	for (int i = 0; i < y_piece_cnt; ++i) {
		for (int j = 0; j < x_piece_cnt; ++j) {
			render.RenderGerber(gerber);
			image->save(GetExportFileName(gerber, i, j));

			engine->Move(-piece_pix_size.first, 0);
		}

		engine->Move(piece_pix_size.first * x_piece_cnt, -piece_pix_size.second);
	}
}

int main(int argc, char* argv[]) {
	gflags::SetUsageMessage("Usage: gerber2image --gerber_files=\"path/to/gerber/file1, path/to/gerber/file2...\" --output_path=\"path/\" --um_pixel=5.\
 Tip: The separator of file path MUST be slash rather than backslash.");
	gflags::ParseCommandLineFlags(&argc, &argv, true);

	QApplication app(argc, argv);

	QDir dir(FLAGS_output_path.c_str());
	if (!dir.exists()) {
		dir.mkpath(FLAGS_output_path.c_str());
	}

	GerberGroup gerber_group(FLAGS_gerber_files.c_str());
	const auto total_pix_size = gerber_group.GetPixelSize(FLAGS_um_pixel);

	for (const auto gerber : gerber_group) {
		const auto piece_pix_size = GetPiecePixelSize(total_pix_size, 20000);
		ExportGerber(gerber, gerber_group.GetBoundBox(), piece_pix_size, total_pix_size);
	}

	return 0;
}
