#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <time.h>

#ifdef WIN32
#include <winsock.h>
#include <stdlib.h>
#include <io.h>
#include <time.h>
#else

#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h> 
#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/nameser.h>
#include <resolv.h>
#include <net/if.h>
#endif
#include "udp.h"
#include "stun.h"
#include "Common.h"

#if defined(__sparc__) || defined(WIN32)
#define NOSSL
#endif
#define NOSSL

static void
computeHmac(char* hmac, const char* input, int length, const char* key, int keySize);

static BOOL 
stunParseAtrAddress( char* body, unsigned int hdrLen,  StunAtrAddress4* result )
{
	UInt16 nport;
    UInt32 naddr;

   if ( hdrLen != 8 )
   {
	   fprintf(stderr, "hdrLen wrong for Address\n");
      return FALSE;
   }
   result->pad = *body++;
   result->family = *body++;
   if (result->family == IPv4Family)
   {

      memcpy(&nport, body, 2); body+=2;
      result->ipv4.port = ntohs(nport);
		

      memcpy(&naddr, body, 4); body+=4;
      result->ipv4.addr = ntohl(naddr);
      return TRUE;
   }
   else if (result->family == IPv6Family)
   {
	   fprintf(stderr, "ipv6 not supported\n");
   }
   else
   {
	   fprintf(stderr, "bad address family: %d\n", result->family);
   }
	
   return FALSE;
}

static BOOL 
stunParseAtrChangeRequest( char* body, unsigned int hdrLen, StunAtrChangeRequest* result )
{
   if ( hdrLen != 4 )
   {
	   fprintf(stderr, "hdr length = %d expecting %d\n", hdrLen, sizeof(*result));
       // clog << "hdr length = " << hdrLen << " expecting " << sizeof(result) << endl;
	   fprintf(stderr, "Incorrect size for ChangeRequest\n");
       //clog << "Incorrect size for ChangeRequest" << endl;
      return FALSE;
   }
   else
   {
      memcpy(&(result->value), body, 4);
      result->value = ntohl(result->value);
      return TRUE;
   }
}

static BOOL 
stunParseAtrError( char* body, unsigned int hdrLen,  StunAtrError* result )
{
   if ( hdrLen >= sizeof(*result) )
   {
	   fprintf(stderr, "head on Error too large\n");
       //clog << "head on Error too large" << endl;
      return FALSE;
   }
   else
   {
      memcpy(&result->pad, body, 2); body+=2;
      result->pad = ntohs(result->pad);
      result->errorClass = *body++;
      result->number = *body++;
		
      result->sizeReason = hdrLen - 4;
      memcpy(&result->reason, body, result->sizeReason);
      result->reason[result->sizeReason] = 0;
      return TRUE;
   }
}

static BOOL 
stunParseAtrUnknown( char* body, unsigned int hdrLen,  StunAtrUnknown* result )
{
	int i;
   if ( hdrLen >= sizeof(*result) )
   {
      return FALSE;
   }
   else
   {
      if (hdrLen % 4 != 0) return FALSE;
      result->numAttributes = hdrLen / 4;
      for (i=0; i<result->numAttributes; i++)
      {
         memcpy(&result->attrType[i], body, 2); body+=2;
         result->attrType[i] = ntohs(result->attrType[i]);
      }
      return TRUE;
   }
}


static BOOL 
stunParseAtrString( char* body, unsigned int hdrLen,  StunAtrString* result )
{
   if ( hdrLen >= STUN_MAX_STRING )
   {
	   fprintf(stderr, "String is too large\n");
       //clog << "String is too large" << endl;
      return FALSE;
   }
   else
   {
      if (hdrLen % 4 != 0)
      {
		  fprintf(stderr, "Bad length string %d\n", hdrLen);
          //clog << "Bad length string " << hdrLen << endl;
         return FALSE;
      }
		
      result->sizeValue = hdrLen;
      memcpy(&result->value, body, hdrLen);
      result->value[hdrLen] = 0;
      return TRUE;
   }
}


static BOOL 
stunParseAtrIntegrity( char* body, unsigned int hdrLen,  StunAtrIntegrity* result )
{
   if ( hdrLen != 20)
   {
	   fprintf(stderr, "MessageIntegrity must be 20 bytes\n");
       //clog << "MessageIntegrity must be 20 bytes" << endl;
      return FALSE;
   }
   else
   {
      memcpy(&result->hash, body, hdrLen);
      return TRUE;
   }
}


BOOL
stunParseMessage( char* buf, unsigned int bufLen, StunMessage* msg, BOOL verbose)
{
   char* body;
   unsigned int size;
   StunAtrHdr* attr;
   unsigned int attrLen;
   int atrType;

   if (verbose) 
	   fprintf(stderr, "Received stun message: %d bytes\n", bufLen);
	   //clog << "Received stun message: " << bufLen << " bytes" << endl;

   memset(msg, 0, sizeof(*msg));
	
   if (sizeof(StunMsgHdr) > bufLen)
   {
	   fprintf(stderr, "Bad message\n");
       //clog << "Bad message" << endl;
      return FALSE;
   }
	
   memcpy(&msg->msgHdr, buf, sizeof(StunMsgHdr));
   msg->msgHdr.msgType = ntohs(msg->msgHdr.msgType);
   msg->msgHdr.msgLength = ntohs(msg->msgHdr.msgLength);
	
   if (msg->msgHdr.msgLength + sizeof(StunMsgHdr) != bufLen)
   {
	   fprintf(stderr, "Message header length doesn't match message size: %d - %d\n", msg->msgHdr.msgLength, bufLen);
       //clog << "Message header length doesn't match message size: " << msg.msgHdr.msgLength << " - " << bufLen << endl;
      return FALSE;
   }
	
   body = buf + sizeof(StunMsgHdr);
   size = msg->msgHdr.msgLength;
	
   //clog << "bytes after header = " << size << endl;
	
   while ( size > 0 )
   {
      // !jf! should check that there are enough bytes left in the buffer
		
      attr = (StunAtrHdr*)(body);
		
      attrLen = ntohs(attr->length);
      atrType = ntohs(attr->type);
		
      //if (verbose) clog << "Found attribute type=" << AttrNames[atrType] << " length=" << attrLen << endl;
      if ( attrLen+4 > size ) 
      {
		  fprintf(stderr, "claims attribute is larger than size of message (attribute type=%d)\n", atrType);
         //clog << "claims attribute is larger than size of message" <<"(attribute type="<<atrType<<")"<< endl;
         return FALSE;
      }
		
      body += 4; // skip the length and type in attribute header
      size -= 4;
		
      switch ( atrType )
      {
         case STUN_MappedAddress:
            msg->hasMappedAddress = TRUE;
            if ( stunParseAtrAddress(  body,  attrLen,  &msg->mappedAddress )== FALSE )
            {
				fprintf(stderr, "problem parsing MappedAddress\n");
               //clog << "problem parsing MappedAddress" << endl;
               return FALSE;
            }
            else
            {
               //if (verbose) 
			   //   clog << "MappedAddress = " << msg.mappedAddress.ipv4 << endl;
            }
					
            break;  

         case STUN_ResponseAddress:
            msg->hasResponseAddress = TRUE;
            if ( stunParseAtrAddress(  body,  attrLen,  &msg->responseAddress )== FALSE )
            {
				fprintf(stderr, "problem parsing ResponseAddress\n");
               //clog << "problem parsing ResponseAddress" << endl;
               return FALSE;
            }
            else
            {
               //if (verbose) clog << "ResponseAddress = " << msg.responseAddress.ipv4 << endl;
            }
            break;  
				
         case STUN_ChangeRequest:
            msg->hasChangeRequest = TRUE;
            if (stunParseAtrChangeRequest( body, attrLen, &msg->changeRequest) == FALSE)
            {
				fprintf(stderr, "problem parsing ChangeRequest\n");
                //clog << "problem parsing ChangeRequest" << endl;
               return FALSE;
            }
            else
            {
               //if (verbose) clog << "ChangeRequest = " << msg.changeRequest.value << endl;
            }
            break;
				
         case STUN_SourceAddress:
            msg->hasSourceAddress = TRUE;
            if ( stunParseAtrAddress(  body,  attrLen,  &msg->sourceAddress )== FALSE )
            {
				fprintf(stderr, "problem parsing SourceAddress\n");
               //clog << "problem parsing SourceAddress" << endl;
               return FALSE;
            }
            else
            {
               //if (verbose) clog << "SourceAddress = " << msg.sourceAddress.ipv4 << endl;
            }
            break;  
				
         case STUN_ChangedAddress:
            msg->hasChangedAddress = TRUE;
            if ( stunParseAtrAddress(  body,  attrLen,  &msg->changedAddress )== FALSE )
            {
				fprintf(stderr, "problem parsing ChangedAddress\n");
               //clog << "problem parsing ChangedAddress" << endl;
               return FALSE;
            }
            else
            {
               //if (verbose) clog << "ChangedAddress = " << msg.changedAddress.ipv4 << endl;
            }
            break;  
				
         case STUN_Username: 
            msg->hasUsername = TRUE;
            if (stunParseAtrString( body, attrLen, &msg->username) == FALSE)
            {
				fprintf(stderr, "problem parsing Username\n");
               //clog << "problem parsing Username" << endl;
               return FALSE;
            }
            else
            {
               //if (verbose) clog << "Username = " << msg.username.value << endl;
            }
					
            break;
				
         case STUN_Password: 
            msg->hasPassword = TRUE;
            if (stunParseAtrString( body, attrLen,&msg->password) == FALSE)
            {
				fprintf(stderr, "problem parsing Password\n");
               //clog << "problem parsing Password" << endl;
               return FALSE;
            }
            else
            {
               //if (verbose) clog << "Password = " << msg.password.value << endl;
            }
            break;
				
         case STUN_MessageIntegrity:
            msg->hasMessageIntegrity = TRUE;
            if (stunParseAtrIntegrity( body, attrLen, &msg->messageIntegrity) == FALSE)
            {
				fprintf(stderr, "problem parsing MessageIntegrity\n");
               //clog << "problem parsing MessageIntegrity" << endl;
               return FALSE;
            }
            else
            {
               //if (verbose) clog << "MessageIntegrity = " << msg.messageIntegrity.hash << endl;
            }
					
            // read the current HMAC
            // look up the password given the user of given the transaction id 
            // compute the HMAC on the buffer
            // decide if they match or not
            break;
				
         case STUN_ErrorCode:
            msg->hasErrorCode = TRUE;
            if (stunParseAtrError(body, attrLen, &msg->errorCode) == FALSE)
            {
				fprintf(stderr, "problem parsing ErrorCode\n");
                //clog << "problem parsing ErrorCode" << endl;
               return FALSE;
            }
            else
            {
               /*if (verbose) clog << "ErrorCode = " << int(msg.errorCode.errorClass) 
                                 << " " << int(msg.errorCode.number) 
                                 << " " << msg.errorCode.reason << endl;*/
            }
					
            break;
				
         case STUN_UnknownAttribute:
            msg->hasUnknownAttributes = TRUE;
            if (stunParseAtrUnknown(body, attrLen, &msg->unknownAttributes) == FALSE)
            {
				fprintf(stderr, "problem parsing UnknownAttribute\n");
                //clog << "problem parsing UnknownAttribute" << endl;
               return FALSE;
            }
            break;
				
         case STUN_ReflectedFrom:
            msg->hasReflectedFrom = TRUE;
            if ( stunParseAtrAddress(  body,  attrLen,  &msg->reflectedFrom ) == FALSE )
            {
				fprintf(stderr, "problem parsing ReflectedForm\n");
               //clog << "problem parsing ReflectedFrom" << endl;
               return FALSE;
            }
            break;  
				
         case STUN_XorMappedAddress:
            msg->hasXorMappedAddress = TRUE;
            if ( stunParseAtrAddress(  body,  attrLen,  &msg->xorMappedAddress ) == FALSE )
            {
				fprintf(stderr, "problem parsing XorMappedAddress\n");
               //clog << "problem parsing XorMappedAddress" << endl;
               return FALSE;
            }
            else
            {
               //if (verbose) clog << "XorMappedAddress = " << msg.mappedAddress.ipv4 << endl;
            }
            break;  

         case STUN_XorOnly:
            msg->xorOnly = TRUE;
            if (verbose) 
            {
				fprintf(stderr, "xorOnly = TRUE\n");
               //clog << "xorOnly = TRUE" << endl;
            }
            break;  
				
         case STUN_ServerName: 
            msg->hasServerName = TRUE;
            if (stunParseAtrString( body, attrLen, &msg->serverName) == FALSE)
            {
				fprintf(stderr, "problem parsing ServerName\n");
               //clog << "problem parsing ServerName" << endl;
               return FALSE;
            }
            else
            {
               //if (verbose) clog << "ServerName = " << msg.serverName.value << endl;
            }
            break;
				
         case STUN_SecondaryAddress:
            msg->hasSecondaryAddress = TRUE;
            if ( stunParseAtrAddress(  body,  attrLen,  &msg->secondaryAddress ) == FALSE )
            {
				fprintf(stderr, "problem parsing secondaryAddress\n");
               //clog << "problem parsing secondaryAddress" << endl;
               return FALSE;
            }
            else
            {
               //if (verbose) clog << "SecondaryAddress = " << msg.secondaryAddress.ipv4 << endl;
            }
            break;  
					
         default:
            if (verbose) 
				fprintf(stderr, "Unknown attribute: %d\n", atrType);
				//clog << "Unknown attribute: " << atrType << endl;
            if ( atrType <= 0x7FFF ) 
            {
               return FALSE;
            }
      }
		
      body += attrLen;
      size -= attrLen;
   }
    
   return TRUE;
}


