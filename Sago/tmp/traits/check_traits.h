#ifndef SG_TMP_TRAITS_H
#define SG_TMP_TRAITS_H
#include <type_traits>



namespace Tmp::Traits{

template <typename T>
inline consteval bool checkPOD() {
	if (std::is_trivial<T>::value && std::is_standard_layout<T>::value) {
		return true;
	} else {
		return false;
	}
}


}









#endif