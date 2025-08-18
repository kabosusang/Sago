#ifndef SG_META_CLASS_TRAITS_H
#define SG_META_CLASS_TRAITS_H

#define DEFINE_CLASS_NAME(ClassName)                   \
	static constexpr const char* GetClassName() { \
		return #ClassName;                             \
}





#endif