static char* 
encode16(char* buf, UInt16 data)
{
   UInt16 ndata = htons(data);
   memcpy(buf, (void*)(&ndata), sizeof(UInt16));
   return buf + sizeof(UInt16);
}

static char* 
encode32(char* buf, UInt32 data)
{
   UInt32 ndata = htonl(data);
   memcpy(buf, (void*)(&ndata), sizeof(UInt32));
   return buf + sizeof(UInt32);
}


static char* 
encode(char* buf, const char* data, unsigned int length)
{
   memcpy(buf, data, length);
   return buf + length;
}


static char* 
encodeAtrAddress4(char* ptr, UInt16 type, const StunAtrAddress4* atr)
{
   ptr = encode16(ptr, type);
   ptr = encode16(ptr, 8);
   *ptr++ = atr->pad;
   *ptr++ = IPv4Family;
   ptr = encode16(ptr, atr->ipv4.port);
   ptr = encode32(ptr, atr->ipv4.addr);
	
   return ptr;
}

static char* 
encodeAtrChangeRequest(char* ptr, const StunAtrChangeRequest* atr)
{
   ptr = encode16(ptr, STUN_ChangeRequest);
   ptr = encode16(ptr, 4);
   ptr = encode32(ptr, atr->value);
   return ptr;
}

static char* 
encodeAtrError(char* ptr, const StunAtrError* atr)
{
   ptr = encode16(ptr, STUN_ErrorCode);
   ptr = encode16(ptr, 6 + atr->sizeReason);
   ptr = encode16(ptr, atr->pad);
   *ptr++ = atr->errorClass;
   *ptr++ = atr->number;
   ptr = encode(ptr, atr->reason, atr->sizeReason);
   return ptr;
}


static char* 
encodeAtrUnknown(char* ptr, const StunAtrUnknown* atr)
{
   int i;
   ptr = encode16(ptr, STUN_UnknownAttribute);
   ptr = encode16(ptr, 2+2*atr->numAttributes);
   for (i=0; i<atr->numAttributes; i++)
   {
      ptr = encode16(ptr, atr->attrType[i]);
   }
   return ptr;
}


static char* 
encodeXorOnly(char* ptr)
{
   ptr = encode16(ptr, STUN_XorOnly );
   return ptr;
}


static char* 
encodeAtrString(char* ptr, UInt16 type, const StunAtrString* atr)
{
   assert(atr->sizeValue % 4 == 0);
	
   ptr = encode16(ptr, type);
   ptr = encode16(ptr, atr->sizeValue);
   ptr = encode(ptr, atr->value, atr->sizeValue);
   return ptr;
}


static char* 
encodeAtrIntegrity(char* ptr, const StunAtrIntegrity* atr)
{
   ptr = encode16(ptr, STUN_MessageIntegrity);
   ptr = encode16(ptr, 20);
   ptr = encode(ptr, atr->hash, sizeof(atr->hash));
   return ptr;
}


