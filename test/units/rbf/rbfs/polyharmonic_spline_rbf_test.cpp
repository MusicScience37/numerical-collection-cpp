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
 * \brief Test of polyharmonic_spline_rbf class.
 */
#include "num_collect/rbf/rbfs/polyharmonic_spline_rbf.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "is_finite.h"
#include "num_collect/rbf/concepts/rbf.h"
#include "num_collect/rbf/rbfs/differentiated.h"

TEST_CASE("num_collect::rbf::rbfs::polyharmonic_spline_rbf") {
    using num_collect::rbf::concepts::rbf;
    using num_collect::rbf::rbfs::differentiated_t;
    using num_collect::rbf::rbfs::polyharmonic_spline_rbf;

    SECTION("check of concepts") {
        STATIC_REQUIRE(rbf<polyharmonic_spline_rbf<double, 1>>);
        STATIC_REQUIRE(rbf<polyharmonic_spline_rbf<float, 2>>);
    }

    constexpr double rel_tol = 1e-8;
    constexpr double abs_tol = 1e-12;

    SECTION("calculate values of RBF") {
        SECTION("Degree 1") {
            const polyharmonic_spline_rbf<double, 1> rbf;

            double distance_rate{};
            double expected_value{};
            std::tie(distance_rate, expected_value) =
                GENERATE(table<double, double>({
                    std::make_tuple(0.0, 0.0),
                    std::make_tuple(1e-6, 1.0e-6),
                    std::make_tuple(0.1, 0.1),
                    std::make_tuple(0.9, 0.9),
                    std::make_tuple(1.0, 1.0),
                    std::make_tuple(1.23, 1.23),
                    std::make_tuple(7.89, 7.89),
                }));
            INFO("distance_rate = " << distance_rate);

            const double actual_value = rbf(distance_rate);
            CHECK_THAT(actual_value,
                Catch::Matchers::WithinAbs(expected_value,
                    std::max(abs_tol, std::abs(expected_value) * rel_tol)));
        }

        SECTION("Degree 2") {
            const polyharmonic_spline_rbf<double, 2> rbf;

            double distance_rate{};
            double expected_value{};
            std::tie(distance_rate, expected_value) =
                GENERATE(table<double, double>({
                    std::make_tuple(0.0, 0.0),
                    std::make_tuple(1e-6, -1.381551055796427e-11),
                    std::make_tuple(0.1, -0.02302585092994046),
                    std::make_tuple(0.9, -0.0853420176828393),
                    std::make_tuple(1.0, 0.0),
                    std::make_tuple(1.23, 0.313191736861547),
                    std::make_tuple(7.89, 128.5876971467802),
                }));
            INFO("distance_rate = " << distance_rate);

            const double actual_value = rbf(distance_rate);
            CHECK_THAT(actual_value,
                Catch::Matchers::WithinAbs(expected_value,
                    std::max(abs_tol, std::abs(expected_value) * rel_tol)));
        }

        SECTION("Degree 3") {
            const polyharmonic_spline_rbf<double, 3> rbf;

            double distance_rate{};
            double expected_value{};
            std::tie(distance_rate, expected_value) =
                GENERATE(table<double, double>({
                    std::make_tuple(0.0, 0.0),
                    std::make_tuple(1e-6, 1.0e-18),
                    std::make_tuple(0.1, 0.001),
                    std::make_tuple(0.9, 0.729),
                    std::make_tuple(1.0, 1.0),
                    std::make_tuple(1.23, 1.860867),
                    std::make_tuple(7.89, 491.169069),
                }));
            INFO("distance_rate = " << distance_rate);

            const double actual_value = rbf(distance_rate);
            CHECK_THAT(actual_value,
                Catch::Matchers::WithinAbs(expected_value,
                    std::max(abs_tol, std::abs(expected_value) * rel_tol)));
        }

        SECTION("Degree 4") {
            const polyharmonic_spline_rbf<double, 4> rbf;

            double distance_rate{};
            double expected_value{};
            std::tie(distance_rate, expected_value) =
                GENERATE(table<double, double>({
                    std::make_tuple(0.0, 0.0),
                    std::make_tuple(1e-6, -1.381551055796427e-23),
                    std::make_tuple(0.1, -0.0002302585092994046),
                    std::make_tuple(0.9, -0.06912703432309984),
                    std::make_tuple(1.0, 0.0),
                    std::make_tuple(1.23, 0.4738277786978345),
                    std::make_tuple(7.89, 8004.854181551075),
                }));
            INFO("distance_rate = " << distance_rate);

            const double actual_value = rbf(distance_rate);
            CHECK_THAT(actual_value,
                Catch::Matchers::WithinAbs(expected_value,
                    std::max(abs_tol, std::abs(expected_value) * rel_tol)));
        }

        SECTION("Degree 5") {
            const polyharmonic_spline_rbf<double, 5> rbf;

            double distance_rate{};
            double expected_value{};
            std::tie(distance_rate, expected_value) =
                GENERATE(table<double, double>({
                    std::make_tuple(0.0, 0.0),
                    std::make_tuple(1e-6, 1.0e-30),
                    std::make_tuple(0.1, 1.0e-5),
                    std::make_tuple(0.9, 0.59049),
                    std::make_tuple(1.0, 1.0),
                    std::make_tuple(1.23, 2.8153056843),
                    std::make_tuple(7.89, 30576.3060002949),
                }));
            INFO("distance_rate = " << distance_rate);

            const double actual_value = rbf(distance_rate);
            CHECK_THAT(actual_value,
                Catch::Matchers::WithinAbs(expected_value,
                    std::max(abs_tol, std::abs(expected_value) * rel_tol)));
        }

        SECTION("Degree 6") {
            const polyharmonic_spline_rbf<double, 6> rbf;

            double distance_rate{};
            double expected_value{};
            std::tie(distance_rate, expected_value) =
                GENERATE(table<double, double>({
                    std::make_tuple(0.0, 0.0),
                    std::make_tuple(1e-6, -1.381551055796427e-35),
                    std::make_tuple(0.1, -2.302585092994046e-6),
                    std::make_tuple(0.9, -0.05599289780171087),
                    std::make_tuple(1.0, 0.0),
                    std::make_tuple(1.23, 0.7168540463919537),
                    std::make_tuple(7.89, 498318.9829953357),
                }));
            INFO("distance_rate = " << distance_rate);

            const double actual_value = rbf(distance_rate);
            CHECK_THAT(actual_value,
                Catch::Matchers::WithinAbs(expected_value,
                    std::max(abs_tol, std::abs(expected_value) * rel_tol)));
        }

        SECTION("Degree 7") {
            const polyharmonic_spline_rbf<double, 7> rbf;

            double distance_rate{};
            double expected_value{};
            std::tie(distance_rate, expected_value) =
                GENERATE(table<double, double>({
                    std::make_tuple(0.0, 0.0),
                    std::make_tuple(1e-6, 1.0e-42),
                    std::make_tuple(0.1, 1.0e-7),
                    std::make_tuple(0.9, 0.4782969),
                    std::make_tuple(1.0, 1.0),
                    std::make_tuple(1.23, 4.25927596977747),
                    std::make_tuple(7.89, 1903439.258760958),
                }));
            INFO("distance_rate = " << distance_rate);

            const double actual_value = rbf(distance_rate);
            CHECK_THAT(actual_value,
                Catch::Matchers::WithinAbs(expected_value,
                    std::max(abs_tol, std::abs(expected_value) * rel_tol)));
        }

        SECTION("Degree 8") {
            const polyharmonic_spline_rbf<double, 8> rbf;

            double distance_rate{};
            double expected_value{};
            std::tie(distance_rate, expected_value) =
                GENERATE(table<double, double>({
                    std::make_tuple(0.0, 0.0),
                    std::make_tuple(1e-6, -1.381551055796427e-47),
                    std::make_tuple(0.1, -2.302585092994046e-8),
                    std::make_tuple(0.9, -0.0453542472193858),
                    std::make_tuple(1.0, 0.0),
                    std::make_tuple(1.23, 1.084528486786387),
                    std::make_tuple(7.89, 31021403.16132394),
                }));
            INFO("distance_rate = " << distance_rate);

            const double actual_value = rbf(distance_rate);
            CHECK_THAT(actual_value,
                Catch::Matchers::WithinAbs(expected_value,
                    std::max(abs_tol, std::abs(expected_value) * rel_tol)));
        }
    }

    SECTION("calculate first-order derivatives of RBF") {
        SECTION("Degree 1") {
            const differentiated_t<polyharmonic_spline_rbf<double, 1>>
                differentiated_rbf;

            SECTION("ordinary cases") {
                double distance_rate{};
                double expected_value{};
                std::tie(distance_rate, expected_value) =
                    GENERATE(table<double, double>({
                        std::make_tuple(1e-6, -1000000.0),
                        std::make_tuple(0.1, -10.0),
                        std::make_tuple(0.9, -1.111111111111111),
                        std::make_tuple(1.0, -1.0),
                        std::make_tuple(1.23, -0.8130081300813008),
                        std::make_tuple(7.89, -0.1267427122940431),
                    }));
                INFO("distance_rate = " << distance_rate);

                const double actual_value = differentiated_rbf(distance_rate);
                CHECK_THAT(actual_value,
                    Catch::Matchers::WithinAbs(expected_value,
                        std::max(abs_tol, std::abs(expected_value) * rel_tol)));
            }
            SECTION("calculate for zero") {
                constexpr double distance_rate = 0.0;

                const double actual_value = differentiated_rbf(distance_rate);
                CHECK_THAT(actual_value, is_finite());
            }
        }

        SECTION("Degree 2") {
            const differentiated_t<polyharmonic_spline_rbf<double, 2>>
                differentiated_rbf;

            SECTION("ordinary cases") {
                double distance_rate{};
                double expected_value{};
                std::tie(distance_rate, expected_value) =
                    GENERATE(table<double, double>({
                        std::make_tuple(1e-6, 26.63102111592855),
                        std::make_tuple(0.1, 3.605170185988091),
                        std::make_tuple(0.9, -0.7892789686843474),
                        std::make_tuple(1.0, -1.0),
                        std::make_tuple(1.23, -1.414028338768652),
                        std::make_tuple(7.89, -5.131192269715566),
                    }));
                INFO("distance_rate = " << distance_rate);

                const double actual_value = differentiated_rbf(distance_rate);
                CHECK_THAT(actual_value,
                    Catch::Matchers::WithinAbs(expected_value,
                        std::max(abs_tol, std::abs(expected_value) * rel_tol)));
            }
            SECTION("calculate for zero") {
                constexpr double distance_rate = 0.0;

                const double actual_value = differentiated_rbf(distance_rate);
                CHECK_THAT(actual_value, is_finite());
            }
        }

        SECTION("Degree 3") {
            const differentiated_t<polyharmonic_spline_rbf<double, 3>>
                differentiated_rbf;

            SECTION("ordinary cases") {
                double distance_rate{};
                double expected_value{};
                std::tie(distance_rate, expected_value) =
                    GENERATE(table<double, double>({
                        std::make_tuple(1e-6, -3.0e-6),
                        std::make_tuple(0.1, -0.3),
                        std::make_tuple(0.9, -2.7),
                        std::make_tuple(1.0, -3.0),
                        std::make_tuple(1.23, -3.69),
                        std::make_tuple(7.89, -23.67),
                    }));
                INFO("distance_rate = " << distance_rate);

                const double actual_value = differentiated_rbf(distance_rate);
                CHECK_THAT(actual_value,
                    Catch::Matchers::WithinAbs(expected_value,
                        std::max(abs_tol, std::abs(expected_value) * rel_tol)));
            }
            SECTION("calculate for zero") {
                constexpr double distance_rate = 0.0;
                constexpr double expected_value = 0.0;

                const double actual_value = differentiated_rbf(distance_rate);
                CHECK_THAT(actual_value,
                    Catch::Matchers::WithinAbs(expected_value, abs_tol));
            }
        }

        SECTION("Degree 4") {
            const differentiated_t<polyharmonic_spline_rbf<double, 4>>
                differentiated_rbf;

            SECTION("ordinary cases") {
                double distance_rate{};
                double expected_value{};
                std::tie(distance_rate, expected_value) =
                    GENERATE(table<double, double>({
                        std::make_tuple(1e-6, 5.42620422318571e-11),
                        std::make_tuple(0.1, 0.08210340371976183),
                        std::make_tuple(0.9, -0.4686319292686428),
                        std::make_tuple(1.0, -1.0),
                        std::make_tuple(1.23, -2.765666947446188),
                        std::make_tuple(7.89, -576.6028885871207),
                    }));
                INFO("distance_rate = " << distance_rate);

                const double actual_value = differentiated_rbf(distance_rate);
                CHECK_THAT(actual_value,
                    Catch::Matchers::WithinAbs(expected_value,
                        std::max(abs_tol, std::abs(expected_value) * rel_tol)));
            }
            SECTION("calculate for zero") {
                constexpr double distance_rate = 0.0;
                constexpr double expected_value = 0.0;

                const double actual_value = differentiated_rbf(distance_rate);
                CHECK_THAT(actual_value,
                    Catch::Matchers::WithinAbs(expected_value, abs_tol));
            }
        }

        SECTION("Degree 5") {
            const differentiated_t<polyharmonic_spline_rbf<double, 5>>
                differentiated_rbf;

            SECTION("ordinary cases") {
                double distance_rate{};
                double expected_value{};
                std::tie(distance_rate, expected_value) =
                    GENERATE(table<double, double>({
                        std::make_tuple(1e-6, -5.0e-18),
                        std::make_tuple(0.1, -0.005),
                        std::make_tuple(0.9, -3.645),
                        std::make_tuple(1.0, -5.0),
                        std::make_tuple(1.23, -9.304335),
                        std::make_tuple(7.89, -2455.845345),
                    }));
                INFO("distance_rate = " << distance_rate);

                const double actual_value = differentiated_rbf(distance_rate);
                CHECK_THAT(actual_value,
                    Catch::Matchers::WithinAbs(expected_value,
                        std::max(abs_tol, std::abs(expected_value) * rel_tol)));
            }
            SECTION("calculate for zero") {
                constexpr double distance_rate = 0.0;
                constexpr double expected_value = 0.0;

                const double actual_value = differentiated_rbf(distance_rate);
                CHECK_THAT(actual_value,
                    Catch::Matchers::WithinAbs(expected_value, abs_tol));
            }
        }

        SECTION("Degree 6") {
            const differentiated_t<polyharmonic_spline_rbf<double, 6>>
                differentiated_rbf;

            SECTION("ordinary cases") {
                double distance_rate{};
                double expected_value{};
                std::tie(distance_rate, expected_value) =
                    GENERATE(table<double, double>({
                        std::make_tuple(1e-6, 8.189306334778564e-23),
                        std::make_tuple(0.1, 0.001281551055796427),
                        std::make_tuple(0.9, -0.241337794061401),
                        std::make_tuple(1.0, -1.0),
                        std::make_tuple(1.23, -5.131833082187007),
                        std::make_tuple(7.89, -51904.44904371645),
                    }));
                INFO("distance_rate = " << distance_rate);

                const double actual_value = differentiated_rbf(distance_rate);
                CHECK_THAT(actual_value,
                    Catch::Matchers::WithinAbs(expected_value,
                        std::max(abs_tol, std::abs(expected_value) * rel_tol)));
            }
            SECTION("calculate for zero") {
                constexpr double distance_rate = 0.0;
                constexpr double expected_value = 0.0;

                const double actual_value = differentiated_rbf(distance_rate);
                CHECK_THAT(actual_value,
                    Catch::Matchers::WithinAbs(expected_value, abs_tol));
            }
        }

        SECTION("Degree 7") {
            const differentiated_t<polyharmonic_spline_rbf<double, 7>>
                differentiated_rbf;

            SECTION("ordinary cases") {
                double distance_rate{};
                double expected_value{};
                std::tie(distance_rate, expected_value) =
                    GENERATE(table<double, double>({
                        std::make_tuple(1e-6, -7.0e-30),
                        std::make_tuple(0.1, -7.0e-5),
                        std::make_tuple(0.9, -4.13343),
                        std::make_tuple(1.0, -7.0),
                        std::make_tuple(1.23, -19.7071397901),
                        std::make_tuple(7.89, -214034.1420020643),
                    }));
                INFO("distance_rate = " << distance_rate);

                const double actual_value = differentiated_rbf(distance_rate);
                CHECK_THAT(actual_value,
                    Catch::Matchers::WithinAbs(expected_value,
                        std::max(abs_tol, std::abs(expected_value) * rel_tol)));
            }
            SECTION("calculate for zero") {
                constexpr double distance_rate = 0.0;
                constexpr double expected_value = 0.0;

                const double actual_value = differentiated_rbf(distance_rate);
                CHECK_THAT(actual_value,
                    Catch::Matchers::WithinAbs(expected_value, abs_tol));
            }
        }

        SECTION("Degree 8") {
            const differentiated_t<polyharmonic_spline_rbf<double, 8>>
                differentiated_rbf;

            SECTION("ordinary cases") {
                double distance_rate{};
                double expected_value{};
                std::tie(distance_rate, expected_value) =
                    GENERATE(table<double, double>({
                        std::make_tuple(1e-6, 1.095240844637142e-34),
                        std::make_tuple(0.1, 1.742068074395237e-5),
                        std::make_tuple(0.9, -0.08349781758631306),
                        std::make_tuple(1.0, -1.0),
                        std::make_tuple(1.23, -9.19765836282463),
                        std::make_tuple(7.89, -4227798.918305012),
                    }));
                INFO("distance_rate = " << distance_rate);

                const double actual_value = differentiated_rbf(distance_rate);
                CHECK_THAT(actual_value,
                    Catch::Matchers::WithinAbs(expected_value,
                        std::max(abs_tol, std::abs(expected_value) * rel_tol)));
            }
            SECTION("calculate for zero") {
                constexpr double distance_rate = 0.0;
                constexpr double expected_value = 0.0;

                const double actual_value = differentiated_rbf(distance_rate);
                CHECK_THAT(actual_value,
                    Catch::Matchers::WithinAbs(expected_value, abs_tol));
            }
        }
    }

    SECTION("calculate second-order derivatives of RBF") {
        SECTION("Degree 3") {
            const differentiated_t<
                differentiated_t<polyharmonic_spline_rbf<double, 3>>>
                differentiated_rbf;

            SECTION("ordinary cases") {
                double distance_rate{};
                double expected_value{};
                std::tie(distance_rate, expected_value) =
                    GENERATE(table<double, double>({
                        std::make_tuple(1e-6, 3000000.0),
                        std::make_tuple(0.1, 30.0),
                        std::make_tuple(0.9, 3.333333333333333),
                        std::make_tuple(1.0, 3.0),
                        std::make_tuple(1.23, 2.439024390243902),
                        std::make_tuple(7.89, 0.3802281368821293),
                    }));
                INFO("distance_rate = " << distance_rate);

                const double actual_value = differentiated_rbf(distance_rate);
                CHECK_THAT(actual_value,
                    Catch::Matchers::WithinAbs(expected_value,
                        std::max(abs_tol, std::abs(expected_value) * rel_tol)));
            }
            SECTION("calculate for zero") {
                constexpr double distance_rate = 0.0;

                const double actual_value = differentiated_rbf(distance_rate);
                CHECK_THAT(actual_value, is_finite());
            }
        }

        SECTION("Degree 4") {
            const differentiated_t<
                differentiated_t<polyharmonic_spline_rbf<double, 4>>>
                differentiated_rbf;

            SECTION("ordinary cases") {
                double distance_rate{};
                double expected_value{};
                std::tie(distance_rate, expected_value) =
                    GENERATE(table<double, double>({
                        std::make_tuple(1e-6, -104.5240844637142),
                        std::make_tuple(0.1, -12.42068074395237),
                        std::make_tuple(0.9, 5.15711587473739),
                        std::make_tuple(1.0, 6.0),
                        std::make_tuple(1.23, 7.656113355074609),
                        std::make_tuple(7.89, 22.52476907886226),
                    }));
                INFO("distance_rate = " << distance_rate);

                const double actual_value = differentiated_rbf(distance_rate);
                CHECK_THAT(actual_value,
                    Catch::Matchers::WithinAbs(expected_value,
                        std::max(abs_tol, std::abs(expected_value) * rel_tol)));
            }
            SECTION("calculate for zero") {
                constexpr double distance_rate = 0.0;

                const double actual_value = differentiated_rbf(distance_rate);
                CHECK_THAT(actual_value, is_finite());
            }
        }

        SECTION("Degree 5") {
            const differentiated_t<
                differentiated_t<polyharmonic_spline_rbf<double, 5>>>
                differentiated_rbf;

            SECTION("ordinary cases") {
                double distance_rate{};
                double expected_value{};
                std::tie(distance_rate, expected_value) =
                    GENERATE(table<double, double>({
                        std::make_tuple(1e-6, 1.5e-5),
                        std::make_tuple(0.1, 1.5),
                        std::make_tuple(0.9, 13.5),
                        std::make_tuple(1.0, 15.0),
                        std::make_tuple(1.23, 18.45),
                        std::make_tuple(7.89, 118.35),
                    }));
                INFO("distance_rate = " << distance_rate);

                const double actual_value = differentiated_rbf(distance_rate);
                CHECK_THAT(actual_value,
                    Catch::Matchers::WithinAbs(expected_value,
                        std::max(abs_tol, std::abs(expected_value) * rel_tol)));
            }
            SECTION("calculate for zero") {
                constexpr double distance_rate = 0.0;
                constexpr double expected_value = 0.0;

                const double actual_value = differentiated_rbf(distance_rate);
                CHECK_THAT(actual_value,
                    Catch::Matchers::WithinAbs(expected_value, abs_tol));
            }
        }

        SECTION("Degree 6") {
            const differentiated_t<
                differentiated_t<polyharmonic_spline_rbf<double, 6>>>
                differentiated_rbf;

            SECTION("ordinary cases") {
                double distance_rate{};
                double expected_value{};
                std::tie(distance_rate, expected_value) =
                    GENERATE(table<double, double>({
                        std::make_tuple(1e-6, -3.215722533911426e-10),
                        std::make_tuple(0.1, -0.452620422318571),
                        std::make_tuple(0.9, 6.051791575611857),
                        std::make_tuple(1.0, 10.0),
                        std::make_tuple(1.23, 22.64560168467713),
                        std::make_tuple(7.89, 3708.625731522724),
                    }));
                INFO("distance_rate = " << distance_rate);

                const double actual_value = differentiated_rbf(distance_rate);
                CHECK_THAT(actual_value,
                    Catch::Matchers::WithinAbs(expected_value,
                        std::max(abs_tol, std::abs(expected_value) * rel_tol)));
            }
            SECTION("calculate for zero") {
                constexpr double distance_rate = 0.0;
                constexpr double expected_value = 0.0;

                const double actual_value = differentiated_rbf(distance_rate);
                CHECK_THAT(actual_value,
                    Catch::Matchers::WithinAbs(expected_value, abs_tol));
            }
        }

        SECTION("Degree 7") {
            const differentiated_t<
                differentiated_t<polyharmonic_spline_rbf<double, 7>>>
                differentiated_rbf;

            SECTION("ordinary cases") {
                double distance_rate{};
                double expected_value{};
                std::tie(distance_rate, expected_value) =
                    GENERATE(table<double, double>({
                        std::make_tuple(1e-6, 3.5e-17),
                        std::make_tuple(0.1, 0.035),
                        std::make_tuple(0.9, 25.515),
                        std::make_tuple(1.0, 35.0),
                        std::make_tuple(1.23, 65.130345),
                        std::make_tuple(7.89, 17190.917415),
                    }));
                INFO("distance_rate = " << distance_rate);

                const double actual_value = differentiated_rbf(distance_rate);
                CHECK_THAT(actual_value,
                    Catch::Matchers::WithinAbs(expected_value,
                        std::max(abs_tol, std::abs(expected_value) * rel_tol)));
            }
            SECTION("calculate for zero") {
                constexpr double distance_rate = 0.0;
                constexpr double expected_value = 0.0;

                const double actual_value = differentiated_rbf(distance_rate);
                CHECK_THAT(actual_value,
                    Catch::Matchers::WithinAbs(expected_value, abs_tol));
            }
        }

        SECTION("Degree 8") {
            const differentiated_t<
                differentiated_t<polyharmonic_spline_rbf<double, 8>>>
                differentiated_rbf;

            SECTION("ordinary cases") {
                double distance_rate{};
                double expected_value{};
                std::tie(distance_rate, expected_value) =
                    GENERATE(table<double, double>({
                        std::make_tuple(1e-6, -6.491445067822852e-22),
                        std::make_tuple(0.1, -0.009652408446371419),
                        std::make_tuple(0.9, 5.867302352491208),
                        std::make_tuple(1.0, 14.0),
                        std::make_tuple(1.23, 54.78786311749605),
                        std::make_tuple(7.89, 438487.5360761916),
                    }));
                INFO("distance_rate = " << distance_rate);

                const double actual_value = differentiated_rbf(distance_rate);
                CHECK_THAT(actual_value,
                    Catch::Matchers::WithinAbs(expected_value,
                        std::max(abs_tol, std::abs(expected_value) * rel_tol)));
            }
            SECTION("calculate for zero") {
                constexpr double distance_rate = 0.0;
                constexpr double expected_value = 0.0;

                const double actual_value = differentiated_rbf(distance_rate);
                CHECK_THAT(actual_value,
                    Catch::Matchers::WithinAbs(expected_value, abs_tol));
            }
        }
    }

    SECTION("calculate third-order derivatives of RBF") {
        SECTION("Degree 5") {
            const differentiated_t<differentiated_t<
                differentiated_t<polyharmonic_spline_rbf<double, 5>>>>
                differentiated_rbf;

            SECTION("ordinary cases") {
                double distance_rate{};
                double expected_value{};
                std::tie(distance_rate, expected_value) =
                    GENERATE(table<double, double>({
                        std::make_tuple(1e-6, -15000000.0),
                        std::make_tuple(0.1, -150.0),
                        std::make_tuple(0.9, -16.66666666666667),
                        std::make_tuple(1.0, -15.0),
                        std::make_tuple(1.23, -12.19512195121951),
                        std::make_tuple(7.89, -1.901140684410646),
                    }));
                INFO("distance_rate = " << distance_rate);

                const double actual_value = differentiated_rbf(distance_rate);
                CHECK_THAT(actual_value,
                    Catch::Matchers::WithinAbs(expected_value,
                        std::max(abs_tol, std::abs(expected_value) * rel_tol)));
            }
            SECTION("calculate for zero") {
                constexpr double distance_rate = 0.0;

                const double actual_value = differentiated_rbf(distance_rate);
                CHECK_THAT(actual_value, is_finite());
            }
        }

        SECTION("Degree 7") {
            const differentiated_t<differentiated_t<
                differentiated_t<polyharmonic_spline_rbf<double, 7>>>>
                differentiated_rbf;

            SECTION("ordinary cases") {
                double distance_rate{};
                double expected_value{};
                std::tie(distance_rate, expected_value) =
                    GENERATE(table<double, double>({
                        std::make_tuple(1e-6, -0.000105),
                        std::make_tuple(0.1, -10.5),
                        std::make_tuple(0.9, -94.5),
                        std::make_tuple(1.0, -105.0),
                        std::make_tuple(1.23, -129.15),
                        std::make_tuple(7.89, -828.45),
                    }));
                INFO("distance_rate = " << distance_rate);

                const double actual_value = differentiated_rbf(distance_rate);
                CHECK_THAT(actual_value,
                    Catch::Matchers::WithinAbs(expected_value,
                        std::max(abs_tol, std::abs(expected_value) * rel_tol)));
            }
            SECTION("calculate for zero") {
                constexpr double distance_rate = 0.0;
                constexpr double expected_value = 0.0;

                const double actual_value = differentiated_rbf(distance_rate);
                CHECK_THAT(actual_value,
                    Catch::Matchers::WithinAbs(expected_value, abs_tol));
            }
        }
    }

    SECTION("calculate fourth-order derivatives of RBF") {
        SECTION("Degree 7") {
            const differentiated_t<differentiated_t<differentiated_t<
                differentiated_t<polyharmonic_spline_rbf<double, 7>>>>>
                differentiated_rbf;

            SECTION("ordinary cases") {
                double distance_rate{};
                double expected_value{};
                std::tie(distance_rate, expected_value) =
                    GENERATE(table<double, double>({
                        std::make_tuple(1e-6, 105000000.0),
                        std::make_tuple(0.1, 1050.0),
                        std::make_tuple(0.9, 116.6666666666667),
                        std::make_tuple(1.0, 105.0),
                        std::make_tuple(1.23, 85.36585365853659),
                        std::make_tuple(7.89, 13.30798479087452),
                    }));
                INFO("distance_rate = " << distance_rate);

                const double actual_value = differentiated_rbf(distance_rate);
                CHECK_THAT(actual_value,
                    Catch::Matchers::WithinAbs(expected_value,
                        std::max(abs_tol, std::abs(expected_value) * rel_tol)));
            }
            SECTION("calculate for zero") {
                constexpr double distance_rate = 0.0;

                const double actual_value = differentiated_rbf(distance_rate);
                CHECK_THAT(actual_value, is_finite());
            }
        }

        SECTION("Degree 9") {
            const differentiated_t<differentiated_t<differentiated_t<
                differentiated_t<polyharmonic_spline_rbf<double, 9>>>>>
                differentiated_rbf;

            SECTION("ordinary cases") {
                double distance_rate{};
                double expected_value{};
                std::tie(distance_rate, expected_value) =
                    GENERATE(table<double, double>({
                        std::make_tuple(1e-6, 0.000945),
                        std::make_tuple(0.1, 94.5),
                        std::make_tuple(0.9, 850.5),
                        std::make_tuple(1.0, 945.0),
                        std::make_tuple(1.23, 1162.35),
                        std::make_tuple(7.89, 7456.05),
                    }));
                INFO("distance_rate = " << distance_rate);

                const double actual_value = differentiated_rbf(distance_rate);
                CHECK_THAT(actual_value,
                    Catch::Matchers::WithinAbs(expected_value,
                        std::max(abs_tol, std::abs(expected_value) * rel_tol)));
            }
            SECTION("calculate for zero") {
                constexpr double distance_rate = 0.0;
                constexpr double expected_value = 0.0;

                const double actual_value = differentiated_rbf(distance_rate);
                CHECK_THAT(actual_value,
                    Catch::Matchers::WithinAbs(expected_value, abs_tol));
            }
        }
    }
}
