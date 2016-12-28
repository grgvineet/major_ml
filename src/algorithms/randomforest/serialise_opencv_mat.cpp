#include <hpx/runtime/serialization/serialize.hpp>
#include <opencv2/opencv.hpp>
#include <hpx/runtime/serialization/array.hpp>

namespace hpx
{
    namespace serialization {
        void serialize(hpx::serialization::input_archive &ar, cv::Mat &m, int version) {
            // load
            int cols, rows;
            size_t elem_size, elem_type;

            ar >> cols;
            ar >> rows;
            ar >> elem_size;
            ar >> elem_type;

            m.create(rows, cols, elem_type);

            size_t data_size = m.cols * m.rows * elem_size;
            ar >> hpx::serialization::make_array(m.ptr(), data_size);
        }

        void serialize(hpx::serialization::output_archive &ar, cv::Mat &m, int version) {
            // normal save ...
            size_t elem_size = m.elemSize(), elem_type = m.type();

            ar << m.cols;
            ar << m.rows;
            ar << elem_size;
            ar << elem_type;

            const size_t data_size = m.cols * m.rows * elem_size;
            ar << hpx::serialization::make_array(m.ptr(), data_size);
        }
    }
}