unsigned int
stunEncodeMessage( const StunMessage* msg, 
                   char* buf, 
                   unsigned int bufLen, 
                   const StunAtrString* password, 
                   BOOL verbose)
{
   char* ptr = buf;
   char* lengthp;
   StunAtrIntegrity integrity;

   assert(bufLen >= sizeof(StunMsgHdr));
   	
   ptr = encode16(ptr, msg->msgHdr.msgType);
   lengthp = ptr;
   ptr = encode16(ptr, 0);
   ptr = encode(ptr, (const char*)(msg->msgHdr.id.octet), sizeof(msg->msgHdr.id));
	
   //if (verbose) clog << "Encoding stun message: " << endl;
   if (msg->hasMappedAddress)
   {
      //if (verbose) clog << "Encoding MappedAddress: " << msg->mappedAddress.ipv4 << endl;
      ptr = encodeAtrAddress4 (ptr, STUN_MappedAddress, &msg->mappedAddress);
   }
   if (msg->hasResponseAddress)
   {
      //if (verbose) clog << "Encoding ResponseAddress: " << msg->responseAddress.ipv4 << endl;
      ptr = encodeAtrAddress4(ptr, STUN_ResponseAddress, &msg->responseAddress);
   }
   if (msg->hasChangeRequest)
   {
      //if (verbose) clog << "Encoding ChangeRequest: " << msg->changeRequest.value << endl;
      ptr = encodeAtrChangeRequest(ptr, &msg->changeRequest);
   }
   if (msg->hasSourceAddress)
   {
      //if (verbose) clog << "Encoding SourceAddress: " << msg->sourceAddress.ipv4 << endl;
      ptr = encodeAtrAddress4(ptr, STUN_SourceAddress, &msg->sourceAddress);
   }
   if (msg->hasChangedAddress)
   {
      //if (verbose) clog << "Encoding ChangedAddress: " << msg->changedAddress.ipv4 << endl;
      ptr = encodeAtrAddress4(ptr, STUN_ChangedAddress, &msg->changedAddress);
   }
   if (msg->hasUsername)
   {
      //if (verbose) clog << "Encoding Username: " << msg->username.value << endl;
      ptr = encodeAtrString(ptr, STUN_Username, &msg->username);
   }
   if (msg->hasPassword)
   {
      //if (verbose) clog << "Encoding Password: " << msg->password->value << endl;
      ptr = encodeAtrString(ptr, STUN_Password, &msg->password);
   }
   if (msg->hasErrorCode)
   {
      /*if (verbose) clog << "Encoding ErrorCode: class=" 
			<< int(msg->errorCode.errorClass)  
			<< " number=" << int(msg->errorCode.number) 
			<< " reason=" 
			<< msg->errorCode.reason 
			<< endl;*/
		
      ptr = encodeAtrError(ptr, &msg->errorCode);
   }
   if (msg->hasUnknownAttributes)
   {
      //if (verbose) clog << "Encoding UnknownAttribute: ???" << endl;
      ptr = encodeAtrUnknown(ptr, &msg->unknownAttributes);
   }
   if (msg->hasReflectedFrom)
   {
      //if (verbose) clog << "Encoding ReflectedFrom: " << msg->reflectedFrom.ipv4 << endl;
      ptr = encodeAtrAddress4(ptr, STUN_ReflectedFrom, &msg->reflectedFrom);
   }
   if (msg->hasXorMappedAddress)
   {
      //if (verbose) clog << "Encoding XorMappedAddress: " << msg->xorMappedAddress.ipv4 << endl;
      ptr = encodeAtrAddress4 (ptr, STUN_XorMappedAddress, &msg->xorMappedAddress);
   }
   if (msg->xorOnly)
   {
      //if (verbose) clog << "Encoding xorOnly: " << endl;
      ptr = encodeXorOnly( ptr );
   }
   if (msg->hasServerName)
   {
      //if (verbose) clog << "Encoding ServerName: " << msg->serverName.value << endl;
      ptr = encodeAtrString(ptr, STUN_ServerName, &msg->serverName);
   }
   if (msg->hasSecondaryAddress)
   {
      //if (verbose) clog << "Encoding SecondaryAddress: " << msg->secondaryAddress.ipv4 << endl;
      ptr = encodeAtrAddress4 (ptr, STUN_SecondaryAddress, &msg->secondaryAddress);
   }

   if (password->sizeValue > 0)
   {
      //if (verbose) clog << "HMAC with password: " << password->value << endl;

      computeHmac(integrity.hash, buf, (int)(ptr-buf) , password->value, password->sizeValue);
      ptr = encodeAtrIntegrity(ptr, &integrity);
   }
   //if (verbose) clog << endl;
	
   encode16(lengthp, (UInt16)(ptr - buf - sizeof(StunMsgHdr)));
   return (int)(ptr - buf);
}

int 
stunRand()
{
   // return 32 bits of random stuff
   static BOOL init=FALSE;
   UInt64 tick;
   int seed;
#if defined(WIN32)
   volatile unsigned int lowtick=0,hightick=0;
   int r1,r2,ret;
#endif
#if defined(__MACH__)
   int fd;
#endif

   assert( sizeof(int) == 4 );
   
   if ( !init )
   { 
      init = TRUE;
			
#if defined(WIN32) 
      __asm
         {
            rdtsc 
               mov lowtick, eax
               mov hightick, edx
               }
      tick = hightick;
      tick <<= 32;
      tick |= lowtick;
#elif defined(__GNUC__) && ( defined(__i686__) || defined(__i386__) )
      asm("rdtsc" : "=A" (tick));
#elif defined (__SUNPRO_CC) || defined( __sparc__ )	
      tick = gethrtime();
#elif defined(__MACH__) 
      fd=open("/dev/random",O_RDONLY);
      read(fd,&tick,sizeof(tick));
      closesocket(fd);
#else
//     error Need some way to seed the random number generator 
#endif 
     seed = (int)(tick);
#ifdef WIN32
      srand(seed);
#else
      srandom(seed);
#endif
   }
	
#ifdef WIN32
   assert( RAND_MAX == 0x7fff );
   r1 = rand();
   r2 = rand();
	
   ret = (r1<<16) + r2;
	
   return ret;
#else
   return random(); 
#endif
}


/// return a random number to use as a port 
int
stunRandomPort()
{
   int min=0x4000;
   int max=0x7FFF;
	
   int ret = stunRand();
   ret = ret|min;
   ret = ret&max;
	
   return ret;
}


#ifdef NOSSL
static void
computeHmac(char* hmac, const char* input, int length, const char* key, int sizeKey)
{
   strncpy(hmac,"hmac-not-implemented",20);
}
#else
#include <openssl/hmac.h>

static void
computeHmac(char* hmac, const char* input, int length, const char* key, int sizeKey)
{
   unsigned int resultSize=0;
   HMAC(EVP_sha1(), 
        key, sizeKey, 
        (const unsigned char*)(input), length, 
        (unsigned char*)(hmac), &resultSize);
   assert(resultSize == 20);
}
#endif


static void
toHex(const char* buffer, int bufferSize, char* output) 
{
   static char hexmap[] = "0123456789abcdef";
	
   const char* p = buffer;
   char* r = output;
   unsigned char temp;
   int i, hi, low;

   for (i=0; i < bufferSize; i++)
   {
      temp = *p++;
		
      hi = (temp & 0xf0)>>4;
      low = (temp & 0xf);
		
      *r++ = hexmap[hi];
      *r++ = hexmap[low];
   }
   *r = 0;
}

void
stunCreateUserName(const StunAddress4* source, StunAtrString* username)
{
   UInt64 time = stunGetSystemTimeSecs();
   UInt64 lotime = time & 0xFFFFFFFF;
   char buffer[1024];
   char hmac[20];
   char key[] = "Jason";
   char hmacHex[41];
   int l;

   time -= (time % 20*60);
	
   sprintf(buffer,
           "%08x:%08x:%08x:", 
           (UInt32)(source->addr),
           (UInt32)(stunRand()),
           (UInt32)(lotime));
   assert( strlen(buffer) < 1024 );
	
   assert(strlen(buffer) + 41 < STUN_MAX_STRING);
	
   
   computeHmac(hmac, buffer, strlen(buffer), key, strlen(key) );
   
   toHex(hmac, 20, hmacHex );
   hmacHex[40] =0;
   strcat(buffer,hmacHex);
	
   l = strlen(buffer);
   assert( l+1 < STUN_MAX_STRING );
   assert( l%4 == 0 );
   
   username->sizeValue = l;
   memcpy(username->value,buffer,l);
   username->value[l]=0;
}

void
stunCreatePassword(const StunAtrString* username, StunAtrString* password)
{
   char hmac[20];
   char key[] = "Fluffy";
   //char buffer[STUN_MAX_STRING];
   computeHmac(hmac, username->value, strlen(username->value), key, strlen(key));
   toHex(hmac, 20, password->value);
   password->sizeValue = 40;
   password->value[40]=0;
	
   //clog << "password=" << password->value << endl;
}


UInt64
stunGetSystemTimeSecs()
{
   UInt64 time=0;
#if defined(WIN32)  
   SYSTEMTIME t;
   // CJ TODO - this probably has bug on wrap around every 24 hours
   GetSystemTime( &t );
   time = (t.wHour*60+t.wMinute)*60+t.wSecond; 
#else
   struct timeval now;
   gettimeofday( &now , NULL );
   //assert( now );
   time = now.tv_sec;
#endif
   return time;
}


/*ostream& operator<< ( ostream& strm, const UInt128& r )
{
   strm << int(r.octet[0]);
   for ( int i=1; i<16; i++ )
   {
      strm << ':' << int(r.octet[i]);
   }
    
   return strm;
}*/

/*ostream& 
operator<<( ostream& strm, const StunAddress4& addr)
{
   UInt32 ip = addr.addr;
   strm << ((int)(ip>>24)&0xFF) << ".";
   strm << ((int)(ip>>16)&0xFF) << ".";
   strm << ((int)(ip>> 8)&0xFF) << ".";
   strm << ((int)(ip>> 0)&0xFF) ;
	
   strm << ":" << addr.port;
	
   return strm;
}*/


