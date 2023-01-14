#include "EncryptModule.h"
#include <iostream>
#include <time.h>
#include <Windows.h>
#include "datetime_formatter.h"
#include <sstream>
#include "messagePipe.h"

using namespace std;

#define KEY_LEN 16
#define IV_LEN 16

EncryptModule::EncryptModule(PacketList& packetList) : Module(packetList)
{
	setDescription("Secures packets by encoding their contents using a secret key.");
	setShortName("Encrypt");
}

EncryptModule::~EncryptModule()
{
	
}

void EncryptModule::startUp()
{
	Module::startUp();
}

bool EncryptModule::process()
{
	return encryptPacketsData();
}

bool EncryptModule::encryptPacketsData()
{
	bool encrypted = false;

	Node* current = packetList.getTail()->prev;

	while (shouldKeepEncrypting(current))
	{
		if (shouldEncrypt(current)) encrypted = encryptData(current);
		current = current->prev;
	}

	return encrypted;
}

void EncryptModule::printInfo()
{
	std::stringstream messageStream;
	messageStream << "Encrypting packet with a chance of " << getChance() << "%";
	sendMessageEx(messageStream);
}

/*
*	@brief Encrypt the data in a packet.
*	@param current The Node object that holds the packet to be the encrypted.
*	@return True if the encryption was successful, false otherwise.
*/
bool EncryptModule::encryptData(Node* current)
{
	printInfo();
	
	// Parse the packet to get the IP and TCP headers
	// and the payload data
	PWINDIVERT_IPHDR ipHeader;
	PWINDIVERT_TCPHDR tcpHeader;
	PVOID payload;
	UINT payloadLen;

	if (!parsePacket(current, tcpHeader, ipHeader, payload, payloadLen))
	{
		sendMessage("ERROR: Failed to parse packet");
		return false;
	}

	sendMessage("Packet successfully parsed.");

	// Generate a random key and IV for this encryption session (current packet)

	/*
	*	In cryptography, a key is a value that is used in combination with an algorithm
	*	to transform plaintext into ciphertext (encryption) or vice versa (decryption).
	*	
	*	A key has the same function as a password, but it is usually longer and more
	*	complex, making it more secure. The key is an essential element of a cryptographic
	*	system because it determines the output of the encryption and decryption process.
	*/ 
	unsigned char key[KEY_LEN];

	/*
	*	The initialization vector (IV) is a block of bits that is used in combination 
	*	with a secret key to encrypt data. It is used to ensure that the same plaintext
	*	will be encrypted to different ciphertext blocks each time it is encrypted, even
	*	if the key remains the same. 
	* 
	*	This is useful because it makes the ciphertext more resistant to attacks such as
	*	frequency analysis, where an attacker tries to determine the plaintext by analyzing
	*	the frequency of characters in the ciphertext.
	*	By making the ciphertext less predictable, the IV helps to improve the security
	*	of the encrypted data.
	*/
	
	unsigned char iv[IV_LEN];

	if (!generateRandomKeyAndIV(key, iv))
	{
		sendMessage("Error: failed to generate random key");
		return false;
	}

	sendMessage("Random keys generated successfully");

	// Initialize the OpenSSL encryption context
	EVP_CIPHER_CTX* ctx;
	ctx = EVP_CIPHER_CTX_new();
	if (ctx == NULL)
	{
		sendMessage("Error initializing the encryption context.");
		return false;
	}

	/*
		The encryption context is an object that stores the state of an
		encryption operation. It is used to control the encryption
		process and to specify certain encryption parameters, such as the
		encryption algorithm and key to be used. 
		
		The context can be initialized, updated with additional data to be
		encrypted, and finalized to complete the encryption process.
		In this way, the encryption context allows for the encryption of
		data in a flexible and efficient manner.
	*/
	sendMessage("OpenSSL encryption context initialized successfully.");

	if (!initializeEncryptionContext(ctx, key, iv))
	{
		sendMessage("Error initializing the encryption.");
		EVP_CIPHER_CTX_free(ctx);
		return false;
	}
	
	sendMessage("Encryption initialized successfully.");

	// Allocate a buffer for the encrypted data:
	unsigned char* encryptedData = new unsigned char[payloadLen + IV_LEN];

	// Encrypt the payload data
	int encryptedDataLen;

	if (!encryptData(ctx, encryptedData, encryptedDataLen, (unsigned char*)payload, payloadLen))
	{
		sendMessage("Error encrypting the data.");
		cleanup(ctx, encryptedData);
		return false;
	}

	sendMessage("Data encrypted successfully.");

	// Finalize the encryption
	if (!finalizeEncryption(ctx, encryptedData, encryptedDataLen))
	{
		sendMessage("Error finalizing the encryption.");
		cleanup(ctx, encryptedData);
		return false;
	}

	sendMessage("Encryption finalized successfully.");

	// Update the payload with the encrypted data
	sendMessage("Updating payload...");
	std::memcpy(payload, encryptedData, payloadLen);
	sendMessage("Payload updated successfully.");

	// Update the packet checksum
	if (!updatePacketChecksum(current))
	{
		sendMessage("Error: Failed to update the packet checksum.");
		cleanup(ctx, encryptedData);
		return false;
	}

	sendMessage("Packet checksum updated successfully.");

	// Clean up
	cleanup(ctx, encryptedData);

	sendMessage("Packet encrypted successfully.");
	return true;
}

