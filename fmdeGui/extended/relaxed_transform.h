#ifndef RELAXED_TRANSFORM_H
#define RELAXED_TRANSFORM_H

// своя версия std::transform для того, чтобы гарантировать порядок обработки
// последовательности (в стандарте C++ порядок не специфицирован)
template <class InputIterator, class OutputIterator, class Transformator>
OutputIterator relaxed_transform(InputIterator first, InputIterator last,
                         OutputIterator result, Transformator trans) {
  for ( ; first != last; ++first, ++result)
    *result = trans(*first);
  return result;
}

template <class InputIterator1, class InputIterator2,
           class OutputIterator, class BinaryOperator>

  OutputIterator relaxed_transform(InputIterator1 first1, InputIterator1 last1,
							 InputIterator2 first2, InputIterator2 last2,
                             OutputIterator result, BinaryOperator op) {
  while (first1 != last1 || first2 != last2)
    *result++=binary_op(*first1++,*first2++);
  return result;
}

#endif // RELAXED_TRANSFORM_H
