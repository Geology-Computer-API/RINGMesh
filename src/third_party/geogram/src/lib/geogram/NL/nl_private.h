/*
 *  Copyright (c) 2004-2010, Bruno Levy
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *
 *  * Redistributions of source code must retain the above copyright notice,
 *  this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright notice,
 *  this list of conditions and the following disclaimer in the documentation
 *  and/or other materials provided with the distribution.
 *  * Neither the name of the ALICE Project-Team nor the names of its
 *  contributors may be used to endorse or promote products derived from this
 *  software without specific prior written permission.
 * 
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *
 *  If you modify this software, you should include a notice giving the
 *  name of the person performing the modification, the date of modification,
 *  and the reason for such modification.
 *
 *  Contact: Bruno Levy
 *
 *     levy@loria.fr
 *
 *     ALICE Project
 *     LORIA, INRIA Lorraine, 
 *     Campus Scientifique, BP 239
 *     54506 VANDOEUVRE LES NANCY CEDEX 
 *     FRANCE
 *
 */

#ifndef __NL_PRIVATE__
#define __NL_PRIVATE__

#include "nl.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

/**
 * \file geogram/NL/nl_private.h
 * \brief Some macros and functions used internally by OpenNL.
 */


/**
 * \brief Suppresses unsused argument warnings
 * \details Some callbacks do not necessary use all their
 *  arguments.
 * \param[in] x the argument to be tagged as used
 */
#define nl_arg_used(x) (void)x

/**
 * \brief A "brute force" cast operator
 * \note Use only if you know exactly what you are doing
 * \param[in] T the new type 
 * \param[in] x the variable to be casted
 */
#define nl_cast(T,x)   (*(T*)&(x))


/**
 * \name Assertion checks
 * @{ 
 */

/**
 * \brief Displays an error message and aborts the program when
 *  an assertion failed.
 * \details Called by nl_assert() whenever the assertion failed
 * \param[in] cond the textual representation of the condition
 * \param[in] file the source filename
 * \param[in] line the line number
 */
void nl_assertion_failed(const char* cond, const char* file, int line) ;

/**
 * \brief Displays an error message and aborts the program
 *  when a range assertion failed.
 * \details Called by nl_range_assert() whenever the assertion failed
 * \param[in] double the variable
 * \param[in] double min_val the minimum value
 * \param[in] double max_val the maximum value
 * \param[in] file the source filename
 * \param[in] line the line number
 */
void nl_range_assertion_failed(
    double x, double min_val, double max_val, const char* file, int line
) ;

/**
 * \brief Displays an error message and aborts the program
 *  when the execution flow reached a point it should not
 *  have reached.
 * \details called by nl_assert_not_reached
 * \param[in] file the source filename
 * \param[in] line the line number
 */
void nl_should_not_have_reached(const char* file, int line) ;

/**
 * \brief Tests an assertion and aborts the program if the test fails
 * \param[in] x the condition to be tested
 */
#define nl_assert(x) {                                          \
    if(!(x)) {                                                  \
        nl_assertion_failed(#x,__FILE__, __LINE__) ;            \
    }                                                           \
} 

/**
 * \brief Tests a range assertion and aborts the program if the test fails
 * \param[in] x the variable to be tested
 * \param[in] min_val the minimum admissible value for the variable
 * \param[in] max_val the maximum admissible value for the variable
 */
#define nl_range_assert(x,min_val,max_val) {                    \
    if(((x) < (min_val)) || ((x) > (max_val))) {                \
        nl_range_assertion_failed(x, min_val, max_val,          \
            __FILE__, __LINE__                                  \
        ) ;                                                     \
    }                                                           \
}

/**
 * \brief Triggers an assertion failure when the execution flow
 *  reaches a specific location in the code.
 */
#define nl_assert_not_reached {                                 \
    nl_should_not_have_reached(__FILE__, __LINE__) ;            \
}

#ifdef NL_DEBUG
    #define nl_debug_assert(x) nl_assert(x)
    #define nl_debug_range_assert(x,min_val,max_val)            \
                               nl_range_assert(x,min_val,max_val)
#else
    #define nl_debug_assert(x) 
    #define nl_debug_range_assert(x,min_val,max_val) 
#endif

#ifdef NL_PARANOID
    #define nl_parano_assert(x) nl_assert(x)
    #define nl_parano_range_assert(x,min_val,max_val)           \
                               nl_range_assert(x,min_val,max_val)
#else
    #define nl_parano_assert(x) 
    #define nl_parano_range_assert(x,min_val,max_val) 
#endif

/**
 * @}
 * \name Error reporting
 * @{ 
 */

/**
 * \brief Displays an error message
 * \param[in] function name of the function that triggered the error
 * \param[in] message error message
 */
void nlError(const char* function, const char* message) ;

/**
 * \brief Displays a warning message
 * \param[in] function name of the function that triggered the error
 * \param[in] message warning message
 */
void nlWarning(const char* function, const char* message) ;

/**
 * @}
 * \name OS
 * @{ 
 */

/**
 * \brief Gets the current time in seconds
 * \return the current time in seconds (starting from a given reference time)
 */
NLdouble nlCurrentTime()  ;

/******************************************************************************/
/* classic macros */

#ifndef MIN
#define MIN(x,y) (((x) < (y)) ? (x) : (y)) 
#endif

#ifndef MAX
#define MAX(x,y) (((x) > (y)) ? (x) : (y)) 
#endif


/**
 * @}
 * \name Memory management
 * @{ 
 */

/**
 * \brief Allocates a new element
 * \details Memory is zeroed after allocation
 * \param[in] T type of the element to be allocated
 */
#define NL_NEW(T)                (T*)(calloc(1, sizeof(T))) 

/**
 * \brief Allocates a new array of elements
 * \details Memory is zeroed after allocation
 * \param[in] T type of the elements 
 * \param[in] NB number of elements 
 */
#define NL_NEW_ARRAY(T,NB)       (T*)(calloc((size_t)(NB),sizeof(T)))

/**
 * \brief Changes the size of an already allocated array of elements
 * \details Memory is zeroed after allocation
 * \param[in] T type of the elements 
 * \param[in,out] x a pointer to the array to be resized
 * \param[in] NB number of elements 
 */
#define NL_RENEW_ARRAY(T,x,NB)   (T*)(realloc(x,(size_t)(NB)*sizeof(T))) 

/**
 * \brief Deallocates an element
 * \param[in,out] x a pointer to the element to be deallocated
 */
#define NL_DELETE(x)             free(x); x = NULL 

/**
 * \brief Deallocates an array
 * \param[in,out] x a pointer to the first element of the array to 
 *  be deallocated
 */
#define NL_DELETE_ARRAY(x)       free(x); x = NULL

/**
 * \brief Clears an element
 * \param[in] T type of the element to be cleared
 * \param[in,out] x a pointer to the element
 */
#define NL_CLEAR(T, x)           memset(x, 0, sizeof(T)) 

/**
 * \brief Clears an array of elements
 * \param[in] T type of the element to be cleared
 * \param[in,out] x a pointer to the element
 * \param[in] NB number of elements
 */
#define NL_CLEAR_ARRAY(T,x,NB)   memset(x, 0, (size_t)(NB)*sizeof(T)) 

/**
 * @}
 * \name Integer bounds
 * @{ 
 */


/**
 * \brief Maximum unsigned 32 bits integer
 */
#define NL_UINT_MAX 0xffffffff

/**
 * \brief Maximum unsigned 16 bits integer
 */
#define NL_USHORT_MAX 0xffff

/**
 * @}
 */

#endif