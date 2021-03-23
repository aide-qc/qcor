#pragma once

#include <memory>
#include <vector>
#include <stack>
#include "qalloc.hpp"
#include "qir-types.hpp"

namespace xacc {
    class AcceleratorBuffer;
}

extern "C" {
using qreg = xacc::internal_compiler::qreg;

// Note: The QIR spec requires ResultZero and ResultOne
// as External Global constants of *pointer* type.
extern Result *ResultZero;
extern Result *ResultOne;
extern unsigned long allocated_qbits;
extern bool initialized;
extern bool verbose;
// Global register instance.
extern std::shared_ptr<xacc::AcceleratorBuffer> global_qreg;
extern QRT_MODE mode;

void initialize();

// Initialize/Finalize/Config API
void __quantum__rt__initialize(int argc, int8_t** argv);
void __quantum__rt__finalize();
void __quantum__rt__set_external_qreg(qreg* q);

// QIS API (i.e. quantum instructions)
void __quantum__qis__cnot(Qubit* src, Qubit* tgt);
void __quantum__qis__h(Qubit* q);
void __quantum__qis__s(Qubit* q);
void __quantum__qis__sdg(Qubit* q);
void __quantum__qis__t(Qubit* q);
void __quantum__qis__tdg(Qubit* q);
void __quantum__qis__reset(Qubit* q);

void __quantum__qis__x(Qubit* q);
void __quantum__qis__y(Qubit* q);
void __quantum__qis__z(Qubit* q);

void __quantum__qis__rx(double x, Qubit* q);
void __quantum__qis__ry(double x, Qubit* q);
void __quantum__qis__rz(double x, Qubit* q);
void __quantum__qis__u3(double theta, double phi, double lambda, Qubit* q);
Result* __quantum__qis__mz(Qubit* q);
// Compare results.
bool __quantum__rt__result_equal(Result *res, Result *comp);
void __quantum__rt__result_update_reference_count(Result *, int64_t count);

// Qubit Alloc/Dealloc API
Array* __quantum__rt__qubit_allocate_array(uint64_t idx);
void __quantum__rt__qubit_release_array(Array* q);

void __quantum__rt__start_ctrl_u_region();
void __quantum__rt__end_ctrl_u_region(Qubit * ctrl_qubit);
void __quantum__rt__start_adj_u_region();
void __quantum__rt__end_adj_u_region();
void __quantum__rt__start_pow_u_region();
void __quantum__rt__end_pow_u_region(int64_t power);

// Array API
// Create an array
Array *__quantum__rt__array_create_1d(int32_t itemSizeInBytes,
                                      int64_t count_items);
// Get size
int64_t __quantum__rt__array_get_size_1d(Array *array);
// Get element at an index
int8_t *__quantum__rt__array_get_element_ptr_1d(Array *q, uint64_t idx);
// Copy
Array *__quantum__rt__array_copy(Array *array, bool forceNewInstance);
// Concatenate
Array *__quantum__rt__array_concatenate(Array *head, Array *tail);
// Slice
// Creates and returns an array that is a slice of an existing array. 
// The int dim which dimension the slice is on (0 for 1d arrays). 
// The Range range specifies the slice.
// Note: QIR defines a Range as type { i64, i64, i64 }
// i.e. a struct of 3 int64_t
// and define an API at the *LLVM IR* level of passing this by value
// i.e. the signature is %Range, not "%struct.Range* byval(%struct.Range)" 
// Hence, it is actually equivalent to an expanded list of struct member.
// https://lists.llvm.org/pipermail/llvm-dev/2018-April/122714.html
// Until the spec. is updated (see https://github.com/microsoft/qsharp-language/issues/31)
// this is actually the C-ABI that will match the QIR IR.
Array *__quantum__rt__array_slice(Array *array, int32_t dim, int64_t range_start, int64_t range_step, int64_t range_end);
// Note: Overloading is not possible in C, so just keep the implementation in this local func.
Array *quantum__rt__array_slice(Array *array, int32_t dim, Range range);

// Ref. counting
void __quantum__rt__array_update_alias_count(Array *array, int64_t increment);
void __quantum__rt__array_update_reference_count(Array *aux, int64_t count);

// Multi-dimension Array API
int32_t __quantum__rt__array_get_dim(Array *array);
int64_t __quantum__rt__array_get_size(Array *array, int32_t dim);
Array *__quantum__rt__array_create_nonvariadic(int itemSizeInBytes,
                                               int countDimensions,
                                               va_list dims);
Array *__quantum__rt__array_create(int itemSizeInBytes, int countDimensions,
                                   ...);
int8_t *__quantum__rt__array_get_element_ptr_nonvariadic(Array *array,
                                                         va_list args);
int8_t *__quantum__rt__array_get_element_ptr(Array *array, ...);
Array *__quantum__rt__array_project(Array *array, int dim, int64_t index);

// String-related API
void __quantum__rt__string_update_reference_count(void *str, int64_t count);
}