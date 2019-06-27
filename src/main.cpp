#include <vector>

#include "kbest.hxx"

#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>

namespace py = pybind11;

/// wrapper around calculate_kbest_intervals
std::vector<int>
wrapped_k_best(py::array_t<double> weights, size_t k)
{
    auto k_best = kbest::calculate_kbest_intervals(
        weights.data(), weights.data() + weights.size(), k);

    return kbest::intervals_to_labels<int>(k_best, weights.size());
}

PYBIND11_MAKE_OPAQUE(std::vector<int>);

PYBIND11_MODULE(kbest_intervals, m)
{
    m.def("calculate_kbest_intervals", &wrapped_k_best,
          R"pbdoc(
        Calculate the <= k intervals that maximize the sum over the given weights.
    )pbdoc");

    // bind return type.
    py::bind_vector<std::vector<int>>(m, "VectorInt");

#ifdef VERSION_INFO
    m.attr("__version__") = VERSION_INFO;
#else
    m.attr("__version__") = "dev";
#endif
}
