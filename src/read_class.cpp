/*
Copyright (C) 2017 Melissa Gymrek <mgymrek@ucsd.edu>
and Nima Mousavi (mousavi@ucsd.edu)

This file is part of GangSTR.

GangSTR is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

GangSTR is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GangSTR.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "src/mathops.h"
#include "src/read_class.h"

#include <math.h>
#include <iostream>
#include <iomanip>
#include <typeinfo>
using namespace std;

ReadClass::ReadClass() {}

void ReadClass::SetCoverage(const int32_t& _coverage) {
  cov = _coverage;
}

void ReadClass::SetLocusParams(const STRLocusInfo& sli) {
  stutter_up = sli.stutter_up;
  stutter_down = sli.stutter_down;
  stutter_p = sli.stutter_p;
}

void ReadClass::SetGlobalParams(const SampleProfile& _sample_profile,
				const int32_t& _flank_len, 
				const bool& _read_prob_mode,
				const bool& _hist_mode) {
  dist_mean = _sample_profile.dist_mean;
  dist_sdev = _sample_profile.dist_sdev;
  dist_pdf = _sample_profile.dist_pdf;
  dist_cdf = _sample_profile.dist_cdf;
  dist_distribution_size = dist_pdf.size();
  flank_len = _flank_len;
  read_prob_mode = _read_prob_mode;
  hist_mode = _hist_mode;
}

void ReadClass::AddData(const int32_t& data) {
  read_class_data_.push_back(data);
}

double ReadClass::InsertSizeCDF(int32_t x){
  int32_t shifted_x = x - dist_mean;
  // Gaussian
  //return gsl_cdf_gaussian_P(shifted_x, dist_sdev);
  // Non-paramteric
  if (x < 0){
    return 0.0;
  }
  else if (x > 0 and x < dist_distribution_size){
    return dist_cdf[x];
  }
  else{
    return 1.0;
  }
}
double ReadClass::InsertSizePDF(int32_t x){
  int32_t shifted_x = x - dist_mean;
  // Gaussian
  //return gsl_ran_gaussian_pdf(shifted_x, dist_sdev);
  // Non-paramteric
  if (x < 0){
    return 0.0;
  }
  else if (x > 0 and x < dist_distribution_size){
    return dist_pdf[x];
  }
  else{
    return 0.0;
  }
}

/*
  Calculates log P(read_class_data_ | <allele1, allele2>) and sets class_ll

  log P(data|<allelele1, allele2>) = sum_i log P(data_i | <allele1, allele2>)
  P(data_i | <allele1, allele2> = allele1_weight*P(data_i|allele1) + allele2_weight*P(data_i|allele2)

  Return false if something goes wrong.
 */
bool ReadClass::GetClassLogLikelihood(const int32_t& allele1,
				      const int32_t& allele2,
				      const int32_t& read_len, const int32_t& motif_len,
				      const int32_t& ref_count, const int32_t& ploidy,
				      double* class_ll) {
  *class_ll = 0;
  double samp_log_likelihood, a1_ll, a2_ll;
  for (std::vector<int32_t>::iterator data_it = read_class_data_.begin();
       data_it != read_class_data_.end();
       data_it++) {
    if (!GetAlleleLogLikelihood(allele1, *data_it, read_len, motif_len, ref_count, &a1_ll)) {
      return false;
    }
    if (!GetAlleleLogLikelihood(allele2, *data_it, read_len, motif_len, ref_count, &a2_ll)) {
      return false;
    }
    // TODO delete
    // cerr<<typeid(*this).name()<<"\t";
    // cerr<<*data_it<<"\t"<<fast_log_sum_exp(log(allele1_weight_)+a1_ll, log(allele2_weight_)+a2_ll)<<endl;
    if (ploidy == 2){
      *class_ll += fast_log_sum_exp(log(allele1_weight_)+a1_ll, log(allele2_weight_)+a2_ll);
    }
    else if (ploidy == 1){
      *class_ll += log(allele1_weight_) + a1_ll;
    }
  }
  return true;
}

/*
  Calculates logP(data_i|allele) and sets allele_ll

  Return false if something goes wrong.
 */
bool ReadClass::GetAlleleLogLikelihood(const int32_t& allele,
				       const int32_t& data,
				       const int32_t& read_len, const int32_t&  motif_len,
				       const int32_t& ref_count,
				       double* allele_ll) {
  double log_class_prob, log_read_prob;
  if (!GetLogClassProb(allele, read_len, motif_len, &log_class_prob)) {
    return false;
  }
  if (!GetLogReadProb(allele, data, read_len, motif_len, ref_count, &log_read_prob)) {
    return false;
  }
  if (read_prob_mode){
    *allele_ll = log_read_prob - log(allele);
  }
  else{
    *allele_ll = log_class_prob + log_read_prob;
  }
  
  return true;
}

bool ReadClass::GetLogClassProb(const int32_t& allele,
				const int32_t& read_len, const int32_t& motif_len,
				double* log_class_prob) {
  return false; // Implement in child classes
}

bool ReadClass::GetLogReadProb(const int32_t& allele,
			       const int32_t& data,
			       const int32_t& read_len, const int32_t& motif_len,
			       const int32_t& ref_count,
			       double* log_allele_prob) {
  return false; // Implement in child classes
}

bool ReadClass::GetGridBoundaries(int32_t* min_allele, int32_t* max_allele) {
  return false; // Implemented in child classes
}

void ReadClass::Reset() {
  read_class_data_.clear();
}

std::size_t ReadClass::GetDataSize() {
  return read_class_data_.size();
}

ReadClass::~ReadClass() {}
