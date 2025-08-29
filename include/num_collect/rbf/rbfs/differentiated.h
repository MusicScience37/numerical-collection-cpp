/*
 * Copyright 2025 MusicScience37 (Kenta Kabashima)
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
 * \brief Declaration of differentiated class.
 */
#pragma once

#include "num_collect/rbf/concepts/rbf.h"

namespace num_collect::rbf::rbfs {

/*!
 * \brief Traits to get the differentiated version of a RBF.
 *
 * \tparam RBF Type of the RBF.
 *
 * Specialization of this class must have a type `type`
 * which satisfies \ref num_collect::rbf::concepts::rbf concept
 * and represents a function given by applying the following operator to the
 * original RBF:
 *
 * \f[
 * D \equiv - \frac{1}{r} \frac{d}{dr}
 * \f]
 *
 * where \f$ r \f$ is the argument of the RBF.
 */
template <concepts::rbf RBF>
struct differentiated;

/*!
 * \brief Type of the differentiated version of a RBF.
 *
 * \tparam RBF Type of the RBF.
 */
template <concepts::rbf RBF>
using differentiated_t = typename differentiated<RBF>::type;

}  // namespace num_collect::rbf::rbfs
