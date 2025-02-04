/******************************************************************************
 * Project:  PROJ
 * Purpose:  Implements proj_trans_bounds()
 *
 * Author:   Alan D. Snow <alansnow21@gmail.com>
 *
 ******************************************************************************
 * Copyright (c) 2021, Alan D. Snow <alansnow21@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *****************************************************************************/

#define FROM_PROJ_CPP

#include "proj.h"
#include "proj_internal.h"
#include <math.h>

// ---------------------------------------------------------------------------
static double simple_min(const double *data, const int arr_len) {
    double min_value = data[0];
    for (int iii = 1; iii < arr_len; iii++) {
        if (data[iii] < min_value)
            min_value = data[iii];
    }
    return min_value;
}

// ---------------------------------------------------------------------------
static double simple_max(const double *data, const int arr_len) {
    double max_value = data[0];
    for (int iii = 1; iii < arr_len; iii++) {
        if ((data[iii] > max_value || max_value == HUGE_VAL) &&
            data[iii] != HUGE_VAL)
            max_value = data[iii];
    }
    return max_value;
}

// ---------------------------------------------------------------------------
static int find_previous_index(const int iii, const double *data,
                               const int arr_len) {
    // find index of nearest valid previous value if exists
    int prev_iii = iii - 1;
    if (prev_iii == -1) // handle wraparound
        prev_iii = arr_len - 1;
    while (data[prev_iii] == HUGE_VAL && prev_iii != iii) {
        prev_iii--;
        if (prev_iii == -1) // handle wraparound
            prev_iii = arr_len - 1;
    }
    return prev_iii;
}

// ---------------------------------------------------------------------------
/******************************************************************************
Handles the case when longitude values cross the antimeridian
when calculating the minimum.
Note: The data array must be in a linear ring.
Note: This requires a densified ring with at least 2 additional
        points per edge to correctly handle global extents.
If only 1 additional point:
    |        |
    |RL--x0--|RL--
    |        |
-180    180|-180
If they are evenly spaced and it crosses the antimeridian:
x0 - L = 180
R - x0 = -180
For example:
Let R = -179.9, x0 = 0.1, L = -179.89
x0 - L = 0.1 - -179.9 = 180
R - x0 = -179.89 - 0.1 ~= -180
This is the same in the case when it didn't cross the antimeridian.
If you have 2 additional points:
    |            |
    |RL--x0--x1--|RL--
    |            |
-180        180|-180
If they are evenly spaced and it crosses the antimeridian:
x0 - L = 120
x1 - x0 = 120
R - x1 = -240
For example:
Let R = -179.9, x0 = -59.9, x1 = 60.1 L = -179.89
x0 - L = 59.9 - -179.9 = 120
x1 - x0 = 60.1 - 59.9 = 120
R - x1 = -179.89 - 60.1 ~= -240
However, if they are evenly spaced and it didn't cross the antimeridian:
x0 - L = 120
x1 - x0 = 120
R - x1 = 120
From this, we have a delta that is guaranteed to be significantly
large enough to tell the difference reguarless of the direction
the antimeridian was crossed.
However, even though the spacing was even in the source projection, it isn't
guaranteed in the target geographic projection. So, instead of 240, 200 is used
as it significantly larger than 120 to be sure that the antimeridian was crossed
but smalller than 240 to account for possible irregularities in distances
when re-projecting. Also, 200 ensures latitudes are ignored for axis order
handling.
******************************************************************************/
static double antimeridian_min(const double *data, const int arr_len) {
    double positive_min = HUGE_VAL;
    double min_value = HUGE_VAL;
    int crossed_meridian_count = 0;
    bool positive_meridian = false;

    for (int iii = 0; iii < arr_len; iii++) {
        if (data[iii] == HUGE_VAL)
            continue;
        int prev_iii = find_previous_index(iii, data, arr_len);
        // check if crossed meridian
        double delta = data[prev_iii] - data[iii];
        // 180 -> -180
        if (delta >= 200 && delta != HUGE_VAL) {
            if (crossed_meridian_count == 0)
                positive_min = min_value;
            crossed_meridian_count++;
            positive_meridian = false;
            // -180 -> 180
        } else if (delta <= -200 && delta != HUGE_VAL) {
            if (crossed_meridian_count == 0)
                positive_min = data[iii];
            crossed_meridian_count++;
            positive_meridian = true;
        }
        // positive meridian side min
        if (positive_meridian && data[iii] < positive_min)
            positive_min = data[iii];
        // track general min value
        if (data[iii] < min_value)
            min_value = data[iii];
    }

    if (crossed_meridian_count == 2)
        return positive_min;
    else if (crossed_meridian_count == 4)
        // bounds extends beyond -180/180
        return -180;
    return min_value;
}

