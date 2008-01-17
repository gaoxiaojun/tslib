#ifndef WINDOW_FUNCTION_HPP
#define WINDOW_FUNCTION_HPP

#include <algorithm>

#include <tslib/tseries.data.hpp>
#include <tslib/range.specifier/rangeSpecifier.hpp>
#include <tslib/utils/window.intersection.apply.hpp>
//#include <tslib/range.specifier/range.opp.hpp>

namespace tslib {

  template<typename ReturnType,
           template<class> class F,
           class TDATE,
           class TDATA,
           class TSDIM,
           template<typename,typename,typename> class TSDATABACKEND,
           template<typename> class DatePolicy,
           template<class U, class V, class W, template<typename,typename,typename> class DATABACKEND, template<typename> class DP> class TSeries>

  const TSeries<TDATE,ReturnType,TSDIM,TSDATABACKEND,DatePolicy> window_function(const TSeries<TDATE,TDATA,TSDIM,TSDATABACKEND,DatePolicy>& lhs,
                                                                                 const TSeries<TDATE,TDATA,TSDIM,TSDATABACKEND,DatePolicy>& rhs,
                                                                                 const int window) {
    TSDIM lhs_ncol = lhs.ncol();
    TSDIM rhs_ncol = rhs.ncol();

    if(lhs_ncol != rhs_ncol && lhs_ncol != 1 && rhs_ncol != 1)
      return TSeries<TDATE,TDATA,TSDIM,TSDATABACKEND,DatePolicy>();

    // find date intersection
    RangeSpecifier<TDATE,TSDIM> range(lhs.getDates(), rhs.getDates(), lhs.nrow(), rhs.nrow() );

    if(!range.getSize())
      return TSeries<TDATE,TDATA,TSDIM,TSDATABACKEND,DatePolicy>();

    TSDIM ans_ncol = (lhs_ncol > rhs_ncol) ? lhs_ncol : rhs_ncol;

    // allocate new answer
    TSeries<TDATE,ReturnType,TSDIM,TSDATABACKEND,DatePolicy> ans(range.getSize(),ans_ncol);

    // copy over dates
    std::copy(range.getDates(),range.getDates()+range.getSize(),ans.getDates());

    // set new colnames
    vector<string> lhs_cnames = lhs.getColnames();
    vector<string> rhs_cnames = rhs.getColnames();
    vector<string> ans_cnames;


    // FIXME: this will take some work to get correct
    if(lhs_cnames==rhs_cnames) {
      ans_cnames = lhs_cnames;
    } else {
      ans_cnames = lhs_cnames.size() ? lhs_cnames : rhs_cnames;
    }

    ans.setColnames(ans_cnames);

    TDATA* ans_data = ans.getData();
    TDATA* lhs_data = lhs.getData();
    TDATA* rhs_data = rhs.getData();

    for(TSDIM col = 0; col < lhs.ncol(); col++) {

      RangeIterator<const TDATA*, const TSDIM*> lhs_iter(lhs_data, range.getArg1());
      RangeIterator<const TDATA*, const TSDIM*> rhs_iter(rhs_data, range.getArg2());

      windowIntersectionApply<ReturnType,F>::apply(ans_data, lhs_iter, rhs_iter, range.getSize(), window);

      // increment column
      ans_data+= ans.nrow();
      lhs_data+= lhs.nrow();
      rhs_data+= rhs.nrow();
    }
    return ans;
  }

} // namespace tslib

#endif // WINDOW_FUNCTION_HPP
