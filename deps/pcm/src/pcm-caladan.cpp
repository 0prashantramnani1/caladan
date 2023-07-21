
#include "cpucounters.h"

using namespace pcm;

extern "C" {

	std::shared_ptr<ServerPCICFGUncore> server_pcicfg_uncore;
	ServerPCICFGUncore *caladan_cfg;
	PCM *cpcm;

	uint32_t pcm_caladan_get_cas_count(uint32 channel)
	{
		return caladan_cfg->getMCCounter(channel, 0);
	}

	uint32_t pcm_caladan_get_active_channel_count(void)
	{
		uint32_t count = 0;
		for (uint32_t i = 0; i < cpcm->getMCChannelsPerSocket(); i++)
			count += pcm_caladan_get_cas_count(i) != 0;
		return count;
	}

	int pcm_caladan_init(int socket)
	{
		cpcm = PCM::getInstance();
		server_pcicfg_uncore = cpcm->server_pcicfg_uncore.at(socket);
		caladan_cfg = server_pcicfg_uncore.get();
		caladan_cfg->programServerUncoreMemoryMetrics(PartialWrites, -1, -1);
		return 0;
	}
}
