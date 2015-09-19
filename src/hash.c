#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>
#include "bitboard.h"
#include "pawn.h"

#ifdef UNIT_TEST
#include "unit_tests.h"
#endif

uint64_t ZOBRIST_table_size;
int32_t *ZOBRIST_transposition_table;

/* Zobrist Hashing */
int seed = 0x4CF10177;
uint64_t ZOBRIST_piece_keys[2][6][64] = {
    {
        {
            0x21150CD672434756, 0x6E303DDB17DC1B22, 0x300CBD066BE8EDD5,
            0xE6CDF7C298221D7, 0x40D60D093152646A, 0x1EA81BFC2E0E7CBC,
            0x15DD34B51BDE7C6, 0x48CB69612A21CAF9, 0x4D0AC0B5613F41F5,
            0x3E0EDF554A0154E0, 0x500672E71DE008E7, 0x472BCD7A3DB6336B,
            0x46AC8E6C582603C9, 0x402AF08C2E737980, 0x6421A43410046058,
            0x4A95DFF90536B10A, 0x247A7AE38C61DD4, 0x1D12CC2C325464B4,
            0x24AF0BA92B7FABA8, 0x5BD6868C658518B3, 0x5CD210127A7EA288,
            0x1393956F5E2FE35E, 0x4C3C8A4F5C5EFED1, 0x851AE5719474B04,
            0x3D9E40C646608DAC, 0x63489FE50DA4B3AD, 0x644096932A746D5F,
            0x4B5AE7182AED2500, 0x29A71280B85D7A5, 0x59609E8066BC155C,
            0x1B8A37FD23F67E79, 0x6BF2C6661DD1DFAC, 0x5CBC9C4E09059293,
            0x50264460016BA7F7, 0x34853E3B2BFCCAEC, 0x66F0C0AA11574E4E,
            0x267B6D757A84561A, 0x6F8731AC72B7F7C4, 0x56E354EB77D8E003,
            0xBFF42C8148195B1, 0x3E396DAF6F47E2AD, 0x2226495E227A0442,
            0x19BC500C6D813076, 0x4D6729421C56C134, 0x7907081B26C7C7C3,
            0x312D69014914019, 0x4ABE463C6F059CF7, 0x32631FC5277AE28A,
            0x780B2F8A02896425, 0x28E68A822C906DC5, 0x2E862F120FD74B2C,
            0x3DE7BC1355019C87, 0xA5BA1462D6EEDBF, 0x47B9944B613EF631,
            0x2547CDC253B8D713, 0x75C08BE263813B71, 0x4300B9C117E6D540,
            0x5FB3FB45CBD09CD, 0x56805B7536268F6, 0x7913CB027E6F0DD2,
            0x7A2A30B97C26A192, 0x13004DEB44E876F6, 0x6B2C3E8945636DB0,
            0x6C63598063376E13
        },
        {
            0x47ECD1D61549E402, 0xFC7DBD9767300E8, 0x25212F2F4DAF97EC,
            0x4B749D6F2F7CD075, 0x7B1E85AC132E31BA, 0x10BBC6A72066536E,
            0x66E708CD067C5289, 0x3E78EE029E7C28E, 0x1E6327CA09E2CE94,
            0x6A4CC5C23CB2D81, 0x5D45378A7FB8975E, 0x223A3B53576F6844,
            0x7BDF38F0353A893F, 0x1C57DF3A670B777A, 0x7A9DF6EF08BB38BA,
            0x4A42E58D428AC8C5, 0x1E051CBD5A0AC166, 0x38FDC9AD43264BEC,
            0x27BA59530472671C, 0x72A31C6122D8DEFF, 0x17A098D6035EE308,
            0x433F326D7E87A1A4, 0x9DB35924726C14D, 0x286F6432283E5D5C,
            0x51098FE12F14308E, 0x4C098ADD2E4EC76C, 0x2ECCC7EC6E43C630,
            0x5BE2FB02AAC00DD, 0x237E4F6F22160EEA, 0x11B778571E1C465F,
            0x2AD147A45BFA5DE4, 0x60A70F2448D66461, 0x36051F4B19A4D8D2,
            0xBFCB04D5DBF789E, 0x1E173FEE7E9FCCAF, 0x98579D35B7D8C5,
            0x1FEAFB743D78A0A, 0x343F7A690BD9E549, 0xAFE4B585CAEDE9B,
            0x341842A55C07DB39, 0xBC30F2A0021CD82, 0xA56A2A53A8FD716,
            0x6E6593B31014D255, 0x653BD7F311E3E322, 0x322AE13F76F3504A,
            0x300029815CFC28E4, 0x52EDAE2F10A738A6, 0x25D28D4508F2CD7A,
            0x2A4C117831CF3D93, 0x66B2461848635166, 0x306F0A42674A9DB5,
            0x7E1B2A2B326DB9F9, 0x2B2227BF325AA494, 0x3E479F4336207317,
            0xF098330725FE1E8, 0x12284E511ACC925A, 0x7281AF6B1C7EF0F6,
            0x555C697060E7431E, 0x2C93C34C3A984164, 0x72CB26405EBEA48B,
            0x318B91AE22CB4FC2, 0x3BBACD6F04793FDD, 0x33728868618D5AB5,
            0xD6C0D575DBE99E0
        },
        {
            0x135C9848741E536F, 0x2621EB4643CBA28A, 0x5B68F124243D1572,
            0x76395C83068B18E4, 0x5697BA063480FBC6, 0x3CAB8BFB65A13D36,
            0x26E0DDAF4ED3DA4C, 0x6DCF9019628D1A, 0x6B52CB4355CA3901,
            0x7A49D03817E68E8F, 0x10627A656D14F678, 0x76A5331A41EE0C13,
            0xFE0463A3260008A, 0x46674BF14352CEA2, 0x13ED5B3F53D35948,
            0x211168822749F387, 0x47F1ACB8473353C9, 0x6B159611235A9DDC,
            0x6B70693B614EF294, 0x29E5B6C042082341, 0x15CFEE5B669142BC,
            0x27A960783CB0CC0A, 0x35651D0828173008, 0x5613592420B7E84B,
            0x7DE16909505D295C, 0x389E76DA0E43E36E, 0x3D721FD42F43A9F5,
            0x5031EF824D52660F, 0x61A3AA7F16993B73, 0x10A534B1759105BE,
            0x6A6C94BB31B69D34, 0x1CDAF945325E4173, 0x78E9F0FD07F08F56,
            0x55B8DF50645A5A38, 0x693F81EA7F9E9610, 0x26627D797F0F7045,
            0x662FD8CC4E0BDDF1, 0x3BC03C4F1B94F5D5, 0x76230DFA11D39573,
            0x3C4CDE2074047703, 0x6230BECF74EB54FB, 0x2485A721FA2DEA4,
            0x242EFEF0527A49F4, 0x6CF544B305D2A96F, 0x691385677D9A7964,
            0x7B63AF2D53801A22, 0x2F511698183EA872, 0x5DE5B96283B0795,
            0x202F37C85B973AE6, 0xC9561CD096EB9B2, 0x5B35D0F632F7DF47,
            0x87E29F84165A9C3, 0x103BD38443E6647, 0x5CFA9F987726CB32,
            0x5611FBBB19477DB8, 0x6B2B42363842BA8A, 0xE32D2B36D739CA8,
            0x57E5992E3261D1A3, 0x3FEDE69C44DADDE1, 0x38347B1229016C03,
            0x4275574633982A3F, 0x7C81862571C66DDE, 0x4BD6D2B1025FE1BB,
            0x1A0175746C060A79
        },
        {
            0x5DF71CA12696D741, 0x7574C42C392CED98, 0x598EB6887DF2EE24,
            0x7A92975B5A9273C1, 0x4231546B578D36F3, 0x51B93EF318435026,
            0x70D4B4AB3CE48129, 0x50860AB17F07875F, 0x2A581DD1286BA3DF,
            0x316959026A46046D, 0x6D4681C1699DD415, 0x134770702FBBD907,
            0x1D35FE540FC8F696, 0x218246E5690CD106, 0x1228D8513B83BC59,
            0x5512DB7F701FF4F3, 0x621A939B4A879FAB, 0x294CE28B3BA94A23,
            0x487A8DCF23DF79E6, 0x163BBDE40AABE23B, 0x7B6CB0D967F4FCD8,
            0x22EF32616C416584, 0x24D97E0173753D12, 0x6B48ECE34F319BD3,
            0x1BE0E0F21CB245E6, 0x3977A040092762B3, 0x65019FB4CBF10B1,
            0x38E33BBA2386184F, 0x5C8807475A65829F, 0xC92E9556EB0DF98,
            0x15E93EF961A5C4D5, 0x5ED0D48B7803D294, 0x2C2D6480081DB716,
            0x33AD1CB774A7F250, 0x2BFD30FC49E8DA9C, 0x7F53D48B2769E1D5,
            0x31DDD774224306EC, 0x13AB475A56B75575, 0x15B843FF7EF4343D,
            0x25E8F148319924F1, 0x1BA67A235F609189, 0x3AC087A421F6941E,
            0x2C1FA23A73A3C35E, 0x457CAC6E08A7A981, 0x4E0945FD520F95C3,
            0x7758891963F284F6, 0x33B55A9856295DA5, 0x5BF6578A5FE2BF19,
            0x5E4714BB0FA37442, 0x548AB1690A4445B8, 0x598C4EDE53DE85F4,
            0x31AE278D0B6A2652, 0x76218CE045596EE7, 0x62217BC70BD9D0DF,
            0x444DA325080A6D10, 0x3D72F5D05FF41D48, 0x676AFE9978337D74,
            0x1EAB167138AA0D3, 0x6BD740D247675DD5, 0x1C324A5439E086D0,
            0x1976F398138AD36D, 0x1DD30BC64D2C4E31, 0x69B4311279C96351,
            0x2D0F0D4A47FB45CE
        },
        {
            0x96CD7930199BEB3, 0x523F8B8662F92671, 0x557844A703EDB313,
            0x6E634CC34B99D187, 0x494721FB5084C88A, 0x5773A2670D94C520,
            0x588F359A14E69837, 0x6D88E2683FFA3433, 0xD1A15AC6F7393CF,
            0x5384D50678F1567E, 0x36DAF1A46FB71F5A, 0x32D1DD4E5051E53D,
            0x341F2C850A4E915, 0x1D7E336E6CF623DA, 0x4A6E4C664A8D40B8,
            0x34F169A853DB23F9, 0x4C26FF6B0730F52E, 0x36D44A6A219F4412,
            0xB1EA8422537972D, 0x6D3915995465CA3D, 0x75BC5FB744ACB800,
            0x61FA8F5D4E4B9552, 0x599350384F8371C5, 0xE45C98566AD65E4,
            0x3EF7059561CA9E8C, 0x5F9EBC6275D1F739, 0x5181BDE6127099B1,
            0x4623DC7654C3B0AE, 0x631582C663A20FE4, 0x41B9D4892D83CF2C,
            0x2E2F509C76AB3E31, 0x15EF3257A565007, 0x7DDC336038333D8F,
            0x1BF5941908FADBA2, 0x5D6AD4BC092EA9B3, 0x5D60A5DF53273473,
            0x4DDB61B33F5B353C, 0x2172C9C5276EB1EB, 0xEDEA7012FB8934B,
            0xE1C17CF4DD5AC96, 0x118331D76DBAD432, 0x43A7A3D06304EFBD,
            0x2B6DE309CB8046, 0x37C8A06C6340F0A9, 0x6D6D902B798274F5,
            0x10C4BFD51B9CE0C7, 0x702DB3261223B2FA, 0x15F330CF6E09E686,
            0x4A56F08931E8C4E8, 0x7704C22827C1C545, 0x3B176E9B54656807,
            0x7AE8F9B808F2D04F, 0x13C09D431C5BC37E, 0x3061823A229F4445,
            0x4C1456C93E7D9A0A, 0x7074F0DB5D9788A0, 0x2C386E3C341C94AB,
            0x409C785D2C63DC1F, 0x3DE814F2786518C9, 0xFA4CCC82B55A51D,
            0x71E78DBE20698C9D, 0x46F285E4621540E5, 0x328D3F975CE5B6B3,
            0x501F276B7CE43020
        },
        {
            0xECE7B9C4723E994, 0x24A5F56549E5EA37, 0x1B89519B1F8EEF1D,
            0x52D8BA862F49EEDF, 0x3BEAB29B033A3CC1, 0x51E9332407FF0964,
            0x41B7D6CB425E23FF, 0x659692046DF04507, 0x767AB8AB26330A62,
            0x1A5421263462CD9D, 0x1E98232B29F8EDEE, 0x5FB872BA107FB0EA,
            0x4A627A8B26AAF89E, 0x7294F1CF7CEFBA22, 0x390AF5242B4193A,
            0x79D3EA42125F2AEE, 0x9D802CE1E79DFA7, 0x5C4515252561546A,
            0x3E08CEC42F1DCFAC, 0x54AB434979F38160, 0x32580C6D2694766D,
            0x1F28AC4740FE338, 0x68F29A6C67891CC9, 0x6200283F5F6D5317,
            0xDBC272B7C544965, 0x13D020B42C544A56, 0x264D37537388936E,
            0x3CD3FB4070AFB1DE, 0x1A338C0D2F68ED0F, 0x6D9F6C001DC43B5F,
            0x721D064A67735642, 0x3023664D7BF50918, 0x5ED35E90C687B72,
            0x21565D8243F604AD, 0x3B864B1E7601A0CB, 0x3DE9860D6DDE578B,
            0x1C9617383FDC10D2, 0x61EE3AC30588B1A5, 0x27652D9B43EE6302,
            0x64F604BC352154C6, 0x4042AC6778C62571, 0x61759F1C668FE3BA,
            0x6C4EB8DF1E499A5D, 0x573F9598068244EC, 0x4DB2876C44DF0198,
            0x2446804B3FCF8DB6, 0x2C5257DA5469E698, 0x3BC496CF323F8DC3,
            0x60D2620B5D1AF451, 0x763592711C58AD29, 0x531C951D341F187E,
            0xA3704B56FB2AC55, 0x73FB29506C253F78, 0x753B5DFA1B6056EB,
            0x3013A27B5A3162B7, 0x5081ABB170564EE2, 0x52F7882831F74ACE,
            0x56E6329D3F464107, 0x5040E52B2E25C835, 0x45C885F41DF36C97,
            0x7304C9CE6A0F063F, 0x5DC2FA4E1F5721A8, 0x3E78ECD81987911D,
            0x5196AF6C1F4B4EE3
        },
    },
    {
        {
            0x76A2856E47CC41DD, 0x3BA3FC0C49BF1A8B, 0x7BEB5A5B45DB00C1,
            0x3971C6E16FE683AC, 0x3200403A2EAD24DB, 0xB46DA976213E2B5,
            0x8DE87925BC88649, 0x526A31975BD60FBA, 0xDBFD11729506434,
            0x1B1C50C25E00B642, 0x57762C6A60E4D6B6, 0x7BF422D94A7AF638,
            0x4AF3DCF559B71D27, 0x69D217E0096CC9CD, 0x733EAE443B68C74C,
            0x28B818B069E133B3, 0x3350929645C14BD, 0x33A04E3E7F206385,
            0x2A37157E6D12151F, 0x6F06E7315C3755B8, 0x1BBF39FB7A4DC1C8,
            0x3E4B386D249DC18D, 0x5616481110B56A05, 0x73D14863D61928,
            0x3A05CE391B90220A, 0x41D6CF6A117BFAA3, 0x7C74F8C03DCAF244,
            0x5BF6F0DB4768D5B5, 0x17820F6B45C908BC, 0x50D59F830AC0BDB0,
            0x131D008798DB833, 0x74A1F1630466D932, 0x5DE9CCF028423FA1,
            0x3873CB70820E26F, 0x155454C1728E23E8, 0x6458382731138EBC,
            0x6CDBE5B022A37095, 0x55B1504942F22DC2, 0x3358DA9A56252191,
            0x26C846EA6D5EA8D3, 0x71B5439B689F1655, 0x7EDAA3776E2A3C5B,
            0x266A08995AD19452, 0x359312113DEC1804, 0x209A9D0E0668B194,
            0x48ACD5B421CC6D17, 0x7FF669C73D4EC717, 0x263346495DE036B8,
            0x659106B929BA8300, 0x660119277AE55B7A, 0x1C48A6E84A59514E,
            0x2BF8EA3609248C98, 0x6CFCC1E301AA3A7F, 0x4C16BA5A20559C7D,
            0x57CF5C1172DF0145, 0xDB4455149849FAC, 0x5B7E179A0C8EE8C8,
            0x37AEDC0801E82033, 0x67607D1A6D41EE19, 0x3FD4383707FB1A29,
            0x73AA9FAD08810DEC, 0x29C7874073A10974, 0x45CFD5034FFACD89,
            0x5181402C2B60DBBC
        },
        {
            0x79B5508937825953, 0x2646373615FDF771, 0x1DBAAA2523F216C,
            0x1F2284096ED86C85, 0x53E95BEC6B393E64, 0xF2E09032BB8B7FD,
            0x5E183FA91CE24E54, 0x753D57A939965743, 0x2971371C2CEC33B1,
            0x3B7E777610D1B436, 0x1A2E21CA7B52AFAD, 0x18CCCE5F0DD8C177,
            0x3D3BD994294559F, 0x179CAEC49A3929D, 0x128F232852FB0B18,
            0x75046E590C4473B1, 0xA7D646C1B4AA590, 0x22426B220C590F0E,
            0x6D89C6FC4164EF2C, 0x7B317B93417322E8, 0x2C9E2D900A5F8496,
            0x6D2BDAE50AB66D39, 0x2741D2EA6269328F, 0x444CC47C50B30A06,
            0xF5566407FCB3BF2, 0x6184BE3D2983880B, 0x7B1DEB9F7A518C9C,
            0x375C49827EF1A939, 0x3CE5E23C38D6146E, 0x48953BD64F750564,
            0xBD11F873D99AA2F, 0x5BB97916164E83F3, 0x58E44FBF7DFBE438,
            0x22A79301466E16BC, 0x3F60D3641DD90E94, 0x7E139A46BFF00F4,
            0x2838932B750D148A, 0x76B56E2D4F7A6615, 0x577647193B0232A9,
            0x202D701C66CBAD59, 0x3ACD6E9B01B22E59, 0x104F356435EB5A3B,
            0x7C03BAF547AB7EE7, 0x34DD037438E99D31, 0x8193557D723F4A,
            0x85EA2960C52B2DC, 0x3B0BE97964181BAC, 0x22A136CF13F03939,
            0x6213FFE44548C9D0, 0x5A5E4FF52174D349, 0x6321D865623F8999,
            0xD73D43D0B5A6B90, 0x574C9E230429426B, 0x5AD4D1A52EC2E53C,
            0x3F2B75147B0241C1, 0x158E929679F8E3B0, 0x7CB4701A25DDC7FA,
            0x2FE43DEB78B82B10, 0x6D8946E164C1415F, 0x31A1C8416E0ADA37,
            0x623380A93A006AD7, 0x7A5D8D131D3F6A22, 0x1E1886831CFEC3E3,
            0x312FA35B002C8668
        },
        {
            0x62478DB30B8DF350, 0x21A159B145696618, 0x6DCD7CEA2F152DEE,
            0x50C3D1A8451A1B0D, 0x333E70592B98A34E, 0x73DD004A7269E56E,
            0x269AE50F096B92E0, 0x6C62C91E234F552A, 0x2F495ADA1C470709,
            0x1C07803A1CD2A1BC, 0x10848684DA9487B, 0xADD7BF3633BC911,
            0x7A9B353053B0906, 0x7B333325C239D6, 0x2239CCE931AAD68F,
            0x25EEC03E04815A9D, 0x3D38C9DF479019EF, 0x49EAC0B52B0646C9,
            0x76A547DE1AAE925E, 0x702061D729E3B837, 0x464735AC63FD6221,
            0x1C4D9DA56CE21ABB, 0x6D68F50108B066C3, 0x10316FE51CB24FDB,
            0x24F76DCC2C38F01F, 0x3984F19725FFB634, 0x79E2389B44626D8A,
            0x93B7F45018BEBEE, 0x499D769109B6B279, 0x274E25C46BD7437A,
            0x3B6189084D3CE603, 0x70589E17789A52E7, 0x14CCFFF23A435ECD,
            0x23A099B10B7247D0, 0x54F1F12B13C0FB88, 0x355600081B3926D7,
            0x77BE5DA951A39DAD, 0x81B4192652752AA, 0x5A540471184CB178,
            0x1D9A2857F4B723D, 0x4485A1973B5E941D, 0x254B28723E67DA32,
            0x7FC101A72E86A7B7, 0x3FF3C620495E7838, 0x383D5A306741EBE5,
            0x3535BBB3739EE338, 0x347ED1E8258E59CA, 0x6C393620494BD1DA,
            0x5FD1B8970FD9CFD1, 0x54BE19AB34C3A9C2, 0x239ACB590A1419B3,
            0x4FFCD0991B592902, 0x5BB7B7605818122C, 0x807BAC360BBBD1,
            0x7064C3A4025A1E31, 0x35572E0F34EA653B, 0x3DB8B24E5AA25681,
            0x73523F6E3D79B3F6, 0x928FE383346058E, 0x6D82C2E41665869,
            0x1A87F1733C0DE7E1, 0x35053BA14F06C35B, 0x619C41AC213E71C1,
            0x18529536416DFA43
        },
        {
            0x311841926D10AEE1, 0x7631A40654B30CEB, 0x7724C894462E749F,
            0x700C35ED52DC7FF4, 0x1E4686CB708CB199, 0x8E83BC60EAB4A6F,
            0x72E6CFCB3E3F69D5, 0x4395AFAB309F8219, 0x18E1C05636E7EF19,
            0x6E19360F220ABE8E, 0x6A2DF4A774F1623E, 0x637116F704B5E61B,
            0x30FF4A1F18765299, 0x53BCA976129B8BCB, 0x39B4C45A6C0F3EAC,
            0x5409860F6ACD05ED, 0x591FED8D4A3B2A15, 0x3F8012D85044B621,
            0x10699EB42F8C48C6, 0x232136162EB02580, 0x2018FA5F2C0971DC,
            0x3D5B6FEF12FFCA2A, 0x6A48DBB100F11F9A, 0x439F4C44032A9C07,
            0x37D90EB331B88253, 0x25355A952207035B, 0x26A9E49108A6718D,
            0x26BCE97657A92EB1, 0x211CC4267A7992EC, 0x6A44BA7C5AD18880,
            0x6688D1993E4E408B, 0x459E8E6D3FA8BF26, 0x8896AA0051EA146,
            0xFED754818F30955, 0x34AAEA0C330EAB5E, 0x47A32ED554C3E46B,
            0x5F181D3A04FE9EC4, 0x67C3AE964960F8EB, 0x5EFBE5F2B62FADA,
            0x4C8B94F23DC8CD12, 0x5D1B7D2D71C0EF87, 0x5FCFD06D03C561BF,
            0x7A676114068CB9E3, 0x5B6E90701B84253A, 0x1064CD045B34AEC,
            0x7655ADBB678F1E69, 0x4018B783BF43C28, 0x2737DD8F0C8AF618,
            0x4112DD6E372552D7, 0x257DFF6D75BDC77A, 0x6A33FE356D212E42,
            0x4A81ABE6494C1B6F, 0x721FCD0732455A7C, 0x12AD145A780F8B66,
            0x5DA855565F38A94C, 0x35D858783AC3D283, 0x50F998D415A828E6,
            0x3E8934424B60F9E8, 0x1C34E2C919F7C4B2, 0x66E51F231D3B2F99,
            0x5FAB0F9F5D3ACCDE, 0x4CA4E0263AC9B17, 0x192F09062C022B92,
            0x7037912F5A41E675
        },
        {
            0x63277E6915B5909D, 0x4FFFADEF4D5B7C9F, 0x2D6BEDF1A8159D5,
            0x16A7980E74F68BE6, 0x4CC6B4512954AC69, 0x6D06174C2A6F09A7,
            0x88D55B522DE6FC5, 0x6532DC2B5986EE89, 0x388698AB23BC106D,
            0x24E7E87254BB7B74, 0x3DB3D5200BCD0795, 0x71F6AB0E1D5EE4BF,
            0x6907D47376C0F910, 0x10B7FD60236DD79, 0x22C324A271431105,
            0x5C78C3EE05EAA30C, 0x6F8A1A22C7871DE, 0x53461FAB09CF6082,
            0x46F9CBB369EDB7B9, 0x7EC5EC6813C08005, 0x134264226BCC03B5,
            0x3E2F89AC1BCFB9D8, 0xEAA737A236265D7, 0x7556A86147310C25,
            0x471E76451A3E90D3, 0x1BEC879904D24B65, 0x260B98680DE332A7,
            0x223130240F136CDB, 0x4A42BB8233CAFFA, 0x114A4A552767505A,
            0x147FC0FF6DC30E43, 0x2D51F3661B7862A2, 0x1A3B802100981311,
            0x2547C32461354BD5, 0x6A85CACB240DAF8C, 0x74F5CBDA7DC82EED,
            0xFD9B34133255586, 0x1997E8C51E8426BB, 0x5687BB5E0EEE9127,
            0x65B532E01DA631A3, 0x292D21FA01A1BA7A, 0x22787D084F38BA63,
            0xF84ED2144A9AD2C, 0x5E4C273E142918D9, 0x67E65D266F967193,
            0x3B9069347C661E25, 0x5D597FD768E25C9A, 0x17DE80C77794FFF8,
            0x697A6FAC3D2643EB, 0x58CA4BCD54003A77, 0x6133F3784DC017A7,
            0x51C86964710DA6B9, 0xE56D2E6B60522A, 0xF91CD75576D288C,
            0x7A4EE35175470055, 0x75135A2F237C054B, 0x76E8BACF178BD737,
            0x72B4BFAE066DA7F1, 0x5C3584635100E6ED, 0x1A96C0CA441BE189,
            0x40975880562729FE, 0x4081FFAE1DF0D857, 0x3F09869958608076,
            0x1585D8502883F645
        },
        {
            0x1586C4616E50241D, 0x7C8430BC76BAB7D9, 0x3C103BC54E4C9A20,
            0x67C85E933CF5A8F3, 0x39ACEC4A775A2C08, 0x1462D17F33FBCF9B,
            0x6CA12C5D09762BAE, 0x5777D4E76389E72D, 0x210202E54A2C9495,
            0x69F78F1E7D378748, 0x1B2D7B82048E4FE8, 0x415368D15BC4D403,
            0x5AB579E701D5687F, 0x79B5AC5A19BF0080, 0x5A35E8F50F3B84AA,
            0x4242F6C56FBCAD57, 0x7D8BA8C83EC72781, 0x66776530399BE48D,
            0xD13C1A14E3FC3C3, 0x76918D8046C0ADEC, 0x4599EFCB0AF45EFF,
            0x7ABC7D87323B1C29, 0x146A8AAD5234526E, 0x15C50356356C8D92,
            0x1C60E7047FBC9274, 0x32A414DA378E6286, 0x44AE25C73F77DAB,
            0x135336895F005C43, 0x75CCE62A0D08E2E4, 0x78BF5CC35002CF20,
            0x1C44678E3B025388, 0x3FBF7C7719D01056, 0x79C97B092636E1A7,
            0x536BF4E306DD3CAB, 0x7476A56B49FD8263, 0x4D9DEA973A109536,
            0x54F1E162485A681E, 0x6C4BB15F695C6C0F, 0x1A8EBA8D0210B4B5,
            0x1EC8F9A136EFA191, 0x1CD4729516D0E7B, 0x6E7E041706182986,
            0x45648C2601D13AA1, 0x651885C93B317251, 0xEDA1D855DD7E28D,
            0xB3441712B1E8513, 0x18DA36154AF3BDE8, 0x44EE956A12A3B11F,
            0x712A9F8F185A8A4D, 0x1980EDCA65A144FA, 0x62580CB1671ED861,
            0x1FB1DA313749EE13, 0x2F79407F0BFD8B90, 0x20A65A234A07FB0C,
            0xE0E40463F6F53C4, 0xF79C9D0FDB876F, 0x10DC62406F75A0B5,
            0x15F3B0F55640EE66, 0x7146DB567B0C36BF, 0x117260B70020F8DB,
            0x58E4194C1CA6A228, 0x2B3F7DEE71BE4F61, 0x679A6010702E1358,
            0x462008058C4FFA0
        },
    },
};
uint64_t ZOBRIST_castle_q_keys[2] = {0x8889DA61DE2EE4A, 0x3E66449A6AE0AA57};
uint64_t ZOBRIST_castle_k_keys[2] = {0x501C6AB5E181ECB, 0x222A986A347B072B};
uint64_t ZOBRIST_enpassant_keys[8] = {
    0x6A15AA5C42D0F28D, 0x7E8302377823EAA2, 0x24046527F7A9ED5,
    0x7FF7211131CA892, 0x6EF03F8A1DF32307, 0x695D96F860371AE0,
    0x18FF59C67ACFF7B0, 0x605813BB71E37312
};
uint64_t ZOBRIST_turn_key = 0x177699D80B9791A9;

