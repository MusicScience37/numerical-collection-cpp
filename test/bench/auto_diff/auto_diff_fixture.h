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
 * \brief Definition of auto_diff_fixture class.
 */
#include <cmath>

#include <Eigen/Core>
#include <celero/Celero.h>

#include "error_udm.h"

class auto_diff_fixture : public celero::TestFixture {
public:
    auto_diff_fixture() = default;

    void check_error(double actual, double expected) {
        log_error_->addValue(std::abs(actual - expected));
    }

    template <typename Derived>
    void check_error(const Eigen::MatrixBase<Derived>& actual,
        const Eigen::MatrixBase<Derived>& expected) {
        log_error_->addValue((actual - expected).norm());
    }

    [[nodiscard]] auto getUserDefinedMeasurements() const -> std::vector<
        std::shared_ptr<celero::UserDefinedMeasurement>> override {
        return {log_error_};
    }

private:
    std::shared_ptr<error_udm> log_error_{std::make_shared<error_udm>()};
};