// ---------------------------------------------------------------------------
// Handles the case when longitude values cross the antimeridian
// when calculating the minimum.
// Note: The data array must be in a linear ring.
// Note: This requires a densified ring with at least 2 additional
//       points per edge to correctly handle global extents.
// See antimeridian_min docstring for reasoning.
static double antimeridian_max(const double *data, const int arr_len) {
    double negative_max = -HUGE_VAL;
    double max_value = -HUGE_VAL;
    bool negative_meridian = false;
    int crossed_meridian_count = 0;

    for (int iii = 0; iii < arr_len; iii++) {
        if (data[iii] == HUGE_VAL)
            continue;
        int prev_iii = find_previous_index(iii, data, arr_len);
        // check if crossed meridian
        double delta = data[prev_iii] - data[iii];
        // 180 -> -180
        if (delta >= 200 && delta != HUGE_VAL) {
            if (crossed_meridian_count == 0)
                negative_max = data[iii];
            crossed_meridian_count++;
            negative_meridian = true;
            // -180 -> 180
        } else if (delta <= -200 && delta != HUGE_VAL) {
            if (crossed_meridian_count == 0)
                negative_max = max_value;
            negative_meridian = false;
            crossed_meridian_count++;
        }
        // negative meridian side max
        if (negative_meridian &&
            (data[iii] > negative_max || negative_max == HUGE_VAL) &&
            data[iii] != HUGE_VAL)
            negative_max = data[iii];
        // track general max value
        if ((data[iii] > max_value || max_value == HUGE_VAL) &&
            data[iii] != HUGE_VAL)
            max_value = data[iii];
    }
    if (crossed_meridian_count == 2)
        return negative_max;
    else if (crossed_meridian_count == 4)
        // bounds extends beyond -180/180
        return 180;
    return max_value;
}

// ---------------------------------------------------------------------------
// Check if the original projected bounds contains
// the north pole.
// This assumes that the destination CRS is geographic.
static bool contains_north_pole(PJ *projobj, PJ_DIRECTION pj_direction,
                                const double xmin, const double ymin,
                                const double xmax, const double ymax,
                                bool lon_lat_order) {
    double pole_y = 90;
    double pole_x = 0;
    if (!lon_lat_order) {
        pole_y = 0;
        pole_x = 90;
    }
    proj_trans_generic(projobj, pj_opposite_direction(pj_direction), &pole_x,
                       sizeof(double), 1, &pole_y, sizeof(double), 1, nullptr,
                       sizeof(double), 0, nullptr, sizeof(double), 0);
    if (xmin < pole_x && pole_x < xmax && ymax > pole_y && pole_y > ymin)
        return true;
    return false;
}

// ---------------------------------------------------------------------------
// Check if the original projected bounds contains
// the south pole.
// This assumes that the destination CRS is geographic.
static bool contains_south_pole(PJ *projobj, PJ_DIRECTION pj_direction,
                                const double xmin, const double ymin,
                                const double xmax, const double ymax,
                                bool lon_lat_order) {
    double pole_y = -90;
    double pole_x = 0;
    if (!lon_lat_order) {
        pole_y = 0;
        pole_x = -90;
    }
    proj_trans_generic(projobj, pj_opposite_direction(pj_direction), &pole_x,
                       sizeof(double), 1, &pole_y, sizeof(double), 1, nullptr,
                       sizeof(double), 0, nullptr, sizeof(double), 0);
    if (xmin < pole_x && pole_x < xmax && ymax > pole_y && pole_y > ymin)
        return true;
    return false;
}

