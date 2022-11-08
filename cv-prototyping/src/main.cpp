
#include <opencv2/opencv.hpp>

#include <catan/board_coords.hpp>
#include <catan/board_detection.hpp>
#include <catan/image_correction.hpp>
#include <catan/utility_opencv.hpp>
#include <catan/board_ir.hpp>
#include <catan/common_math.hpp>
#include <catan/analysis_cells.hpp>

#include <fmt/format.h>

double GetTime()
{
	static auto t0 = std::chrono::high_resolution_clock::now();
	auto dif = std::chrono::high_resolution_clock::now() - t0;
	return 0.000000001 * std::chrono::duration_cast<std::chrono::nanoseconds>(dif).count();
}


template<>
struct fmt::formatter<ctn::CellCoord>
{
	template<typename ParseContext>
	constexpr auto parse(ParseContext& ctx)
	{
		return ctx.begin();
	}

	template<typename FormatContext>
	auto format(const ctn::CellCoord& x, FormatContext& ctx)
	{
		return fmt::format_to(ctx.out(), "({},{},{})", x.x,x.y,x.z);
	}
};

template<>
struct fmt::formatter<cvutil::MeanStdDev>
{
	template<typename ParseContext>
	constexpr auto parse(ParseContext& ctx)
	{
		return ctx.begin();
	}

	template<typename FormatContext>
	auto format(const cvutil::MeanStdDev& x, FormatContext& ctx)
	{
		return fmt::format_to(ctx.out(), "[{:.3f}/{:.3f}]", x.mean, x.stddev);
	}
};

cv::Mat FoldedSpectrum(cv::Mat input)
{
	cv::Mat dct = NEW_MAT(tmp) {cv::dct(cvutil::ToFloat(input), tmp);};
	cvmath::Apply<float>(dct, std::abs<float>);
	cv::Mat dctHalf = NEW_MAT(tmp) {
		tmp = dct(cv::Rect{0,0,std::min(dct.rows,dct.cols),std::min(dct.rows,dct.cols)});
		cv::resize(tmp, tmp, {tmp.cols/2, tmp.rows/2}, 0, 0, cv::INTER_AREA);
	};

	for(int y=0; y<dctHalf.rows; y++) {
		for(int x=0; x<=y; x++) {
			float& x1 = dctHalf.at<float>(x, y);
			float& x2 = dctHalf.at<float>(y, x);
			float sum = x1+x2;
			x1 = sum;
			x2 = sum;
		}
	}

	return dctHalf;
}

double SimilarityRating(cv::Mat a, cv::Mat b)
{
	if(a.type() != CV_32FC1 || b.type() != CV_32FC1) {
		throw std::runtime_error("SimilarityRating needs two 32FC1 matrices");
	}

	cv::Scalar am, ad, bm, bd;
	cv::meanStdDev(a, am, ad);
	cv::meanStdDev(b, bm, bd);

	if(ad[0] < 10e-9 || bd[0] < 10e-9) {
		return -1;
	}

	cv::Mat ax = (a - am) / ad;
	cv::Mat bx = (b - bm) / bd;

	cv::Mat sd = cvmath::TransformBin<float, float>(ax, bx, cvmath::SquareDist);
	return cv::sum(sd)[0];
}

cv::Mat FoldedSpectrumBGR(cv::Mat input)
{
	auto bgr = cvutil::SplitBGR(input);
	for(auto& ch: bgr) {
		ch = FoldedSpectrum(ch);
	}
	return cvutil::MergeBGR(bgr);
}

struct {
	cv::Mat f32, u8;
	void init()
	{
		f32 = ctn::GenerateCellMask(50, 1000);
		u8 = cvutil::Convert(cvutil::ToByte(f32), cv::COLOR_GRAY2BGR);
	}
} CellMask;

cv::Mat WarpCell(cv::Mat src)
{
	cv::Mat dst;
	cv::warpPolar(src, dst, {150,150}, {65, 75}, 60, cv::WARP_POLAR_LINEAR);
	return dst({75, 0, 75, 150});
}

struct ReferenceCell 
{
	cv::Mat sourceImage;
	cv::Mat polarWarped;
	cv::Mat masked;
	cv::Mat maskYCrCb;
	cv::Mat maskYCrCb32;

	ReferenceCell(){}

	explicit ReferenceCell(cv::Mat source)
	{
		sourceImage = source;
		polarWarped = WarpCell(sourceImage);
		masked = cvmath::TransformBin<cv::Vec3b, cv::Vec3b>(sourceImage, CellMask.u8, cvmath::Mask8UC3);
		maskYCrCb = cvutil::Convert(masked, cv::COLOR_BGR2YCrCb);
		maskYCrCb32 = cvutil::ToFloat(maskYCrCb);
	}
};

cv::Scalar DominantColor(cv::Mat image)
{
	if(image.type() != CV_8UC3) {
		throw std::runtime_error("bruh");
	} 

	cv::Mat labels, centers;
	
}

