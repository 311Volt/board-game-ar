#ifndef INCLUDE_CATAN_COMMON_MATH
#define INCLUDE_CATAN_COMMON_MATH

#include <opencv2/opencv.hpp>

#include <concepts>
#include <stdexcept>

#ifdef _MSC_VER
#define CTN_FORCE_INLINE __forceinline
#elif defined( __GNUC__ ) || defined( __MINGW__ ) || defined ( __clang__ )
#define CTN_FORCE_INLINE __attribute__((always_inline))
#else
#define CTN_FORCE_INLINE
#endif

namespace cvmath {

	template<typename T>
	concept MatFragment = requires {
		cv::traits::Type<T>::value;
	};

	template<typename T>
	concept MatFragmentVector = requires {
		cv::traits::Type<T>::value;
		T::value_type;
		T::ch;
	};

	template<typename FuncT, typename InFragT, typename OutFragT>
	concept UnaryImageFn = 
		MatFragment<InFragT> && 
		MatFragment<OutFragT> &&
		std::invocable<FuncT, InFragT> && 
		requires(FuncT fn, InFragT fr) {
			{fn(fr)} -> std::same_as<OutFragT>;
		};

	template<typename FuncT, typename FragT>
	concept UnaryImageOp = 
		MatFragment<FragT> && 
		std::invocable<FuncT, FragT> && 
		requires(FuncT fn, FragT fr) {
			fr = fn(fr);
			{fn(fr)} -> std::same_as<FragT>;
		};

	template<typename FuncT, typename InFragT, typename OutFragT>
	concept BinaryImageFn = 
		MatFragment<InFragT> && 
		MatFragment<OutFragT> &&
		std::invocable<FuncT, InFragT, InFragT> && 
		requires(FuncT fn, InFragT fr) {
			{fn(fr, fr)} -> std::same_as<OutFragT>;
		};

	template<typename FuncT, typename FragT>
	concept BinaryImageOp = 
		MatFragment<FragT> && 
		std::invocable<FuncT, FragT, FragT> &&
		requires(FuncT fn, FragT fr) {
			fr = fn(fr, fr);
			{fn(fr, fr)} -> std::same_as<FragT>;
		};

	

	/**
		* @brief Applies a unary operation to exery pixel of the given matrix.
		* 
		* @tparam FragT Pixel type (uchar, float, Vec3b, etc...)
		* @tparam FuncT Function type. Will be inferred.
		* @param mat The matrix to be modified. Pixel format must match FragT.
		* @param fn The function to be applied.
		* @throws std::invalid_argument when mat.type() does not match FragT.
		*/
	template<MatFragment FragT, UnaryImageOp<FragT> FuncT>
	void Apply(cv::Mat& mat, FuncT fn)
	{
		if(mat.type() != cv::traits::Type<FragT>::value) {
			throw std::invalid_argument("Apply called with incompatible matrix type");
		}
		const int height = mat.rows;
		const int width = mat.cols;

		for(int y=0; y<height; y++) {
			FragT* row = mat.ptr<FragT>(y);
			for(int x=0; x<width; x++) {
				row[x] = fn(row[x]);
			}
		}
	}


	/**
		* @brief Returns the result of applying a unary function to every pixel of a matrix.
		* 
		* @tparam InFragT Input pixel type (uchar, float, Vec3b, etc...)
		* @tparam OutFragT Output pixel type
		* @tparam FuncT Function type. Will be inferred.
		* @param mat The matrix to be processed. Pixel format must match InFragT.
		* @param fn The function to be applied.
		* @throws std::invalid_argument when mat.type() does not match InFragT.
		*/
	template<MatFragment InFragT, MatFragment OutFragT, UnaryImageFn<InFragT, OutFragT> FuncT>
	inline CTN_FORCE_INLINE cv::Mat Transform(const cv::Mat& mat, FuncT fn)
	{
		if(mat.type() != cv::traits::Type<InFragT>::value) {
			throw std::invalid_argument("Transform called with incompatible matrix type");
		}
		cv::Mat output = cv::Mat::zeros(mat.rows, mat.cols, cv::traits::Type<OutFragT>::value);

		const int height = mat.rows;
		const int width = mat.cols;

		for(int y=0; y<height; y++) {
			const InFragT* inRow = mat.ptr<InFragT>(y);
			OutFragT* outRow = output.ptr<OutFragT>(y);
			for(int x=0; x<width; x++) {
				outRow[x] = fn(inRow[x]);
			}
		}
		return output;
	}


