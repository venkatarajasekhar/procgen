#ifndef _INPUT_HANDLERS_H_
#define _INPUT_HANDLERS_H_

#include <math.h>

class InputHanlder_YawPitch {
public:
	InputHanlder_YawPitch( int left, int right, int up, int down ) :
		m_LeftKey(left), m_RightKey(right),
		m_UpKey(up), m_DownKey(down),
		m_Yaw(0.0f), m_Pitch(0.0f),
		m_YawRate(1.0f), m_PitchRate(1.0f)
	{

	}
	
	InputHanlder_YawPitch() :
		m_LeftKey(GLFW_KEY_LEFT), m_RightKey(GLFW_KEY_RIGHT),
		m_UpKey(GLFW_KEY_UP), m_DownKey(GLFW_KEY_DOWN),
		m_Yaw(0.0f), m_Pitch(0.0f),
		m_YawRate(1.0f), m_PitchRate(1.0f)
	{

	}

	void SetRate( float yawRadiansPerSecond, float pitchRadiansPerSecond ) {
		m_PitchRate = pitchRadiansPerSecond;
		m_YawRate = yawRadiansPerSecond;
	}

	float GetYaw() { return m_Yaw; }
	float GetPitch() { return m_Pitch; }

	void Update( float seconds ) {
		float upDown = 0.0f;
		float leftRight = 0.0f;
		if( glfwGetKey(m_UpKey) )
			upDown += 1.0f;
		if( glfwGetKey(m_DownKey) )
			upDown -= 1.0f;
		if( glfwGetKey(m_LeftKey) )
			leftRight += 1.0f;
		if( glfwGetKey(m_RightKey) )
			leftRight -= 1.0f;
		m_Yaw += leftRight * seconds * m_YawRate;
		m_Pitch += upDown * seconds * m_PitchRate;
		if( m_Pitch < - M_PI_2 ) {
			m_Pitch = - M_PI_2;
		}
		if( m_Pitch > M_PI_2 ) {
			m_Pitch = M_PI_2;
		}
	}
	
private:
	int m_LeftKey, m_RightKey, m_UpKey, m_DownKey;
	float m_Yaw; // allow for 360 degree (2 pi)
	float m_Pitch; // allow for plus minus 90 degrees (pi / 2)
	float m_YawRate, m_PitchRate;
};


#endif
