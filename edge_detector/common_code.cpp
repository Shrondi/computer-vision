#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "common_code.hpp"

void fsiv_compute_derivate(cv::Mat const &img, cv::Mat &dx, cv::Mat &dy, int g_r,
                           int s_ap)
{
    CV_Assert(img.type() == CV_8UC1);
    // TODO
    // Remember: if g_r > 0 apply a previous Gaussian Blur operation with kernel size 2*g_r+1.
    // Hint: use Sobel operator to compute derivate.

    cv::Mat img_ = img.clone();
    if (g_r > 0){
        cv::GaussianBlur(img, img_, cv::Size(2 * g_r + 1, 2 * g_r + 1), 0, 0);
    }

    cv::Sobel(img_, dx, CV_32FC1, 1, 0, s_ap);
    cv::Sobel(img_, dy, CV_32FC1, 0, 1, s_ap);

    //
    CV_Assert(dx.size() == img.size());
    CV_Assert(dy.size() == dx.size());
    CV_Assert(dx.type() == CV_32FC1);
    CV_Assert(dy.type() == CV_32FC1);
}

void fsiv_compute_gradient_magnitude(cv::Mat const &dx, cv::Mat const &dy,
                                     cv::Mat &gradient)
{
    CV_Assert(dx.size() == dy.size());
    CV_Assert(dx.type() == CV_32FC1);
    CV_Assert(dy.type() == CV_32FC1);

    // TODO
    // Hint: use cv::magnitude.

    cv::magnitude(dx, dy, gradient);

    //

    CV_Assert(gradient.size() == dx.size());
    CV_Assert(gradient.type() == CV_32FC1);
}

void fsiv_compute_gradient_histogram(cv::Mat const &gradient, int n_bins, cv::Mat &hist, float &max_gradient)
{
    // TODO
    // Hint: use cv::minMaxLoc to get the gradient range {0, max_gradient}

    double maxVal;
    cv::minMaxLoc(gradient, NULL, &maxVal, NULL, NULL);

    max_gradient = (float) maxVal;

    std::vector<cv::Mat> img = {gradient};
    std::vector<int> chs = {0};
    std::vector<int> histSize = {n_bins};
    std::vector<float> ranges = {0, max_gradient};

    cv::calcHist(img, chs, cv::Mat(), hist, histSize, ranges, false);

    //
    CV_Assert(max_gradient > 0.0);
    CV_Assert(hist.rows == n_bins);
}

int fsiv_compute_histogram_percentile(cv::Mat const &hist, float percentile)
{
    CV_Assert(percentile >= 0.0 && percentile <= 1.0);
    CV_Assert(hist.type() == CV_32FC1);
    CV_Assert(hist.cols == 1);
    int idx = -1;
    // TODO
    // Hint: use cv::sum to compute the histogram area.
    // Remember: The percentile p is the first i that sum{h[0], h[1], ..., h[i]} >= p

    float area = 0;
    for (int i = 0; i < hist.rows; i++){
        area += cv::sum(hist.at<float>(i))[0];
        idx = i;

        if (area >= percentile*cv::sum(hist)[0]){
            break;
        }
    }

    //
    CV_Assert(idx >= 0 && idx < hist.rows);
    CV_Assert(idx == 0 || cv::sum(hist(cv::Range(0, idx), cv::Range::all()))[0] / cv::sum(hist)[0] < percentile);
    CV_Assert(cv::sum(hist(cv::Range(0, idx + 1), cv::Range::all()))[0] / cv::sum(hist)[0] >= percentile);
    return idx;
}

float fsiv_histogram_idx_to_value(int idx, int n_bins, float max_value,
                                  float min_value)
{
    CV_Assert(idx >= 0);
    CV_Assert(idx < n_bins);
    float value = 0.0;
    // TODO
    // Remember: Map integer range [0, n_bins) into float
    // range [min_value, max_value)

    value = min_value + (idx * (max_value - min_value) / n_bins);

    //
    CV_Assert(value >= min_value);
    CV_Assert(value < max_value);
    return value;
}

void fsiv_percentile_edge_detector(cv::Mat const &gradient, cv::Mat &edges,
                                   float th, int n_bins)
{
    CV_Assert(gradient.type() == CV_32FC1);

    // TODO
    // Remember: user other fsiv_xxx to compute histogram and percentiles.
    // Remember: map histogram range {0, ..., n_bins} to the gradient range
    // {0.0, ..., max_grad}
    // Hint: use "operator >=" to threshold the gradient magnitude image.

    cv::Mat hist;
    float max_value;
    fsiv_compute_gradient_histogram(gradient, n_bins, hist, max_value);

    int idx = fsiv_compute_histogram_percentile(hist, th);

    float value =  fsiv_histogram_idx_to_value(idx, n_bins, max_value);

    edges = gradient >= value;

    //
    CV_Assert(edges.type() == CV_8UC1);
    CV_Assert(edges.size() == gradient.size());
}

void fsiv_otsu_edge_detector(cv::Mat const &gradient, cv::Mat &edges)
{
    CV_Assert(gradient.type() == CV_32FC1);

    // TODO
    // Hint: normalize input gradient into rango [0, 255] to use
    // cv::threshold properly.
    //

    cv::Mat normGrad;
    cv::normalize(gradient, normGrad, 0, 255, cv::NORM_MINMAX, CV_8UC1);

    cv::threshold(normGrad, edges, 0, 255, cv::THRESH_OTSU);

    //
    CV_Assert(edges.type() == CV_8UC1);
    CV_Assert(edges.size() == gradient.size());
}