/*
*	@brief Finalizes the encryption process and returns the result.
*	@param ctx The OpenSSL encryption context.
*	@param encryptedData The encryted data.
*	@param encryptedDataLen The length of the encrypted data.
*	@return True if the encryption is finalized successfully, false otherwise.
*/
bool EncryptModule::finalizeEncryption(EVP_CIPHER_CTX* ctx, unsigned char* encryptedData, int& encryptedDataLen)
{
	return EVP_EncryptFinal(ctx, encryptedData + encryptedDataLen, &encryptedDataLen) == 1;
}

/*
*	@brief Encrypts the given data using the provided encryption context.
*	@param ctx The encryption context to use for encrypting the data.
*	@param encryptedData The buffer to store the encrypted data in.
*	@param encryptedDataLen The length of the encrypted data.
*	@param data The data to encrypt.
*	@param dataLen The length of the data to encrypt.
*	@return True if the data was succesfully encrypted, false otherwise.
*/
bool EncryptModule::encryptData(EVP_CIPHER_CTX* ctx, unsigned char* encryptedData, int& encryptedDataLen, unsigned char* data, int dataLen)
{
	return EVP_EncryptUpdate(ctx, encryptedData, &encryptedDataLen, data, dataLen) == 1;
}

bool EncryptModule::updatePacketChecksum(Node* node)
{
	return WinDivertHelperCalcChecksums(node->packet.getPacket(), node->packet.getPacketLen(), NULL, 0);
}

void EncryptModule::cleanup(EVP_CIPHER_CTX* ctx, unsigned char* encryptedData)
{
	EVP_CIPHER_CTX_free(ctx);
	delete[] encryptedData;
}

/*
*	@brief Initialize the encryption context for OpenSSL.
*	@param ctx Pointer to the encryption context.
*	@param key The encryption key.
*	@param iv The initialization vector.
*	@return True if the initialization was successful, false otherwise.
*/
bool EncryptModule::initializeEncryptionContext(EVP_CIPHER_CTX* ctx, unsigned char* key, unsigned char* iv)
{
	// Initialize the encryption context with the given key and IV using the AES-128-CBC
	// algorithm.
	return EVP_EncryptInit_ex(ctx, EVP_aes_128_cbc(), NULL, key, iv) == 1;
}

/*
*	@brief Generates a random key and initialization vector (IV) using CryptGenRandom function.
*	@param key Output buffer where the generated key is stored.
*	@param iv Output buffer where the generated IV is stored.
*	@return True if the key and IV are generated successfully, false otherwise.
*/
bool EncryptModule::generateRandomKeyAndIV(unsigned char* key, unsigned char* iv)
{
	// Declare and initialize necessary variables
	HCRYPTPROV hCryptProv = NULL;
	bool result = true;

	// Attempt to acquire a handle to the default provider
	if (!CryptAcquireContext(&hCryptProv, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT | CRYPT_SILENT))
	{
		int error = (int)GetLastError();
		// Handle error
		sendMessage("Error: failed to acquire handle to default provider.");
		std::stringstream messageStream;
		messageStream << "Error code: " << error;
		sendMessageEx(messageStream);
		result = false;
	}
	else
	{
		// Generate random key and IV using CryptGenRandom function
		if (!CryptGenRandom(hCryptProv, KEY_LEN, key))
		{
			// Handle error
			sendMessage("Error: failed to generate random key");
			result = false;
		}

		if (!CryptGenRandom(hCryptProv, IV_LEN, iv))
		{
			// Handle error
			sendMessage("Error: failed to generate random IV");
			result = false;
		}

		// Release the handle to the provider
		CryptReleaseContext(hCryptProv, 0);
	}

	return result;
}


bool EncryptModule::shouldKeepEncrypting(Node*& current)
{
	return current != packetList.getHead();
}

bool EncryptModule::shouldEncrypt(Node*& current)
{
	return checkDirection(current->packet.getPacketAddress()->Outbound, getInbound(), getOutbound())
		&& calcChance(getChance());
}

void EncryptModule::closeDown()
{
	Module::closeDown();
}