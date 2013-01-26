/*
             LUFA Library
     Copyright (C) Dean Camera, 2013.

  dean [at] fourwalledcubicle [dot] com
           www.lufa-lib.org
*/

/*
  Copyright 2013  Dean Camera (dean [at] fourwalledcubicle [dot] com)

  Permission to use, copy, modify, distribute, and sell this
  software and its documentation for any purpose is hereby granted
  without fee, provided that the above copyright notice appear in
  all copies and that both that the copyright notice and this
  permission notice and warranty disclaimer appear in supporting
  documentation, and that the name of the author not be used in
  advertising or publicity pertaining to distribution of the
  software without specific, written prior permission.

  The author disclaims all warranties with regard to this
  software, including all implied warranties of merchantability
  and fitness.  In no event shall the author be liable for any
  special, indirect or consequential damages or any damages
  whatsoever resulting from loss of use, data or profits, whether
  in an action of contract, negligence or other tortious action,
  arising out of or in connection with the use or performance of
  this software.
*/

/** \file
 *  \brief Compiler specific definitions for code optimization and correctness.
 *
 *  \copydetails Group_CompilerSpecific
 *
 *  \note Do not include this file directly, rather include the Common.h header file instead to gain this file's
 *        functionality.
 */

/** \ingroup Group_Common
 *  \defgroup Group_CompilerSpecific Compiler Specific Definitions
 *  \brief Compiler specific definitions for code optimization and correctness.
 *
 *  Compiler specific definitions to expose certain compiler features which may increase the level of code optimization
 *  for a specific compiler, or correct certain issues that may be present such as memory barriers for use in conjunction
 *  with atomic variable access.
 *
 *  Where possible, on alternative compilers, these macros will either have no effect, or default to returning a sane value
 *  so that they can be used in existing code without the need for extra compiler checks in the user application code.
 *
 *  @{
 */

#ifndef __LUFA_COMPILERSPEC_H__
#define __LUFA_COMPILERSPEC_H__

	/* Preprocessor Checks: */
		#if !defined(__INCLUDE_FROM_COMMON_H)
			#error Do not include this file directly. Include LUFA/Common/Common.h instead to gain this functionality.
		#endif

	/* Public Interface - May be used in end-application: */
		/* Macros: */
			#if defined(__GNUC__) || defined(__DOXYGEN__)
				/** Forces GCC to use pointer indirection (via the device's pointer register pairs) when accessing the given
				 *  struct pointer. In some cases GCC will emit non-optimal assembly code when accessing a structure through
				 *  a pointer, resulting in a larger binary. When this macro is used on a (non \c const) structure pointer before
				 *  use, it will force GCC to use pointer indirection on the elements rather than direct store and load
				 *  instructions.
				 *
				 *  \param[in, out] StructPtr  Pointer to a structure which is to be forced into indirect access mode.
				 */
				#define GCC_FORCE_POINTER_ACCESS(StructPtr)   __asm__ __volatile__("" : "=b" (StructPtr) : "0" (StructPtr))

				/** Forces GCC to create a memory barrier, ensuring that memory accesses are not reordered past the barrier point.
				 *  This can be used before ordering-critical operations, to ensure that the compiler does not re-order the resulting
				 *  assembly output in an unexpected manner on sections of code that are ordering-specific.
				 */
				#define GCC_MEMORY_BARRIER()                  __asm__ __volatile__("" ::: "memory");

				/** Determines if the specified value can be determined at compile-time to be a constant value when compiling under GCC.
				 *
				 *  \param[in] x  Value to check compile-time constantness of.
				 *
				 *  \return Boolean true if the given value is known to be a compile time constant, false otherwise.
				 */
				#define GCC_IS_COMPILE_CONST(x)               __builtin_constant_p(x)

				#if ((ARCH == ARCH_AVR) || (ARCH == ARCH_XMEGA))
					#define GCC_IS_PART_DEFINED(x)            defined(__AVR_ ## x ## __)
				#elif (ARCH == ARCH_UC3)
					#define GCC_IS_PART_DEFINED(x)            defined(__AVR32_ ## x ## __)				
				#endif
				
				#define GCC_ISR(Name, ...)                    void Name (void) __attribute__((__interrupt__)) __VA_ARGS__; void Name (void)

				#define GCC_READ_SYS_REGISTER(Reg)            __builtin_mfsr(Reg)
				#define GCC_WRITE_SYS_REGISTER(Reg, Val)      __builtin_mtsr(Reg, Val)
				#define GCC_CLEAR_STATUS_FLAG(Bitmask)        __builtin_csrf(Bitmask)
				#define GCC_SET_STATUS_FLAG(Bitmask)          __builtin_ssrf(Bitmask)

				#if !defined(__DOXYGEN__)
					#define __COMPILER_SPECIFIC(x)            GCC_ ## x
				#endif
			#else
				#define GCC_FORCE_POINTER_ACCESS(StructPtr)
				#define GCC_MEMORY_BARRIER()
				#define GCC_IS_COMPILE_CONST(x)               0
				#define GCC_IS_PART_DEFINED(x)                0
				#define GCC_ISR(Name, ...)                    
				#define GCC_READ_SYS_REGISTER(Reg)       
				#define GCC_WRITE_SYS_REGISTER(Reg, Val)      
				#define GCC_CLEAR_STATUS_FLAG(Bitmask)        
				#define GCC_SET_STATUS_FLAG(Bitmask)          
			#endif

			#if defined(__ICCAVR32__) || defined(__DOXYGEN__)
				#include <intrinsics.h>
			
				#define IAR_MEMORY_BARRIER()                  __asm("")

				#if ((ARCH == ARCH_AVR) || (ARCH == ARCH_XMEGA))
					#define IAR_IS_PART_DEFINED(x)            defined(__AT ## x ## __)
				#elif (ARCH == ARCH_UC3)
					#define IAR_IS_PART_DEFINED(x)            defined(__AT32 ## x ## __)				
				#endif

				#if !defined(__DOXYGEN__)
					#define __IAR_ISR_PRAGMA(...)             _Pragma(#__VA_ARGS__)
				#endif
				
				#define IAR_ISR(Name, Group, Level)           __IAR_ISR_PRAGMA(handler = Group, Level) __interrupt void Name (void)
				
				#define IAR_READ_SYS_REGISTER(Reg)            __get_system_register(Reg)
				#define IAR_WRITE_SYS_REGISTER(Reg, Val)      __set_system_register(Reg, Val)
				#define IAR_CLEAR_STATUS_FLAG(Bitmask)        __clear_status_flag(Bitmask)
				#define IAR_SET_STATUS_FLAG(Bitmask)          __set_status_flag(Bitmask)

				#if !defined(__DOXYGEN__)
					#define __COMPILER_SPECIFIC(x)            IAR_ ## x
				#endif
			#else
				#define IAR_IS_PART_DEFINED(x)                0
				#define IAR_ISR(Name, ...)                    
				#define IAR_READ_SYS_REGISTER(Reg)       
				#define IAR_WRITE_SYS_REGISTER(Reg, Val)      
				#define IAR_CLEAR_STATUS_FLAG(Bitmask)        
				#define IAR_SET_STATUS_FLAG(Bitmask)          
			#endif
			
#endif

/** @} */