int main()
{
	CellMask.init();
	auto src = cv::imread("resources/sampleGlare.jpg");
	CatanBoardDetector detector {SEA_COLOR_YCBCR_6500K};

	double t0,t1;
	t0=GetTime();
	auto warpedOpt = detector.findBoard(src);
	if(!warpedOpt.has_value()) {
		std::cerr << "error: board not found\n";
		return 1;
	}
	cv::Mat warped = warpedOpt.value();
	t1=GetTime();

	fmt::print("finding board: {:.6f} secs\n", t1-t0);

	ctn::BoardIR boardIR = ctn::CreateBoardIR(warped);
	std::vector<std::pair<ctn::CellCoord, cv::Mat>> cells {boardIR.cells.begin(), boardIR.cells.end()};

	std::array<std::string, 6> cellTypeNames = {
		"desert", "fields", "forest", "hills", "mountains", "pasture"
	};

	std::array<ReferenceCell, 6> refCells;
	
	for(int i=0; i<refCells.size(); i++) {
		refCells[i] = ReferenceCell(cv::imread(fmt::format("resources/cells/{}.jpg", cellTypeNames[i])));
	}
	ctn::ScreenCoordMapper mapper ({.center = {500,433}, .size = 150});
	
	t0=GetTime();

	for(auto& [coord, img]: boardIR.cells) {
		cv::Mat maskedImg = cvmath::TransformBin<cv::Vec3b, cv::Vec3b>(img, CellMask.u8, cvmath::Mask8UC3);
		cv::Mat maskedImg32 = cvutil::ToFloat(maskedImg);
		
		cv::imshow(fmt::format("mask_{}",coord), maskedImg);
		cv::imshow(fmt::format("warp_{}",coord), WarpCell(img));
		//cv::imshow(fmt::format("fold_{}",coord), cvutil::ToByte(FoldedSpectrumBGR(maskedImg)));

		std::array<float, 6> results;
		results[0] = 10e40;
		for(int i=1; i<refCells.size(); i++) {
			//cv::Mat sqDiff = cvmath::TransformBin<cv::Vec3f, float>(cvutil::convertToCrCb(maskedImg32), cvutil::convertToCrCb(refCells32[i]), cvmath::SquareDist);
			auto ref = cvutil::ToFloat(cvutil::Convert(refCells[i].polarWarped, cv::COLOR_BGR2YUV));
			auto curr = cvutil::ToFloat(cvutil::Convert(WarpCell(img), cv::COLOR_BGR2YUV));
			
			auto k1 = cvutil::MeanStdDevBGR(ref);
			auto k2 = cvutil::MeanStdDevBGR(curr);
			
			decltype(k1) dif;
			for(int i=0; i<dif.size(); i++)
				dif[i] = {.mean=50.0*std::abs(k1[i].mean - k2[i].mean), .stddev=50.0*std::abs(k1[i].stddev - k2[i].stddev)};

			double lumStdMul = 1.0;
			double crMul = 1.0;
			if(cellTypeNames[i] == "mountains") {
				lumStdMul = 2.0;
			}
			if(cellTypeNames[i] == "forest") {
				crMul = 4.0;
			}

			cv::Vec3d m1 {k1[0].mean*0.0, k1[1].mean, k1[2].mean};
			cv::Vec3d d1 {k1[0].stddev*lumStdMul, k1[1].stddev*0.5, k1[2].stddev*crMul};

			cv::Vec3d m2 {k2[0].mean*0.0, k2[1].mean, k2[2].mean};
			cv::Vec3d d2 {k2[0].stddev*lumStdMul, k2[1].stddev*0.5, k2[2].stddev*crMul};

			double total = cvmath::SquareDist(m1, m2) + cvmath::SquareDist(d1, d2);

			fmt::print("at {} dif to {}: {} / {} / {} || total dif {}\n", coord, cellTypeNames[i], dif[0], dif[1], dif[2], total);

			results[i] = total;
		}

		auto min = std::min_element(results.begin(), results.end());
		int idx = min - results.begin();
		if(coord == ctn::CellCoord{0,0,0}) {
			idx = 0;
			continue;
		}



		cv::Mat& ximg = img;
		cv::Mat imgGray = NEW_MAT(tmp) {cv::cvtColor(ximg, tmp, cv::COLOR_BGR2GRAY);};
		std::vector<cv::Vec3f> circles;
		cv::HoughCircles(imgGray, circles, cv::HOUGH_GRADIENT, 1, 90, 100, 15, 21, 27);
		
		if(circles.size() == 1) {
			double radius = circles[0][2];
			cv::Point2d center {circles[0][0], circles[0][1]};
			auto origin = mapper(coord) - cv::Point2d{65, 75};
			cv::Point2d offset {radius, radius};

			cv::Mat numberROI = img(cv::Rect(center - offset, center + offset)).clone();
			//cv::circle(warped, center+origin, radius, {255,0,255}, 1);

			//cv::imshow(fmt::format("numROI_{}", coord), numberROI);
		}


		cv::putText(warped, cellTypeNames[idx], mapper(coord) - cv::Point2d{40,0}, cv::FONT_HERSHEY_COMPLEX, 0.6, {0,255,0}, 2);

	}

	t1=GetTime();
	fmt::print("classifying: {:.6f} secs\n", t1-t0);


	cv::imshow("Warped board", warped);
	
	cv::waitKey();
}
