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
 * \brief Header to check and warn about use of fast-math mode for
 * Eigen::BDCSVD.
 */
#pragma once

#ifdef __FAST_MATH__
#warning \
    "Use of -ffast-math is unsafe here ( https://eigen.tuxfamily.org/dox/classEigen_1_1BDCSVD.html )."
#endif
