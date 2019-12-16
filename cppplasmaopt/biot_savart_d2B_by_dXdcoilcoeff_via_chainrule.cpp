#include "biot_savart.h"
//#include <tuple>
#include <vector>
using std::vector;
//#include <omp.h>
#include <chrono>

void biot_savart_d2B_by_dXdcoilcoeff_via_chainrule(Array& points, Array& gamma, Array& dgamma_by_dphi, Array& dgamma_by_dcoeff, Array& d2gamma_by_dphidcoeff, Array& res, Array& res_coil_gamma, Array& res_coil_gammadash) {
    auto t1 = std::chrono::high_resolution_clock::now();
    int num_points = points.shape(0);
    int num_quad_points = gamma.shape(0);
    int num_coeffs      = dgamma_by_dcoeff.shape(1);
    for (int i = 0; i < num_points; ++i) {
        auto point = Vec3d(3, &points.at(i, 0));
        for (int j = 0; j < num_quad_points; ++j) {
            auto gamma_j          = Vec3d(3, &gamma.at(j, 0));
            auto dgamma_j_by_dphi = Vec3d(3, &dgamma_by_dphi.at(j, 0));

            auto diff      = point - gamma_j;
            auto norm_diff = norm(diff);

            auto norm_diff_3_inv           = 1/(norm_diff*norm_diff*norm_diff);
            auto norm_diff_5_inv           = norm_diff_3_inv/(norm_diff*norm_diff);
            auto norm_diff_7_inv           = norm_diff_5_inv/(norm_diff*norm_diff);
            auto dgamma_by_dphi_cross_diff = cross(dgamma_j_by_dphi, diff);

            for(int k=0; k<3; k++) { // iterate over the three pertubation directions of the curve
                auto ek = Vec3d{0., 0., 0.};
                ek.at(k) = 1.;
                for (int l = 0; l < 3; ++l) { // iterate over the three directions that the gradient is taken in
                    auto el  = Vec3d{0., 0., 0.};
                    el.at(l) = 1.0;
                    auto term1 = norm_diff_3_inv * cross(ek, el);
                    auto term2 = 3 * inner(diff, ek) * norm_diff_5_inv * cross(dgamma_j_by_dphi, el);
                    auto term3 = -15 * inner(diff, ek) * diff.at(l) * norm_diff_7_inv * dgamma_by_dphi_cross_diff;
                    auto term4 = 3 * ek.at(l) * norm_diff_5_inv * dgamma_by_dphi_cross_diff;
                    auto term5 = -3 * diff.at(l) * norm_diff_5_inv * cross(ek, diff);
                    auto term6 = 3 * diff.at(l) * norm_diff_5_inv * cross(dgamma_j_by_dphi, ek);
                    res_coil_gamma.at(k, l, 0, i, j) = (term2+term3+term4+term6).at(0);
                    res_coil_gamma.at(k, l, 1, i, j) = (term2+term3+term4+term6).at(1);
                    res_coil_gamma.at(k, l, 2, i, j) = (term2+term3+term4+term6).at(2);
                    res_coil_gammadash.at(k, l, 0, i, j) = (term1+term5).at(0);
                    res_coil_gammadash.at(k, l, 1, i, j) = (term1+term5).at(1);
                    res_coil_gammadash.at(k, l, 2, i, j) = (term1+term5).at(2);
                }
            }
        }
    }
    auto t2 = std::chrono::high_resolution_clock::now();
    //Array res = xt::zeros<double>({num_points, num_coeffs, 3, 3});
    RowMat res_coil_00(num_points, num_coeffs);
    RowMat res_coil_10(num_points, num_coeffs);
    RowMat res_coil_20(num_points, num_coeffs);
    RowMat res_coil_01(num_points, num_coeffs);
    RowMat res_coil_11(num_points, num_coeffs);
    RowMat res_coil_21(num_points, num_coeffs);
    RowMat res_coil_02(num_points, num_coeffs);
    RowMat res_coil_12(num_points, num_coeffs);
    RowMat res_coil_22(num_points, num_coeffs);
    int j = 0;
    res_coil_00 = RowMat(num_points, num_quad_points, &res_coil_gamma.at(j, 0, 0, 0, 0)) * ColMat(num_quad_points, num_coeffs, &dgamma_by_dcoeff.at(0, 0, j)) + RowMat(num_points, num_quad_points, &res_coil_gammadash.at(j, 0, 0, 0, 0)) * ColMat(num_quad_points, num_coeffs, &d2gamma_by_dphidcoeff.at(0, 0, j));
    res_coil_10 = RowMat(num_points, num_quad_points, &res_coil_gamma.at(j, 0, 1, 0, 0)) * ColMat(num_quad_points, num_coeffs, &dgamma_by_dcoeff.at(0, 0, j)) + RowMat(num_points, num_quad_points, &res_coil_gammadash.at(j, 0, 1, 0, 0)) * ColMat(num_quad_points, num_coeffs, &d2gamma_by_dphidcoeff.at(0, 0, j));
    res_coil_20 = RowMat(num_points, num_quad_points, &res_coil_gamma.at(j, 0, 2, 0, 0)) * ColMat(num_quad_points, num_coeffs, &dgamma_by_dcoeff.at(0, 0, j)) + RowMat(num_points, num_quad_points, &res_coil_gammadash.at(j, 0, 2, 0, 0)) * ColMat(num_quad_points, num_coeffs, &d2gamma_by_dphidcoeff.at(0, 0, j));
    res_coil_01 = RowMat(num_points, num_quad_points, &res_coil_gamma.at(j, 1, 0, 0, 0)) * ColMat(num_quad_points, num_coeffs, &dgamma_by_dcoeff.at(0, 0, j)) + RowMat(num_points, num_quad_points, &res_coil_gammadash.at(j, 1, 0, 0, 0)) * ColMat(num_quad_points, num_coeffs, &d2gamma_by_dphidcoeff.at(0, 0, j));
    res_coil_11 = RowMat(num_points, num_quad_points, &res_coil_gamma.at(j, 1, 1, 0, 0)) * ColMat(num_quad_points, num_coeffs, &dgamma_by_dcoeff.at(0, 0, j)) + RowMat(num_points, num_quad_points, &res_coil_gammadash.at(j, 1, 1, 0, 0)) * ColMat(num_quad_points, num_coeffs, &d2gamma_by_dphidcoeff.at(0, 0, j));
    res_coil_21 = RowMat(num_points, num_quad_points, &res_coil_gamma.at(j, 1, 2, 0, 0)) * ColMat(num_quad_points, num_coeffs, &dgamma_by_dcoeff.at(0, 0, j)) + RowMat(num_points, num_quad_points, &res_coil_gammadash.at(j, 1, 2, 0, 0)) * ColMat(num_quad_points, num_coeffs, &d2gamma_by_dphidcoeff.at(0, 0, j));
    res_coil_02 = RowMat(num_points, num_quad_points, &res_coil_gamma.at(j, 2, 0, 0, 0)) * ColMat(num_quad_points, num_coeffs, &dgamma_by_dcoeff.at(0, 0, j)) + RowMat(num_points, num_quad_points, &res_coil_gammadash.at(j, 2, 0, 0, 0)) * ColMat(num_quad_points, num_coeffs, &d2gamma_by_dphidcoeff.at(0, 0, j));
    res_coil_12 = RowMat(num_points, num_quad_points, &res_coil_gamma.at(j, 2, 1, 0, 0)) * ColMat(num_quad_points, num_coeffs, &dgamma_by_dcoeff.at(0, 0, j)) + RowMat(num_points, num_quad_points, &res_coil_gammadash.at(j, 2, 1, 0, 0)) * ColMat(num_quad_points, num_coeffs, &d2gamma_by_dphidcoeff.at(0, 0, j));
    res_coil_22 = RowMat(num_points, num_quad_points, &res_coil_gamma.at(j, 2, 2, 0, 0)) * ColMat(num_quad_points, num_coeffs, &dgamma_by_dcoeff.at(0, 0, j)) + RowMat(num_points, num_quad_points, &res_coil_gammadash.at(j, 2, 2, 0, 0)) * ColMat(num_quad_points, num_coeffs, &d2gamma_by_dphidcoeff.at(0, 0, j));
    for (j = 1; j < 3; ++j) {
        res_coil_00 += RowMat(num_points, num_quad_points, &res_coil_gamma.at(j, 0, 0, 0, 0)) * ColMat(num_quad_points, num_coeffs, &dgamma_by_dcoeff.at(0, 0, j)) + RowMat(num_points, num_quad_points, &res_coil_gammadash.at(j, 0, 0, 0, 0)) * ColMat(num_quad_points, num_coeffs, &d2gamma_by_dphidcoeff.at(0, 0, j));
        res_coil_10 += RowMat(num_points, num_quad_points, &res_coil_gamma.at(j, 0, 1, 0, 0)) * ColMat(num_quad_points, num_coeffs, &dgamma_by_dcoeff.at(0, 0, j)) + RowMat(num_points, num_quad_points, &res_coil_gammadash.at(j, 0, 1, 0, 0)) * ColMat(num_quad_points, num_coeffs, &d2gamma_by_dphidcoeff.at(0, 0, j));
        res_coil_20 += RowMat(num_points, num_quad_points, &res_coil_gamma.at(j, 0, 2, 0, 0)) * ColMat(num_quad_points, num_coeffs, &dgamma_by_dcoeff.at(0, 0, j)) + RowMat(num_points, num_quad_points, &res_coil_gammadash.at(j, 0, 2, 0, 0)) * ColMat(num_quad_points, num_coeffs, &d2gamma_by_dphidcoeff.at(0, 0, j));
        res_coil_01 += RowMat(num_points, num_quad_points, &res_coil_gamma.at(j, 1, 0, 0, 0)) * ColMat(num_quad_points, num_coeffs, &dgamma_by_dcoeff.at(0, 0, j)) + RowMat(num_points, num_quad_points, &res_coil_gammadash.at(j, 1, 0, 0, 0)) * ColMat(num_quad_points, num_coeffs, &d2gamma_by_dphidcoeff.at(0, 0, j));
        res_coil_11 += RowMat(num_points, num_quad_points, &res_coil_gamma.at(j, 1, 1, 0, 0)) * ColMat(num_quad_points, num_coeffs, &dgamma_by_dcoeff.at(0, 0, j)) + RowMat(num_points, num_quad_points, &res_coil_gammadash.at(j, 1, 1, 0, 0)) * ColMat(num_quad_points, num_coeffs, &d2gamma_by_dphidcoeff.at(0, 0, j));
        res_coil_21 += RowMat(num_points, num_quad_points, &res_coil_gamma.at(j, 1, 2, 0, 0)) * ColMat(num_quad_points, num_coeffs, &dgamma_by_dcoeff.at(0, 0, j)) + RowMat(num_points, num_quad_points, &res_coil_gammadash.at(j, 1, 2, 0, 0)) * ColMat(num_quad_points, num_coeffs, &d2gamma_by_dphidcoeff.at(0, 0, j));
        res_coil_02 += RowMat(num_points, num_quad_points, &res_coil_gamma.at(j, 2, 0, 0, 0)) * ColMat(num_quad_points, num_coeffs, &dgamma_by_dcoeff.at(0, 0, j)) + RowMat(num_points, num_quad_points, &res_coil_gammadash.at(j, 2, 0, 0, 0)) * ColMat(num_quad_points, num_coeffs, &d2gamma_by_dphidcoeff.at(0, 0, j));
        res_coil_12 += RowMat(num_points, num_quad_points, &res_coil_gamma.at(j, 2, 1, 0, 0)) * ColMat(num_quad_points, num_coeffs, &dgamma_by_dcoeff.at(0, 0, j)) + RowMat(num_points, num_quad_points, &res_coil_gammadash.at(j, 2, 1, 0, 0)) * ColMat(num_quad_points, num_coeffs, &d2gamma_by_dphidcoeff.at(0, 0, j));
        res_coil_22 += RowMat(num_points, num_quad_points, &res_coil_gamma.at(j, 2, 2, 0, 0)) * ColMat(num_quad_points, num_coeffs, &dgamma_by_dcoeff.at(0, 0, j)) + RowMat(num_points, num_quad_points, &res_coil_gammadash.at(j, 2, 2, 0, 0)) * ColMat(num_quad_points, num_coeffs, &d2gamma_by_dphidcoeff.at(0, 0, j));
    }
    for (int i = 0; i < num_points; ++i) {
        for (int j = 0; j < num_coeffs; ++j) {
           res.at(i, j, 0, 0) = res_coil_00(i, j); 
           res.at(i, j, 1, 0) = res_coil_10(i, j); 
           res.at(i, j, 2, 0) = res_coil_20(i, j); 
           res.at(i, j, 0, 1) = res_coil_01(i, j); 
           res.at(i, j, 1, 1) = res_coil_11(i, j); 
           res.at(i, j, 2, 1) = res_coil_21(i, j); 
           res.at(i, j, 0, 2) = res_coil_02(i, j); 
           res.at(i, j, 1, 2) = res_coil_12(i, j); 
           res.at(i, j, 2, 2) = res_coil_22(i, j); 
        }
    }
    auto t3 = std::chrono::high_resolution_clock::now();
    double time1 = std::chrono::duration_cast<std::chrono::microseconds>( t2 - t1 ).count();
    double time2 = std::chrono::duration_cast<std::chrono::microseconds>( t3 - t2 ).count();
    std::cout << "Calc: " << time1 << " micros." << std::endl;
    std::cout << "Mult: " << time2 << " micros." << std::endl;
}

