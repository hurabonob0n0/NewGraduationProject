#pragma once

enum
{
	S_TEST = 1,
	C_LOGIN = 1001
};


class ClientPacketHandler
{
public:
	static void HandlePacket(BYTE* buffer, int32 len);
	static void Handle_S_TEST(BYTE* buffer, int32 len);

	static SendBufferRef Make_C_LOGIN(uint64 id, uint32 hp, uint16 attack);
};