uint64_t rand_int64() {
    return ((uint64_t)rand() << 32) | (uint64_t)rand();
}

void ZOBRIST_prng_init(int seed) {
    srand(seed);
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 6; j++) {
            for (int k = 0; k < 64; k++) {
                ZOBRIST_piece_keys[i][j][k] = rand_int64();
            }
        }
    }
    ZOBRIST_castle_q_keys[0] = rand_int64();
    ZOBRIST_castle_q_keys[1] = rand_int64();
    ZOBRIST_castle_k_keys[0] = rand_int64();
    ZOBRIST_castle_k_keys[1] = rand_int64();
    for (int i = 0; i < 8; i++) {
        ZOBRIST_enpassant_keys[i] = rand_int64();
    }
    ZOBRIST_turn_key = rand_int64();
}

void ZOBRIST_rng_init() {
    FILE *f = fopen("/dev/urandom", "r");
    fread(ZOBRIST_piece_keys, 1, sizeof(ZOBRIST_piece_keys), f);
    fread(ZOBRIST_castle_q_keys, 1, sizeof(ZOBRIST_castle_q_keys), f);
    fread(ZOBRIST_castle_k_keys, 1, sizeof(ZOBRIST_castle_k_keys), f);
    fread(ZOBRIST_enpassant_keys, 1, sizeof(ZOBRIST_enpassant_keys), f);
    fread(&ZOBRIST_turn_key, 1, sizeof(ZOBRIST_turn_key), f);
}

