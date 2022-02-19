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
 * \brief Definition of macros for built-in functions of compilers.
 */
#pragma once

/* ***********************************************************************
 * NUM_COLLECT_HAS_MSVC_SOURCE_LOCATION macro.
 * ***********************************************************************/

#if defined(NUM_COLLECT_DOCUMENTATION)

/*!
 * \brief Whether this environment is MSVC and has source location support.
 */
#define NUM_COLLECT_HAS_MSVC_SOURCE_LOCATION 1

#elif defined(_MSC_VER)

// on MSVC
#if _MSC_VER >= 1929
// Visual Studio 2019 16.10 and later supports source location.
#define NUM_COLLECT_HAS_MSVC_SOURCE_LOCATION 1
#else
#define NUM_COLLECT_HAS_MSVC_SOURCE_LOCATION 0
#endif

#else

// Other than MSVC.
#define NUM_COLLECT_HAS_MSVC_SOURCE_LOCATION 0

#endif

/* ***********************************************************************
 * NUM_COLLECT_HAS_BUILTIN macro.
 * ***********************************************************************/

#if defined(NUM_COLLECT_DOCUMENTATION)

/*!
 * \brief Check existence of a builtins.
 *
 * \note This macro simply returns zero for platforms without `__has_builtin`.
 *
 * \param[in] NAME Name of builtin.
 */
#define NUM_COLLECT_HAS_BUILTIN(NAME) __has_builtin(NAME)

#elif defined(__has_builtin)
#define NUM_COLLECT_HAS_BUILTIN(NAME) __has_builtin(NAME)
#else
#define NUM_COLLECT_HAS_BUILTIN(NAME) 0
#endif

/* ***********************************************************************
 * NUM_COLLECT_HAS_BUILTIN_FILE, NUM_COLLECT_BUILTIN_FILE macros.
 * ***********************************************************************/

#if defined(NUM_COLLECT_DOCUMENTATION)

//! Whether NUM_COLLECT_BUILTIN_FILE returns correct file paths.
#define NUM_COLLECT_HAS_BUILTIN_FILE 1

/*!
 * \brief Get file path.
 *
 * \return File path.
 */
#define NUM_COLLECT_BUILTIN_FILE() __builtin_FILE()

#elif NUM_COLLECT_HAS_BUILTIN(__builtin_FILE) || \
    NUM_COLLECT_HAS_MSVC_SOURCE_LOCATION
#define NUM_COLLECT_HAS_BUILTIN_FILE 1
#define NUM_COLLECT_BUILTIN_FILE() __builtin_FILE()
#else
#define NUM_COLLECT_HAS_BUILTIN_FILE 0
#define NUM_COLLECT_BUILTIN_FILE() static_cast<const char*>("")
#endif

/* ***********************************************************************
 * NUM_COLLECT_HAS_BUILTIN_FUNCTION, NUM_COLLECT_BUILTIN_FUNCTION macros.
 * ***********************************************************************/

#if defined(NUM_COLLECT_DOCUMENTATION)

//! Whether NUM_COLLECT_BUILTIN_FUNCTION returns correct function name.
#define NUM_COLLECT_HAS_BUILTIN_FUNCTION 1

/*!
 * \brief Get function name.
 *
 * \return Function name.
 */
#define NUM_COLLECT_BUILTIN_FUNCTION() __builtin_FUNCTION()

#elif NUM_COLLECT_HAS_BUILTIN(__builtin_FUNCTION) || \
    NUM_COLLECT_HAS_MSVC_SOURCE_LOCATION
#define NUM_COLLECT_HAS_BUILTIN_FUNCTION 1
#define NUM_COLLECT_BUILTIN_FUNCTION() __builtin_FUNCTION()
#else
#define NUM_COLLECT_HAS_BUILTIN_FUNCTION 0
#define NUM_COLLECT_BUILTIN_FUNCTION() static_cast<const char*>("")
#endif

/* ***********************************************************************
 * NUM_COLLECT_HAS_BUILTIN_LINE, NUM_COLLECT_BUILTIN_LINE macros.
 * ***********************************************************************/

#if defined(NUM_COLLECT_DOCUMENTATION)

//! Whether NUM_COLLECT_HAS_BUILTIN_LINE returns correct line number.
#define NUM_COLLECT_HAS_BUILTIN_LINE 1

/*!
 * \brief Get line number.
 *
 * \return Line number.
 */
#define NUM_COLLECT_BUILTIN_LINE() __builtin_LINE()

#elif NUM_COLLECT_HAS_BUILTIN(__builtin_LINE) || \
    NUM_COLLECT_HAS_MSVC_SOURCE_LOCATION
#define NUM_COLLECT_HAS_BUILTIN_LINE 1
#define NUM_COLLECT_BUILTIN_LINE() __builtin_LINE()
#else
#define NUM_COLLECT_HAS_BUILTIN_LINE 0
#define NUM_COLLECT_BUILTIN_LINE() 0
#endif

/* ***********************************************************************
 * NUM_COLLECT_HAS_BUILTIN_COLUMN, NUM_COLLECT_BUILTIN_COLUMN macros.
 * ***********************************************************************/

#if defined(NUM_COLLECT_DOCUMENTATION)

//! Whether NUM_COLLECT_BUILTIN_COLUMN returns correct column number.
#define NUM_COLLECT_HAS_BUILTIN_COLUMN 1

/*!
 * \brief Get column number.
 *
 * \return column number.
 */
#define NUM_COLLECT_BUILTIN_COLUMN() __builtin_COLUMN()

#elif NUM_COLLECT_HAS_BUILTIN(__builtin_COLUMN) || \
    NUM_COLLECT_HAS_MSVC_SOURCE_LOCATION
#define NUM_COLLECT_HAS_BUILTIN_COLUMN 1
#define NUM_COLLECT_BUILTIN_COLUMN() __builtin_COLUMN()
#else
#define NUM_COLLECT_HAS_BUILTIN_COLUMN 0
#define NUM_COLLECT_BUILTIN_COLUMN() 0
#endif
