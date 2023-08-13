#ifndef DARTS_DEFINES_H_
#define DARTS_DEFINES_H_
#define WD_NODISCARD [[nodiscard]]
#define WD_INLINE inline
#define WD_FORCEINLINE __forceinline
#define WD_OVERRIDE override
#define WD_NOCOPY(Class) Class(const Class&) = delete; Class& operator=(const Class&) = delete;
#define WD_STATIC_ASSERT(exp, msg) static_assert(exp, msg)
// https://stackoverflow.com/a/4415646/17059957
#define WD_ARRAY_LEN(x) ((sizeof(x)/sizeof(0[x])) / ((size_t)(!(sizeof(x) % sizeof(0[x])))))
#endif // !DARTS_DEFINES_H_
