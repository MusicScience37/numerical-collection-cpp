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
 * \brief Definition of log_energy_change class.
 */
#pragma once

#include <celero/Celero.h>

/*!
 * \brief Class of Celero's user defined measurement for change of energy.
 */
class log_energy_change final
    : public celero::UserDefinedMeasurementTemplate<double> {
private:
    /*!
     * \brief Get the name.
     *
     * \return Name.
     */
    [[nodiscard]] auto getName() const -> std::string override {
        return "Log(dE)";
    }

    [[nodiscard]] auto reportSize() const -> bool override { return false; }

    [[nodiscard]] auto reportMean() const -> bool override { return true; }

    [[nodiscard]] auto reportVariance() const -> bool override { return false; }

    [[nodiscard]] auto reportStandardDeviation() const -> bool override {
        return false;
    }

    [[nodiscard]] auto reportSkewness() const -> bool override { return false; }

    [[nodiscard]] auto reportKurtosis() const -> bool override { return false; }

    [[nodiscard]] auto reportZScore() const -> bool override { return false; }

    [[nodiscard]] auto reportMin() const -> bool override { return false; }

    [[nodiscard]] auto reportMax() const -> bool override { return false; }
};
