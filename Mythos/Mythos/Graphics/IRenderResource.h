#pragma once

struct IRenderResource
{
public:
	inline unsigned int GetID() { return m_ID; }
	inline void SetID(unsigned int id) { m_ID = id; }
private:
	unsigned int m_ID = 0;
};