// returns TRUE if it scucceeded
BOOL 
stunParseHostName( char* peerName,
               UInt32* ip,
               UInt16* portVal,
               UInt16 defaultPort )
{
   struct in_addr sin_addr; 
   char host[MAXURLSIZE];
   char* port = NULL;
   char* sep;
   int portNum;
   char* endPtr=NULL;
   
   // figure out the host part 
   struct hostent* h;
   	int err;

#ifdef WIN32
	unsigned long a;
#endif

   strncpy(host, peerName, MAXURLSIZE);
   host[MAXURLSIZE-1]='\0';
   	
   portNum = defaultPort;
	
   // pull out the port part if present.
   sep = strchr(host,':');
	
   if ( sep == NULL )
   {
      portNum = defaultPort;
   }
   else
   {
      *sep = '\0';
      port = sep + 1;
      // set port part
		
      portNum = strtol(port,&endPtr,10);
		
      if ( endPtr != NULL )
      {
         if ( *endPtr != '\0' )
         {
            portNum = defaultPort;
         }
      }
   }
    
   if ( portNum < 1024 ) return FALSE;
   if ( portNum >= 0xFFFF ) return FALSE;
	
#ifdef WIN32
   assert( strlen(host) >= 1 );
   if ( isdigit( host[0] ) )
   {
      // assume it is a ip address 
      a = inet_addr(host);
      //cerr << "a=0x" << hex << a << dec << endl;
		
      *ip = ntohl( a );
   }
   else
   {
      // assume it is a host name 
      h = gethostbyname( host );
		
      if ( h == NULL )
      {
         err = getErrno();
		 fprintf(stderr, "error was %d\n", err);
         //std::cerr << "error was " << err << std::endl;
         assert( err != WSANOTINITIALISED );
			
         *ip = ntohl( 0x7F000001L );
			
         return FALSE;
      }
      else
      {
         sin_addr = *(struct in_addr*)h->h_addr;
         *ip = ntohl( sin_addr.s_addr );
      }
   }
	
#else
   h = gethostbyname( host );
   if ( h == NULL )
   {
      err = getErrno();
	  fprintf(stderr, "error was %d\n", err);
      //std::cerr << "error was " << err << std::endl;
      *ip = ntohl( 0x7F000001L );
      return FALSE;
   }
   else
   {
      sin_addr = *(struct in_addr*)h->h_addr;
      *ip = ntohl( sin_addr.s_addr );
   }
#endif
	
   *portVal = portNum;
	
   return TRUE;
}


BOOL
stunParseServerName( char* name, StunAddress4* addr)
{
   // TODO - put in DNS SRV stuff.
	
   BOOL ret = stunParseHostName( name, &addr->addr, &addr->port, 3478); 
   if ( ret != TRUE ) 
   {
       addr->port=0xFFFF;
   }	
   return ret;
}


/*static void
stunCreateErrorResponse(StunMessage* response, int cl, int number, const char* msg)
{
   response->msgHdr.msgType = BindErrorResponseMsg;
   response->hasErrorCode = TRUE;
   response->errorCode.errorClass = cl;
   response->errorCode.number = number;
   strcpy(response->errorCode.reason, msg);
}*/

/*static void
stunCreateSharedSecretResponse(const StunMessage& request, const StunAddress4& source, StunMessage& response)
{
   response.msgHdr.msgType = SharedSecretResponseMsg;
   response.msgHdr.id = request.msgHdr.id;
	
   response.hasUsername = TRUE;
   stunCreateUserName( source, &response.username);
	
   response.hasPassword = TRUE;
   stunCreatePassword( response.username, &response.password);
}*/


