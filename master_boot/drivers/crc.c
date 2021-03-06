// CRC with Polynomial 0xE7
static uint8_t CRC8_TABLE[] =  [ 
   0x0,  0xe7, 0x29, 0xce, 0x52, 0xb5, 0x7b, 0x9c, 0xa4, 0x43, 0x8d, 0x6a, 0xf6, 0x11, 0xdf, 0x38,
   0xaf, 0x48, 0x86, 0x61, 0xfd, 0x1a, 0xd4, 0x33, 0xb,  0xec, 0x22, 0xc5, 0x59, 0xbe, 0x70, 0x97,
   0xb9, 0x5e, 0x90, 0x77, 0xeb, 0xc,  0xc2, 0x25, 0x1d, 0xfa, 0x34, 0xd3, 0x4f, 0xa8, 0x66, 0x81,
   0x16, 0xf1, 0x3f, 0xd8, 0x44, 0xa3, 0x6d, 0x8a, 0xb2, 0x55, 0x9b, 0x7c, 0xe0, 0x7,  0xc9, 0x2e,
   0x95, 0x72, 0xbc, 0x5b, 0xc7, 0x20, 0xee, 0x9,  0x31, 0xd6, 0x18, 0xff, 0x63, 0x84, 0x4a, 0xad,
   0x3a, 0xdd, 0x13, 0xf4, 0x68, 0x8f, 0x41, 0xa6, 0x9e, 0x79, 0xb7, 0x50, 0xcc, 0x2b, 0xe5, 0x2,
   0x2c, 0xcb, 0x5,  0xe2, 0x7e, 0x99, 0x57, 0xb0, 0x88, 0x6f, 0xa1, 0x46, 0xda, 0x3d, 0xf3, 0x14,
   0x83, 0x64, 0xaa, 0x4d, 0xd1, 0x36, 0xf8, 0x1f, 0x27, 0xc0, 0xe,  0xe9, 0x75, 0x92, 0x5c, 0xbb,
   0xcd, 0x2a, 0xe4, 0x3,  0x9f, 0x78, 0xb6, 0x51, 0x69, 0x8e, 0x40, 0xa7, 0x3b, 0xdc, 0x12, 0xf5,
   0x62, 0x85, 0x4b, 0xac, 0x30, 0xd7, 0x19, 0xfe, 0xc6, 0x21, 0xef, 0x8,  0x94, 0x73, 0xbd, 0x5a,
   0x74, 0x93, 0x5d, 0xba, 0x26, 0xc1, 0xf,  0xe8, 0xd0, 0x37, 0xf9, 0x1e, 0x82, 0x65, 0xab, 0x4c,
   0xdb, 0x3c, 0xf2, 0x15, 0x89, 0x6e, 0xa0, 0x47, 0x7f, 0x98, 0x56, 0xb1, 0x2d, 0xca, 0x4,  0xe3,
   0x58, 0xbf, 0x71, 0x96, 0xa,  0xed, 0x23, 0xc4, 0xfc, 0x1b, 0xd5, 0x32, 0xae, 0x49, 0x87, 0x60,
   0xf7, 0x10, 0xde, 0x39, 0xa5, 0x42, 0x8c, 0x6b, 0x53, 0xb4, 0x7a, 0x9d, 0x1,  0xe6, 0x28, 0xcf,
   0xe1, 0x6,  0xc8, 0x2f, 0xb3, 0x54, 0x9a, 0x7d, 0x45, 0xa2, 0x6c, 0x8b, 0x17, 0xf0, 0x3e, 0xd9,
   0x4e, 0xa9, 0x67, 0x80, 0x1c, 0xfb, 0x35, 0xd2, 0xea, 0xd,  0xc3, 0x24, 0xb8, 0x5f, 0x91, 0x76]

