#pragma once
/** @file M2L.hpp
 * @brief Dispatch methods for the M2L stage
 *
 */

#include "fmmtl/KernelTraits.hpp"
#include <type_traits>

class M2L
{
  /** If no other M2L dispatcher matches */
  template <typename Expansion, typename... Args>
  inline static void eval(const Expansion&, Args...) {
    std::cerr << "Expansion does not have a correct M2L!\n";
    std::cerr << ExpansionTraits<Expansion>() << std::endl;
    exit(1);
  }

  template <typename Expansion>
  inline static
  typename std::enable_if<ExpansionTraits<Expansion>::has_M2L>::type
  eval(const Expansion& K,
       const typename Expansion::multipole_type& source,
       typename Expansion::local_type& target,
       const typename Expansion::point_type& translation) {
    K.M2L(source, target, translation);
  }

 public:

  template <typename Context>
  inline static void eval(Context& c,
                          const typename Context::source_box_type& source,
                          const typename Context::target_box_type& target)
  {
#ifdef DEBUG
    std::cout << "M2L:\n  " << source << "\n  " << target << std::endl;
#endif

    M2L::eval(c.expansion(),
              c.multipole(source),
              c.local(target),
              target.center() - source.center());
  }
};


#include "Evaluator.hpp"

/** A lazy M2L evaluator which saves a list of pairs of boxes
 * That are sent to the M2L dispatcher on demand.
 */
template <typename Context>
class M2L_Batch {
  //! Type of box
  typedef typename Context::source_box_type source_box_type;
  typedef typename Context::target_box_type target_box_type;

  //! Box list for P2P interactions    TODO: could further compress these...
  typedef std::pair<source_box_type, target_box_type> box_pair;

  std::vector<box_pair> m2l_list;

 public:
  /** Insert a source-target box interaction to the interaction list */
  void insert(const source_box_type& s, const target_box_type& t) {
    m2l_list.push_back(std::make_pair(s,t));
  }

  /** Compute all interations in the interaction list */
  virtual void execute(Context& c) {
    auto b_end = m2l_list.end();
    //#pragma omp parallel for   TODO: Make thread safe!
    for (auto bi = m2l_list.begin(); bi < b_end; ++bi)
      M2L::eval(c, bi->first, bi->second);
  }
};
