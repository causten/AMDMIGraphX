#include <migraphx/gpu/device/layernorm.hpp>
#include <migraphx/gpu/device/reduce.hpp>
#include <migraphx/gpu/device/pow.hpp>
#include <migraphx/gpu/device/fast_div.hpp>

namespace migraphx {
inline namespace MIGRAPHX_INLINE_NS {
namespace gpu {
namespace device {

template <class T>
struct vector_type
{
};

template <class T, index_int N>
struct vector_type<vec<T, N>>
{
    using type = T;
};

template <class T>
using vector_type_t = typename vector_type<T>::type;

template <class T>
struct vector_size
: std::integral_constant<index_int, 1>
{
};

template <class T, index_int N>
struct vector_size<vec<T, N>>
: std::integral_constant<index_int, N>
{};

template<class T, class F>
__device__ auto vec_transform(T x, F f)
{
    return f(x);
}

template<class T, index_int N, class F>
__device__ auto vec_transform(vec<T, N> x, F f)
{
    vec<T, N> y = x;
    for(index_int k = 0; k < N; k++)
        y[k] = f(x[k]);
    return y;
}

template<class T, class U, class Op>
__device__ auto vec_reduce(T x, U, Op)
{
    return x;
}

template<class T, index_int N, class U, class Op>
__device__ auto vec_reduce(vec<T, N> x, U init, Op op)
{
    T r = init;
    for(index_int k = 0; k < N; k++)
        r = op(r, x[k]);
    return r;
}

template <index_int N, class Op, class T, class F>
__device__ auto auto_block_reduce(index idx, Op op, T init, index_int n, F f)
{
    using type = decltype(f(0));
    auto r = block_reduce<N>(idx, op, init, n, f);
    return vec_reduce(r, 0, op);
}

template<index_int MaxBlockSize, class Input, class Output>
__device__ void layernorm(index_int i, index idx, std::size_t block_size_div, index_int relements, Input input, Output output)
{
    using value_type = decltype(input(idx.local));
    const auto relements_v = relements / vector_size<value_type>{};
    const auto out_idx   = fast_div(i, block_size_div);
    const auto base_idx  = out_idx * relements_v;
    const auto input_idx = base_idx + idx.local;
    const bool in_range  = idx.local < relements_v;

    auto mean = [&](auto z) {
        return auto_block_reduce<MaxBlockSize>(
            idx, sum{}, value_type(0), relements_v, [=](auto) { return z; }) / relements;

    };


    // m = x - mean(x)
    value_type x = in_range ? input(input_idx) : 0;
    value_type m = x - mean(x);

    // mean(m ^ 2) + 1e-12
    value_type r = mean(m * m) + 1e-12;

    // m * rsqrt(mean(m ^ 2) + 1e-12)
    if(in_range)
        output(input_idx, m * vec_transform(r, &rsqrt));
}

// m = x - mean(x)
// m / sqrt(mean(m ^ 2) + 1e-12)

template <index_int N>
void layernorm_vec_impl(hipStream_t stream,
                        const argument& result,
                        const argument& arg1,
                        index_int nelements,
                        index_int relements)
{
    hip_vec_visit_all<N>(result, arg1)([&](auto output, auto input) {
        using value_type = typename decltype(input)::value_type;

        const auto relements_v           = relements / N;
        const std::size_t max_block_size = 256;
        const std::size_t block_size     = compute_block_size(relements_v, max_block_size);
        const std::size_t block_size_div = encode_divisor(block_size);
        assert(relements_v <= block_size);

        gs_launch(stream, nelements * block_size, block_size)([=](auto i, auto idx) __device__ {
            layernorm<max_block_size>(i, idx, block_size_div, relements, [&](auto input_idx) { return input.data()[input_idx]; }, [&](auto input_idx, auto x) {
                output.data()[input_idx] = x;
            });
        });
    });
}

void layernorm_impl(hipStream_t stream,
                    const argument& result,
                    const argument& arg1,
                    index_int nelements,
                    index_int relements)
{
    hip_visit_all(result, arg1)([&](auto output, auto input) {
        using value_type = typename decltype(input)::value_type;

        const std::size_t max_block_size = 256;
        const std::size_t block_size     = compute_block_size(relements, max_block_size);
        const std::size_t block_size_div = encode_divisor(block_size);
        assert(relements <= block_size);

        gs_launch(stream, nelements * block_size, block_size)([=](auto i, auto idx) __device__ {
            layernorm<max_block_size>(i, idx, block_size_div, relements, [&](auto input_idx) { return input.data()[input_idx]; }, [&](auto input_idx, auto x) {
                output.data()[input_idx] = x;
            });
        });
    });
}

void layernorm(hipStream_t stream, const argument& result, const argument& arg1)
{
    auto relements    = arg1.get_shape().lens().back();
    auto nelements    = result.get_shape().elements() / relements;
    auto output_shape = result.get_shape();
    auto reduce_output_lens(output_shape.lens());
    reduce_output_lens.back() = 1;

    if((relements % 4) == 0)
        layernorm_vec_impl<4>(stream, result, arg1, nelements, relements);
    else if(relements < 256)
        layernorm_impl(stream, result, arg1, nelements, relements);
    else
        MIGRAPHX_THROW("No kernel for layernorm");
}

} // namespace device
} // namespace gpu
} // namespace MIGRAPHX_INLINE_NS
} // namespace migraphx
