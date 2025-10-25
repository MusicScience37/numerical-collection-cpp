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
 * \brief Test of pointer_iterator class.
 */
#include "num_collect/util/impl/pointer_iterator.h"

#include <array>
#include <type_traits>

#include <catch2/catch_test_macros.hpp>

TEST_CASE("num_collect::util::impl::pointer_iterator(non-const)") {
    using num_collect::util::impl::pointer_iterator;

    SECTION("check types") {
        using iterator_type = pointer_iterator<int*>;
        STATIC_REQUIRE(std::is_same_v<typename iterator_type::pointer, int*>);
        STATIC_REQUIRE(std::is_same_v<typename iterator_type::difference_type,
            std::ptrdiff_t>);
        STATIC_REQUIRE(std::is_same_v<typename iterator_type::value_type, int>);
        STATIC_REQUIRE(std::is_same_v<typename iterator_type::reference, int&>);
        STATIC_REQUIRE(std::is_same_v<typename iterator_type::iterator_category,
            std::random_access_iterator_tag>);
        STATIC_REQUIRE(std::is_same_v<typename iterator_type::iterator_concept,
            std::contiguous_iterator_tag>);
    }

    SECTION("satisfies legacy iterator requirements") {
        using iterator_type = pointer_iterator<int*>;
        STATIC_REQUIRE(std::is_copy_constructible_v<iterator_type>);
        STATIC_REQUIRE(std::is_copy_assignable_v<iterator_type>);
        STATIC_REQUIRE(std::is_destructible_v<iterator_type>);
        STATIC_REQUIRE(std::is_swappable_v<iterator_type>);

        SECTION("dereferenceable") {
            int value = 123;
            iterator_type iterator(&value);
            CHECK(*iterator == 123);
        }

        SECTION("incrementable") {
            std::array<int, 3> array = {1, 2, 3};
            iterator_type iterator(array.data());

            CHECK(*iterator == 1);
            CHECK(*(++iterator) == 2);
            CHECK(*(iterator++) == 2);
            CHECK(*iterator == 3);
        }
    }

    SECTION("satisfies legacy input iterator requirements") {
        using iterator_type = pointer_iterator<std::string*>;

        // requires legacy iterator requirements tested above

        SECTION("equality comparable") {
            std::array<std::string, 3> array = {"1", "2", "3"};
            iterator_type iterator1(array.data());
            iterator_type iterator2(array.data());

            CHECK(iterator1 == iterator2);
            ++iterator1;
            CHECK(iterator1 != iterator2);
            ++iterator2;
            CHECK(iterator1 == iterator2);
        }

        SECTION("access to member functions") {
            std::string str = "abc";
            iterator_type iterator(&str);

            CHECK(iterator->size() == 3);
            CHECK_NOTHROW(iterator->push_back('d'));
            CHECK(*iterator == "abcd");
        }
    }

    SECTION("satisfy legacy output iterator requirements") {
        using iterator_type = pointer_iterator<int*>;

        // requires legacy iterator requirements tested above

        SECTION("writable") {
            int value = 0;
            iterator_type iterator(&value);

            *iterator = 123;

            CHECK(value == 123);
        }
    }

    SECTION("satisfy legacy forward iterator requirements") {
        using iterator_type = pointer_iterator<int*>;

        // requires legacy input/output iterator requirements tested above

        SECTION("multi-pass") {
            std::array<int, 3> array = {1, 2, 3};
            iterator_type iterator1(array.data());
            iterator_type iterator2 = iterator1;

            CHECK(*iterator1 == 1);
            CHECK(*iterator2 == 1);
            ++iterator1;
            CHECK(*iterator1 == 2);
            CHECK(*iterator2 == 1);
            ++iterator2;
            CHECK(*iterator1 == 2);
            CHECK(*iterator2 == 2);
        }
    }

    SECTION("satisfy legacy bidirectional iterator requirements") {
        using iterator_type = pointer_iterator<int*>;

        // requires legacy forward iterator requirements tested above

        SECTION("decrementable") {
            std::array<int, 3> array = {1, 2, 3};
            iterator_type iterator(array.data() + 3);

            CHECK(*(--iterator) == 3);
            CHECK(*(iterator--) == 3);
            CHECK(*iterator == 2);
        }
    }

    SECTION("satisfy legacy random access iterator requirements") {
        using iterator_type = pointer_iterator<int*>;

        // requires legacy bidirectional iterator requirements tested above

        SECTION("add difference") {
            std::array<int, 3> array = {1, 2, 3};
            iterator_type iterator(array.data());

            CHECK(*(iterator + 2) == 3);
            CHECK(*(2 + iterator) == 3);
            CHECK(*(iterator += 2) == 3);
        }

        SECTION("subtract difference") {
            std::array<int, 3> array = {1, 2, 3};
            iterator_type iterator(array.data() + 3);

            CHECK(*(iterator - 2) == 2);
            CHECK(*(iterator -= 2) == 2);
        }

        SECTION("calculate distance") {
            std::array<int, 3> array = {1, 2, 3};
            iterator_type iterator1(array.data());
            iterator_type iterator2(array.data() + 3);

            CHECK((iterator2 - iterator1) == 3);
            CHECK((iterator1 - iterator2) == -3);
        }

        SECTION("access with offset") {
            std::array<int, 3> array = {1, 2, 3};
            iterator_type iterator(array.data());

            CHECK(iterator[0] == 1);
            CHECK(iterator[1] == 2);
            CHECK(iterator[2] == 3);

            iterator[1] = 4;  // NOLINT

            CHECK(array[1] == 4);
        }

        SECTION("compare order") {
            std::array<int, 3> array = {1, 2, 3};
            iterator_type iterator1(array.data());
            iterator_type iterator2(array.data() + 1);

            CHECK(iterator1 < iterator2);
            CHECK(iterator1 <= iterator2);
            CHECK(iterator2 > iterator1);
            CHECK(iterator2 >= iterator1);

            ++iterator1;

            CHECK(iterator1 == iterator2);
            CHECK_FALSE(iterator1 < iterator2);
            CHECK(iterator1 <= iterator2);
            CHECK_FALSE(iterator1 > iterator2);
            CHECK(iterator1 >= iterator2);
        }
    }

    SECTION("check concepts") {
        using iterator_type = pointer_iterator<int*>;
        STATIC_REQUIRE(std::input_or_output_iterator<iterator_type>);
        STATIC_REQUIRE(std::input_iterator<iterator_type>);
        STATIC_REQUIRE(std::output_iterator<iterator_type, const int&>);
        STATIC_REQUIRE(std::forward_iterator<iterator_type>);
        STATIC_REQUIRE(std::bidirectional_iterator<iterator_type>);
        STATIC_REQUIRE(std::random_access_iterator<iterator_type>);
        STATIC_REQUIRE(std::contiguous_iterator<iterator_type>);
    }
}

