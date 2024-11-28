#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/calib3d.hpp>
#include "common_code.hpp"

std::vector<cv::Point3f>
fsiv_generate_3d_calibration_points(const cv::Size &board_size,
                                    float square_size)
{
    std::vector<cv::Point3f> ret_v;
    // TODO
    // Remember: the first inner point has (1,1) in board coordinates.
    for (int i = 1; i <= board_size.height; i++){
        for (int j = 1; j <= board_size.width; j++){
            ret_v.push_back(cv::Point3f(j*square_size, i*square_size, 0.0));
        }

    }
    //
    CV_Assert(ret_v.size() ==
              static_cast<size_t>(board_size.width * board_size.height));
    return ret_v;
}

bool fsiv_fast_find_chessboard_corners(const cv::Mat &img, const cv::Size &board_size,
                                       std::vector<cv::Point2f> &corner_points)
{
    CV_Assert(img.type() == CV_8UC3);
    bool was_found = false;
    // TODO
    // Hint: use cv::findChessboardCorners adding fast check to the defaults flags.
    // Remember: do not refine the corner points to get a better computational performance.

    was_found = cv::findChessboardCorners(img, board_size, corner_points, cv::CALIB_CB_FAST_CHECK);

    //
    return was_found;
}

void fsiv_compute_camera_pose(const std::vector<cv::Point3f> &_3dpoints,
                              const std::vector<cv::Point2f> &_2dpoints,
                              const cv::Mat &camera_matrix,
                              const cv::Mat &dist_coeffs,
                              cv::Mat &rvec,
                              cv::Mat &tvec)
{
    CV_Assert(_3dpoints.size() >= 4 && _3dpoints.size() == _2dpoints.size());
    // TODO
    // Hint: use cv::solvePnP to the pose of a calibrated camera.

    cv::solvePnP(_3dpoints, _2dpoints, camera_matrix, dist_coeffs, rvec, tvec);

    //
    CV_Assert(rvec.rows == 3 && rvec.cols == 1 && rvec.type() == CV_64FC1);
    CV_Assert(tvec.rows == 3 && tvec.cols == 1 && tvec.type() == CV_64FC1);
}

void fsiv_draw_axes(cv::Mat &img,
                    const cv::Mat &camera_matrix, const cv::Mat &dist_coeffs,
                    const cv::Mat &rvec, const cv::Mat &tvec,
                    const float size, const int line_width)
{
    // TODO
    // Hint: use cv::projectPoints to get the image coordinates of the 3D points
    // (0,0,0), (size, 0, 0), (0, size, 0) and (0, 0, -size) and draw a line for
    // each axis: blue for axis OX, green for axis OY and red for axis OZ.
    // Warning: use of cv::drawFrameAxes() is not allowed.

    std::vector<cv::Point3f> object_points = {cv::Point3f(0, 0, 0),
                                            cv::Point3f(size, 0, 0),
                                            cv::Point3f(0, size, 0),
                                            cv::Point3f(0, 0, -size)};

    std::vector<cv::Point2f> image_points;
    cv::projectPoints(object_points, rvec, tvec, camera_matrix, dist_coeffs, image_points);

    cv::line(img, image_points[0], image_points[1], cv::Scalar(255,0,0), line_width);
    cv::line(img, image_points[0], image_points[2], cv::Scalar(0,255,0), line_width);
    cv::line(img, image_points[0], image_points[3], cv::Scalar(0,0,255), line_width);

    //BGR para el escalar


    //
}

void fsiv_load_calibration_parameters(cv::FileStorage &fs,
                                      cv::Size &camera_size,
                                      float &error,
                                      cv::Mat &camera_matrix,
                                      cv::Mat &dist_coeffs,
                                      cv::Mat &rvec,
                                      cv::Mat &tvec)
{
    CV_Assert(fs.isOpened());
    // TODO
    //  Hint: use fs["label"] >> var to load data items from the file.
    //  @see cv::FileStorage operators "[]" and ">>"

    fs["image-width"] >> camera_size.width;
    fs["image-height"] >> camera_size.height;
    fs["error"] >> error;
    fs["camera-matrix"] >> camera_matrix;
    fs["distortion-coefficients"] >> dist_coeffs;
    fs["rvec"] >> rvec;
    fs["tvec"] >> tvec;

    //
    CV_Assert(fs.isOpened());
    CV_Assert(camera_matrix.type() == CV_64FC1 && camera_matrix.rows == 3 && camera_matrix.cols == 3);
    CV_Assert(dist_coeffs.type() == CV_64FC1 && dist_coeffs.rows == 1 && dist_coeffs.cols == 5);
    CV_Assert(rvec.type() == CV_64FC1 && rvec.rows == 3 && rvec.cols == 1);
    CV_Assert(tvec.type() == CV_64FC1 && tvec.rows == 3 && tvec.cols == 1);
    return;
}