uint64_t ZOBRIST_hash(struct board_state *board) {
    uint64_t hash = 0ul;
    if (board->turn == BLACK) {
        hash ^= ZOBRIST_turn_key;
    }
    for (int i = 0; i < 2; i++) {
        /* Pieces */
        for (int j = KING; j < ALL; j++) {
            /* bit scan */ 
            uint64_t set = board->bb[i][j];
            while (set) {
                int index = lsb(set);
                hash ^= ZOBRIST_piece_keys[i][j][index];
                set &= set - 1;
            }
        }
        /* Castling Rights */
        if (board->flags.castle_q[i]) {
            hash ^= ZOBRIST_castle_q_keys[i];
        }
        if (board->flags.castle_k[i]) {
            hash ^= ZOBRIST_castle_k_keys[i];
        }
        /* En passant */
        for (int j = 0; j < 8; j++) {
            if ((1ul << j) &bboard_to_fset(board->flags.en_passant[i])) {
                hash ^= ZOBRIST_enpassant_keys[i];
            }
        }
    }
    return hash;
}

void ZOBRIST_transposition_init(uint64_t table_size) {
    ZOBRIST_table_size = table_size;
    ZOBRIST_transposition_table = malloc(sizeof(int32_t) * table_size);
    memset(ZOBRIST_transposition_table, 0xFF, sizeof(int32_t) * table_size);
}