vector<Array> biot_savart_d2B_by_dXdcoilcoeff_via_chainrule_allcoils(vector<Array>& points, vector<Array>& gammas, vector<Array>& dgamma_by_dphis, vector<Array>& dgamma_by_dcoeffs, vector<Array>& d2gamma_by_dphidcoeffs) {
    int num_coils = gammas.size();
    auto res = vector<Array>();
    res.reserve(num_coils);
    auto res_coil_gamma = vector<Array>();
    res_coil_gamma.reserve(num_coils);
    auto res_coil_gammadash = vector<Array>();
    res_coil_gammadash.reserve(num_coils);

    // Actually slightly quicker if we assume the dimensions are the same on every thread.
    // Should do that optimisation at some point.
    //int tid = omp_get_thread_num();
    //int tsize = omp_get_num_threads();
    for(int i=0; i<num_coils; i++) {
        long unsigned int num_points = points[i].shape(0);
        long unsigned int num_quad_points = gammas[i].shape(0);
        long unsigned int num_coeffs = dgamma_by_dcoeffs[i].shape(1);
        res.push_back(xt::pyarray<double>::from_shape({num_points, num_coeffs, 3, 3}));
        res_coil_gamma.push_back(xt::pyarray<double>::from_shape({3, 3, 3, num_points, num_quad_points}));
        res_coil_gammadash.push_back(xt::pyarray<double>::from_shape({3, 3, 3, num_points, num_quad_points}));
    }
    #pragma omp parallel for
    for(int i=0; i<num_coils; i++) {
       biot_savart_d2B_by_dXdcoilcoeff_via_chainrule(points[i], gammas[i], dgamma_by_dphis[i], dgamma_by_dcoeffs[i], d2gamma_by_dphidcoeffs[i], res[i], res_coil_gamma[i], res_coil_gammadash[i]);
    }
    return res;
}
