#pragma once
#include <memory>

class GerberLevel;
class GerberAperture;

class Plotter {
public:
	Plotter(GerberLevel& level);

	void OutlineBegin(unsigned line_number);
	void OutlineEnd(unsigned line_number);
	void Do(unsigned LineNumber);
	void ApertureSelect(std::shared_ptr<GerberAperture> aperture_, unsigned LineNumber);

	void SetInPath(bool in_path) {
		in_path_ = in_path;
	}

	bool IsInPath() const {
		return in_path_;
	}

	void SetPreX(double pre_x) {
		pre_x_ = pre_x;
	}

	double GetPreX() const {
		return pre_x_;
	}

	void SetPreY(double pre_y) {
		pre_y_ = pre_y;
	}

	double GetPreY() const {
		return pre_y_;
	}

	std::shared_ptr<GerberAperture> GetCurrentAperture() const {
		return current_aperture;
	}

	void SetCurrentApeture(std::shared_ptr<GerberAperture> aperture) {
		current_aperture = aperture;
	}

private:
	void Move(unsigned LineNumber);
	void Line();
	void Arc();
	void Flash();

	double Get_mm(double Number);
	double GetAngle(
		double x1, double y1, // Start, relative to center
		double x2, double y2  // End, relative to center
	);

	GerberLevel& level_;

	bool outline_fill_{ false };

	bool in_path_{ false };
	double pre_x_{ 0.0 }, pre_y_{ 0.0 };
	double first_x_{ 0.0 }, first_y_{ 0.0 };

	std::shared_ptr<GerberAperture> current_aperture;
};
