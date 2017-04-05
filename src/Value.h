#pragma once

#include "Listenable.h"

class Value: public Listenable
{
protected:
	int mValue;
	
public:
	int getValue() const { return mValue; }

	Value(): Listenable(), mValue(0) {}
	Value(int value): Listenable(), mValue(value) {}
	operator int() const { return mValue; }

	inline Value& operator=(int rhs) { if (rhs != mValue) { mValue = rhs; notify(); } return *this; }
	inline Value& operator=(const Value& rhs) { if (rhs.getValue() != mValue) { mValue = rhs.getValue(); notify(); } return *this; }
	inline Value& operator--() { --mValue; notify(); return *this; }
	inline Value& operator++() { ++mValue; notify(); return *this; }
	inline Value& operator+=(const Value& rhs) { if (rhs.getValue() != 0) { mValue += rhs.getValue(); notify(); } return *this; }
	inline Value& operator-=(const Value& rhs) { if (rhs.getValue() != 0) { mValue -= rhs.getValue(); notify(); } return *this; }
	inline Value& operator/=(const Value& rhs) { mValue /= rhs.getValue(); notify(); return *this; }
	inline Value& operator%=(const Value& rhs) { mValue %= rhs.getValue(); notify(); return *this; }
	
	inline int operator+() const { return +mValue; }
    inline int operator-() const { return -mValue; }
    inline int operator!() const { return !mValue; }
    inline int operator~() const { return ~mValue; }
};

inline bool operator!=(const Value& lhs, const Value& rhs){ return lhs.getValue() != rhs.getValue(); }
inline bool operator==(const Value& lhs, const Value& rhs){ return lhs.getValue() == rhs.getValue(); }
inline bool operator< (const Value& lhs, const Value& rhs){ return lhs.getValue() < rhs.getValue(); }
inline bool operator> (const Value& lhs, const Value& rhs){ return lhs.getValue() > rhs.getValue(); }
inline bool operator<=(const Value& lhs, const Value& rhs){ return lhs.getValue() <= rhs.getValue(); }
inline bool operator>=(const Value& lhs, const Value& rhs){ return lhs.getValue() >= rhs.getValue(); }

inline bool operator!=(int lhs, const Value& rhs){ return lhs != rhs.getValue(); }
inline bool operator==(int lhs, const Value& rhs){ return lhs == rhs.getValue(); }
inline bool operator< (int lhs, const Value& rhs){ return lhs < rhs.getValue(); }
inline bool operator> (int lhs, const Value& rhs){ return lhs > rhs.getValue(); }
inline bool operator<=(int lhs, const Value& rhs){ return lhs <= rhs.getValue(); }
inline bool operator>=(int lhs, const Value& rhs){ return lhs >= rhs.getValue(); }

inline bool operator!=(const Value& lhs, int rhs){ return lhs.getValue() != rhs; }
inline bool operator==(const Value& lhs, int rhs){ return lhs.getValue() == rhs; }
inline bool operator< (const Value& lhs, int rhs){ return lhs.getValue() < rhs; }
inline bool operator> (const Value& lhs, int rhs){ return lhs.getValue() > rhs; }
inline bool operator<=(const Value& lhs, int rhs){ return lhs.getValue() <= rhs; }
inline bool operator>=(const Value& lhs, int rhs){ return lhs.getValue() >= rhs; }

inline int operator+(int lhs, const Value& rhs){ return lhs + rhs.getValue(); }
inline int operator-(int lhs, const Value& rhs){ return lhs	- rhs.getValue(); }
inline int operator*(int lhs, const Value& rhs){ return lhs * rhs.getValue(); }
inline int operator/(int lhs, const Value& rhs){ return lhs / rhs.getValue(); }
inline int operator%(int lhs, const Value& rhs){ return lhs % rhs.getValue(); }

inline int operator+(const Value& lhs, int rhs){ return lhs.getValue() + rhs; }
inline int operator-(const Value& lhs, int rhs){ return lhs.getValue() - rhs; }
inline int operator*(const Value& lhs, int rhs){ return lhs.getValue() * rhs; }
inline int operator/(const Value& lhs, int rhs){ return lhs.getValue() / rhs; }
inline int operator%(const Value& lhs, int rhs){ return lhs.getValue() % rhs; }

inline int operator+(const Value& lhs, const Value& rhs){ return lhs.getValue() + rhs.getValue(); }
inline int operator-(const Value& lhs, const Value& rhs){ return lhs.getValue() - rhs.getValue(); }
inline int operator*(const Value& lhs, const Value& rhs){ return lhs.getValue() * rhs.getValue(); }
inline int operator/(const Value& lhs, const Value& rhs){ return lhs.getValue() / rhs.getValue(); }
inline int operator%(const Value& lhs, const Value& rhs){ return lhs.getValue() % rhs.getValue(); }
