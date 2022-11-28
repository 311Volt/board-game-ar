#include <catan.hpp>

#include <fmt/format.h>



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

int main()
{
	msg("init");
	auto src = cvutil::SafeImread("resources/samples/sample1.jpg");
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

	ctn::BoardIR boardIR = ctn::CreateBoardIR(warped);

	msg("board IR created");
	ctn::BoardInfo boardInfo = ctn::AnalyzeBoard(boardIR);

	msg("board info created");

	ctn::DrawBoardInfo(boardInfo, warped);

	msg("info drawn");
	
	cv::imshow("Warped board", warped);
	cv::waitKey();
}