// ---------------------------------------------------------------------------
// Check if the target CRS of the transformation
// has the longitude latitude axis order.
// This assumes that the destination CRS is geographic.
static int target_crs_lon_lat_order(PJ_CONTEXT *transformer_ctx,
                                    PJ *transformer_pj,
                                    PJ_DIRECTION pj_direction) {
    PJ *target_crs = nullptr;
    if (pj_direction == PJ_FWD)
        target_crs = proj_get_target_crs(transformer_ctx, transformer_pj);
    else if (pj_direction == PJ_INV)
        target_crs = proj_get_source_crs(transformer_ctx, transformer_pj);
    if (target_crs == nullptr) {
        proj_context_log_debug(transformer_ctx,
                               "Unable to retrieve target CRS");
        return -1;
    }
    PJ *coord_system_pj;
    if (proj_get_type(target_crs) == PJ_TYPE_COMPOUND_CRS) {
        PJ *horiz_crs = proj_crs_get_sub_crs(transformer_ctx, target_crs, 0);
        if (!horiz_crs)
            return -1;
        coord_system_pj =
            proj_crs_get_coordinate_system(transformer_ctx, horiz_crs);
        proj_destroy(horiz_crs);
    } else {
        coord_system_pj =
            proj_crs_get_coordinate_system(transformer_ctx, target_crs);
    }
    proj_destroy(target_crs);
    if (coord_system_pj == nullptr) {
        proj_context_log_debug(transformer_ctx,
                               "Unable to get target CRS coordinate system.");
        return -1;
    }
    const char *abbrev = nullptr;
    int success = proj_cs_get_axis_info(transformer_ctx, coord_system_pj, 0,
                                        nullptr, &abbrev, nullptr, nullptr,
                                        nullptr, nullptr, nullptr);
    proj_destroy(coord_system_pj);
    if (success != 1)
        return -1;
    return strcmp(abbrev, "lon") == 0 || strcmp(abbrev, "Lon") == 0;
}

// ---------------------------------------------------------------------------

/** \brief Transform boundary,
 *
 * Transform boundary densifying the edges to account for nonlinear
 * transformations along these edges and extracting the outermost bounds.
 *
 * If the destination CRS is geographic, the first axis is longitude,
 * and xmax < xmin then the bounds crossed the antimeridian.
 * In this scenario there are two polygons, one on each side of the
 * antimeridian. The first polygon should be constructed with (xmin, ymin, 180,
 * ymax) and the second with (-180, ymin, xmax, ymax).
 *
 * If the destination CRS is geographic, the first axis is latitude,
 * and ymax < ymin then the bounds crossed the antimeridian.
 * In this scenario there are two polygons, one on each side of the
 * antimeridian. The first polygon should be constructed with (ymin, xmin, ymax,
 * 180) and the second with (ymin, -180, ymax, xmax).
 *
 * @param context The PJ_CONTEXT object.
 * @param P The PJ object representing the transformation.
 * @param direction The direction of the transformation.
 * @param xmin Minimum bounding coordinate of the first axis in source CRS
 *             (target CRS if direction is inverse).
 * @param ymin Minimum bounding coordinate of the second axis in source CRS.
 *             (target CRS if direction is inverse).
 * @param xmax Maximum bounding coordinate of the first axis in source CRS.
 *             (target CRS if direction is inverse).
 * @param ymax Maximum bounding coordinate of the second axis in source CRS.
 *             (target CRS if direction is inverse).
 * @param out_xmin Minimum bounding coordinate of the first axis in target CRS
 *             (source CRS if direction is inverse).
 * @param out_ymin Minimum bounding coordinate of the second axis in target CRS.
 *             (source CRS if direction is inverse).
 * @param out_xmax Maximum bounding coordinate of the first axis in target CRS.
 *             (source CRS if direction is inverse).
 * @param out_ymax Maximum bounding coordinate of the second axis in target CRS.
 *             (source CRS if direction is inverse).
 * @param densify_pts Recommended to use 21. This is the number of points
 *     to use to densify the bounding polygon in the transformation.
 * @return an integer. 1 if successful. 0 if failures encountered.
 * @since 8.2
 */
