#ifndef TOKEN_H
#define TOKEN_H
#include "serialize.h"
#include <string>
#include "streams.h"
class GenesisToken
{
public:
	std::string strProtocol;
	uint8_t chTokenType;
	std::string strTokenType;
	std::string strShortName;
	std::string strFullName;
	std::string strTokenUrl;
	std::string strWhitePaperHash;
	uint8_t chPrecision;
	uint8_t chBatonVout;
	uint64_t nTotal;

	GenesisToken()
	{
		SetNull();
	}

	ADD_SERIALIZE_METHODS;
	
	template <typename Stream, typename Operation>
    inline void SerializationOp(Stream &s, Operation ser_action)
    {
        // Legacy 32 bit sizes used for reading and writing.
        // When writing size larger or equal than max 32 bit value,
        // max 32 bit value (0xFFFFFFFF) is written in 32 bit field
        // and actual size is written in separate 64 bit field.
        // When reading, separate 64 bit value should be read when 32 bit value
        // is max (0xFFFFFFFF).
        READWRITE(strProtocol);
        READWRITE(chTokenType);
        READWRITE(strTokenType);
        READWRITE(strShortName);
        READWRITE(strFullName);
        READWRITE(strTokenUrl);
        READWRITE(strWhitePaperHash);
        READWRITE(chPrecision);
        READWRITE(chBatonVout);
        READWRITE(nTotal);
      }

	void SetNull() 
	{
	}

	bool IsNull() const { return 0; }
	
	bool operator==(const GenesisToken& rhs)
    {
		return true;
    }


};

class MintToken
{
public:
	std::string strProtocol;
	uint8_t chTokenType;
	std::string strTokenType;
	std::string strTokenId;
	uint8_t chPrecision;
	uint8_t chBatonVout;
	uint64_t nMintTotal;
	MintToken()
	{
		SetNull();
	}

	ADD_SERIALIZE_METHODS;
	
	template <typename Stream, typename Operation>
    inline void SerializationOp(Stream &s, Operation ser_action)
    {
        // Legacy 32 bit sizes used for reading and writing.
        // When writing size larger or equal than max 32 bit value,
        // max 32 bit value (0xFFFFFFFF) is written in 32 bit field
        // and actual size is written in separate 64 bit field.
        // When reading, separate 64 bit value should be read when 32 bit value
        // is max (0xFFFFFFFF).
        READWRITE(strProtocol);
        READWRITE(chTokenType);
        READWRITE(strTokenType);
        READWRITE(strTokenId);
		READWRITE(chPrecision);
        READWRITE(chBatonVout);
        READWRITE(nMintTotal);
      }

	void SetNull() 
	{
	}

	bool IsNull() const { return 0; }
	
	bool operator==(const GenesisToken& rhs)
    {
		return true;
    }


};

class SendToken
{
public:
	std::string strProtocol;
	uint8_t chTokenType;
	std::string strTokenType;
	std::string strTokenId;
	std::vector<uint8_t> vectPrecition;
	std::vector<uint8_t> vectTokenOutput;
	std::vector<uint64_t> vectTokenAmount;

	SendToken()
	{
		SetNull();
	}

	ADD_SERIALIZE_METHODS;
	
	template <typename Stream, typename Operation>
    inline void SerializationOp(Stream &s, Operation ser_action)
    {
        // Legacy 32 bit sizes used for reading and writing.
        // When writing size larger or equal than max 32 bit value,
        // max 32 bit value (0xFFFFFFFF) is written in 32 bit field
        // and actual size is written in separate 64 bit field.
        // When reading, separate 64 bit value should be read when 32 bit value
        // is max (0xFFFFFFFF).
        READWRITE(strProtocol);	
        READWRITE(chTokenType);
        READWRITE(strTokenType);
        READWRITE(strTokenId);
		READWRITE(vectPrecition);
		READWRITE(vectTokenOutput);
		READWRITE(vectTokenAmount);
		
      }

	void SetNull() 
	{
	}

	bool IsNull() const { return 0; }
	
	bool operator==(const SendToken& rhs)
    {
		return true;
    }


};
#endif