	/**
		* @brief Applies a binary operation to exery pixel of mat1 and mat2. Stores the result in mat1.
		* 
		* @tparam FragT Pixel type (uchar, float, Vec3b, etc...)
		* @param mat The matrix to be modified. Pixel format must match InFragT.
		* @param fn The function to be applied.
		* @throws std::invalid_argument when matX.type() does not match InFragT.
		* @throws std::invalid_argument when mat1 and mat2 differ in size.
		*/
	template<MatFragment FragT, BinaryImageOp<FragT> FnT>
	inline CTN_FORCE_INLINE void ApplyBin(cv::Mat& mat1, const cv::Mat& mat2, FnT fn)
	{
		if(mat1.type() != cv::traits::Type<FragT>::value || mat2.type() != cv::traits::Type<FragT>::value) {
			throw std::invalid_argument("ApplyBin called with incompatible matrix types");
		}
		if(mat1.rows != mat2.rows || mat1.cols != mat2.cols) {
			throw std::invalid_argument("Matrices of different sizes supplied to ApplyBin");
		}

		const int height = mat1.rows;
		const int width = mat1.cols;

		for(int y=0; y<height; y++) {
			FragT* row1 = mat1.ptr<FragT>(y);
			const FragT* row2 = mat2.ptr<FragT>(y);
			
			for(int x=0; x<width; x++) {
				row1[x] = fn(row1[x], row2[x]);
			}
		}
	}

	/**
		* @brief Returns the result of applying a binary function to each pixel of mat1 and mat2.
		* 
		* @tparam T Input pixel type (uchar, float, Vec3b, etc...)
		* @param mat The matrix to be modified. Pixel format must match T.
		* @param fn The function to be applied.
		* @throws std::invalid_argument when mat.type() does not match T.
		* @throws std::invalid_argument when mat1 and mat2 differ in size.
		*/
	template<MatFragment InFragT, MatFragment OutFragT, BinaryImageFn<InFragT, OutFragT> FnT>
	inline CTN_FORCE_INLINE cv::Mat TransformBin(const cv::Mat& mat1, const cv::Mat& mat2, FnT fn)
	{
		if(mat1.type() != cv::traits::Type<InFragT>::value || mat2.type() != cv::traits::Type<InFragT>::value) {
			throw std::invalid_argument("TransformBin called with incompatible matrix types");
		}
		if(mat1.rows != mat2.rows || mat1.cols != mat2.cols) {
			throw std::invalid_argument("Matrices of different sizes supplied to TransformBin");
		}

		const int height = mat1.rows;
		const int width = mat1.cols;

		cv::Mat output = cv::Mat::zeros(mat1.rows, mat1.cols, cv::traits::Type<OutFragT>::value);

		for(int y=0; y<height; y++) {
			const InFragT* row1 = mat1.ptr<InFragT>(y);
			const InFragT* row2 = mat2.ptr<InFragT>(y);
			OutFragT* out = output.ptr<OutFragT>(y);
			
			for(int x=0; x<width; x++) {
				out[x] = fn(row1[x], row2[x]);
			}
		}
		return output;
	}
	
	inline float SquareDist(const cv::Vec3f a, const cv::Vec3f b)
	{
		return (a[0]-b[0])*(a[0]-b[0]) + (a[1]-b[1])*(a[1]-b[1]) + (a[2]-b[2])*(a[2]-b[2]);
	}

	template<int PercentX, int PercentY, int PercentZ>
	inline float WeightedSquareDist(const cv::Vec3f a, const cv::Vec3f b)
	{
		static constexpr float wx = PercentX / 100.0f;
		static constexpr float wy = PercentY / 100.0f;
		static constexpr float wz = PercentZ / 100.0f;
		return wx*(a[0]-b[0])*(a[0]-b[0]) + wy*(a[1]-b[1])*(a[1]-b[1]) + wz*(a[2]-b[2])*(a[2]-b[2]);
	}

	inline uint8_t MaskU8(uint8_t a, uint8_t b)
	{
		uint16_t wa=a, wb=b;
		return (wa*wb) >> 8;
	}

	inline cv::Vec3b Mask8UC3(cv::Vec3b a, cv::Vec3b b)
	{
		return {MaskU8(a[0], b[0]), MaskU8(a[1], b[1]), MaskU8(a[2], b[2])};
	}
}

#endif /* INCLUDE_CATAN_COMMON_MATH */
