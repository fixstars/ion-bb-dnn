#ifndef ION_BB_DNN_RT_H
#define ION_BB_DNN_RT_H

#include <HalideBuffer.h>

#include "rt_tfl.h"
#include "rt_ort.h"
#include "rt_trt.h"

#ifdef _WIN32
#define ION_EXPORT __declspec(dllexport)
#else
#define ION_EXPORT
#endif

extern "C" ION_EXPORT int ion_bb_dnn_generic_object_detection(halide_buffer_t *in,
                                                              halide_buffer_t *session_id_buf,
                                                              halide_buffer_t *model_root_url_buf,
                                                              halide_buffer_t *cache_root_buf,
                                                              bool cuda_enable,
                                                              halide_buffer_t *out) {
    try {

        if (in->is_bounds_query()) {
            // Both input and output is (N)HWC
            for (int i=0; i<in->dimensions; ++i) {
                in->dim[i].min = out->dim[i].min;
                in->dim[i].extent = out->dim[i].extent;
            }
            return 0;
        }

        Halide::Runtime::Buffer<float> in_buf(*in);
        in_buf.copy_to_host();

        std::string model_root_url(reinterpret_cast<const char *>(model_root_url_buf->host));
        std::string cache_root(reinterpret_cast<const char *>(cache_root_buf->host));

        using namespace ion::bb::dnn;

        if (is_tfl_available()) {
            return object_detection_tfl(in, model_root_url, cache_root, out);
        } else if (is_ort_available()) {
            std::string session_id(reinterpret_cast<const char *>(session_id_buf->host));
            return object_detection_ort(in, session_id, model_root_url, cache_root, cuda_enable, out);
        } else {
            std::cerr << "No available runtime" << std::endl;
            return -1;
        }

        return 0;

    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return -1;
    } catch (...) {
        std::cerr << "Unknown error" << std::endl;
        return -1;
    }
}

extern "C" ION_EXPORT int ion_bb_dnn_tlt_object_detection_ssd(halide_buffer_t *in,
                                                              halide_buffer_t *session_id_buf,
                                                              halide_buffer_t *model_root_url_buf,
                                                              halide_buffer_t *cache_root_buf,
                                                              halide_buffer_t *out) {
    try {

        if (in->is_bounds_query()) {
            // Both input and output is (N)HWC
            for (int i=0; i<in->dimensions; ++i) {
                in->dim[i].min = out->dim[i].min;
                in->dim[i].extent = out->dim[i].extent;
            }
            return 0;
        }

        Halide::Runtime::Buffer<float> in_buf(*in);
        in_buf.copy_to_host();

        std::string model_root_url(reinterpret_cast<const char *>(model_root_url_buf->host));
        std::string cache_root(reinterpret_cast<const char *>(cache_root_buf->host));

        using namespace ion::bb::dnn;

        if (trt::is_available()) {
            std::string session_id(reinterpret_cast<const char *>(session_id_buf->host));
            return trt::object_detection_ssd(in, session_id, model_root_url, cache_root, out);
        } else {
            std::cerr << "No available runtime" << std::endl;
            return -1;
        }

        return 0;

    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return -1;
    } catch (...) {
        std::cerr << "Unknown error" << std::endl;
        return -1;
    }
}

#undef ION_EXPORT

#endif  // ION_BB_DNN_BB_H