// This funtion takes a single message sent to a stun server, parses
// and constructs an apropriate repsonse - returns TRUE if message is
// valid
/*BOOL
stunServerProcessMsg( char* buf,
                      unsigned int bufLen,
                      StunAddress4& from, 
                      StunAddress4& secondary,
                      StunAddress4& myAddr,
                      StunAddress4& altAddr, 
                      StunMessage* resp,
                      StunAddress4* destination,
                      StunAtrString* hmacPassword,
                      BOOL* changePort,
                      BOOL* changeIp,
                      BOOL verbose)
{
    
   // set up information for default response 
	
   memset( resp, 0 , sizeof(*resp) );
	
   *changeIp = FALSE;
   *changePort = FALSE;
	
   StunMessage req;
   BOOL ok = stunParseMessage( buf,bufLen, req, verbose);
	
   if (!ok)      // Complete garbage, drop it on the floor
   {
      if (verbose) clog << "Request did not parse" << endl;
      return FALSE;
   }
   if (verbose) clog << "Request parsed ok" << endl;
	
   StunAddress4 mapped = req.mappedAddress.ipv4;
   StunAddress4 respondTo = req.responseAddress.ipv4;
   UInt32 flags = req.changeRequest.value;
	
   switch (req.msgHdr.msgType)
   {
      case SharedSecretRequestMsg:
         if(verbose) clog << "Received SharedSecretRequestMsg on udp. send error 433." << endl;
         // !cj! - should fix so you know if this came over TLS or UDP
         stunCreateSharedSecretResponse(req, from, *resp);
         //stunCreateSharedSecretErrorResponse(*resp, 4, 33, "this request must be over TLS");
         return TRUE;
			
      case BindRequestMsg:
         if (!req.hasMessageIntegrity)
         {
            if (verbose) clog << "BindRequest does not contain MessageIntegrity" << endl;
				
            if (0) // !jf! mustAuthenticate
            {
               if(verbose) clog << "Received BindRequest with no MessageIntegrity. Sending 401." << endl;
               stunCreateErrorResponse(*resp, 4, 1, "Missing MessageIntegrity");
               return TRUE;
            }
         }
         else
         {
            if (!req.hasUsername)
            {
               if (verbose) clog << "No UserName. Send 432." << endl;
               stunCreateErrorResponse(*resp, 4, 32, "No UserName and contains MessageIntegrity");
               return TRUE;
            }
            else
            {
               if (verbose) clog << "Validating username: " << req.username.value << endl;
               // !jf! could retrieve associated password from provisioning here
               if (strcmp(req.username.value, "test") == 0)
               {
                  if (0)
                  {
                     // !jf! if the credentials are stale 
                     stunCreateErrorResponse(*resp, 4, 30, "Stale credentials on BindRequest");
                     return TRUE;
                  }
                  else
                  {
                     if (verbose) clog << "Validating MessageIntegrity" << endl;
                     // need access to shared secret
							
                     unsigned char hmac[20];
#ifndef NOSSL
                     unsigned int hmacSize=20;

                     HMAC(EVP_sha1(), 
                          "1234", 4, 
                          reinterpret_cast<const unsigned char*>(buf), bufLen-20-4, 
                          hmac, &hmacSize);
                     assert(hmacSize == 20);
#endif
							
                     if (memcmp(buf, hmac, 20) != 0)
                     {
                        if (verbose) clog << "MessageIntegrity is bad. Sending " << endl;
                        stunCreateErrorResponse(*resp, 4, 3, "Unknown username. Try test with password 1234");
                        return TRUE;
                     }
							
                     // need to compute this later after message is filled in
                     resp->hasMessageIntegrity = TRUE;
                     assert(req.hasUsername);
                     resp->hasUsername = TRUE;
                     resp->username = req.username; // copy username in
                  }
               }
               else
               {
                  if (verbose) clog << "Invalid username: " << req.username.value << "Send 430." << endl; 
               }
            }
         }
			
         // TODO !jf! should check for unknown attributes here and send 420 listing the
         // unknown attributes. 
			
         if ( respondTo.port == 0 ) respondTo = from;
         if ( mapped.port == 0 ) mapped = from;
				
         *changeIp   = ( flags & ChangeIpFlag )?TRUE:FALSE;
         *changePort = ( flags & ChangePortFlag )?TRUE:FALSE;
			
         if (verbose)
         {
            clog << "Request is valid:" << endl;
            clog << "\t flags=" << flags << endl;
            clog << "\t changeIp=" << *changeIp << endl;
            clog << "\t changePort=" << *changePort << endl;
            clog << "\t from = " << from << endl;
            clog << "\t respond to = " << respondTo << endl;
            clog << "\t mapped = " << mapped << endl;
         }
				
         // form the outgoing message
         resp->msgHdr.msgType = BindResponseMsg;
         for ( int i=0; i<16; i++ )
         {
            resp->msgHdr.id.octet[i] = req.msgHdr.id.octet[i];
         }
		
         if ( req.xorOnly == FALSE )
         {
            resp->hasMappedAddress = TRUE;
            resp->mappedAddress.ipv4.port = mapped.port;
            resp->mappedAddress.ipv4.addr = mapped.addr;
         }

         if (1) // do xorMapped address or not 
         {
            resp->hasXorMappedAddress = TRUE;
            UInt16 id16 = req.msgHdr.id.octet[0]<<8 
               | req.msgHdr.id.octet[1];
            UInt32 id32 = req.msgHdr.id.octet[0]<<24 
               | req.msgHdr.id.octet[1]<<16 
               | req.msgHdr.id.octet[2]<<8 
               | req.msgHdr.id.octet[3];
            resp->xorMappedAddress.ipv4.port = mapped.port^id16;
            resp->xorMappedAddress.ipv4.addr = mapped.addr^id32;
         }
         
         resp->hasSourceAddress = TRUE;
         resp->sourceAddress.ipv4.port = (*changePort) ? altAddr.port : myAddr.port;
         resp->sourceAddress.ipv4.addr = (*changeIp)   ? altAddr.addr : myAddr.addr;
			
         resp->hasChangedAddress = TRUE;
         resp->changedAddress.ipv4.port = altAddr.port;
         resp->changedAddress.ipv4.addr = altAddr.addr;
	
         if ( secondary.port != 0 )
         {
            resp->hasSecondaryAddress = TRUE;
            resp->secondaryAddress.ipv4.port = secondary.port;
            resp->secondaryAddress.ipv4.addr = secondary.addr;
         }
         
         if ( req.hasUsername && req.username.sizeValue > 0 ) 
         {
            // copy username in
            resp->hasUsername = TRUE;
            assert( req.username.sizeValue % 4 == 0 );
            assert( req.username.sizeValue < STUN_MAX_STRING );
            memcpy( resp->username.value, req.username.value, req.username.sizeValue );
            resp->username.sizeValue = req.username.sizeValue;
         }
		
         if (1) // add ServerName 
         {
            resp->hasServerName = TRUE;
            const char serverName[] = "Vovida.org " STUN_VERSION; // must pad to mult of 4
            
            assert( sizeof(serverName) < STUN_MAX_STRING );
            //cerr << "sizeof serverName is "  << sizeof(serverName) << endl;
            assert( sizeof(serverName)%4 == 0 );
            memcpy( resp->serverName.value, serverName, sizeof(serverName));
            resp->serverName.sizeValue = sizeof(serverName);
         }
         
         if ( req.hasMessageIntegrity & req.hasUsername )  
         {
            // this creates the password that will be used in the HMAC when then
            // messages is sent
            stunCreatePassword( req.username, hmacPassword );
         }
				
         if (req.hasUsername && (req.username.sizeValue > 64 ) )
         {
            UInt32 source;
            assert( sizeof(int) == sizeof(UInt32) );
					
            sscanf(req.username.value, "%x", &source);
            resp->hasReflectedFrom = TRUE;
            resp->reflectedFrom.ipv4.port = 0;
            resp->reflectedFrom.ipv4.addr = source;
         }
				
         destination->port = respondTo.port;
         destination->addr = respondTo.addr;
			
         return TRUE;
			
      default:
         if (verbose) clog << "Unknown or unsupported request " << endl;
         return FALSE;
   }
	
   assert(0);
   return FALSE;
}*/
/*
BOOL
stunInitServer(StunServerInfo& info, const StunAddress4& myAddr,
               const StunAddress4& altAddr, int startMediaPort, BOOL verbose )
{
   assert( myAddr.port != 0 );
   assert( altAddr.port!= 0 );
   assert( myAddr.addr  != 0 );
   //assert( altAddr.addr != 0 );
	
   info.myAddr = myAddr;
   info.altAddr = altAddr;
	
   info.myFd = INVALID_SOCKET;
   info.altPortFd = INVALID_SOCKET;
   info.altIpFd = INVALID_SOCKET;
   info.altIpPortFd = INVALID_SOCKET;

   memset(info.relays, 0, sizeof(info.relays));
   if (startMediaPort > 0)
   {
      info.relay = TRUE;

      for (int i=0; i<MAX_MEDIA_RELAYS; ++i)
      {
         StunMediaRelay* relay = &info.relays[i];
         relay->relayPort = startMediaPort+i;
         relay->fd = 0;
         relay->expireTime = 0;
      }
   }
   else
   {
      info.relay = FALSE;
   }
   
   if ((info.myFd = openPort(myAddr.port, myAddr.addr,verbose)) == INVALID_SOCKET)
   {
      clog << "Can't open " << myAddr << endl;
      stunStopServer(info);

      return FALSE;
   }
   //if (verbose) clog << "Opened " << myAddr.addr << ":" << myAddr.port << " --> " << info.myFd << endl;

   if ((info.altPortFd = openPort(altAddr.port,myAddr.addr,verbose)) == INVALID_SOCKET)
   {
      clog << "Can't open " << myAddr << endl;
      stunStopServer(info);
      return FALSE;
   }
   //if (verbose) clog << "Opened " << myAddr.addr << ":" << altAddr.port << " --> " << info.altPortFd << endl;
   
   
   info.altIpFd = INVALID_SOCKET;
   if (  altAddr.addr != 0 )
   {
      if ((info.altIpFd = openPort( myAddr.port, altAddr.addr,verbose)) == INVALID_SOCKET)
      {
         clog << "Can't open " << altAddr << endl;
         stunStopServer(info);
         return FALSE;
      }
      //if (verbose) clog << "Opened " << altAddr.addr << ":" << myAddr.port << " --> " << info.altIpFd << endl;;
   }
   
   info.altIpPortFd = INVALID_SOCKET;
   if (  altAddr.addr != 0 )
   {  if ((info.altIpPortFd = openPort(altAddr.port, altAddr.addr,verbose)) == INVALID_SOCKET)
      {
         clog << "Can't open " << altAddr << endl;
         stunStopServer(info);
         return FALSE;
      }
      //if (verbose) clog << "Opened " << altAddr.addr << ":" << altAddr.port << " --> " << info.altIpPortFd << endl;;
   }
   
   return TRUE;
}
*/
/*
void
stunStopServer(StunServerInfo& info)
{
   if (info.myFd > 0) closesocket(info.myFd);
   if (info.altPortFd > 0) closesocket(info.altPortFd);
   if (info.altIpFd > 0) closesocket(info.altIpFd);
   if (info.altIpPortFd > 0) closesocket(info.altIpPortFd);
   
   if (info.relay)
   {
      for (int i=0; i<MAX_MEDIA_RELAYS; ++i)
      {
         StunMediaRelay* relay = &info.relays[i];
         if (relay->fd)
         {
            closesocket(relay->fd);
            relay->fd = 0;
         }
      }
   }
}
*/
/*
BOOL
stunServerProcess(StunServerInfo& info, BOOL verbose)
{
   char msg[STUN_MAX_MESSAGE_SIZE];
   int msgLen = sizeof(msg);
   	
   BOOL ok = FALSE;
   BOOL recvAltIp =FALSE;
   BOOL recvAltPort = FALSE;
	
   fd_set fdSet; 
   Socket maxFd=0;

   FD_ZERO(&fdSet); 
   FD_SET(info.myFd,&fdSet); 
   if ( info.myFd >= maxFd ) maxFd=info.myFd+1;
   FD_SET(info.altPortFd,&fdSet); 
   if ( info.altPortFd >= maxFd ) maxFd=info.altPortFd+1;

   if ( info.altIpFd != INVALID_SOCKET )
   {
      FD_SET(info.altIpFd,&fdSet);
      if (info.altIpFd>=maxFd) maxFd=info.altIpFd+1;
   }
   if ( info.altIpPortFd != INVALID_SOCKET )
   {
      FD_SET(info.altIpPortFd,&fdSet);
      if (info.altIpPortFd>=maxFd) maxFd=info.altIpPortFd+1;
   }

   if (info.relay)
   {
      for (int i=0; i<MAX_MEDIA_RELAYS; ++i)
      {
         StunMediaRelay* relay = &info.relays[i];
         if (relay->fd)
         {
            FD_SET(relay->fd, &fdSet);
            if (relay->fd >= maxFd) 
			{
				maxFd=relay->fd+1;
			}
         }
      }
   }
   
   if ( info.altIpFd != INVALID_SOCKET )
   {
      FD_SET(info.altIpFd,&fdSet);
      if (info.altIpFd>=maxFd) maxFd=info.altIpFd+1;
   }
   if ( info.altIpPortFd != INVALID_SOCKET )
   {
      FD_SET(info.altIpPortFd,&fdSet);
      if (info.altIpPortFd>=maxFd) maxFd=info.altIpPortFd+1;
   }
   
   struct timeval tv;
   tv.tv_sec = 0;
   tv.tv_usec = 1000;
	
   int e = select( maxFd, &fdSet, NULL,NULL, &tv );
   if (e < 0)
   {
      int err = getErrno();
      clog << "Error on select: " << strerror(err) << endl;
   }
   else if (e >= 0)
   {
      StunAddress4 from;

      // do the media relaying
      if (info.relay)
      {
         time_t now = time(0);
         for (int i=0; i<MAX_MEDIA_RELAYS; ++i)
         {
            StunMediaRelay* relay = &info.relays[i];
            if (relay->fd)
            {
               if (FD_ISSET(relay->fd, &fdSet))
               {
                  char msg[MAX_RTP_MSG_SIZE];
                  int msgLen = sizeof(msg);
                  
                  StunAddress4 rtpFrom;
                  ok = getMessage( relay->fd, msg, &msgLen, &rtpFrom.addr, &rtpFrom.port ,verbose);
                  if (ok)
                  {
                     sendMessage(info.myFd, msg, msgLen, relay->destination.addr, relay->destination.port, verbose);
                     relay->expireTime = now + MEDIA_RELAY_TIMEOUT;
                     if ( verbose ) clog << "Relay packet on " 
                                         << relay->fd 
                                         << " from " << rtpFrom 
                                         << " -> " << relay->destination 
                                         << endl;
                  }
               }
               else if (now > relay->expireTime)
               {
                  closesocket(relay->fd);
                  relay->fd = 0;
               }
            }
         }
      }
      
     
      if (FD_ISSET(info.myFd,&fdSet))
      {
         if (verbose) clog << "received on A1:P1" << endl;
         recvAltIp = FALSE;
         recvAltPort = FALSE;
         ok = getMessage( info.myFd, msg, &msgLen, &from.addr, &from.port,verbose );
      }
      else if (FD_ISSET(info.altPortFd, &fdSet))
      {
         if (verbose) clog << "received on A1:P2" << endl;
         recvAltIp = FALSE;
         recvAltPort = TRUE;
         ok = getMessage( info.altPortFd, msg, &msgLen, &from.addr, &from.port,verbose );
      }
      else if ( (info.altIpFd!=INVALID_SOCKET) && FD_ISSET(info.altIpFd,&fdSet))
      {
         if (verbose) clog << "received on A2:P1" << endl;
         recvAltIp = TRUE;
         recvAltPort = FALSE;
         ok = getMessage( info.altIpFd, msg, &msgLen, &from.addr, &from.port ,verbose);
      }
      else if ( (info.altIpPortFd!=INVALID_SOCKET) && FD_ISSET(info.altIpPortFd, &fdSet))
      {
         if (verbose) clog << "received on A2:P2" << endl;
         recvAltIp = TRUE;
         recvAltPort = TRUE;
         ok = getMessage( info.altIpPortFd, msg, &msgLen, &from.addr, &from.port,verbose );
      }
      else
      {
         return TRUE;
      }

      int relayPort = 0;
      if (info.relay)
      {
         for (int i=0; i<MAX_MEDIA_RELAYS; ++i)
         {
            StunMediaRelay* relay = &info.relays[i];
            if (relay->destination.addr == from.addr && 
                relay->destination.port == from.port)
            {
               relayPort = relay->relayPort;
               relay->expireTime = time(0) + MEDIA_RELAY_TIMEOUT;
               break;
            }
         }

         if (relayPort == 0)
         {
            for (int i=0; i<MAX_MEDIA_RELAYS; ++i)
            {
               StunMediaRelay* relay = &info.relays[i];
               if (relay->fd == 0)
               {
                  if ( verbose ) clog << "Open relay port " << relay->relayPort << endl;
                  
                  relay->fd = openPort(relay->relayPort, info.myAddr.addr, verbose);
                  relay->destination.addr = from.addr;
                  relay->destination.port = from.port;
                  relay->expireTime = time(0) + MEDIA_RELAY_TIMEOUT;
                  relayPort = relay->relayPort;
                  break;
               }
            }
         }
      }
         
      if ( !ok ) 
      {
         if ( verbose ) clog << "Get message did not return a valid message" <<endl;
         return TRUE;
      }
		
      if ( verbose ) clog << "Got a request (len=" << msgLen << ") from " << from << endl;
		
      if ( msgLen <= 0 )
      {
         return TRUE;
      }
		
      BOOL changePort = FALSE;
      BOOL changeIp = FALSE;
		
      StunMessage resp;
      StunAddress4 dest;
      StunAtrString hmacPassword;  
      hmacPassword.sizeValue = 0;

      StunAddress4 secondary;
      secondary.port = 0;
      secondary.addr = 0;
               
      if (info.relay && relayPort)
      {
         secondary = from;
         
         from.addr = info.myAddr.addr;
         from.port = relayPort;
      }
      
      ok = stunServerProcessMsg( msg, msgLen, from, secondary,
                                 recvAltIp ? info.altAddr : info.myAddr,
                                 recvAltIp ? info.myAddr : info.altAddr, 
                                 &resp,
                                 &dest,
                                 &hmacPassword,
                                 &changePort,
                                 &changeIp,
                                 verbose );
		
      if ( !ok )
      {
         if ( verbose ) clog << "Failed to parse message" << endl;
         return TRUE;
      }
		
      char buf[STUN_MAX_MESSAGE_SIZE];
      int len = sizeof(buf);
      		
      len = stunEncodeMessage( resp, buf, len, hmacPassword,verbose );
		
      if ( dest.addr == 0 )  ok=FALSE;
      if ( dest.port == 0 ) ok=FALSE;
		
      if ( ok )
      {
         assert( dest.addr != 0 );
         assert( dest.port != 0 );
			
         Socket sendFd;
			
         BOOL sendAltIp   = recvAltIp;   // send on the received IP address 
         BOOL sendAltPort = recvAltPort; // send on the received port
			
         if ( changeIp )   sendAltIp   = !sendAltIp;   // if need to change IP, then flip logic 
         if ( changePort ) sendAltPort = !sendAltPort; // if need to change port, then flip logic 
			
         if ( !sendAltPort )
         {
            if ( !sendAltIp )
            {
               sendFd = info.myFd;
            }
            else
            {
               sendFd = info.altIpFd;
            }
         }
         else
         {
            if ( !sendAltIp )
            {
               sendFd = info.altPortFd;
            }
            else
            {
               sendFd = info.altIpPortFd;
            }
         }
	
         if ( sendFd != INVALID_SOCKET )
         {
            sendMessage( sendFd, buf, len, dest.addr, dest.port, verbose );
         }
      }
   }
	
   return TRUE;
}
*/
int 
stunFindLocalInterfaces(UInt32* addresses,int maxRet)
{
#if defined(WIN32) || defined(__sparc__)
   return 0;
#else
   struct ifconf ifc;
	char buf[ 1024 ];
   int s = socket( AF_INET, SOCK_DGRAM, 0 );
   //int len = 100 * sizeof(struct ifreq); Alvin modify for variable no reference
   int e ;
   char *ptr;
   int tl;
   int count=0;
   struct ifreq* ifr;
   int si;
   struct ifreq ifr2;
   struct sockaddr a;
   struct sockaddr_in* addr;
   UInt32 ai;
	
   ifc.ifc_len = 1024;
   ifc.ifc_buf = buf;
	
   e = ioctl(s,SIOCGIFCONF,&ifc);
   ptr = buf;
   tl = ifc.ifc_len;
	
   while ( (tl > 0) && ( count < maxRet) )
   {
      ifr = (struct ifreq *)ptr;
		
      si = sizeof(ifr->ifr_name) + sizeof(struct sockaddr);
      tl -= si;
      ptr += si;
		 
      ifr2 = *ifr;
		
      e = ioctl(s,SIOCGIFADDR,&ifr2);
      if ( e == -1 )
      {
         break;
      }
		
      a = ifr2.ifr_addr;
      addr = (struct sockaddr_in*) &a;
		
      ai = ntohl( addr->sin_addr.s_addr );
      if ((int)((ai>>24)&0xFF) != 127)
      {
         addresses[count++] = ai;
      }
		
#if 0
      cerr << "Detected interface "
           << int((ai>>24)&0xFF) << "." 
           << int((ai>>16)&0xFF) << "." 
           << int((ai>> 8)&0xFF) << "." 
           << int((ai    )&0xFF) << endl;
#endif
   }
	
   closesocket(s);
	
   return count;
#endif
}


