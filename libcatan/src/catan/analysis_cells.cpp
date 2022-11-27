#include <catan/analysis_cells.hpp>

#include <catan/common_math.hpp>
#include <catan/utility_opencv.hpp>


inline float rating1(float x)
{
    return 0.5f - std::atan(4.0f*x - 5) / 3.141591f;
}

cv::Mat GenerateCellMask(double mu, double sigma)
{
	cv::Mat mask = cv::Mat::zeros({130, 150}, CV_32FC1);
	cv::Vec2f center {mask.cols*0.5f, mask.rows*0.5f};

	mu *= mu;

	for(int y=0; y<mask.rows; y++) {
		float* row = mask.ptr<float>(y);
		for(int x=0; x<mask.cols; x++) {
			float dx = x-center[0];
			float dy = y-center[1];

			float sqDist = dx*dx + dy*dy;
			float dfd = std::abs<float>(mu - sqDist);

			row[x] = rating1(dfd / sigma);
			row[x] *= row[x];
		}
	}

	return mask;
}


struct CellMaskT {
	cv::Mat f32, u8;

	CellMaskT()
	{
		f32 = GenerateCellMask(50, 1000);
		u8 = cvutil::Convert(cvutil::ToByte(f32), cv::COLOR_GRAY2BGR);
	}
};

static CellMaskT CellMask;

cv::Mat WarpCell(cv::Mat src)
{
	cv::Mat dst;
	cv::warpPolar(src, dst, {150,150}, {65, 75}, 60, cv::WARP_POLAR_LINEAR);
	return dst({75, 0, 75, 150});
}


struct ReferenceCell 
{
	std::string typeName;
	cv::Mat sourceImage;
	cv::Mat polarWarped;
	cv::Mat masked;
	cv::Mat maskYCrCb;
	cv::Mat maskYCrCb32;

	ReferenceCell(){}

	ReferenceCell(const char* typeName)
		: typeName(typeName)
	{
		sourceImage = cvutil::SafeImread("resources/cells/" + std::string(typeName) + ".jpg");
		polarWarped = WarpCell(sourceImage);
		masked = cvmath::TransformBin<cv::Vec3b, cv::Vec3b>(sourceImage, CellMask.u8, cvmath::Mask8UC3);
		maskYCrCb = cvutil::Convert(masked, cv::COLOR_BGR2YCrCb);
		maskYCrCb32 = cvutil::ToFloat(maskYCrCb);
	}
};

std::map<ctn::CellCoord, std::string> ctn::DetermineCellTypes(const ctn::BoardIR& boardIR)
{
	static std::array<ReferenceCell, 6> refCells = {
		"desert", "fields", "forest", "hills", "mountains", "pasture"
	};

	std::map<ctn::CellCoord, std::string> result;

	for(auto& [coord, img]: boardIR.cells) {
		cv::Mat maskedImg = cvmath::TransformBin<cv::Vec3b, cv::Vec3b>(img, CellMask.u8, cvmath::Mask8UC3);
		cv::Mat maskedImg32 = cvutil::ToFloat(maskedImg);

		std::array<float, 6> invLikelihood;
		invLikelihood[0] = 10e40;
		
		for(int i=1; i<refCells.size(); i++) {
			auto ref = cvutil::ToFloat(cvutil::Convert(refCells[i].polarWarped, cv::COLOR_BGR2YUV));
			auto curr = cvutil::ToFloat(cvutil::Convert(WarpCell(img), cv::COLOR_BGR2YUV));
			
			auto k1 = cvutil::MeanStdDevBGR(ref);
			auto k2 = cvutil::MeanStdDevBGR(curr);
			
			decltype(k1) dif;
			for(int i=0; i<dif.size(); i++) {
				dif[i] = {
					.mean = 50.0*std::abs(k1[i].mean - k2[i].mean), 
					.stddev = 50.0*std::abs(k1[i].stddev - k2[i].stddev)
				};
			}

			double lumStdMul = 1.0;
			double crMul = 1.0;
			if(refCells[i].typeName == "mountains") {
				lumStdMul = 2.0;
			}
			if(refCells[i].typeName == "forest") {
				crMul = 4.0;
			}

			cv::Vec3d m1 {k1[0].mean*0.0, k1[1].mean, k1[2].mean};
			cv::Vec3d d1 {k1[0].stddev*lumStdMul, k1[1].stddev*0.5, k1[2].stddev*crMul};

			cv::Vec3d m2 {k2[0].mean*0.0, k2[1].mean, k2[2].mean};
			cv::Vec3d d2 {k2[0].stddev*lumStdMul, k2[1].stddev*0.5, k2[2].stddev*crMul};

			double total = cvmath::SquareDist(m1, m2) + cvmath::SquareDist(d1, d2);

			invLikelihood[i] = total;
		}

		auto min = std::min_element(invLikelihood.begin(), invLikelihood.end());
		int mostLikelyType = min - invLikelihood.begin();
		if(coord == ctn::CellCoord{0,0,0}) {
			mostLikelyType = 0; //desert
			continue;
		}

		result[coord] = refCells[mostLikelyType].typeName;
	}

	return result;
}

/*


*/
