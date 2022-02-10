#pragma once
#ifndef TIMETYPE_H
#define TIMETYPE_H
#include "HEinsSim.h"

// Class for Simluation Time

class UDESIM_EXT_CLASS TimeType final {
public:
	static const double infinity_;
	static const double epsilon_;

public:
	TimeType(void);
	TimeType(const double & t);
	TimeType(const TimeType& t);

	// 2007.11.05
	double GetValue() const { return double_time_; }

	// 2009.11.03 - chsung (uncomment)
	TimeType& operator=(const TimeType& value)
	{
		double_time_ = value.GetValue();
		return *this;
	}

	TimeType& operator=(const double & value)
	{
		double_time_ = value;
		return *this;
	}

	TimeType& operator+=(const TimeType& value)
	{
		double_time_ = ((*this) + value).GetValue();
		return *this;
	}

	TimeType& operator-=(const TimeType& value)
	{
		double_time_ = ((*this) - value).GetValue();
		return *this;
	}

	TimeType& operator*=(const TimeType& value)
	{
		double_time_ = ((*this) * value).GetValue();
		return *this;
	}

	TimeType& operator/=(const TimeType& value)
	{
		double_time_ = ((*this) / value).GetValue();
		return *this;
	}

	TimeType& operator+=( const double & value)
	{
		double_time_ = ((*this) + TimeType(value)).GetValue();
		return *this;
	}

	TimeType& operator-=(const double & value)
	{
		double_time_ = ((*this) - TimeType(value)).GetValue();
		return *this;
	}

	TimeType& operator*=(const double & value)
	{
		double_time_ = ((*this) * TimeType(value)).GetValue();
		return *this;
	}

	TimeType& operator/=(const double & value)
	{
		double_time_ = ((*this) / TimeType(value)).GetValue();
		return *this;
	}

	TimeType operator+(const TimeType& flt) const {
		if (double_time_ == infinity_ || flt.GetValue() == infinity_) return TimeType(infinity_);
		return TimeType(double_time_ + flt.GetValue());
	}

	TimeType operator-(const TimeType& flt) const {
		if (double_time_ == infinity_ || flt.GetValue() == infinity_) return TimeType(infinity_);
		return TimeType(double_time_ - flt.GetValue());
	}

	TimeType operator*(const TimeType& flt) const {
		if (double_time_ == 0 || flt.GetValue() == 0) return TimeType(0.0);
		if (double_time_ != 0 && flt.GetValue() != 0 && (double_time_ == infinity_ || flt.GetValue() == infinity_))
			return
				TimeType(infinity_);
		return TimeType(double_time_ * flt.GetValue());
	}

	TimeType operator/(const TimeType& flt) const {
		if (double_time_ == infinity_ || flt.GetValue() == 0) return TimeType(infinity_);
		if (double_time_ != infinity_ && flt.GetValue() == infinity_) return TimeType(0.0);
		return TimeType(double_time_ / flt.GetValue());
	}

	TimeType operator+( const double  & value) const {
		if (double_time_ == infinity_ || value == infinity_) return TimeType(infinity_);
		return TimeType(double_time_ + value);
	}

	TimeType operator-( const double & value) const {
		if (double_time_ == infinity_ || value == infinity_) return TimeType(infinity_);
		return TimeType(double_time_ - value);
	}

	TimeType operator*( const double  & value) const {
		if (double_time_ == 0 || value == 0) return TimeType(0.0);
		if (double_time_ != 0 && value != 0 && (double_time_ == infinity_ || value == infinity_)) return TimeType(infinity_);
		return TimeType(double_time_ * value);
	}

	TimeType operator/( const double & value) const {
		if (double_time_ == infinity_ || value == 0) return TimeType(infinity_);
		if (double_time_ != infinity_ && value == infinity_) return TimeType(0.0);
		return TimeType(double_time_ / value);
	}

	bool operator==(const TimeType& flt) const
	{
		if (double_time_ == infinity_ && flt.GetValue() == infinity_) return true;
		return (-epsilon_ < (double_time_ - flt.GetValue())) && ((double_time_ - flt.GetValue()) < epsilon_);
	}

	bool operator!=(const TimeType& flt) const { return !((*this) == flt); }

	bool operator<(const TimeType& flt) const
	{
		if (double_time_ == infinity_ && flt.GetValue() == infinity_) return false;
		return (-epsilon_ > (double_time_ - flt.GetValue()));
	}

	bool operator>(const TimeType& flt) const
	{
		if (double_time_ == infinity_ && flt.GetValue() == infinity_) return false;
		return (double_time_ - flt.GetValue()) > epsilon_;
	}

	bool operator<=(const TimeType& flt) const { return ((*this) == flt || (*this) < flt); }
	bool operator>=(const TimeType& flt) const { return ((*this) == flt || (*this) > flt); }

	bool operator==( const double & value) const
	{
		if (double_time_ == infinity_ && value == infinity_) return true;
		return (-epsilon_ < (double_time_ - value)) && ((double_time_ - value) < epsilon_);
	}

	bool operator!=( const double & value) const { return !((*this) == value); }

	bool operator<( const double  & value) const
	{
		if (double_time_ == infinity_ && value == infinity_) return false;
		return (-epsilon_ > (double_time_ - value));
	}

	bool operator>( const double & value) const
	{
		if (double_time_ == infinity_ && value == infinity_) return false;
		return (double_time_ - value) > epsilon_;
	}

	bool operator<=( const double &  value) const { return ((*this) == value || (*this) < value); }
	bool operator>=( const double & value) const { return ((*this) == value || (*this) > value); }

	friend TimeType operator+( const double  &value, const TimeType& flt) { return TimeType(value + flt.GetValue()); }
	friend TimeType operator-( const double  &value, const TimeType& flt) { return TimeType(value - flt.GetValue()); }
	friend TimeType operator*( const double  & value, const TimeType& flt) { return TimeType(value * flt.GetValue()); }
	friend TimeType operator/( const double  & value, const TimeType& flt) { return TimeType(value / flt.GetValue()); }

	friend bool operator==( const double  & value, const TimeType& flt) { return (TimeType(value) == flt); }
	friend bool operator!=( const double & value, const TimeType& flt) { return (TimeType(value) != flt); }
	friend bool operator<( const double  & value, const TimeType& flt) { return (TimeType(value) < flt); }
	friend bool operator>( const double  & value, const TimeType& flt) { return (TimeType(value) > flt); }
	friend bool operator<=( const double & value, const TimeType& flt) { return (TimeType(value) <= flt); }
	friend bool operator>=( const double & value, const TimeType& flt) { return (TimeType(value) >= flt); }

protected:
	double double_time_;
};

#endif // TIMETYPE_H
