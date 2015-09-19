#include "rook.h"
#include "magic.h"

const struct magic_entry rook_magic_table[64] = {
    {rook_0, 0x101010101017e, 0x8000884000a230, 52},
    {rook_1, 0x202020202027c, 0x1040100120004002, 53},
    {rook_2, 0x404040404047a, 0x62000a0020803041, 53},
    {rook_3, 0x8080808080876, 0x200040840220010, 53},
    {rook_4, 0x1010101010106e, 0xa00280200201084, 53},
    {rook_5, 0x2020202020205e, 0x200049006000128, 53},
    {rook_6, 0x4040404040403e, 0x80008026000100, 53},
    {rook_7, 0x8080808080807e, 0x6080004060800100, 52},
    {rook_8, 0x1010101017e00, 0xc0802040008001, 53},
    {rook_9, 0x2020202027c00, 0x2002100409201, 54},
    {rook_10, 0x4040404047a00, 0x1001441082000, 54},
    {rook_11, 0x8080808087600, 0x2002012004088, 54},
    {rook_12, 0x10101010106e00, 0x4901001008010005, 54},
    {rook_13, 0x20202020205e00, 0x4000800200808400, 54},
    {rook_14, 0x40404040403e00, 0x4000802041001, 54},
    {rook_15, 0x80808080807e00, 0x401000040810012, 53},
    {rook_16, 0x10101017e0100, 0x1080004003502000, 53},
    {rook_17, 0x20202027c0200, 0x4680250040010880, 54},
    {rook_18, 0x40404047a0400, 0x1820050030204102, 54},
    {rook_19, 0x8080808760800, 0x1009010010018820, 54},
    {rook_20, 0x101010106e1000, 0x2043010008000510, 54},
    {rook_21, 0x202020205e2000, 0x200081801c000200, 54},
    {rook_22, 0x404040403e4000, 0x4000140010a80213, 54},
    {rook_23, 0x808080807e8000, 0xa060034014483, 53},
    {rook_24, 0x101017e010100, 0x8004424010a000, 53},
    {rook_25, 0x202027c020200, 0x1020200408020, 54},
    {rook_26, 0x404047a040400, 0x10205a200408010, 54},
    {rook_27, 0x8080876080800, 0x1020222001028c0, 54},
    {rook_28, 0x1010106e101000, 0x15280100050010, 54},
    {rook_29, 0x2020205e202000, 0x106200920010080c, 54},
    {rook_30, 0x4040403e404000, 0x10400500882, 54},
    {rook_31, 0x8080807e808000, 0x481910a00004094, 53},
    {rook_32, 0x1017e01010100, 0x500400028800080, 53},
    {rook_33, 0x2027c02020200, 0x81804004802000, 54},
    {rook_34, 0x4047a04040400, 0x4908809000802002, 54},
    {rook_35, 0x8087608080800, 0x18801001800800, 54},
    {rook_36, 0x10106e10101000, 0x40800800801400, 54},
    {rook_37, 0x20205e20202000, 0x2420800400800200, 54},
    {rook_38, 0x40403e40404000, 0x2400410204000810, 54},
    {rook_39, 0x80807e80808000, 0x100080411a000184, 53},
    {rook_40, 0x17e0101010100, 0x801a04000808000, 53},
    {rook_41, 0x27c0202020200, 0x5020022820042, 54},
    {rook_42, 0x47a0404040400, 0xa22200100910041, 54},
    {rook_43, 0x8760808080800, 0x1002000810220040, 54},
    {rook_44, 0x106e1010101000, 0x80100080011000d, 54},
    {rook_45, 0x205e2020202000, 0x4881000400090002, 54},
    {rook_46, 0x403e4040404000, 0x109203340008, 54},
    {rook_47, 0x807e8080808000, 0x1011008400420001, 53},
    {rook_48, 0x7e010101010100, 0x41110800060c300, 53},
    {rook_49, 0x7c020202020200, 0x400121008500, 54},
    {rook_50, 0x7a040404040400, 0x81530340200300, 54},
    {rook_51, 0x76080808080800, 0x2048820800100280, 54},
    {rook_52, 0x6e101010101000, 0x2800400c80080, 54},
    {rook_53, 0x5e202020202000, 0x181001208840100, 54},
    {rook_54, 0x3e404040404000, 0x7024102809020c00, 54},
    {rook_55, 0x7e808080808000, 0x120013044008200, 53},
    {rook_56, 0x7e01010101010100, 0x2400820020104106, 52},
    {rook_57, 0x7c02020202020200, 0xc00023008013, 53},
    {rook_58, 0x7a04040404040400, 0x140008c30012a001, 53},
    {rook_59, 0x7608080808080800, 0x10010004205059, 53},
    {rook_60, 0x6e10101010101000, 0x100200242009105a, 53},
    {rook_61, 0x5e20202020202000, 0x5081000400082607, 53},
    {rook_62, 0x3e40404040404000, 0x21000400a20001, 53},
    {rook_63, 0x7e80808080808000, 0x200022d00840246, 52},
};

uint64_t rook_attacks(uint64_t rook, uint64_t occupancy) {
    if (!rook)
        return 0ul;
    int index = lsb(rook);
    const struct magic_entry *m = &rook_magic_table[index];
    return m->attack_table[((occupancy & m->mask) * m->magic) >> m->shift];
}
