/**
 * @file visioner/src/diag_loss.cc
 * @date Fri 27 Jul 13:58:57 2012 CEST
 * @author Andre Anjos <andre.anjos@idiap.ch>
 *
 * @brief This file was part of Visioner and originally authored by "Cosmin
 * Atanasoaei <cosmin.atanasoaei@idiap.ch>". It was only modified to conform to
 * Bob coding standards and structure.
 *
 * Copyright (C) 2011-2012 Idiap Research Institute, Martigny, Switzerland
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3 of the License.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "visioner/model/losses/diag_loss.h"

namespace bob { namespace visioner {

  // Compute the error (associated to the loss)
  scalar_t DiagLoss::error(
      const scalar_t* targets, const scalar_t* scores, index_t size) const
  {
    scalar_t sum_error = 0;

    for (index_t o = 0; o < size; o ++)
    {
      sum_error += error(targets[o], scores[o]);
    }

    return sum_error;
  }

  // Compute the loss value & derivatives
  void DiagLoss::eval(
      const scalar_t* targets, const scalar_t* scores, index_t size,
      scalar_t& value) const
  {
    value = 0.0;

    for (index_t o = 0; o < size; o ++)
    {
      scalar_t ovalue;
      eval(targets[o], scores[o], ovalue);

      value += ovalue;
    }
  }
  void DiagLoss::eval(
      const scalar_t* targets, const scalar_t* scores, index_t size,
      scalar_t& value, scalar_t* grad) const
  {
    value = 0.0;                
    for (index_t o = 0; o < size; o ++)
    {
      scalar_t ovalue, oderiv1;
      eval(targets[o], scores[o], ovalue, oderiv1);

      value += ovalue;
      grad[o] = oderiv1;
    }
  }

}}