void
stunBuildReqSimple( StunMessage* msg,
                    const StunAtrString* username,
                    BOOL changePort, BOOL changeIp, unsigned int id )
{
   int i, r;

   assert( msg );
   memset( msg , 0 , sizeof(*msg) );
	
   msg->msgHdr.msgType = BindRequestMsg;
	
   for (i=0; i<16; i=i+4 )
   {
      assert(i+3<16);
      r = stunRand();
      msg->msgHdr.id.octet[i+0]= r>>0;
      msg->msgHdr.id.octet[i+1]= r>>8;
      msg->msgHdr.id.octet[i+2]= r>>16;
      msg->msgHdr.id.octet[i+3]= r>>24;
   }
	
   if ( id != 0 )
   {
      msg->msgHdr.id.octet[0] = id; 
   }
	
   msg->hasChangeRequest = TRUE;
   msg->changeRequest.value =(changeIp?ChangeIpFlag:0) | 
      (changePort?ChangePortFlag:0);
	
   if ( username->sizeValue > 0 )
   {
      msg->hasUsername = TRUE;
      msg->username = *username;
   }
}


static void 
stunSendTest( SOCKET myFd, StunAddress4* dest, 
              const StunAtrString* username, const StunAtrString* password, 
              int testNum, BOOL verbose )
{ 
   BOOL changePort=FALSE;
   BOOL changeIP=FALSE;
   BOOL discard=FALSE;
   StunMessage req;
   char buf[STUN_MAX_MESSAGE_SIZE];
   int len = STUN_MAX_MESSAGE_SIZE;
#ifdef WIN32
   clock_t now;
#endif

   assert( dest->addr != 0 );
   assert( dest->port != 0 );
	
   switch (testNum)
   {
      case 1:
      case 10:
      case 11:
         break;
      case 2:
         //changePort=TRUE;
         changeIP=TRUE;
         break;
      case 3:
         changePort=TRUE;
         break;
      case 4:
         changeIP=TRUE;
         break;
      case 5:
         discard=TRUE;
         break;
      default:
		  fprintf(stderr, "Test %d is unknown\n", testNum);
         //cerr << "Test " << testNum <<" is unkown\n";
         assert(0);
   }
	

   memset(&req, 0, sizeof(StunMessage));
	
   stunBuildReqSimple( &req, username, 
                       changePort , changeIP , 
                       testNum );
	
   len = stunEncodeMessage( &req, buf, len, password,verbose );
	
   sendMessage( myFd, buf, len, dest->addr, dest->port, verbose );
	
   // add some delay so the packets don't get sent too quickly 
#ifdef WIN32 // !cj! TODO - should fix this up in windows
		 now = clock();
		 assert( CLOCKS_PER_SEC == 1000 );
		 while ( clock() <= now+10 ) { };
#else
		 usleep(10*1000);
#endif

}


