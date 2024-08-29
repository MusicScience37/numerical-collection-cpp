/*
 * Copyright 2024 MusicScience37 (Kenta Kabashima)
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
 * \brief Definition of table_comparator class.
 */
#pragma once

#include <limits>
#include <string>
#include <vector>

#include "fmt_approval_tests.h"
#include "num_collect/base/index_type.h"

class table_comparator : public ApprovalTests::ApprovalComparator {
public:
    table_comparator(std::vector<bool> checked_columns,
        num_collect::index_type num_rows, num_collect::index_type precision);

    [[nodiscard]] auto contentsAreEquivalent(std::string received_path,
        std::string approved_path) const -> bool override;

private:
    std::vector<bool> checked_columns_;

    num_collect::index_type num_rows_;

    static constexpr num_collect::index_type max_precision =
        std::numeric_limits<double>::digits10 - 2;

    num_collect::index_type precision_;

    double tol_rel_error_;

    double tol_abs_error_;
};