void fsiv_canny_edge_detector(cv::Mat const &dx, cv::Mat const &dy, cv::Mat &edges,
                              float th1, float th2, int n_bins)
{
    CV_Assert(dx.size() == dy.size());
    CV_Assert(th1 < th2);

    // TODO
    // Hint: convert the intput derivatives to CV_16C1 to be used with canny.
    // Remember: th1 and th2 are given as percentiles so you must transform to
    //           gradient range to be used in canny method.
    // Remember: we compute gradients with L2_NORM so we must indicate this in
    //           the canny method too.


    cv::Mat gradient;
    fsiv_compute_gradient_magnitude(dx, dy, gradient);

    cv::Mat hist; 
    float maxGradient;
    fsiv_compute_gradient_histogram(gradient, n_bins, hist, maxGradient);
    
    int idx1 = fsiv_compute_histogram_percentile(hist, th1);
    int idx2 = fsiv_compute_histogram_percentile(hist, th2);

    double vth1 =  fsiv_histogram_idx_to_value(idx1, n_bins, maxGradient);
    double vth2 =  fsiv_histogram_idx_to_value(idx2, n_bins, maxGradient);


    cv::Mat dx_, dy_;
    dx.convertTo(dx_, CV_16SC1);
    dy.convertTo(dy_, CV_16SC1);

    cv::Canny(dx_, dy_, edges, vth1, vth2, true);

    //
    CV_Assert(edges.type() == CV_8UC1);
    CV_Assert(edges.size() == dx.size());
}

void fsiv_compute_ground_truth_image(cv::Mat const &consensus_img,
                                     float min_consensus, cv::Mat &gt)
{
    //! TODO
    // Hint: use cv::normalize to normalize consensus_img into range (0, 100)
    // Hint: use "operator >=" to threshold the consensus image.

    cv::Mat consensus_img_ = consensus_img.clone();
    cv::normalize(consensus_img, consensus_img_, 0, 100, cv::NORM_MINMAX);

    gt = consensus_img_ >= min_consensus;

    //
    CV_Assert(consensus_img.size() == gt.size());
    CV_Assert(gt.type() == CV_8UC1);
}

void fsiv_compute_confusion_matrix(cv::Mat const &gt, cv::Mat const &pred, cv::Mat &cm)
{
    CV_Assert(gt.type() == CV_8UC1);
    CV_Assert(pred.type() == CV_8UC1);
    CV_Assert(gt.size() == pred.size());

    // TODO
    // Remember: a edge detector confusion matrix is a 2x2 matrix where the
    // rows are ground truth {Positive: "is edge", Negative: "is not edge"} and
    // the columns are the predictions labels {"is edge", "is not edge"}
    // A pixel value means edge if it is <> 0, else is a "not edge" pixel.

    cm = cv::Mat::zeros(2, 2, CV_32FC1);

    for(int i = 0; i < gt.rows; i++){
        for(int j = 0; j < gt.cols; j++){

            if(gt.at<uchar>(i,j) != 0){ //Edge

                if(pred.at<uchar>(i,j) != 0){
                    cm.at<float>(0,0)++; //True positive
                }else{
                    cm.at<float>(0,1)++; //False negative
                }
            }else{ // Not edge

                if(pred.at<uchar>(i,j) != 0){
                    cm.at<float>(1,0)++; //False positive
                }else{
                    cm.at<float>(1,1)++; //True negative
                }
            }
        }
    }

    //
    CV_Assert(cm.type() == CV_32FC1);
    CV_Assert(cv::abs(cv::sum(cm)[0] - (gt.rows * gt.cols)) < 1.0e-6);
}

float fsiv_compute_sensitivity(cv::Mat const &cm)
{
    CV_Assert(cm.type() == CV_32FC1);
    CV_Assert(cm.size() == cv::Size(2, 2));
    float sensitivity = 0.0;
    // TODO
    // Hint: see https://en.wikipedia.org/wiki/Confusion_matrix

    //https://stats.stackexchange.com/questions/8025/what-are-correct-values-for-precision-and-recall-when-the-denominators-equal-0

    float TP = cm.at<float>(0,0);
    float FN = cm.at<float>(0,1);

    if (TP + FN != 0){
        sensitivity = TP / (TP + FN);
    }

    //
    return sensitivity;
}

float fsiv_compute_precision(cv::Mat const &cm)
{
    CV_Assert(cm.type() == CV_32FC1);
    CV_Assert(cm.size() == cv::Size(2, 2));
    float precision = 0.0;
    // TODO
    // Hint: see https://en.wikipedia.org/wiki/Confusion_matrix

    //https://stats.stackexchange.com/questions/8025/what-are-correct-values-for-precision-and-recall-when-the-denominators-equal-0

    float TP = cm.at<float>(0,0);
    float FP = cm.at<float>(1,0);

    if (TP + FP != 0){
        precision = TP / (TP + FP);
    }
    
    //
    return precision;
}

float fsiv_compute_F1_score(cv::Mat const &cm)
{
    CV_Assert(cm.type() == CV_32FC1);
    CV_Assert(cm.size() == cv::Size(2, 2));
    float F1 = 0.0;
    // TODO
    // Hint: see https://en.wikipedia.org/wiki/Confusion_matrix

    float precision = fsiv_compute_precision(cm);
    float sensitivity = fsiv_compute_sensitivity(cm);

    //Division by zero:
    //When true positive + false positive == 0, precision is 0. 
    //When true positive + false negative == 0, recall is 0. 
    //In such cases, by default the metric F1 will be set to 0

    //Seed with division by zero: 31682507877002

    if (precision + sensitivity != 0){
        F1 = (2 * precision * sensitivity) / (precision + sensitivity);
    }

    //
    return F1;
}
