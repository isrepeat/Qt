#pragma once
#include <type_traits>

_STD_BEGIN
/* --------------------------------------------------- */
/* "type_identity" support for languages before c++20  */
/* --------------------------------------------------- */
#if _HAS_CXX20 == 0
template <class _Ty>
struct type_identity {
	using type = _Ty;
};
template <class _Ty>
using type_identity_t = typename type_identity<_Ty>::type;
#endif // _HAS_CXX20

_STD_END

namespace H {
	 struct nothing {};
}