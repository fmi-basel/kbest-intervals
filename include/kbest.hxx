#pragma once

#include <algorithm>
#include <ostream>
#include <vector>

namespace kbest {

/// Interval utility structure
template <typename T>
struct Interval
{
    Interval(size_t start_, size_t end_, T sum_)
      : start(start_)
      , end(end_)
      , sum(sum_)
    {}
    size_t start;
    size_t end;
    T sum;

    template <typename S>
    friend Interval<S> operator+(Interval<S> const& first,
                                 Interval<S> const& second);

    template <typename S>
    friend std::ostream& operator<<(std::ostream& out,
                                    Interval<S> const& interval);
};

/// concatenating two consecutive intervals
template <typename T>
Interval<T>
operator+(Interval<T> const& first, Interval<T> const& second)
{
    if (first.end != second.start)
        throw std::runtime_error("Intervals cannot be merged!");
    return Interval<T>(first.start, second.end, first.sum + second.sum);
}

/// printing intervals
template <typename S>
std::ostream&
operator<<(std::ostream& out, Interval<S> const& interval)
{
    out << "(" << interval.start << ", " << interval.end
        << "): cost=" << interval.sum;
    return out;
}

/// Determine interval with largest sum.
template <typename Iter>
Interval<typename std::iterator_traits<Iter>::value_type>
single_best_interval(Iter const beginIter, Iter const endIter)
{

    using val_t = typename std::iterator_traits<Iter>::value_type;

    Interval<val_t> best(0, 0, 0.);
    Interval<val_t> current(0, 0, 0.);

    for (auto iter = beginIter; iter < endIter; ++iter) {
        current.sum += *iter;

        if (current.sum <= 0) {
            current.start = current.end = std::distance(beginIter, iter) + 1;
            current.sum = 0;
        } else if (current.sum > best.sum) {
            best.start = current.start;
            best.end = std::distance(beginIter, iter) + 1;
            best.sum = current.sum;
        }
    }

    return best;
}

/// Partition array of values into interval of same sign and strip
/// trailing negative intervals.
template <typename Iter>
std::vector<Interval<typename std::iterator_traits<Iter>::value_type>>
preprocessing(Iter const beginIter, Iter const endIter)
{
    std::vector<Interval<typename std::iterator_traits<Iter>::value_type>>
        intervals;
    intervals.emplace_back(0, 0, 0);

    for (auto iter = beginIter; iter < endIter; ++iter) {

        // current value continues current run.
        if ((*iter <= 0 && intervals.back().sum <= 0) ||
            (*iter >= 0 && intervals.back().sum >= 0)) {
            intervals.back().sum += *iter;
        }

        // a new run starts.
        else {
            auto end = std::distance(beginIter, iter);
            intervals.back().end = end;
            intervals.emplace_back(end, end, *iter);
        }
    }

    // set the last intervals end properly.
    intervals.back().end = std::distance(beginIter, endIter);

    // Drop negative elements at the terminals.
    // NOTE By definition, there can only be one negative element on each side.
    if (!intervals.empty() && intervals.back().sum < 0) {
        intervals.pop_back();
    }

    if (!intervals.empty() && intervals.front().sum < 0) {
        intervals.erase(std::begin(intervals));
    }

    return intervals;
}

/// Determine the k intervals that maximize the sum of values.
template <typename Iter>
std::vector<Interval<typename std::iterator_traits<Iter>::value_type>>
calculate_kbest_intervals(Iter const beginIter, Iter const endIter, size_t k)
{

    // k = 1 is an easier case.
    if (k == 1)
        return { single_best_interval(beginIter, endIter) };

    // Preprocessing step.
    auto intervals = preprocessing(beginIter, endIter);

    // abort if there is nothing positive.
    if (intervals.empty())
        return {};

    // Utility to count positive intervals.
    auto count_positive = [](auto const& intervals) {
        return static_cast<size_t>(std::count_if(
            std::begin(intervals), std::end(intervals),
            [](auto const& interval) { return interval.sum > 0; }));
    };

    // Utility to merge intervals.
    auto merge_to_right = [](auto& intervals, auto const index) {
        auto const next_index = index + 1;
        intervals[index] = intervals[index] + intervals[next_index];
        intervals.erase(std::begin(intervals) + next_index);
    };

    // Csuros algorithm
    while (count_positive(intervals) > k) {

        // NOTE could be faster with a priority queue.
        size_t const min_index = std::distance(
            std::begin(intervals),
            std::min_element(std::begin(intervals), std::end(intervals),
                             [](auto const& first, auto const& second) {
                                 return std::abs(first.sum) <
                                        std::abs(second.sum);
                             }));

        if (min_index + 1 < intervals.size())
            merge_to_right(intervals, min_index);
        if (min_index >= 1)
            merge_to_right(intervals, min_index - 1);
    }

    intervals.erase(
        std::remove_if(std::begin(intervals), std::end(intervals),
                       [](auto const& interval) { return interval.sum < 0; }),
        std::end(intervals));

    return intervals;
}

/// convert intervals to labeling
template <typename L, typename Intervals>
std::vector<L>
intervals_to_labels(Intervals const& intervals, size_t size)
{
    std::vector<L> labels;
    labels.resize(size, 0);
    for (auto const& interval : intervals)
        for (auto iter = std::begin(labels) + interval.start;
             iter < std::begin(labels) + interval.end; ++iter)
            *iter = 1;
    return labels;
}

} // namespace kbest
