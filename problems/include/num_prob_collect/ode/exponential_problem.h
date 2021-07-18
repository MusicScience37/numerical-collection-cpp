/*
 * Copyright 2021 MusicScience37 (Kenta Kabashima)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
/*!
 * \file
 * \brief Definition of exponential_problem class.
 */
#pragma once

namespace num_prob_collect::ode {

/*!
 * \brief Class of test problem to calculate exponential function.
 */
class exponential_problem {
public:
    //! Type of variables.
    using variable_type = double;

    //! Type of scalars.
    using scalar_type = double;

    /*!
     * \brief Evaluate on a (time, variable) pair.
     *
     * \param[in] variable Variable.
     */
    void evaluate_on(double /*time*/, double variable) {
        diff_coeff_ = variable;
    }

    /*!
     * \brief Get the differential coefficient.
     *
     * \return Differential coefficient.
     */
    [[nodiscard]] auto diff_coeff() const -> double { return diff_coeff_; }

private:
    //! Differential coefficient.
    double diff_coeff_{};
};

}  // namespace num_prob_collect::ode