void 
stunGetUserNameAndPassword(  const StunAddress4* dest, 
                             StunAtrString* username,
                             StunAtrString* password)
{ 
   // !cj! This is totally bogus - need to make TLS connection to dest and get a
   // username and password to use 
   stunCreateUserName(dest, username);
   stunCreatePassword(username, password);
}


void 
stunTest( StunAddress4* dest, int testNum, BOOL verbose, StunAddress4* sAddr )
{ 
   int port = stunRandomPort();
   UInt32 interfaceIp=0;
   SOCKET myFd;

   StunAtrString username;
   StunAtrString password;

   char msg[STUN_MAX_MESSAGE_SIZE];
   int msgLen = STUN_MAX_MESSAGE_SIZE;

   StunAddress4 from;
   StunMessage resp;
   BOOL ok;

   assert( dest->addr != 0 );
   assert( dest->port != 0 );	
 
   if (sAddr)
   {
      interfaceIp = sAddr->addr;
      if ( sAddr->port != 0 )
      {
        port = sAddr->port;
      }
   }
   myFd = openPort(port,interfaceIp,verbose);
	
   username.sizeValue = 0;
   password.sizeValue = 0;
	
#ifdef USE_TLS
   stunGetUserNameAndPassword( dest, &username, &password );
#endif
	
   stunSendTest( myFd, dest, &username, &password, testNum, verbose );

   getMessage( myFd,
               msg,
               &msgLen,
               &from.addr,
               &from.port,verbose );
	
   memset(&resp, 0, sizeof(StunMessage));
	
   //if ( verbose ) clog << "Got a response" << endl;
   ok = stunParseMessage( msg,msgLen, &resp,verbose );
	
   /*if ( verbose )
   {
      clog << "\t ok=" << ok << endl;
      clog << "\t id=" << resp.msgHdr.id << endl;
      clog << "\t mappedAddr=" << resp.mappedAddress.ipv4 << endl;
      clog << "\t changedAddr=" << resp.changedAddress.ipv4 << endl;
      clog << endl;
   }*/
	
   if (sAddr)
   {
      sAddr->port = resp.mappedAddress.ipv4.port;
      sAddr->addr = resp.mappedAddress.ipv4.addr;
   }
}


NatType
stunNatType( StunAddress4* dest, 
             BOOL verbose,
             BOOL* preservePort, // if set, is return for if NAT preservers ports or not
             BOOL* hairpin,  // if set, is the return for if NAT will hairpin packets
             int port, // port to use for the test, 0 to choose random port
             StunAddress4* sAddr // NIC to use 
   )
{ 

   UInt32 interfaceIp=0;
   SOCKET myFd1, myFd2;

   BOOL respTestI=FALSE;
   BOOL isNat=TRUE;
   StunAddress4 testIchangedAddr;
   StunAddress4 testImappedAddr;
   BOOL respTestI2=FALSE; 
   BOOL mappedIpSame = TRUE;
   StunAddress4 testI2mappedAddr;
   StunAddress4 testI2dest=*dest;
   BOOL respTestII=FALSE;
   BOOL respTestIII=FALSE;

   BOOL respTestHairpin=FALSE;
   BOOL respTestPreservePort=FALSE;

   StunAtrString username;
   StunAtrString password;

   int count, i;

    struct timeval tv;
    fd_set fdSet; 
#ifdef WIN32
      unsigned int fdSetSize;
#else
      int fdSetSize;
#endif
	int  err,e;
	SOCKET myFd;

	char msg[STUN_MAX_MESSAGE_SIZE];
    int msgLen = sizeof(msg);
                  						
    StunAddress4 from;
	StunMessage resp;

	 SOCKET s;

   assert( dest->addr != 0 );
   assert( dest->port != 0 );
	
   if ( hairpin ) 
   {
      *hairpin = FALSE;
   }
	
   if ( port == 0 )
   {
      port = stunRandomPort();
   }
   
   if (sAddr)
   {
      interfaceIp = sAddr->addr;
   }
   myFd1 = openPort(port,interfaceIp,verbose);
   myFd2 = openPort(port+1,interfaceIp,verbose);

   if ( ( myFd1 == INVALID_SOCKET) || ( myFd2 == INVALID_SOCKET) )
   {
	   fprintf(stderr, "Some problem opening port/interface to send on\n");
        //cerr << "Some problem opening port/interface to send on" << endl;
       return StunTypeFailure; 
   }

   assert( myFd1 != INVALID_SOCKET );
   assert( myFd2 != INVALID_SOCKET );
    
   memset(&testImappedAddr,0,sizeof(testImappedAddr));	
	
   username.sizeValue = 0;
   password.sizeValue = 0;
	
#ifdef USE_TLS 
   stunGetUserNameAndPassword( dest, &username, &password );
#endif
	
   count=0;
   while ( count < 7 )
   {
	  tv.tv_sec = 0;
	  tv.tv_usec = 0;
#ifdef WIN32
      fdSetSize = 0;
#else
      fdSetSize = 0;
#endif

      FD_ZERO(&fdSet); fdSetSize=0;
      FD_SET(myFd1,&fdSet); fdSetSize = (myFd1+1>fdSetSize) ? myFd1+1 : fdSetSize;
      FD_SET(myFd2,&fdSet); fdSetSize = (myFd2+1>fdSetSize) ? myFd2+1 : fdSetSize;
      tv.tv_sec=0;
      tv.tv_usec=150*1000; // 150 ms 
      if ( count == 0 ) tv.tv_usec=0;
		
      err = select(fdSetSize, &fdSet, NULL, NULL, &tv);
      e = getErrno();
      if ( err == SOCKET_ERROR )
      {
         // error occured
		  fprintf(stderr, "Error %d %s in select\n", e, strerror(e));
          //cerr << "Error " << e << " " << strerror(e) << " in select" << endl;
        return StunTypeFailure; 
     }
      else if ( err == 0 )
      {
         // timeout occured 
         count++;
			
         if ( !respTestI ) 
         {
            stunSendTest( myFd1, dest, &username, &password, 1 ,verbose );
         }         
			
         if ( (!respTestI2) && respTestI ) 
         {
            // check the address to send to if valid 
            if (  ( testI2dest.addr != 0 ) &&
                  ( testI2dest.port != 0 ) )
            {
               stunSendTest( myFd1, &testI2dest, &username, &password, 10  ,verbose);
            }
         }
			
         if ( !respTestII )
         {
            stunSendTest( myFd2, dest, &username, &password, 2 ,verbose );
         }
			
         if ( !respTestIII )
         {
            stunSendTest( myFd2, dest, &username, &password, 3 ,verbose );
         }
			
         if ( respTestI && (!respTestHairpin) )
         {
            if (  ( testImappedAddr.addr != 0 ) &&
                  ( testImappedAddr.port != 0 ) )
            {
               stunSendTest( myFd1, &testImappedAddr, &username, &password, 11 ,verbose );
            }
         }
      }
      else
      {
         if (verbose) 
			assert( err>0 );
         // data is avialbe on some fd 
			
         for (i=0; i<2; i++)
         {
            if ( i==0 ) 
            {
               myFd=myFd1;
            }
            else
            {
               myFd=myFd2;
            }
				
            if ( myFd!=INVALID_SOCKET ) 
            {					
			   msgLen = STUN_MAX_MESSAGE_SIZE;
			   memset(msg, 0, msgLen);

               if ( FD_ISSET(myFd,&fdSet) )
               {		
                  getMessage( myFd,
                              msg,
                              &msgLen,
                              &from.addr,
                              &from.port,verbose );
						
                  
                  memset(&resp, 0, sizeof(StunMessage));
                  stunParseMessage( msg,msgLen, &resp,verbose );
						
                  /*if ( verbose )
                  {
                     clog << "Received message of type " << resp.msgHdr.msgType 
                          << "  id=" << (int)(resp.msgHdr.id.octet[0]) << endl;
                  }*/
						
                  switch( resp.msgHdr.id.octet[0] )
                  {
                     case 1:
                     {
                        if ( !respTestI )
                        {
									
                           testIchangedAddr.addr = resp.changedAddress.ipv4.addr;
                           testIchangedAddr.port = resp.changedAddress.ipv4.port;
                           testImappedAddr.addr = resp.mappedAddress.ipv4.addr;
                           testImappedAddr.port = resp.mappedAddress.ipv4.port;
			
                           respTestPreservePort = ( testImappedAddr.port == port ); 
                           if ( preservePort )
                           {
                              *preservePort = respTestPreservePort;
                           }								
									
                           testI2dest.addr = resp.changedAddress.ipv4.addr;
									
                           if (sAddr)
                           {
                              sAddr->port = testImappedAddr.port;
                              sAddr->addr = testImappedAddr.addr;
                           }
									
                           count = 0;
                        }		
                        respTestI=TRUE;
                     }
                     break;
                     case 2:
                     {  
                        respTestII=TRUE;
                     }
                     break;
                     case 3:
                     {
                        respTestIII=TRUE;
                     }
                     break;
                     case 10:
                     {
                        if ( !respTestI2 )
                        {
                           testI2mappedAddr.addr = resp.mappedAddress.ipv4.addr;
                           testI2mappedAddr.port = resp.mappedAddress.ipv4.port;
								
                           mappedIpSame = FALSE;
                           if ( (testI2mappedAddr.addr  == testImappedAddr.addr ) &&
                                (testI2mappedAddr.port == testImappedAddr.port ))
                           { 
                              mappedIpSame = TRUE;
                           }
								
							
                        }
                        respTestI2=TRUE;
                     }
                     break;
                     case 11:
                     {
							
                        if ( hairpin ) 
                        {
                           *hairpin = TRUE;
                        }
                        respTestHairpin = TRUE;
                     }
                     break;
                  }
               }
            }
         }
      }
   }
	
   // see if we can bind to this address 
   //cerr << "try binding to " << testImappedAddr << endl;
  s = openPort( 0/*use ephemeral*/, testImappedAddr.addr, FALSE );
   if ( s != INVALID_SOCKET )
   {
      closesocket(s);
      isNat = FALSE;
      //cerr << "binding worked" << endl;
   }
   else
   {
      isNat = TRUE;
      //cerr << "binding failed" << endl;
   }
	
   if ( myFd1 != INVALID_SOCKET )
	   closesocket(myFd1);

   if ( myFd2 != INVALID_SOCKET )
	   closesocket(myFd2);

   /*if (verbose)
   {
      clog << "test I = " << respTestI << endl;
      clog << "test II = " << respTestII << endl;
      clog << "test III = " << respTestIII << endl;
      clog << "test I(2) = " << respTestI2 << endl;
      clog << "is nat  = " << isNat <<endl;
      clog << "mapped IP same = " << mappedIpSame << endl;
      clog << "hairpin = " << respTestHairpin << endl;
      clog << "preserver port = " << respTestPreservePort << endl;
   }*/
	
#if 0
   // implement logic flow chart from draft RFC
   if ( respTestI )
   {
      if ( isNat )
      {
         if (respTestII)
         {
            return StunTypeConeNat;
         }
         else
         {
            if ( mappedIpSame )
            {
               if ( respTestIII )
               {
                  return StunTypeRestrictedNat;
               }
               else
               {
                  return StunTypePortRestrictedNat;
               }
            }
            else
            {
               return StunTypeSymNat;
            }
         }
      }
      else
      {
         if (respTestII)
         {
            return StunTypeOpen;
         }
         else
         {
            return StunTypeSymFirewall;
         }
      }
   }
   else
   {
      return StunTypeBlocked;
   }
#else
   if ( respTestI ) // not blocked 
   {
      if ( isNat )
      {
         if ( mappedIpSame )
         {
            if (respTestII)
            {
               return StunTypeIndependentFilter;
            }
            else
            {
               if ( respTestIII )
               {
                  return StunTypeDependentFilter;
               }
               else
               {
                  return StunTypePortDependedFilter;
               }
            }
         }
         else // mappedIp is not same 
         {
            return StunTypeDependentMapping;
         }
      }
      else  // isNat is FALSE
      {
         if (respTestII)
         {
            return StunTypeOpen;
         }
         else
         {
            return StunTypeFirewall;
         }
      }
   }
   else
   {
      return StunTypeBlocked;
   }
#endif
	
   return StunTypeUnknown;
}


