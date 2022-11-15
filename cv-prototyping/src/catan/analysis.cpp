#include <catan/analysis.hpp>

ctn::BoardInfo ctn::AnalyzeBoard(const BoardIR &boardIR)
{
	BoardInfo result;
	result.cellTypes = DetermineCellTypes(boardIR);
	result.settlements = FindSettlements(boardIR);

	return result;
}
