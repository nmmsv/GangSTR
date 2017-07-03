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
#include "src/enclosing_class.h"

#include <math.h>
#include <iostream>
using namespace std;

bool EnclosingClass::GetLogClassProb(const int32_t& allele,
				     const int32_t& read_len, const int32_t& motif_len,
				     double* log_class_prob) {
  // TODO these shouldn't be set here
	int str_len = allele * motif_len;					// (L)
	double class_prob;
	if (read_len <= str_len)
		class_prob = 0;
	else
		class_prob = 2.0 * double(read_len - str_len) / double(2 * flank_len + str_len - 2 * read_len);

	if (class_prob > 0){
		*log_class_prob = log(class_prob);
		return true;
	}
	else if (class_prob == 0){
		*log_class_prob = NEG_INF;
		return true;
	}
	else
		return false;
}

bool EnclosingClass::GetLogReadProb(const int32_t& allele,
				    const int32_t& data,
				    const int32_t& read_len,
				    const int32_t& motif_len,
				    const int32_t& ref_count,
				    double* log_allele_prob) {
	double delta = data - allele;
	double allele_prob;
	if (delta == 0)
		allele_prob = 1 - stutter_up - stutter_down;
	else if (delta > 0)
		allele_prob = stutter_up * stutter_p * pow(1.0 - stutter_p, delta - 1.0);
	else
		allele_prob = stutter_down * stutter_p * pow(1.0 - stutter_p, -delta - 1.0);
	if (allele_prob > 0){
		*log_allele_prob = log(allele_prob);
		return true;
	}
	else if (allele_prob == 0){
		*log_allele_prob = NEG_INF;
		return true;
	}
	else
		return false;
}

