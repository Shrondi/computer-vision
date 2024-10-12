#include <iostream>
#include "common_code.hpp"
#include <opencv2/imgproc.hpp>

cv::Mat
fsiv_compute_image_histogram(const cv::Mat &in)
{
    CV_Assert(in.type() == CV_8UC1);
    cv::Mat hist;
    // TODO
    // Hint: Use the function cv::calcHist.

    //
    CV_Assert(!hist.empty());
    CV_Assert(hist.type() == CV_32FC1);
    CV_Assert(hist.rows == 256 && hist.cols == 1);
    return hist;
}

void fsiv_normalize_histogram(cv::Mat &hist)
{
    CV_Assert(hist.type() == CV_32FC1);
    CV_Assert(hist.rows == 256 && hist.cols == 1);

    // TODO
    // Hint: Use the function cv::normalize() with norm L1 = 1.0

    //

    CV_Assert(hist.type() == CV_32FC1);
    CV_Assert(hist.rows == 256 && hist.cols == 1);
    CV_Assert(cv::sum(hist)[0] == 0.0 || cv::abs(cv::sum(hist)[0] - 1.0) <= 1.0e-6);
}

void fsiv_accumulate_histogram(cv::Mat &hist)
{
    CV_Assert(hist.type() == CV_32FC1);
    CV_Assert(hist.rows == 256 && hist.cols == 1);

    // TODO

    //

    CV_Assert(hist.type() == CV_32FC1);
    CV_Assert(hist.rows == 256 && hist.cols == 1);
}

void fsiv_compute_clipped_histogram(cv::Mat &h, float cl)
{
    CV_Assert(h.type() == CV_32FC1);
    CV_Assert(h.rows == 256 && h.cols == 1);

    // TODO
    // Remember: the ouput histogram will be clipped to cl value and the
    // residual area will be redistributed equally in all the histogram's bins.

    //

    CV_Assert(h.type() == CV_32FC1);
    CV_Assert(h.rows == 256 && h.cols == 1);
}

float fsiv_compute_actual_clipping_histogram_value(const cv::Mat &h, float s)
{
    CV_Assert(h.type() == CV_32FC1);
    CV_Assert(h.rows == 256 && h.cols == 1);

    int CL = s * cv::sum(h)[0] / h.rows;

    // TODO: Code the algorithm shown in the practical assignment description.

    //

    return CL;
}

cv::Mat
fsiv_create_equalization_lookup_table(const cv::Mat &hist,
                                      float s)
{
    CV_Assert(hist.type() == CV_32FC1);
    CV_Assert(hist.rows == 256 && hist.cols == 1);
    cv::Mat lkt = hist.clone();

    if (s >= 1.0)
    {
        // TODO: Clip the histogram.
        // Hint: use fsiv_compute_actual_clipping_histogram_value to compute the
        //       clipping level.
        // Hint: use fsiv_compute_clipped_histogram to clip the histogram.

        //
    }

    // TODO: Build the equalization transform function.
    // Remember: the transform function will be the accumulated normalized
    //           image histogram.
    // Hint: use cv::Mat::convertTo() method to convert the float range [0.0, 1.0]
    //       to [0, 255] byte range.
    //

    //

    CV_Assert(lkt.type() == CV_8UC1);
    CV_Assert(lkt.rows == 256 && lkt.cols == 1);
    return lkt;
}

cv::Mat
fsiv_apply_lookup_table(const cv::Mat &in, const cv::Mat &lkt,
                        cv::Mat &out)
{
    CV_Assert(in.type() == CV_8UC1);
    CV_Assert(lkt.type() == CV_8UC1);
    CV_Assert(lkt.rows == 256 && lkt.cols == 1);
    CV_Assert(out.empty() || (out.type() == CV_8UC1 &&
                              out.rows == in.rows && out.cols == in.cols));

    // TODO
    // Hint: you can use the cv::LUT function.

    //
    CV_Assert(out.rows == in.rows && out.cols == in.cols && out.type() == in.type());
    return out;
}
