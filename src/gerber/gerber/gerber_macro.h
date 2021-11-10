#pragma once

#include <memory>
#include <string>
#include <vector>
#include "gerber_command.h"


class RenderCommand;
class GerberMacro {
private:
	enum PRIMITIVE {
		pComment = 0,
		pCircle = 1,
		pLineVector = 2,
		pLineVector2 = 20,
		pLineCenter = 21,
		pLineLowerLeft = 22,
		pEndOfFile = 3,
		pOutline = 4,
		pPolygon = 5,
		pMoire = 6,
		pThermal = 7,
		pAssignment = -1,
	};

	enum OPERATOR {
		opAdd,
		opSubtract,
		opMultiply,
		opDivide,

		opVariable,
		opLiteral
	};

	struct OPERATOR_ITEM {
		OPERATOR       Operator;
		OPERATOR_ITEM* left_;
		OPERATOR_ITEM* right_;

		union {
			int    Index; // Variable Index
			double Value; // Literal Value
		};

		OPERATOR_ITEM();
		~OPERATOR_ITEM();
	};

	struct PRIMITIVE_ITEM {
		PRIMITIVE       Primitive;
		OPERATOR_ITEM** Modifier; // Modifier Tree for each modifier in the array
		int             ModifierCount;
		int             Index;    // Used for assignment primitives

		PRIMITIVE_ITEM* Next;

		PRIMITIVE_ITEM();
		~PRIMITIVE_ITEM();
	};
	PRIMITIVE_ITEM* Primitives;
	PRIMITIVE_ITEM* PrimitivesLast;
	void Add(PRIMITIVE_ITEM* Primitive);

	std::vector<std::shared_ptr<RenderCommand>> render_commands_;

	void Add(std::shared_ptr<RenderCommand> render);

	double Evaluate(OPERATOR_ITEM* root);

	void RenderLine(
		double x1, double y1,
		double x2, double y2,
		double x3, double y3,
		double x4, double y4,
		double xR, double yR, // Rotation Center
		double A
	);

	bool RenderCircle(PRIMITIVE_ITEM* primitive);
	bool RenderLineVector(PRIMITIVE_ITEM* primitive);
	bool RenderLineCenter(PRIMITIVE_ITEM* primitive);
	bool RenderLineLowerLeft(PRIMITIVE_ITEM* primitive);
	bool RenderOutline(PRIMITIVE_ITEM* primitive);
	bool RenderPolygon(PRIMITIVE_ITEM* primitive);
	bool RenderMoire(PRIMITIVE_ITEM* primitive);
	bool RenderThermal(PRIMITIVE_ITEM* primitive);
	bool RenderAssignment(PRIMITIVE_ITEM* primitive);

	double* modifiers_;
	int modifier_count_;
	bool new_modifiers_;
	bool exposure_;

	std::string buffer_;
	unsigned index_;
	bool inches_;

	double Get_mm(double number);

	bool Float(double* number);
	bool Integer(int* integer);
	void SkipWhiteSpace();

	OPERATOR_ITEM* modifier();
	OPERATOR_ITEM* Term();
	OPERATOR_ITEM* Factor();
	OPERATOR_ITEM* Variable();

	bool Primitive();
	bool Comment();
	bool Circle();
	bool Line_Vector();
	bool Line_Center();
	bool Line_LowerLeft();
	bool Outline();
	bool Polygon();
	bool Moire();
	bool Thermal();
	bool Assignment();

public:
	GerberMacro();
	~GerberMacro();

	std::string name_;

	// Modifiers is a null-terminated array
	// Returns a new list of render commands => The user must free the memory
	std::vector<std::shared_ptr<RenderCommand>> Render(double* Modifiers, int ModifierCount);

	bool LoadMacro(const std::string& buffer, bool Inches);
};