int proj_trans_bounds(PJ_CONTEXT *context, PJ *P, PJ_DIRECTION direction,
                      const double xmin, const double ymin, const double xmax,
                      const double ymax, double *out_xmin, double *out_ymin,
                      double *out_xmax, double *out_ymax,
                      const int densify_pts) {
    *out_xmin = HUGE_VAL;
    *out_ymin = HUGE_VAL;
    *out_xmax = HUGE_VAL;
    *out_ymax = HUGE_VAL;

    if (P == nullptr) {
        proj_log_error(P, _("NULL P object not allowed."));
        proj_errno_set(P, PROJ_ERR_INVALID_OP_ILLEGAL_ARG_VALUE);
        return false;
    }
    if (densify_pts < 0 || densify_pts > 10000) {
        proj_log_error(P, _("densify_pts must be between 0-10000."));
        proj_errno_set(P, PROJ_ERR_INVALID_OP_ILLEGAL_ARG_VALUE);
        return false;
    }

    PJ_PROJ_INFO pj_info = proj_pj_info(P);
    if (pj_info.id == nullptr) {
        proj_log_error(P, _("NULL transformation not allowed,"));
        proj_errno_set(P, PROJ_ERR_INVALID_OP_ILLEGAL_ARG_VALUE);
        return false;
    }
    if (strcmp(pj_info.id, "noop") == 0 || direction == PJ_IDENT) {
        *out_xmin = xmin;
        *out_xmax = xmax;
        *out_ymin = ymin;
        *out_ymax = ymax;
        return true;
    }

    bool degree_output = proj_degree_output(P, direction) != 0;
    bool degree_input = proj_degree_input(P, direction) != 0;
    if (degree_output && densify_pts < 2) {
        proj_log_error(
            P,
            _("densify_pts must be at least 2 if the output is geographic."));
        proj_errno_set(P, PROJ_ERR_INVALID_OP_ILLEGAL_ARG_VALUE);
        return false;
    }

    int side_pts = densify_pts + 1; // add one because we are densifying
    const int boundary_len = side_pts * 4;
    std::vector<double> x_boundary_array;
    std::vector<double> y_boundary_array;
    try {
        x_boundary_array.resize(boundary_len);
        y_boundary_array.resize(boundary_len);
    } catch (const std::exception &e) // memory allocation failure
    {
        proj_log_error(P, e.what());
        proj_errno_set(P, PROJ_ERR_INVALID_OP_ILLEGAL_ARG_VALUE);
        return false;
    }
    double delta_x = 0;
    double delta_y = 0;
    bool north_pole_in_bounds = false;
    bool south_pole_in_bounds = false;
    bool input_lon_lat_order = false;
    bool output_lon_lat_order = false;
    if (degree_input) {
        int in_order_lon_lat = target_crs_lon_lat_order(
            context, P, pj_opposite_direction(direction));
        if (in_order_lon_lat == -1)
            return false;
        input_lon_lat_order = in_order_lon_lat != 0;
    }
    if (degree_output) {
        int out_order_lon_lat = target_crs_lon_lat_order(context, P, direction);
        if (out_order_lon_lat == -1)
            return false;
        output_lon_lat_order = out_order_lon_lat != 0;
        north_pole_in_bounds = contains_north_pole(
            P, direction, xmin, ymin, xmax, ymax, output_lon_lat_order);
        south_pole_in_bounds = contains_south_pole(
            P, direction, xmin, ymin, xmax, ymax, output_lon_lat_order);
    }

    if (degree_input && xmax < xmin) {
        if (!input_lon_lat_order) {
            proj_log_error(P, _("latitude max < latitude min."));
            proj_errno_set(P, PROJ_ERR_INVALID_OP_ILLEGAL_ARG_VALUE);
            return false;
        }
        // handle antimeridian
        delta_x = (xmax - xmin + 360.0) / side_pts;
    } else {
        delta_x = (xmax - xmin) / side_pts;
    }
    if (degree_input && ymax < ymin) {
        if (input_lon_lat_order) {
            proj_log_error(P, _("latitude max < latitude min."));
            proj_errno_set(P, PROJ_ERR_INVALID_OP_ILLEGAL_ARG_VALUE);
            return false;
        }
        // handle antimeridian
        delta_y = (ymax - ymin + 360.0) / side_pts;
    } else {
        delta_y = (ymax - ymin) / side_pts;
    }

    // build densified bounding box
    // Note: must be a linear ring for antimeridian logic
    for (int iii = 0; iii < side_pts; iii++) {
        // xmin boundary
        y_boundary_array[iii] = ymax - iii * delta_y;
        x_boundary_array[iii] = xmin;
        // ymin boundary
        y_boundary_array[iii + side_pts] = ymin;
        x_boundary_array[iii + side_pts] = xmin + iii * delta_x;
        // xmax boundary
        y_boundary_array[iii + side_pts * 2] = ymin + iii * delta_y;
        x_boundary_array[iii + side_pts * 2] = xmax;
        // ymax boundary
        y_boundary_array[iii + side_pts * 3] = ymax;
        x_boundary_array[iii + side_pts * 3] = xmax - iii * delta_x;
    }
    proj_trans_generic(P, direction, &x_boundary_array[0], sizeof(double),
                       boundary_len, &y_boundary_array[0], sizeof(double),
                       boundary_len, nullptr, 0, 0, nullptr, 0, 0);

    if (!degree_output) {
        *out_xmin = simple_min(&x_boundary_array[0], boundary_len);
        *out_xmax = simple_max(&x_boundary_array[0], boundary_len);
        *out_ymin = simple_min(&y_boundary_array[0], boundary_len);
        *out_ymax = simple_max(&y_boundary_array[0], boundary_len);
    } else if (north_pole_in_bounds && output_lon_lat_order) {
        *out_xmin = -180;
        *out_ymin = simple_min(&y_boundary_array[0], boundary_len);
        *out_xmax = 180;
        *out_ymax = 90;
    } else if (north_pole_in_bounds) {
        *out_xmin = simple_min(&x_boundary_array[0], boundary_len);
        *out_ymin = -180;
        *out_xmax = 90;
        *out_ymax = 180;
    } else if (south_pole_in_bounds && output_lon_lat_order) {
        *out_xmin = -180;
        *out_ymin = -90;
        *out_xmax = 180;
        *out_ymax = simple_max(&y_boundary_array[0], boundary_len);
    } else if (south_pole_in_bounds) {
        *out_xmin = -90;
        *out_ymin = -180;
        *out_xmax = simple_max(&x_boundary_array[0], boundary_len);
        *out_ymax = 180;
    } else if (output_lon_lat_order) {
        *out_xmin = antimeridian_min(&x_boundary_array[0], boundary_len);
        *out_xmax = antimeridian_max(&x_boundary_array[0], boundary_len);
        *out_ymin = simple_min(&y_boundary_array[0], boundary_len);
        *out_ymax = simple_max(&y_boundary_array[0], boundary_len);
    } else {
        *out_xmin = simple_min(&x_boundary_array[0], boundary_len);
        *out_xmax = simple_max(&x_boundary_array[0], boundary_len);
        *out_ymin = antimeridian_min(&y_boundary_array[0], boundary_len);
        *out_ymax = antimeridian_max(&y_boundary_array[0], boundary_len);
    }
    return true;
}