int
stunOpenSocket( StunAddress4* dest, StunAddress4* mapAddr, 
                int port, StunAddress4* srcAddr, 
                BOOL verbose )
{

   unsigned int interfaceIp = 0;
    SOCKET myFd;

   char msg[STUN_MAX_MESSAGE_SIZE];
   int msgLen = sizeof(msg);
	
   StunAtrString username;
   StunAtrString password;

   StunAddress4 from;
   StunMessage resp;
   BOOL ok;

   StunAddress4 mappedAddr;
   StunAddress4 changedAddr;

   if ( port == 0 )
   {
      port = stunRandomPort();
   }
   
   if ( srcAddr )
   {
      interfaceIp = srcAddr->addr;
   }
   
   myFd = openPort(port,interfaceIp,verbose);
   if (myFd == INVALID_SOCKET)
   {
      return myFd;
   }

   username.sizeValue = 0;
   password.sizeValue = 0;
	
#ifdef USE_TLS
   stunGetUserNameAndPassword( dest, &username, &password );
#endif
	
   stunSendTest(myFd, dest, &username, &password, 1, 0/*FALSE*/ );
	
   getMessage( myFd, msg, &msgLen, &from.addr, &from.port,verbose );

   memset(&resp, 0, sizeof(StunMessage));
	
   ok = stunParseMessage( msg, msgLen, &resp,verbose );
   if (!ok)
   {
      return -1;
   }
	
   mappedAddr = resp.mappedAddress.ipv4;
   changedAddr = resp.changedAddress.ipv4;
	
   *mapAddr = mappedAddr;
	
   return myFd;
}


BOOL
stunOpenSocketPair( StunAddress4* dest, StunAddress4* mapAddr, 
                    int* fd1, int* fd2, 
                    int port, StunAddress4* srcAddr, 
                    BOOL verbose )
{

   int NUM=3;
   	
   char msg[STUN_MAX_MESSAGE_SIZE];
   int msgLen =sizeof(msg);
	
   StunAddress4 from;
   int fd[3];
   int i;
	
   unsigned int interfaceIp = 0;

   StunAtrString username;
   StunAtrString password;

   StunAddress4 mappedAddr[3];
   StunMessage resp;
   BOOL ok;
   StunAddress4 changedAddr;

	
   if ( port == 0 )
   {
      port = stunRandomPort();
   }
	
   *fd1=-1;
   *fd2=-1;

   if ( srcAddr )
   {
      interfaceIp = srcAddr->addr;
   }

   for( i=0; i<NUM; i++)
   {
      fd[i] = openPort( (port == 0) ? 0 : (port + i), 
                        interfaceIp, verbose);
      if (fd[i] < 0) 
      {
         while (i > 0)
         {
            closesocket(fd[--i]);
         }
         return FALSE;
      }
   }
	
   username.sizeValue = 0;
   password.sizeValue = 0;
	
#ifdef USE_TLS
   stunGetUserNameAndPassword( dest, &username, &password );
#endif
	
   for( i=0; i<NUM; i++)
   {
      stunSendTest(fd[i], dest, &username, &password, 1/*testNum*/, verbose );
   }
	
   for( i=0; i<NUM; i++)
   {
      msgLen = sizeof(msg)/sizeof(*msg);
      getMessage( fd[i],
                  msg,
                  &msgLen,
                  &from.addr,
                  &from.port ,verbose);
		

      memset(&resp, 0, sizeof(StunMessage));
		
      ok = stunParseMessage( msg, msgLen, &resp, verbose );
      if (!ok) 
      {
         return FALSE;
      }
		
      mappedAddr[i] = resp.mappedAddress.ipv4;
      changedAddr = resp.changedAddress.ipv4;
   }
	
   if ( mappedAddr[0].port %2 == 0 )
   {
      if (  mappedAddr[0].port+1 ==  mappedAddr[1].port )
      {
         *mapAddr = mappedAddr[0];
         *fd1 = fd[0];
         *fd2 = fd[1];
         closesocket( fd[2] );
         return TRUE;
      }
   }
   else
   {
      if (( mappedAddr[1].port %2 == 0 )
          && (  mappedAddr[1].port+1 ==  mappedAddr[2].port ))
      {
         *mapAddr = mappedAddr[1];
         *fd1 = fd[1];
         *fd2 = fd[2];
         closesocket( fd[0] );
         return TRUE;
      }
   }

   // something failed, close all and return error
   for( i=0; i<NUM; i++)
   {
      closesocket( fd[i] );
   }
	
   return FALSE;
}

/* ====================================================================
 * The Vovida Software License, Version 1.0 
 * 
 * Copyright (c) 2000 Vovida Networks, Inc.  All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 
 * 3. The names "VOCAL", "Vovida Open Communication Application Library",
 *    and "Vovida Open Communication Application Library (VOCAL)" must
 *    not be used to endorse or promote products derived from this
 *    software without prior written permission. For written
 *    permission, please contact vocal@vovida.org.
 *
 * 4. Products derived from this software may not be called "VOCAL", nor
 *    may "VOCAL" appear in their name, without prior written
 *    permission of Vovida Networks, Inc.
 * 
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESSED OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, TITLE AND
 * NON-INFRINGEMENT ARE DISCLAIMED.  IN NO EVENT SHALL VOVIDA
 * NETWORKS, INC. OR ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT DAMAGES
 * IN EXCESS OF $1,000, NOR FOR ANY INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 * USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
 * DAMAGE.
 * 
 * ====================================================================
 * 
 * This software consists of voluntary contributions made by Vovida
 * Networks, Inc. and many individuals on behalf of Vovida Networks,
 * Inc.  For more information on Vovida Networks, Inc., please see
 * <http://www.vovida.org/>.
 *
 */

// Local Variables:
// mode:c++
// c-file-style:"ellemtel"
// c-file-offsets:((case-label . +))
// indent-tabs-mode:nil
// End:


