#pragma once

struct IRenderResource
{
public:
	inline unsigned int GetID() { return m_ID; }
private:
	unsigned int m_ID = 0;
};