void print_literal_keys() {
    printf("uint64_t ZOBRIST_piece_keys[2][6][64] = {\n");
    for (int i = 0; i < 2; i++) {
        printf("    {\n");
        for (int j = 0; j < 6; j++) {
            printf("        {\n");
            for (int k = 0; k < 63; k += 3) {
                printf("            0x%"PRIu64"X, 0x%"PRIu64"X, 0x%"PRIu64"X,\n",
                        ZOBRIST_piece_keys[i][j][k],
                        ZOBRIST_piece_keys[i][j][k + 1],
                        ZOBRIST_piece_keys[i][j][k + 2]);
            }
            printf("            0x%"PRIu64"X\n", ZOBRIST_piece_keys[i][j][63]);
            printf("        },\n");
        }
        printf("    },\n");
    }
    printf("};\n");

    printf("uint64_t ZOBRIST_castle_q_keys[2][2] = {0x%"PRIu64"X, 0x%"PRIu64"X};\n",
            ZOBRIST_castle_q_keys[0], ZOBRIST_castle_q_keys[1]);
    printf("uint64_t ZOBRIST_castle_k_keys[2][2] = {0x%"PRIu64"X, 0x%"PRIu64"X};\n",
            ZOBRIST_castle_k_keys[0], ZOBRIST_castle_k_keys[1]);

    printf("uint64_t ZOBRIST_enpassant_keys[8] = {\n");
    for (int i = 0; i < 8; i++) {
        printf("    0x%"PRIu64"X,\n", ZOBRIST_enpassant_keys[i]);
    }
    printf("};\n");
    printf("uint64_t ZOBRIST_turn_key = 0x%"PRIu64"X;\n", ZOBRIST_turn_key);
}
