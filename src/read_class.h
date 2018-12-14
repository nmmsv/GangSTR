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

#ifndef SRC_READ_CLASS_H__
#define SRC_READ_CLASS_H__

#include "src/options.h"

#include <stdint.h>

#include <vector>
#include <gsl/gsl_math.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
#include <gsl/gsl_cdf.h>

/*

Parent ReadClass
Individual read classes (FRR, enclosing, spanning) inherit
from this and implement their own read and class probability functions

A read class consists of:
- data (a vector of relevant values, e.g. copy number, insert size)
- a method to calculate the class log likelihood for a diploid genotype
 */
class ReadClass {
  friend class ReadClassTest;
  // friend class enclosing_class;
  // friend class FlankingClass;
 public:
  const static double NEG_INF = -25; // TODO make smaller?
  const static bool INS_NORMAL = true;
  ReadClass();
  virtual ~ReadClass();

  // Add a data point to the class data vector
  void AddData(const int32_t& data);
  // Set options (e.g. insert sizes, stutter params)
  void SetOptions(const Options& options);
  // Calculate class log likelihood for diploid genotype P(data|<A,B>)
  bool GetClassLogLikelihood(const int32_t& allele1, const int32_t& allele2,
			     const int32_t& read_len, const int32_t& motif_len,
			     const int32_t& ref_count, const int32_t& ploidy,
			     double* class_ll);
  // Clear all data from the class
  void Reset();
  // Check how many data points
  std::size_t GetDataSize();
  // Get PDF and CDF values of insert size distribution:
  double InsertSizeCDF(int32_t x);
  double InsertSizePDF(int32_t x);
 protected:
  // Calculate log probability P(datapoint | allele)
  bool GetAlleleLogLikelihood(const int32_t& allele, const int32_t& data,
			      const int32_t& read_len, const int32_t& motif_len,
			      const int32_t& ref_count,
			      double* allele_ll);

  // Constants related to models
  int32_t dist_mean;
  int32_t dist_sdev;
  int32_t flank_len;
  double stutter_up;
  double stutter_down;
  double stutter_p;
  bool read_prob_mode;
  // pdf and CDF of non-parametric model for insert size
  int32_t dist_distribution_size;
  std::vector<double> dist_pdf;
  std::vector<double> dist_cdf;
  
  // Store vector of data for this class
  std::vector<int32_t> read_class_data_;
  

  // Allele weights. TODO: change if phasing available, would need per-read weights
  const static double allele1_weight_ = 0.5;
  const static double allele2_weight_ = 0.5;

 private:
  // Calculate class probability for an allele - implemented in children classes
  virtual bool GetLogClassProb(const int32_t& allele,
			       const int32_t& read_len, const int32_t& motif_len,
			       double* log_class_prob);
  // Calculate read probability - implemented in children classes
  virtual bool GetLogReadProb(const int32_t& allele, const int32_t& data,
			      const int32_t& read_len, const int32_t& motif_len,
			      const int32_t& ref_count,
			      double* log_allele_prob);


};

#endif  // SRC_READ_CLASS_H__
