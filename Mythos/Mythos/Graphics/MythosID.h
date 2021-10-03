#pragma once

#include <xhash>

namespace Mythos
{
	class MythosID
	{
	public:
		MythosID();
		MythosID(uint64_t);
		MythosID(const MythosID&) = default;

		operator uint64_t() const { return m_UUID; }
	private:
		uint64_t m_UUID;
	};
}

namespace std
{
	template<>
	struct hash<Mythos::MythosID>
	{
		std::size_t operator()(const Mythos::MythosID& id) const 
		{
			return hash<uint64_t>()((uint64_t)id);
		}
	};
}