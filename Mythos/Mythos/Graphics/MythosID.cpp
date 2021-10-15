#include "MythosID.h"

#include <random>

namespace Mythos
{
	static std::random_device s_RandomDevice;
	static std::mt19937_64 s_Engine(s_RandomDevice());
	static std::uniform_int_distribution<uint64_t> s_Distribution;

	MythosID::MythosID()
		: m_UUID(s_Distribution(s_Engine)) {}

	MythosID::MythosID(uint64_t id)
		: m_UUID(id) {}
}