TEST_CASE("num_collect::util::impl::pointer_iterator(const)") {
    using num_collect::util::impl::pointer_iterator;

    SECTION("check types") {
        using iterator_type = pointer_iterator<const int*>;
        STATIC_REQUIRE(
            std::is_same_v<typename iterator_type::pointer, const int*>);
        STATIC_REQUIRE(std::is_same_v<typename iterator_type::difference_type,
            std::ptrdiff_t>);
        STATIC_REQUIRE(std::is_same_v<typename iterator_type::value_type, int>);
        STATIC_REQUIRE(
            std::is_same_v<typename iterator_type::reference, const int&>);
        STATIC_REQUIRE(std::is_same_v<typename iterator_type::iterator_category,
            std::random_access_iterator_tag>);
        STATIC_REQUIRE(std::is_same_v<typename iterator_type::iterator_concept,
            std::contiguous_iterator_tag>);
    }

    SECTION("satisfies legacy iterator requirements") {
        using iterator_type = pointer_iterator<const int*>;
        STATIC_REQUIRE(std::is_copy_constructible_v<iterator_type>);
        STATIC_REQUIRE(std::is_copy_assignable_v<iterator_type>);
        STATIC_REQUIRE(std::is_destructible_v<iterator_type>);
        STATIC_REQUIRE(std::is_swappable_v<iterator_type>);

        SECTION("dereferenceable") {
            int value = 123;
            iterator_type iterator(&value);
            CHECK(*iterator == 123);
        }

        SECTION("incrementable") {
            std::array<int, 3> array = {1, 2, 3};
            iterator_type iterator(array.data());

            CHECK(*iterator == 1);
            CHECK(*(++iterator) == 2);
            CHECK(*(iterator++) == 2);
            CHECK(*iterator == 3);
        }
    }

    SECTION("satisfies legacy input iterator requirements") {
        using iterator_type = pointer_iterator<const std::string*>;

        // requires legacy iterator requirements tested above

        SECTION("equality comparable") {
            std::array<std::string, 3> array = {"1", "2", "3"};
            iterator_type iterator1(array.data());
            iterator_type iterator2(array.data());

            CHECK(iterator1 == iterator2);
            ++iterator1;
            CHECK(iterator1 != iterator2);
            ++iterator2;
            CHECK(iterator1 == iterator2);
        }

        SECTION("access to member functions") {
            std::string str = "abc";
            iterator_type iterator(&str);

            CHECK(iterator->size() == 3);
        }
    }

    SECTION("satisfy legacy forward iterator requirements") {
        using iterator_type = pointer_iterator<const int*>;

        // requires legacy input iterator requirements tested above

        SECTION("multi-pass") {
            std::array<int, 3> array = {1, 2, 3};
            iterator_type iterator1(array.data());
            iterator_type iterator2 = iterator1;

            CHECK(*iterator1 == 1);
            CHECK(*iterator2 == 1);
            ++iterator1;
            CHECK(*iterator1 == 2);
            CHECK(*iterator2 == 1);
            ++iterator2;
            CHECK(*iterator1 == 2);
            CHECK(*iterator2 == 2);
        }
    }

    SECTION("satisfy legacy bidirectional iterator requirements") {
        using iterator_type = pointer_iterator<const int*>;

        // requires legacy forward iterator requirements tested above

        SECTION("decrementable") {
            std::array<int, 3> array = {1, 2, 3};
            iterator_type iterator(array.data() + 3);

            CHECK(*(--iterator) == 3);
            CHECK(*(iterator--) == 3);
            CHECK(*iterator == 2);
        }
    }

    SECTION("satisfy legacy random access iterator requirements") {
        using iterator_type = pointer_iterator<const int*>;

        // requires legacy bidirectional iterator requirements tested above

        SECTION("add difference") {
            std::array<int, 3> array = {1, 2, 3};
            iterator_type iterator(array.data());

            CHECK(*(iterator + 2) == 3);
            CHECK(*(2 + iterator) == 3);
            CHECK(*(iterator += 2) == 3);
        }

        SECTION("subtract difference") {
            std::array<int, 3> array = {1, 2, 3};
            iterator_type iterator(array.data() + 3);

            CHECK(*(iterator - 2) == 2);
            CHECK(*(iterator -= 2) == 2);
        }

        SECTION("calculate distance") {
            std::array<int, 3> array = {1, 2, 3};
            iterator_type iterator1(array.data());
            iterator_type iterator2(array.data() + 3);

            CHECK((iterator2 - iterator1) == 3);
            CHECK((iterator1 - iterator2) == -3);
        }

        SECTION("access with offset") {
            std::array<int, 3> array = {1, 2, 3};
            iterator_type iterator(array.data());

            CHECK(iterator[0] == 1);
            CHECK(iterator[1] == 2);
            CHECK(iterator[2] == 3);
        }

        SECTION("compare order") {
            std::array<int, 3> array = {1, 2, 3};
            iterator_type iterator1(array.data());
            iterator_type iterator2(array.data() + 1);

            CHECK(iterator1 < iterator2);
            CHECK(iterator1 <= iterator2);
            CHECK(iterator2 > iterator1);
            CHECK(iterator2 >= iterator1);

            ++iterator1;

            CHECK(iterator1 == iterator2);
            CHECK_FALSE(iterator1 < iterator2);
            CHECK(iterator1 <= iterator2);
            CHECK_FALSE(iterator1 > iterator2);
            CHECK(iterator1 >= iterator2);
        }
    }

    SECTION("check concepts") {
        using iterator_type = pointer_iterator<const int*>;
        STATIC_REQUIRE(std::input_or_output_iterator<iterator_type>);
        STATIC_REQUIRE(std::input_iterator<iterator_type>);
        STATIC_REQUIRE(std::forward_iterator<iterator_type>);
        STATIC_REQUIRE(std::bidirectional_iterator<iterator_type>);
        STATIC_REQUIRE(std::random_access_iterator<iterator_type>);
        STATIC_REQUIRE(std::contiguous_iterator<iterator_type>);
    }
}
