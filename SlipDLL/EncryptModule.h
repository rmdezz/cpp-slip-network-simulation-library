#pragma once
#include "PacketList.h"
#include "Module.h"
#include <openssl/ssl.h>
#include <openssl/rand.h>

class EncryptModule : public Module
{
	public:
		EncryptModule(PacketList& packetList);
		virtual ~EncryptModule();
		void startUp();
		void closeDown() override;
		bool process() override;
		bool encryptPacketsData();
		bool shouldKeepEncrypting(Node*& node);
		bool shouldEncrypt(Node*& node);
		bool generateRandomKeyAndIV(unsigned char* key, unsigned char* iv);
		bool initializeEncryptionContext(EVP_CIPHER_CTX* ctx, unsigned char* key, unsigned char* iv);
		void cleanup(EVP_CIPHER_CTX* ctx, unsigned char* encryptedData);
		bool updatePacketChecksum(Node* current);
		bool encryptData(EVP_CIPHER_CTX* ctx, unsigned char* encryptedData, int& encryptedDataLen, unsigned char* data, int dataLen);
		bool finalizeEncryption(EVP_CIPHER_CTX* ctx, unsigned char* encryptedData, int& encryptedDataLen);
		bool encryptData(Node* node);
		void printInfo();

	private:
		
};