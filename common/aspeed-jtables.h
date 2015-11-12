#ifndef __JTABLES_ASPEED_
#define __JTABLES_ASPEED_ 1
  static int8_t zigzag[] = { 0, 1, 5, 6, 14, 15, 27, 28, 2, 4, 7, 13, 16, 26, 29, 42, 3, 8, 12, 17, 25, 30, 41, 43, 9, 11, 18, 24, 31, 40, 44, 53, 10, 19, 23, 32, 39, 45, 52, 54, 20, 22, 33, 38, 46, 51, 55, 60, 21, 34, 37, 47, 50, 56, 59, 61, 35, 36, 48, 49, 57, 58, 62, 63 };
  static int8_t dezigzag[] = { 0, 1, 8, 16, 9, 2, 3, 10, 17, 24, 32, 25, 18, 11, 4, 5, 12, 19, 26, 33, 40, 48, 41, 34, 27, 20, 13, 6, 7, 14, 21, 28, 35, 42, 49, 56, 57, 50, 43, 36, 29, 22, 15, 23, 30, 37, 44, 51, 58, 59, 52, 45, 38, 31, 39, 46, 53, 60, 61, 54, 47, 55, 62, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63 };
  static int8_t std_dc_luminance_nrcodes[] = { 0, 0, 1, 5, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0 };
  static short std_dc_luminance_values[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
  static int8_t std_dc_chrominance_nrcodes[] = { 0, 0, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0 };
  static short std_dc_chrominance_values[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
  static int8_t std_ac_luminance_nrcodes[] = { 0, 0, 2, 1, 3, 3, 2, 4, 3, 5, 5, 4, 4, 0, 0, 1, 125 };
  static short std_ac_luminance_values[] = { 1, 2, 3, 0, 4, 17, 5, 18, 33, 49, 65, 6, 19, 81, 97, 7, 34, 113, 20, 50, 129, 145, 161, 8, 35, 66, 177, 193, 21, 82, 209, 240, 36, 51, 98, 114, 130, 9, 10, 22, 23, 24, 25, 26, 37, 38, 39, 40, 41, 42, 52, 53, 54, 55, 56, 57, 58, 67, 68, 69, 70, 71, 72, 73, 74, 83, 84, 85, 86, 87, 88, 89, 90, 99, 100, 101, 102, 103, 104, 105, 106, 115, 116, 117, 118, 119, 120, 121, 122, 131, 132, 133, 134, 135, 136, 137, 138, 146, 147, 148, 149, 150, 151, 152, 153, 154, 162, 163, 164, 165, 166, 167, 168, 169, 170, 178, 179, 180, 181, 182, 183, 184, 185, 186, 194, 195, 196, 197, 198, 199, 200, 201, 202, 210, 211, 212, 213, 214, 215, 216, 217, 218, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250 };
  static int8_t std_ac_chrominance_nrcodes[] = { 0, 0, 2, 1, 2, 4, 4, 3, 4, 7, 5, 4, 4, 0, 1, 2, 119 };
  static short std_ac_chrominance_values[] = { 0, 1, 2, 3, 17, 4, 5, 33, 49, 6, 18, 65, 81, 7, 97, 113, 19, 34, 50, 129, 8, 20, 66, 145, 161, 177, 193, 9, 35, 51, 82, 240, 21, 98, 114, 209, 10, 22, 36, 52, 225, 37, 241, 23, 24, 25, 26, 38, 39, 40, 41, 42, 53, 54, 55, 56, 57, 58, 67, 68, 69, 70, 71, 72, 73, 74, 83, 84, 85, 86, 87, 88, 89, 90, 99, 100, 101, 102, 103, 104, 105, 106, 115, 116, 117, 118, 119, 120, 121, 122, 130, 131, 132, 133, 134, 135, 136, 137, 138, 146, 147, 148, 149, 150, 151, 152, 153, 154, 162, 163, 164, 165, 166, 167, 168, 169, 170, 178, 179, 180, 181, 182, 183, 184, 185, 186, 194, 195, 196, 197, 198, 199, 200, 201, 202, 210, 211, 212, 213, 214, 215, 216, 217, 218, 226, 227, 228, 229, 230, 231, 232, 233, 234, 242, 243, 244, 245, 246, 247, 248, 249, 250 };
  static int DC_LUMINANCE_HUFFMANCODE[] = { 0, 0, 16384, 2, 24576, 3, 32768, 3, 40960, 3, 49152, 3, 57344, 3, 61440, 4, 63488, 5, 64512, 6, 65024, 7, 65280, 8, 65535, 9 };
  static int DC_CHROMINANCE_HUFFMANCODE[] = { 0, 0, 16384, 2, 32768, 2, 49152, 2, 57344, 3, 61440, 4, 63488, 5, 64512, 6, 65024, 7, 65280, 8, 65408, 9, 65472, 10, 65535, 11 };
  static int AC_LUMINANCE_HUFFMANCODE[] = { 0, 0, 16384, 2, 32768, 2, 40960, 3, 45056, 4, 49152, 4, 53248, 4, 55296, 5, 57344, 5, 59392, 5, 60416, 6, 61440, 6, 61952, 7, 62464, 7, 62976, 7, 63488, 7, 63744, 8, 64000, 8, 64256, 8, 64384, 9, 64512, 9, 64640, 9, 64768, 9, 64896, 9, 64960, 10, 65024, 10, 65088, 10, 65152, 10, 65216, 10, 65248, 11, 65280, 11, 65312, 11, 65344, 11, 65360, 12, 65376, 12, 65392, 12, 65408, 12, 65410, 15, 65535, 16 };
  static int AC_CHROMINANCE_HUFFMANCODE[] = { 0, 0, 16384, 2, 32768, 2, 40960, 3, 45056, 4, 49152, 4, 51200, 5, 53248, 5, 55296, 5, 57344, 5, 58368, 6, 59392, 6, 60416, 6, 61440, 6, 61952, 7, 62464, 7, 62976, 7, 63232, 8, 63488, 8, 63744, 8, 64000, 8, 64128, 9, 64256, 9, 64384, 9, 64512, 9, 64640, 9, 64768, 9, 64896, 9, 64960, 10, 65024, 10, 65088, 10, 65152, 10, 65216, 10, 65248, 11, 65280, 11, 65312, 11, 65344, 11, 65360, 12, 65376, 12, 65392, 12, 65408, 12, 65412, 14, 65414, 15, 65416, 15, 65535, 16 };
  static int8_t Tbl_100Y[] = { 2, 1, 1, 2, 3, 5, 6, 7, 1, 1, 1, 2, 3, 7, 7, 6, 1, 1, 2, 3, 5, 7, 8, 7, 1, 2, 2, 3, 6, 10, 10, 7, 2, 2, 4, 7, 8, 13, 12, 9, 3, 4, 6, 8, 10, 13, 14, 11, 6, 8, 9, 10, 12, 15, 15, 12, 9, 11, 11, 12, 14, 12, 12, 12 };
  static int8_t Tbl_100UV[] = { 3, 3, 4, 8, 18, 18, 18, 18, 3, 3, 4, 12, 18, 18, 18, 18, 4, 4, 10, 18, 18, 18, 18, 18, 8, 12, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18 };
  static int8_t Tbl_086Y[] = { 3, 2, 1, 3, 4, 7, 9, 11, 2, 2, 2, 3, 4, 10, 11, 10, 2, 2, 3, 4, 7, 10, 12, 10, 2, 3, 4, 5, 9, 16, 15, 11, 3, 4, 6, 10, 12, 20, 19, 14, 4, 6, 10, 12, 15, 19, 21, 17, 9, 12, 14, 16, 19, 22, 22, 18, 13, 17, 17, 18, 21, 18, 19, 18 };
  static int8_t Tbl_086UV[] = { 4, 5, 6, 13, 27, 27, 27, 27, 5, 5, 7, 18, 27, 27, 27, 27, 6, 7, 15, 27, 27, 27, 27, 27, 13, 18, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27 };
  static int8_t Tbl_071Y[] = { 6, 4, 3, 6, 9, 15, 19, 22, 4, 4, 5, 7, 9, 21, 22, 20, 5, 4, 6, 9, 15, 21, 25, 21, 5, 6, 8, 10, 19, 32, 30, 23, 6, 8, 13, 21, 25, 40, 38, 28, 9, 13, 20, 24, 30, 39, 42, 34, 18, 24, 29, 32, 38, 45, 45, 37, 27, 34, 35, 36, 42, 37, 38, 37 };
  static int8_t Tbl_071UV[] = { 9, 10, 13, 26, 55, 55, 55, 55, 10, 11, 14, 37, 55, 55, 55, 55, 13, 14, 31, 55, 55, 55, 55, 55, 26, 37, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55 };
  static int8_t Tbl_057Y[] = { 9, 6, 5, 9, 13, 22, 28, 34, 6, 6, 7, 10, 14, 32, 33, 30, 7, 7, 9, 13, 22, 32, 38, 31, 7, 9, 12, 16, 28, 48, 45, 34, 10, 12, 20, 31, 38, 61, 57, 43, 13, 19, 30, 36, 45, 58, 63, 51, 27, 36, 43, 48, 57, 68, 67, 56, 40, 51, 53, 55, 63, 56, 57, 55 };
  static int8_t Tbl_057UV[] = { 13, 14, 19, 38, 80, 80, 80, 80, 14, 17, 21, 53, 80, 80, 80, 80, 19, 21, 45, 80, 80, 80, 80, 80, 38, 53, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80 };
  static int8_t Tbl_043Y[] = { 11, 7, 7, 11, 17, 28, 36, 43, 8, 8, 10, 13, 18, 41, 43, 39, 10, 9, 11, 17, 28, 40, 49, 40, 10, 12, 15, 20, 36, 62, 57, 44, 12, 15, 26, 40, 48, 78, 74, 55, 17, 25, 39, 46, 58, 74, 81, 66, 35, 46, 56, 62, 74, 86, 86, 72, 51, 66, 68, 70, 80, 71, 74, 71 };
  static int8_t Tbl_043UV[] = { 18, 19, 26, 51, 108, 108, 108, 108, 19, 22, 28, 72, 108, 108, 108, 108, 26, 28, 61, 108, 108, 108, 108, 108, 51, 72, 108, 108, 108, 108, 108, 108, 108, 108, 108, 108, 108, 108, 108, 108, 108, 108, 108, 108, 108, 108, 108, 108, 108, 108, 108, 108, 108, 108, 108, 108, 108, 108, 108, 108, 108, 108, 108, 108 };
  static int8_t Tbl_029Y[] = { 14, 9, 9, 14, 21, 36, 46, 55, 10, 10, 12, 17, 23, 52, 54, 49, 12, 11, 14, 21, 36, 51, 62, 50, 12, 15, 19, 26, 46, 78, 72, 56, 16, 19, 33, 50, 61, 98, 93, 69, 21, 31, 49, 58, 73, 94, 102, 83, 44, 58, 70, 78, 93, 109, 108, 91, 65, 83, 86, 88, 101, 90, 93, 89 };
  static int8_t Tbl_029UV[] = { 22, 24, 32, 63, -123, -123, -123, -123, 24, 28, 34, 88, -123, -123, -123, -123, 32, 34, 75, -123, -123, -123, -123, -123, 63, 88, -123, -123, -123, -123, -123, -123, -123, -123, -123, -123, -123, -123, -123, -123, -123, -123, -123, -123, -123, -123, -123, -123, -123, -123, -123, -123, -123, -123, -123, -123, -123, -123, -123, -123, -123, -123, -123, -123 };
  static int8_t Tbl_014Y[] = { 17, 12, 10, 17, 26, 43, 55, 66, 13, 13, 15, 20, 28, 63, 65, 60, 15, 14, 17, 26, 43, 62, 75, 61, 15, 18, 24, 31, 55, 95, 87, 67, 19, 24, 40, 61, 74, 119, 112, 84, 26, 38, 60, 70, 88, 113, 123, 100, 53, 70, 85, 95, 112, -124, -125, 110, 78, 100, 103, 107, 122, 109, 112, 108 };
  static int8_t Tbl_014UV[] = { 27, 29, 39, 76, -96, -96, -96, -96, 29, 34, 42, 107, -96, -96, -96, -96, 39, 42, 91, -96, -96, -96, -96, -96, 76, 107, -96, -96, -96, -96, -96, -96, -96, -96, -96, -96, -96, -96, -96, -96, -96, -96, -96, -96, -96, -96, -96, -96, -96, -96, -96, -96, -96, -96, -96, -96, -96, -96, -96, -96, -96, -96, -96, -96 };
  static int8_t Tbl_000Y[] = { 20, 13, 12, 20, 30, 50, 63, 76, 15, 15, 17, 23, 32, 72, 75, 68, 17, 16, 20, 30, 50, 71, 86, 70, 17, 21, 27, 36, 63, 108, 100, 77, 22, 27, 46, 70, 85, -120, -128, 96, 30, 43, 68, 80, 101, -126, -115, 115, 61, 80, 97, 108, -128, -105, -106, 126, 90, 115, 118, 122, -116, 125, -128, 123 };
  static int8_t Tbl_000UV[] = { 31, 33, 45, 88, -71, -71, -71, -71, 33, 39, 48, 123, -71, -71, -71, -71, 45, 48, 105, -71, -71, -71, -71, -71, 88, 123, -71, -71, -71, -71, -71, -71, -71, -71, -71, -71, -71, -71, -71, -71, -71, -71, -71, -71, -71, -71, -71, -71, -71, -71, -71, -71, -71, -71, -71, -71, -71, -71, -71, -71, -71, -71, -71, -71 };
#endif