// CRC with Polynomial 0x4c11DB7
static uint32_t CRC32_TABLE[] = [
   0x0,        0x4c11db7,  0x9823b6e,  0xd4326d9,  0x130476dc, 0x17c56b6b, 0x1a864db2, 0x1e475005,
   0x2608edb8, 0x22c9f00f, 0x2f8ad6d6, 0x2b4bcb61, 0x350c9b64, 0x31cd86d3, 0x3c8ea00a, 0x384fbdbd,
   0x4c11db70, 0x48d0c6c7, 0x4593e01e, 0x4152fda9, 0x5f15adac, 0x5bd4b01b, 0x569796c2, 0x52568b75,
   0x6a1936c8, 0x6ed82b7f, 0x639b0da6, 0x675a1011, 0x791d4014, 0x7ddc5da3, 0x709f7b7a, 0x745e66cd,
   0x9823b6e0, 0x9ce2ab57, 0x91a18d8e, 0x95609039, 0x8b27c03c, 0x8fe6dd8b, 0x82a5fb52, 0x8664e6e5,
   0xbe2b5b58, 0xbaea46ef, 0xb7a96036, 0xb3687d81, 0xad2f2d84, 0xa9ee3033, 0xa4ad16ea, 0xa06c0b5d,
   0xd4326d90, 0xd0f37027, 0xddb056fe, 0xd9714b49, 0xc7361b4c, 0xc3f706fb, 0xceb42022, 0xca753d95,
   0xf23a8028, 0xf6fb9d9f, 0xfbb8bb46, 0xff79a6f1, 0xe13ef6f4, 0xe5ffeb43, 0xe8bccd9a, 0xec7dd02d,
   0x34867077, 0x30476dc0, 0x3d044b19, 0x39c556ae, 0x278206ab, 0x23431b1c, 0x2e003dc5, 0x2ac12072,
   0x128e9dcf, 0x164f8078, 0x1b0ca6a1, 0x1fcdbb16, 0x18aeb13,  0x54bf6a4,  0x808d07d,  0xcc9cdca,
   0x7897ab07, 0x7c56b6b0, 0x71159069, 0x75d48dde, 0x6b93dddb, 0x6f52c06c, 0x6211e6b5, 0x66d0fb02,
   0x5e9f46bf, 0x5a5e5b08, 0x571d7dd1, 0x53dc6066, 0x4d9b3063, 0x495a2dd4, 0x44190b0d, 0x40d816ba,
   0xaca5c697, 0xa864db20, 0xa527fdf9, 0xa1e6e04e, 0xbfa1b04b, 0xbb60adfc, 0xb6238b25, 0xb2e29692,
   0x8aad2b2f, 0x8e6c3698, 0x832f1041, 0x87ee0df6, 0x99a95df3, 0x9d684044, 0x902b669d, 0x94ea7b2a,
   0xe0b41de7, 0xe4750050, 0xe9362689, 0xedf73b3e, 0xf3b06b3b, 0xf771768c, 0xfa325055, 0xfef34de2,
   0xc6bcf05f, 0xc27dede8, 0xcf3ecb31, 0xcbffd686, 0xd5b88683, 0xd1799b34, 0xdc3abded, 0xd8fba05a,
   0x690ce0ee, 0x6dcdfd59, 0x608edb80, 0x644fc637, 0x7a089632, 0x7ec98b85, 0x738aad5c, 0x774bb0eb,
   0x4f040d56, 0x4bc510e1, 0x46863638, 0x42472b8f, 0x5c007b8a, 0x58c1663d, 0x558240e4, 0x51435d53,
   0x251d3b9e, 0x21dc2629, 0x2c9f00f0, 0x285e1d47, 0x36194d42, 0x32d850f5, 0x3f9b762c, 0x3b5a6b9b,
   0x315d626,  0x7d4cb91,  0xa97ed48,  0xe56f0ff,  0x1011a0fa, 0x14d0bd4d, 0x19939b94, 0x1d528623,
   0xf12f560e, 0xf5ee4bb9, 0xf8ad6d60, 0xfc6c70d7, 0xe22b20d2, 0xe6ea3d65, 0xeba91bbc, 0xef68060b,
   0xd727bbb6, 0xd3e6a601, 0xdea580d8, 0xda649d6f, 0xc423cd6a, 0xc0e2d0dd, 0xcda1f604, 0xc960ebb3,
   0xbd3e8d7e, 0xb9ff90c9, 0xb4bcb610, 0xb07daba7, 0xae3afba2, 0xaafbe615, 0xa7b8c0cc, 0xa379dd7b,
   0x9b3660c6, 0x9ff77d71, 0x92b45ba8, 0x9675461f, 0x8832161a, 0x8cf30bad, 0x81b02d74, 0x857130c3,
   0x5d8a9099, 0x594b8d2e, 0x5408abf7, 0x50c9b640, 0x4e8ee645, 0x4a4ffbf2, 0x470cdd2b, 0x43cdc09c,
   0x7b827d21, 0x7f436096, 0x7200464f, 0x76c15bf8, 0x68860bfd, 0x6c47164a, 0x61043093, 0x65c52d24,
   0x119b4be9, 0x155a565e, 0x18197087, 0x1cd86d30, 0x29f3d35,  0x65e2082,  0xb1d065b,  0xfdc1bec,
   0x3793a651, 0x3352bbe6, 0x3e119d3f, 0x3ad08088, 0x2497d08d, 0x2056cd3a, 0x2d15ebe3, 0x29d4f654,
   0xc5a92679, 0xc1683bce, 0xcc2b1d17, 0xc8ea00a0, 0xd6ad50a5, 0xd26c4d12, 0xdf2f6bcb, 0xdbee767c,
   0xe3a1cbc1, 0xe760d676, 0xea23f0af, 0xeee2ed18, 0xf0a5bd1d, 0xf464a0aa, 0xf9278673, 0xfde69bc4,
   0x89b8fd09, 0x8d79e0be, 0x803ac667, 0x84fbdbd0, 0x9abc8bd5, 0x9e7d9662, 0x933eb0bb, 0x97ffad0c,
   0xafb010b1, 0xab710d06, 0xa6322bdf, 0xa2f33668, 0xbcb4666d, 0xb8757bda, 0xb5365d03, 0xb1f740b4]

#define TOPBIT_8  = 0x80
#define TOPBIT_32 = 0x80000000

uint8_t calcCRC8 ( Packet_t ){
   uint8_t remainder = 0

   for(datum in data){
      curDiv = (datum ^ remainder) & (0xFF)
      remainder = (CRC8_TABLE[curDiv] ^ (remainder << 8)) & (0xFF)
   }
   
   return remainder
}

def calcCRC32 ( data ):
   remainder = 0

   for datum in data:
      curDiv = (datum ^ (remainder >> 24)) & (0xFF)
      remainder = (CRC32_TABLE[curDiv] ^ (remainder << 8)) & (0xFFFFFFFF)

   return remainder

def genLookupCRC (polynomial, width):
   crcTable = [ 0 for i in range(0, 256) ]
   topbit = 1 << (width - 1)

   remainder = 0

   for dividend in range(0, 256):
      remainder = dividend << (width - 8) 

      for bit in range(0, 8):
         if(remainder & topbit):
            remainder = (remainder << 1) ^ polynomial
         else:
            remainder = remainder << 1

      crcTable[dividend] = hex(remainder & 0xFF) # TODO make xFF generic to any width

   return crcTable 


print(hex(calcCRC32( [0x50, 0x60, 0x10, 0x34, 0xFF, 0x11] )))
print(hex(calcCRC8( [0x50, 0x60, 0xFF] ) ))