void fsiv_draw_3d_model(cv::Mat &img, const cv::Mat &M, const cv::Mat &dist_coeffs,
                        const cv::Mat &rvec, const cv::Mat &tvec,
                        const float size)
{
    CV_Assert(img.type() == CV_8UC3);

    // TODO
    // Hint: build a 3D object points vector with pair of segments end points.
    // Use cv::projectPoints to get the 2D image coordinates of 3D object points,
    // build a vector of vectors of Points, one for each segment, and use
    // cv::polylines to draw the wire frame projected model.
    // Hint: use a "reference point" to move the model around the image and update it
    //       at each call to move the 3D model around the scene.

    std::vector<cv::Point3f> object_points = {
        { 0, 0, 0 },                             // 0: Esquina superior izquierda de la base inferior (0, 0, 0)
        { size, 0, 0 },                          // 1: Esquina inferior derecha de la base inferior (size, 0, 0)
        { size, size, 0 },                       // 2: Esquina inferior derecha de la base superior (size, size, 0)
        { 0, size, 0 },                          // 3: Esquina superior izquierda de la base superior (0, size, 0)
        { 0, 0, -size },                         // 4: Esquina superior izquierda de la base inferior detrás (0, 0, -size)
        { size, 0, -size },                      // 5: Esquina inferior derecha de la base inferior detrás (size, 0, -size)
        { size, size, -size },                   // 6: Esquina inferior derecha de la base superior detrás (size, size, -size)
        { 0, size, -size }                       // 7: Esquina superior izquierda de la base superior detrás (0, size, -size)
    };

    // Proyecta los puntos 3D en la imagen 2D.
    std::vector<cv::Point2f> image_points;
    cv::projectPoints(object_points, rvec, tvec, M, dist_coeffs, image_points);

    // Define todas las líneas del modelo en un solo vector.
    std::vector<std::vector<cv::Point>> segments = {
        // Base inferior
        { image_points[0], image_points[1], image_points[2], image_points[3], image_points[0] },
        // Base superior
        { image_points[4], image_points[5], image_points[6], image_points[7], image_points[4] },
        // Conexiones entre las bases
        { image_points[0], image_points[4] },
        { image_points[1], image_points[5] },
        { image_points[2], image_points[6] },
        { image_points[3], image_points[7] }
    };

    cv::polylines(img, segments, true, cv::Scalar(0, 255, 0));

    //
}

void fsiv_project_image(const cv::Mat &model, cv::Mat &scene,
                        const cv::Size &board_size,
                        const std::vector<cv::Point2f> &chess_board_corners)
{
    CV_Assert(!model.empty() && model.type() == CV_8UC3);
    CV_Assert(!scene.empty() && scene.type() == CV_8UC3);
    CV_Assert(static_cast<size_t>(board_size.area()) ==
              chess_board_corners.size());

    // TODO
    // Hint: get the upper-left, upper-right, bottom-right and bottom-left
    //   chess_board_corners and map to the upper-left, upper-right, bottom-right
    //   and bottom-left model image point coordinates.
    //   Use cv::getPerspectiveTransform compute such mapping.
    // Hint: use cv::wrapPerspective to get a wrap version of the model image
    //   using the computed mapping. Use INTER_LINEAR as interpolation method
    //   and use BORDER_TRANSPARENT as a border extrapolation method
    //   to maintain the underlying image.
    //

    std::vector<cv::Point2f> model_points = {cv::Point2f(0, 0),
                                             cv::Point2f(model.cols-1, 0),
                                             cv::Point2f(0, model.rows-1),
                                             cv::Point2f(model.cols-1, model.rows-1)};

    std::vector<cv::Point2f> chess_board_points = {chess_board_corners[0],
                                               chess_board_corners[board_size.width-1],
                                               chess_board_corners[board_size.width*(board_size.height-1)],
                                               chess_board_corners[board_size.width*board_size.height-1]};

    cv::Mat transform = getPerspectiveTransform(model_points, chess_board_points);   

    cv::warpPerspective(model, scene, transform, scene.size(), cv::INTER_LINEAR, cv::BORDER_TRANSPARENT);                                                                                

    //
}
