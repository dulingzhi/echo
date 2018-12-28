#include "anim_property.h"
#include "engine/core/log/Log.h"
#include "engine/core/scene/node.h"

namespace Echo
{
	// create
	AnimProperty* AnimProperty::create(AnimProperty::Type type)
	{
		switch (type)
		{
		case AnimProperty::Type::Float:		return EchoNew(AnimPropertyFloat);
		case AnimProperty::Type::Vector3:	return EchoNew(AnimPropertyVec3);
		case AnimProperty::Type::Vector4:	return EchoNew(AnimPropertyVec4);
		case AnimProperty::Type::Quaternion:return EchoNew(AnimPropertyQuat);
		default:							return nullptr;
		}

		return nullptr;
	}

	// set interpolation type
	void AnimProperty::setInterpolationType(AnimCurve::InterpolationType type)
	{ 
		m_interpolationType = type;
	}

	AnimPropertyCurve::AnimPropertyCurve(AnimProperty::Type type, i32 curveCount)
		: AnimProperty(type)
	{
		m_curves.reserve(curveCount);
		for (i32 i = 0; i < curveCount; i++)
		{
			m_curves.push_back(EchoNew(AnimCurve));
		}
	}

	// set interpolation type
	void AnimPropertyCurve::setInterpolationType(AnimCurve::InterpolationType type)
	{
		AnimProperty::setInterpolationType(type);
		for (AnimCurve* curve : m_curves)
		{
			curve->setType(type);
		}
	}

	// optimize
	void AnimPropertyCurve::optimize()
	{
		for (AnimCurve* curve : m_curves)
		{
			curve->optimize();
		}
	}

	// get length
	float AnimPropertyCurve::getLength()
	{
		float length = 0.f;
		for (AnimCurve* curve : m_curves)
		{
			if (curve->getLength() > length)
				length = curve->getLength();
		}

		return length;
	}

	// get key number
	int AnimPropertyCurve::getKeyNumber()
	{
		return int(m_curves[0]->m_keys.size());
	}

	// get key time
	ui32 AnimPropertyCurve::getKeyTime(int idx)
	{
		return m_curves[0]->getKeyTime(idx);
	}

	// get key value str
	String AnimPropertyCurve::getKeyValueStr(int idx)
	{
		String str;
		for (size_t i = 0; i < m_curves.size(); i++)
		{
			str += StringUtil::Format( "%f ", m_curves[i]->getValueByKeyIdx(idx));
		}

		return str;
	}

	// set key value
	void AnimPropertyCurve::setKeyValue(int curveIdx, int keyIdx, float value)
	{
		m_curves[curveIdx]->setValue(keyIdx, value);
	}

	// add key
	void AnimPropertyCurve::addKeyToCurve(int curveIdx, ui32 time, float value)
	{
		m_curves[curveIdx]->addKey(time, value);
	}

	AnimPropertyFloat::AnimPropertyFloat()
		: AnimPropertyCurve(Type::Float, 1)
	{}

	// add key
	void AnimPropertyFloat::addKey(ui32 time, float value)
	{
		for (size_t i = 0; i < m_curves.size(); i++)
			m_curves[i]->addKey(time, value);
	}

	// update to time
	void AnimPropertyFloat::updateToTime(ui32 time)
	{
		for (size_t i = 0; i < m_curves.size(); i++)
			m_value = m_curves[i]->getValue(time);
	}

	AnimPropertyVec3::AnimPropertyVec3()
		: AnimPropertyCurve(Type::Vector3, 3)
	{}

	// add key
	void AnimPropertyVec3::addKey(ui32 time, const Vector3& value)
	{
		for (int i = 0; i < int(m_curves.size()); i++)
			m_curves[i]->addKey(time, value[i]);
	}

	// update to time
	void AnimPropertyVec3::updateToTime(ui32 time)
	{
		for (int i = 0; i < int(m_curves.size()); i++)
			m_value[i] = m_curves[i]->getValue(time);
	}

	AnimPropertyVec4::AnimPropertyVec4() 
		: AnimPropertyCurve(Type::Vector4, 4)
	{}

	void AnimPropertyVec4::addKey(ui32 time, const Vector4& value)
	{
		for (int i = 0; i < int(m_curves.size()); i++)
			m_curves[i]->addKey(time, value[i]);
	}

	void AnimPropertyVec4::updateToTime(ui32 time)
	{
		for (int i = 0; i < int(m_curves.size()); i++)
			m_value[i] = m_curves[i]->getValue(time);
	}

	void AnimPropertyQuat::addKey(ui32 time, const Quaternion& value)
	{
		Key key;
		key.m_time = time;
		key.m_value = value;
		m_keys.push_back(key);
	}

	// get length
	float AnimPropertyQuat::getLength()
	{
		return m_keys.size() ? m_keys.back().m_time : 0.f;
	}

	void AnimPropertyQuat::updateToTime(ui32 time)
	{
		if (m_keys.empty())
		{
			m_vlaue = Quaternion::IDENTITY;
		}
		else if (m_keys.size() == 1)
		{
			m_vlaue = m_keys[0].m_value;
		}
		else
		{
			// get base key and next key
			i32 curKey = 0;
			i32 keyTotal = i32(m_keys.size()) - 1;
			for (i32 i = 0; i < keyTotal; i++)
			{
				if (time > m_keys[i].m_time && time < m_keys[i + 1].m_time)
					curKey = i;
			}

			// calc value
			const Key& pre = m_keys[curKey];
			const Key& next = m_keys[curKey + 1];
			float ratio = Math::Clamp(float(time - pre.m_time) / float(next.m_time - pre.m_time), 0.f, 1.f);
			Quaternion::Slerp(m_vlaue, pre.m_value, next.m_value, ratio, true);
		}
	}
}