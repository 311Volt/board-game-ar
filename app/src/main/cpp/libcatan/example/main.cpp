


#include <fmt/format.h>

#include <catan.hpp>



double GetTime()
{
	static auto t0 = std::chrono::high_resolution_clock::now();
	auto t1 = std::chrono::high_resolution_clock::now();
	return 1e-9 * std::chrono::duration_cast<std::chrono::nanoseconds>(t1-t0).count();
}

void msg(const std::string& msg)
{
	fmt::print("[{:.6f}] {}\n", GetTime(), msg);
}

void showIR(const ctn::BoardIR& ir) 
{ 
	for(const auto& [coord, vtx]: ir.cells) {
		cv::imshow(fmt::format("cell at {}", coord), vtx);
	}
}

int main()
{
	ctn::InitBitmapResources({
		{"cell_desert", cvutil::SafeImread("resources/cells/desert.jpg")},
		{"cell_fields", cvutil::SafeImread("resources/cells/fields.jpg")},
		{"cell_forest", cvutil::SafeImread("resources/cells/forest.jpg")},
		{"cell_hills", cvutil::SafeImread("resources/cells/hills.jpg")},
		{"cell_mountains", cvutil::SafeImread("resources/cells/mountains.jpg")},
		{"cell_pasture", cvutil::SafeImread("resources/cells/pasture.jpg")},
		{"element_blue", cvutil::SafeImread("resources/elements/blue.png")},
		{"element_orange", cvutil::SafeImread("resources/elements/orange.png")},
		{"element_red", cvutil::SafeImread("resources/elements/red.png")},
		{"road_attenuation_mask", cvutil::SafeImread("resources/road-attenuation-mask.png")},
		{"sea_attenuation_mask", cvutil::SafeImread("resources/sea-attenuation-mask.png")}
	});


	msg("init");
	auto src = cvutil::SafeImread("resources/samples/sampleETI2.jpg");
	CatanBoardDetector detector {SEA_COLOR_YCBCR_6500K};

	msg("detector initialized");

	auto warpedOpt = detector.findBoard(src);
	msg("findBoard finished");
	if(!warpedOpt.has_value()) {
		std::cerr << "error: board not found\n";
		return 1;
	}
	cv::Mat warped = warpedOpt.value();
	msg("board detected");
	cv::imshow("warped", warped);

	ctn::BoardIR boardIR = ctn::CreateBoardIR(warped);
	//showIR(boardIR);

	msg("board IR created");
	ctn::BoardInfo boardInfo = ctn::AnalyzeBoard(boardIR);

	msg("board info created");

	ctn::DrawBoardInfo(boardInfo, warped);

	msg("info drawn");
	
	cv::imshow("Warped board", warped);
	cv::waitKey();
}
