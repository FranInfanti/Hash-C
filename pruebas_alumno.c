#include "pa2m.h"
#include "src/hash.h"
#include <string.h>

void no_se_puede_insertar_en_un_hash_nulo()
{
	char *clave = "0x7eb17cbd";
	char *msj = "Ferriari";
	pa2m_afirmar(hash_insertar(NULL, clave, msj, NULL) == NULL,
		     "No se puede insertar un par en un hash nulo");
}

void no_se_puede_insertar_con_una_clave_nula()
{
	hash_t *hash = hash_crear(3);
	char *msj = "Toyota";
	pa2m_afirmar(hash_insertar(hash, NULL, msj, NULL) == NULL,
		     "No se puede insertar un par con clave nula");
	hash_destruir(hash);
}

void se_puede_insertar_elementos_en_un_hash_sin_remplazos()
{
	hash_t *hash = hash_crear(20);

	char *nombres[] = { "Fran",  "Otto", "Bianca", "Oli",
			    "Mateo", "Tomi", "Rocco" };
	char *equipos[] = { "Ferrari",	"Toyota",  "Glickenhaus", "Porsche",
			    "Cadillac", "Vanwall", "Peugeot" };
	int tamanio = 7;

	for (int i = 0; i < tamanio; i++)
		hash = hash_insertar(hash, nombres[i], equipos[i], NULL);

	bool exito = true;
	for (int i = 0; i < tamanio && exito; i++)
		exito = !!hash_obtener(hash, nombres[i]);

	pa2m_afirmar(exito, "Se insertaron los elementos correctamente");
	pa2m_afirmar(hash_cantidad(hash) == tamanio,
		     "La cantidad de elementos es correcta");

	hash_destruir(hash);
}

void se_puede_insertar_elementos_en_un_hash_con_remplazo()
{
	hash_t *hash = hash_crear(20);

	char *nombres[] = { "Fran",  "Otto", "Bianca", "Oli",
			    "Mateo", "Tomi", "Rocco" };
	char *equipos[] = { "Ferrari",	"Toyota",  "Glickenhaus", "Porsche",
			    "Cadillac", "Vanwall", "Peugeot" };
	int tamanio = 7;

	for (int i = 0; i < tamanio; i++)
		hash = hash_insertar(hash, nombres[i], equipos[i], NULL);

	char *nombre = "Bianca";
	char *equipo = "Jota";
	void *remplazado = equipo;

	hash = hash_insertar(hash, nombre, equipo, (void *)&remplazado);

	pa2m_afirmar(hash_obtener(hash, nombre) == equipo,
		     "Se actualizo el valor correctamente");
	pa2m_afirmar(remplazado == equipos[2],
		     "Se guardo correctamente el valor previo");
	pa2m_afirmar(hash_cantidad(hash) == tamanio,
		     "El tamaño es el correcto");

	hash_destruir(hash);
}

void se_puede_insertar_elementos_nulos()
{
	hash_t *hash = hash_crear(4);

	char *clave[] = { "0x00000000", "0x77073096", "0xee0e612c",
			  "0x990951ba" };
	size_t tamanio = 4;

	for (size_t i = 0; i < tamanio; i++)
		hash = hash_insertar(hash, clave[i], NULL, NULL);

	bool sigo = true;
	for (size_t i = 0; i < tamanio && sigo; i++)
		sigo = hash_contiene(hash, clave[i]);

	pa2m_afirmar(sigo, "Se puede almacenar valores NULL");
	pa2m_afirmar(hash_cantidad(hash) == tamanio,
		     "La cantidad de elementos es correcta");
	hash_destruir(hash);
}

void se_prueba_que_el_rehash_funcione_correctamente()
{
	hash_t *hash = hash_crear(3);

	char *clave[] = {
		"0x00000000",  "0x77073096",  "0xee0e612c",  "0x990951ba",
		"0x076dc419",  "0x2d02ef8d",  "0xe963a535",  "0x9e6495a3",
		"0x0edb8832",  "0x79dcb8a4",  "0xe0d5e91e",  "0x97d2d988",
		"0x09b64c2b",  "0x7eb17cbd",  "0xe7b82d07",  "0x90bf1d91",
		"0x1db71064",  "0x6ab020f2",  "0xf3b97148",  "0x84be41de",
		"0x1adad47d",  "0x6ddde4eb",  "0xf4d4b551",  "0x83d385c7",
		"0x136c9856",  "0x646ba8c0",  "0xfd62f97a",  "0x8a65c9ec",
		"0x14015c4f",  "0x63066cd9",  "0xfa0f3d63",  "0x8d080df5",
		"0x3b6e20c8",  "0x4c69105e",  "0xd56041e4",  "0xa2677172",
		"0x3c03e4d1",  "0x4b04d447",  "0xd20d85fd",  "0xa50ab56b",
		"0x35b5a8fa",  "0x42b2986c",  "0xdbbbc9d6",  "0xacbcf940",
		"0x32d86ce3",  "0x45df5c75",  "0xdcd60dcf",  "0xabd13d59",
		"0x26d930ac",  "0x51de003a",  "0xc8d75180",  "0xbfd06116",
		"0x21b4f4b5",  "0x56b3c423",  "0xcfba9599",  "0xb8bda50f",
		"0x2802b89e",  "0x5f058808",  "0xc60cd9b2",  "0xb10be924",
		"0x2f6f7c87",  "0x58684c11",  "0xc1611dab",  "0xb6662d3d",
		"0x76dc4190",  "0x01db7106",  "0x98d220bc",  "0xefd5102a",
		"0x71b18589",  "0x06b6b51f",  "0x9fbfe4a5",  "0xe8b8d433",
		"0x7807c9a2",  "0x0f00f934",  "0x9609a88e",  "0xe10e9818",
		"0x7f6a0dbb",  "0x086d3d2d",  "0x91646c97",  "0xe6635c01",
		"0x6b6b51f4",  "0x1c6c6162",  "0x856530d8",  "0xf262004e",
		"0x6c0695ed",  "0x1b01a57b",  "0x8208f4c1",  "0xf50fc457",
		"0x65b0d9c6",  "0x12b7e950",  "0x8bbeb8ea",  "0xfcb9887c",
		"0x62dd1ddf",  "0x15da2d49",  "0x8cd37cf3",  "0xfbd44c65",
		"0x4db26158",  "0x3ab551ce",  "0xa3bc0074",  "0xd4bb30e2",
		"0x4adfa541",  "0x3dd895d7",  "0xa4d1c46d",  "0xd3d6f4fb",
		"0x4369e96a",  "0x346ed9fc",  "0xad678846",  "0xda60b8d0",
		"0x44042d73",  "0x33031de5",  "0xaa0a4c5f",  "0xdd0d7cc9",
		"0x5005713c",  "0x270241aa",  "0xbe0b1010",  "0xc90c2086",
		"0x5768b525",  "0x206f85b3",  "0xb966d409",  "0xce61e49f",
		"0x5edef90e",  "0x29d9c998",  "0xb0d09822",  "0xc7d7a8b4",
		"0x59b33d17",  "0x2eb40d81",  "0xb7bd5c3b",  "0xc0ba6cad",
		"0xedb88320",  "0x9abfb3b6",  "0x03b6e20c",  "0x74b1d29a",
		"0xead54739",  "0x9dd277af",  "0x04db2615",  "0x73dc1683",
		"0xe3630b12",  "0x94643b84",  "0x0d6d6a3e",  "0x7a6a5aa8",
		"0xe40ecf0b",  "0x9309ff9d",  "0x0a00ae27",  "0x7d079eb1",
		"0xf00f9344",  "0x8708a3d2",  "0x1e01f268",  "0x6906c2fe",
		"0xf762575d",  "0x806567cb",  "0x196c3671",  "0x6e6b06e7",
		"0xfed41b76",  "0x89d32be0",  "0x10da7a5a",  "0x67dd4acc",
		"0xf9b9df6f",  "0x8ebeeff9",  "0x17b7be43",  "0x60b08ed5",
		"0xd6d6a3e8",  "0xa1d1937e",  "0x38d8c2c4",  "0x4fdff252",
		"0xd1bb67f1",  "0xa6bc5767",  "0x3fb506dd",  "0x48b2364b",
		"0xd80d2bda",  "0xaf0a1b4c",  "0x36034af6",  "0x41047a60",
		"0xdf60efc3",  "0xa867df55",  "0x316e8eef",  "0x4669be79",
		"0xcb61b38c",  "0xbc66831a",  "0x256fd2a0",  "0x5268e236",
		"0xcc0c7795",  "0xbb0b4703",  "0x220216b9",  "0x5505262f",
		"0xc5ba3bbe",  "0xb2bd0b28",  "0x2bb45a92",  "0x5cb36a04",
		"0xc2d7ffa7",  "0xb5d0cf31",  "0x2cd99e8b",  "0x5bdeae1d",
		"0x9b64c2b0",  "0xec63f226",  "0x756aa39c",  "0x026d930a",
		"0x9c0906a9",  "0xeb0e363f",  "0x72076785",  "0x05005713",
		"0x95bf4a82",  "0xe2b87a14",  "0x7bb12bae",  "0x0cb61b38",
		"0x92d28e9b",  "0xe5d5be0d",  "0x7cdcefb7",  "0x0bdbdf21",
		"0x86d3d2d4",  "0xf1d4e242",  "0x68ddb3f8",  "0x1fda836e",
		"0x81be16cd",  "0xf6b9265b",  "0x6fb077e1",  "0x18b74777",
		"0x88085ae6",  "0xff0f6a70",  "0x66063bca",  "0x11010b5c",
		"0x8f659eff",  "0xf862ae69",  "0x616bffd3",  "0x166ccf45",
		"0xa00ae278",  "0xd70dd2ee",  "0x4e048354",  "0x3903b3c2",
		"0xa7672661",  "0xd06016f7",  "0x4969474d",  "0x3e6e77db",
		"0xaed16a4a",  "0xd9d65adc",  "0x40df0b66",  "0x37d83bf0",
		"0xa9bcae53",  "0xdebb9ec5",  "0x47b2cf7f",  "0x30b5ffe9",
		"0xbdbdf21c",  "0xcabac28a",  "0x53b39330",  "0x24b4a3a6",
		"0xbad03605",  "0xcdd70693",  "0x54de5729",  "0x23d967bf",
		"0xb3667a2e",  "0xc4614ab8",  "0x5d681b02",  "0x2a6f2b94",
		"0xb40bbe37",  "0xc30c8ea1",  "0x5a05df1b",  "0x00000000L",
		"0xF26B8303L", "0xE13B70F7L", "0x1350F3F4L", "0x35F1141CL",
		"0x26A1E7E8L", "0xD4CA64EBL", "0xC79A971FL", "0x8AD958CFL",
		"0x78B2DBCCL", "0x6BE22838L", "0x9989AB3BL", "0x4D43CFD0L",
		"0xBF284CD3L", "0xAC78BF27L", "0x5E133C24L", "0x105EC76FL",
		"0xE235446CL", "0xF165B798L", "0x030E349BL", "0xD7C45070L",
		"0x25AFD373L", "0x36FF2087L", "0xC494A384L", "0x9A879FA0L",
		"0x68EC1CA3L", "0x7BBCEF57L", "0x89D76C54L", "0x5D1D08BFL",
		"0xAF768BBCL", "0xBC267848L", "0x4E4DFB4BL", "0x20BD8EDEL",
		"0xD2D60DDDL", "0xC186FE29L", "0x33ED7D2AL", "0xE72719C1L",
		"0x154C9AC2L", "0x061C6936L", "0xF477EA35L", "0xAA64D611L",
		"0x580F5512L", "0x4B5FA6E6L", "0xB93425E5L", "0x6DFE410EL",
		"0x9F95C20DL", "0x8CC531F9L", "0x7EAEB2FAL", "0x30E349B1L",
		"0xC288CAB2L", "0xD1D83946L", "0x23B3BA45L", "0xF779DEAEL",
		"0x05125DADL", "0x1642AE59L", "0xE4292D5AL", "0xBA3A117EL",
		"0x4851927DL", "0x5B016189L", "0xA96AE28AL", "0x7DA08661L",
		"0x8FCB0562L", "0x9C9BF696L", "0x6EF07595L", "0x417B1DBCL",
		"0xB3109EBFL", "0xA0406D4BL", "0x522BEE48L", "0x86E18AA3L",
		"0x748A09A0L", "0x67DAFA54L", "0x95B17957L", "0xCBA24573L",
		"0x39C9C670L", "0x2A993584L", "0xD8F2B687L", "0x0C38D26CL",
		"0xFE53516FL", "0xED03A29BL", "0x1F682198L", "0x5125DAD3L",
		"0xA34E59D0L", "0xB01EAA24L", "0x42752927L", "0x96BF4DCCL",
		"0x64D4CECFL", "0x77843D3BL", "0x85EFBE38L", "0xDBFC821CL",
		"0x2997011FL", "0x3AC7F2EBL", "0xC8AC71E8L", "0x1C661503L",
		"0xEE0D9600L", "0xFD5D65F4L", "0x0F36E6F7L", "0x61C69362L",
		"0x93AD1061L", "0x80FDE395L", "0x72966096L", "0xA65C047DL",
		"0x5437877EL", "0x4767748AL", "0xB50CF789L", "0xEB1FCBADL",
		"0x197448AEL", "0x0A24BB5AL", "0xF84F3859L", "0x2C855CB2L",
		"0xDEEEDFB1L", "0xCDBE2C45L", "0x3FD5AF46L", "0x7198540DL",
		"0x83F3D70EL", "0x90A324FAL", "0x62C8A7F9L", "0xB602C312L",
		"0x44694011L", "0x5739B3E5L", "0xA55230E6L", "0xFB410CC2L",
		"0x092A8FC1L", "0x1A7A7C35L", "0xE811FF36L", "0x3CDB9BDDL",
		"0xCEB018DEL", "0xDDE0EB2AL", "0x2F8B6829L", "0x82F63B78L",
		"0x709DB87BL", "0x63CD4B8FL", "0x91A6C88CL", "0x456CAC67L",
		"0xB7072F64L", "0xA457DC90L", "0x563C5F93L", "0x082F63B7L",
		"0xFA44E0B4L", "0xE9141340L", "0x1B7F9043L", "0xCFB5F4A8L",
		"0x3DDE77ABL", "0x2E8E845FL", "0xDCE5075CL", "0x92A8FC17L",
		"0x60C37F14L", "0x73938CE0L", "0x81F80FE3L", "0x55326B08L",
		"0xA759E80BL", "0xB4091BFFL", "0x466298FCL", "0x1871A4D8L",
		"0xEA1A27DBL", "0xF94AD42FL", "0x0B21572CL", "0xDFEB33C7L",
		"0x2D80B0C4L", "0x3ED04330L", "0xCCBBC033L", "0xA24BB5A6L",
		"0x502036A5L", "0x4370C551L", "0xB11B4652L", "0x65D122B9L",
		"0x97BAA1BAL", "0x84EA524EL", "0x7681D14DL", "0x2892ED69L",
		"0xDAF96E6AL", "0xC9A99D9EL", "0x3BC21E9DL", "0xEF087A76L",
		"0x1D63F975L", "0x0E330A81L", "0xFC588982L", "0xB21572C9L",
		"0x407EF1CAL", "0x532E023EL", "0xA145813DL", "0x758FE5D6L",
		"0x87E466D5L", "0x94B49521L", "0x66DF1622L", "0x38CC2A06L",
		"0xCAA7A905L", "0xD9F75AF1L", "0x2B9CD9F2L", "0xFF56BD19L",
		"0x0D3D3E1AL", "0x1E6DCDEEL", "0xEC064EEDL", "0xC38D26C4L",
		"0x31E6A5C7L", "0x22B65633L", "0xD0DDD530L", "0x0417B1DBL",
		"0xF67C32D8L", "0xE52CC12CL", "0x1747422FL", "0x49547E0BL",
		"0xBB3FFD08L", "0xA86F0EFCL", "0x5A048DFFL", "0x8ECEE914L",
		"0x7CA56A17L", "0x6FF599E3L", "0x9D9E1AE0L", "0xD3D3E1ABL",
		"0x21B862A8L", "0x32E8915CL", "0xC083125FL", "0x144976B4L",
		"0xE622F5B7L", "0xF5720643L", "0x07198540L", "0x590AB964L",
		"0xAB613A67L", "0xB831C993L", "0x4A5A4A90L", "0x9E902E7BL",
		"0x6CFBAD78L", "0x7FAB5E8CL", "0x8DC0DD8FL", "0xE330A81AL",
		"0x115B2B19L", "0x020BD8EDL", "0xF0605BEEL", "0x24AA3F05L",
		"0xD6C1BC06L", "0xC5914FF2L", "0x37FACCF1L", "0x69E9F0D5L",
		"0x9B8273D6L", "0x88D28022L", "0x7AB90321L", "0xAE7367CAL",
		"0x5C18E4C9L", "0x4F48173DL", "0xBD23943EL", "0xF36E6F75L",
		"0x0105EC76L", "0x12551F82L", "0xE03E9C81L", "0x34F4F86AL",
		"0xC69F7B69L", "0xD5CF889DL", "0x27A40B9EL", "0x79B737BAL",
		"0x8BDCB4B9L", "0x988C474DL", "0x6AE7C44EL", "0xBE2DA0A5L",
		"0x4C4623A6L", "0x5F16D052L", "0xAD7D5351L", "0x493C7D27",
		"0x9278FA4E",  "0xDB448769",  "0x211D826D",  "0x6821FF4A",
		"0xB3657823",  "0xFA590504",  "0x7D113D4F",  "0x423B04DA",
		"0x0B0779FD",  "0xD043FE94",  "0x997F83B3",  "0x632686B7",
		"0x2A1AFB90",  "0xF15E7CF9",  "0xB86201DE",  "0x847609B4",
		"0xCD4A7493",  "0x160EF3FA",  "0x5F328EDD",  "0xA56B8BD9",
		"0xEC57F6FE",  "0x37137197",  "0x7E2F0CB0",  "0xC64D0D6E",
		"0x8F717049",  "0x5435F720",  "0x1D098A07",  "0xE7508F03",
		"0xAE6CF224",  "0x7528754D",  "0x3C14086A",  "0x0D006599",
		"0x443C18BE",  "0x9F789FD7",  "0xD644E2F0",  "0x2C1DE7F4",
		"0x65219AD3",  "0xBE651DBA",  "0xF759609D",  "0x4F3B6143",
		"0x06071C64",  "0xDD439B0D",  "0x947FE62A",  "0x6E26E32E",
		"0x271A9E09",  "0xFC5E1960",  "0xB5626447",  "0x89766C2D",
		"0xC04A110A",  "0x1B0E9663",  "0x5232EB44",  "0xA86BEE40",
		"0xE1579367",  "0x3A13140E",  "0x732F6929",  "0xCB4D68F7",
		"0x827115D0",  "0x593592B9",  "0x1009EF9E",  "0xEA50EA9A",
		"0xA36C97BD",  "0x782810D4",  "0x31146DF3",  "0x1A00CB32",
		"0x533CB615",  "0x8878317C",  "0xC1444C5B",  "0x3B1D495F",
		"0x72213478",  "0xA965B311",  "0xE059CE36",  "0x583BCFE8",
		"0x1107B2CF",  "0xCA4335A6",  "0x837F4881",  "0x79264D85",
		"0x301A30A2",  "0xEB5EB7CB",  "0xA262CAEC",  "0x9E76C286",
		"0xD74ABFA1",  "0x0C0E38C8",  "0x453245EF",  "0xBF6B40EB",
		"0xF6573DCC",  "0x2D13BAA5",  "0x642FC782",  "0xDC4DC65C",
		"0x9571BB7B",  "0x4E353C12",  "0x07094135",  "0xFD504431",
		"0xB46C3916",  "0x6F28BE7F",  "0x2614C358",  "0x1700AEAB",
		"0x5E3CD38C",  "0x857854E5",  "0xCC4429C2",  "0x361D2CC6",
		"0x7F2151E1",  "0xA465D688",  "0xED59ABAF",  "0x553BAA71",
		"0x1C07D756",  "0xC743503F",  "0x8E7F2D18",  "0x7426281C",
		"0x3D1A553B",  "0xE65ED252",  "0xAF62AF75",  "0x9376A71F",
		"0xDA4ADA38",  "0x010E5D51",  "0x48322076",  "0xB26B2572",
		"0xFB575855",  "0x2013DF3C",  "0x692FA21B",  "0xD14DA3C5",
		"0x9871DEE2",  "0x4335598B",  "0x0A0924AC",  "0xF05021A8",
		"0xB96C5C8F",  "0x6228DBE6",  "0x2B14A6C1",  "0x34019664",
		"0x7D3DEB43",  "0xA6796C2A",  "0xEF45110D",  "0x151C1409",
		"0x5C20692E",  "0x8764EE47",  "0xCE589360",  "0x763A92BE",
		"0x3F06EF99",  "0xE44268F0",  "0xAD7E15D7",  "0x572710D3",
		"0x1E1B6DF4",  "0xC55FEA9D",  "0x8C6397BA",  "0xB0779FD0",
		"0xF94BE2F7",  "0x220F659E",  "0x6B3318B9",  "0x916A1DBD",
		"0xD856609A",  "0x0312E7F3",  "0x4A2E9AD4",  "0xF24C9B0A",
		"0xBB70E62D",  "0x60346144",  "0x29081C63",  "0xD3511967",
		"0x9A6D6440",  "0x4129E329",  "0x08159E0E",  "0x3901F3FD",
		"0x703D8EDA",  "0xAB7909B3",  "0xE2457494",  "0x181C7190",
		"0x51200CB7",  "0x8A648BDE",  "0xC358F6F9",  "0x7B3AF727",
		"0x32068A00",  "0xE9420D69",  "0xA07E704E",  "0x5A27754A",
		"0x131B086D",  "0xC85F8F04",  "0x8163F223",  "0xBD77FA49",
		"0xF44B876E",  "0x2F0F0007",  "0x66337D20",  "0x9C6A7824",
		"0xD5560503",  "0x0E12826A",  "0x472EFF4D",  "0xFF4CFE93",
		"0xB67083B4",  "0x6D3404DD",  "0x240879FA",  "0xDE517CFE",
		"0x976D01D9",  "0x4C2986B0",  "0x0515FB97",  "0x2E015D56",
		"0x673D2071",  "0xBC79A718",  "0xF545DA3F",  "0x0F1CDF3B",
		"0x4620A21C",  "0x9D642575",  "0xD4585852",  "0x6C3A598C",
		"0x250624AB",  "0xFE42A3C2",  "0xB77EDEE5",  "0x4D27DBE1",
		"0x041BA6C6",  "0xDF5F21AF",  "0x96635C88",  "0xAA7754E2",
		"0xE34B29C5",  "0x380FAEAC",  "0x7133D38B",  "0x8B6AD68F",
		"0xC256ABA8",  "0x19122CC1",  "0x502E51E6",  "0xE84C5038",
		"0xA1702D1F",  "0x7A34AA76",  "0x3308D751",  "0xC951D255",
		"0x806DAF72",  "0x5B29281B",  "0x1215553C",  "0x230138CF",
		"0x6A3D45E8",  "0xB179C281",  "0xF845BFA6",  "0x021CBAA2",
		"0x4B20C785",  "0x906440EC",  "0xD9583DCB",  "0x613A3C15",
		"0x28064132",  "0xF342C65B",  "0xBA7EBB7C",  "0x4027BE78",
		"0x091BC35F",  "0xD25F4436",  "0x9B633911",  "0xA777317B",
		"0xEE4B4C5C",  "0x350FCB35",  "0x7C33B612",  "0x866AB316",
		"0xCF56CE31",  "0x14124958",  "0x5D2E347F",  "0xE54C35A1",
		"0xAC704886",  "0x7734CFEF",  "0x3E08B2C8",  "0xC451B7CC",
		"0x8D6DCAEB",  "0x56294D82",  "0x1F1530A5",  "0x68032CC8",
		"0xD0065990",  "0xB8057558",  "0xA5E0C5D1",  "0xCDE3E919",
		"0x75E69C41",  "0x1DE5B089",  "0x706af48f",  "0x4E2DFD53",
		"0x262ED19B",  "0x9E2BA4C3",  "0xF628880B",  "0xEBCD3882",
		"0x83CE144A",  "0x3BCB6112",  "0x53C84DDA",  "0x9C5BFAA6",
		"0xF458D66E",  "0x4C5DA336",  "0x245E8FFE",  "0x39BB3F77",
		"0x51B813BF",  "0xE9BD66E7",  "0x81BE4A2F",  "0xD27607F5",
		"0xBA752B3D",  "0x02705E65",  "0x6A7372AD",  "0x7796C224",
		"0x1F95EEEC",  "0xA7909BB4",  "0xCF93B77C",  "0x3D5B83BD",
		"0x5558AF75",  "0xED5DDA2D",  "0x855EF6E5",  "0x98BB466C",
		"0xF0B86AA4",  "0x48BD1FFC",  "0x20BE3334",  "0x73767EEE",
		"0x1B755226",  "0xA370277E",  "0xCB730BB6",  "0xD696BB3F",
		"0xBE9597F7",  "0x0690E2AF",  "0x6E93CE67",  "0xA100791B",
		"0xC90355D3",  "0x7106208B",  "0x19050C43",  "0x04E0BCCA",
		"0x6CE39002",  "0xD4E6E55A",  "0xBCE5C992",  "0xEF2D8448",
		"0x872EA880",  "0x3F2BDDD8",  "0x5728F110",  "0x4ACD4199",
		"0x22CE6D51",  "0x9ACB1809",  "0xF2C834C1",  "0x7AB7077A",
		"0x12B42BB2",  "0xAAB15EEA",  "0xC2B27222",  "0xDF57C2AB",
		"0xB754EE63",  "0x0F519B3B",  "0x6752B7F3",  "0x349AFA29",
		"0x5C99D6E1",  "0xE49CA3B9",  "0x8C9F8F71",  "0x917A3FF8",
		"0xF9791330",  "0x417C6668",  "0x297F4AA0",  "0xE6ECFDDC",
		"0x8EEFD114",  "0x36EAA44C",  "0x5EE98884",  "0x430C380D",
		"0x2B0F14C5",  "0x930A619D",  "0xFB094D55",  "0xA8C1008F",
		"0xC0C22C47",  "0x78C7591F",  "0x10C475D7",  "0x0D21C55E",
		"0x6522E996",  "0xDD279CCE",  "0xB524B006",  "0x47EC84C7",
		"0x2FEFA80F",  "0x97EADD57",  "0xFFE9F19F",  "0xE20C4116",
		"0x8A0F6DDE",  "0x320A1886",  "0x5A09344E",  "0x09C17994",
		"0x61C2555C",  "0xD9C72004",  "0xB1C40CCC",  "0xAC21BC45",
		"0xC422908D",  "0x7C27E5D5",  "0x1424C91D",  "0xDBB77E61",
		"0xB3B452A9",  "0x0BB127F1",  "0x63B20B39",  "0x7E57BBB0",
		"0x16549778",  "0xAE51E220",  "0xC652CEE8",  "0x959A8332",
		"0xFD99AFFA",  "0x459CDAA2",  "0x2D9FF66A",  "0x307A46E3",
		"0x58796A2B",  "0xE07C1F73",  "0x887F33BB",  "0xF56E0EF4",
		"0x9D6D223C",  "0x25685764",  "0x4D6B7BAC",  "0x508ECB25",
		"0x388DE7ED",  "0x808892B5",  "0xE88BBE7D",  "0xBB43F3A7",
		"0xD340DF6F",  "0x6B45AA37",  "0x034686FF",  "0x1EA33676",
		"0x76A01ABE",  "0xCEA56FE6",  "0xA6A6432E",  "0x6935F452",
		"0x0136D89A",  "0xB933ADC2",  "0xD130810A",  "0xCCD53183",
		"0xA4D61D4B",  "0x1CD36813",  "0x74D044DB",  "0x27180901",
		"0x4F1B25C9",  "0xF71E5091",  "0x9F1D7C59",  "0x82F8CCD0",
		"0xEAFBE018",  "0x52FE9540",  "0x3AFDB988",  "0xC8358D49",
		"0xA036A181",  "0x1833D4D9",  "0x7030F811",  "0x6DD54898",
		"0x05D66450",  "0xBDD31108",  "0xD5D03DC0",  "0x8618701A",
		"0xEE1B5CD2",  "0x561E298A",  "0x3E1D0542",  "0x23F8B5CB",
		"0x4BFB9903",  "0xF3FEEC5B",  "0x9BFDC093",  "0x546E77EF",
		"0x3C6D5B27",  "0x84682E7F",  "0xEC6B02B7",  "0xF18EB23E",
		"0x998D9EF6",  "0x2188EBAE",  "0x498BC766",  "0x1A438ABC",
		"0x7240A674",  "0xCA45D32C",  "0xA246FFE4",  "0xBFA34F6D",
		"0xD7A063A5",  "0x6FA516FD",  "0x07A63A35",  "0x8FD9098E",
		"0xE7DA2546",  "0x5FDF501E",  "0x37DC7CD6",  "0x2A39CC5F",
		"0x423AE097",  "0xFA3F95CF",  "0x923CB907",  "0xC1F4F4DD",
		"0xA9F7D815",  "0x11F2AD4D",  "0x79F18185",  "0x6414310C",
		"0x0C171DC4",  "0xB412689C",  "0xDC114454",  "0x1382F328",
		"0x7B81DFE0",  "0xC384AAB8",  "0xAB878670",  "0xB66236F9",
		"0xDE611A31",  "0x66646F69",  "0x0E6743A1",  "0x5DAF0E7B",
		"0x35AC22B3",  "0x8DA957EB",  "0xE5AA7B23",  "0xF84FCBAA",
		"0x904CE762",  "0x2849923A",  "0x404ABEF2",  "0xB2828A33",
		"0xDA81A6FB",  "0x6284D3A3",  "0x0A87FF6B",  "0x17624FE2",
		"0x7F61632A",  "0xC7641672",  "0xAF673ABA",  "0xFCAF7760",
		"0x94AC5BA8",  "0x2CA92EF0",  "0x44AA0238",  "0x594FB2B1",
		"0x314C9E79",  "0x8949EB21",  "0xE14AC7E9",  "0x2ED97095",
		"0x46DA5C5D",  "0xFEDF2905",  "0x96DC05CD",  "0x8B39B544",
		"0xE33A998C",  "0x5B3FECD4",  "0x333CC01C",  "0x60F48DC6",
		"0x08F7A10E",  "0xB0F2D456",  "0xD8F1F89E",  "0xC5144817",
		"0xAD1764DF",  "0x15121187"
	};
	size_t tamanio = 813;

	for (size_t i = 0; i < tamanio; i++)
		hash = hash_insertar(hash, clave[i], NULL, NULL);

	bool sigo = true;
	for (size_t i = 0; i < tamanio && sigo; i++)
		sigo = hash_contiene(hash, clave[i]);

	pa2m_afirmar(
		sigo,
		"Se almacenaron 813 elementos correctamente causando un rehash");
	pa2m_afirmar(hash_cantidad(hash) == tamanio,
		     "La cantidad de elementos es correcta");
	hash_destruir(hash);
}

void no_se_puede_quitar_un_elemento_de_un_hash_nulo()
{
	char *clave = "0xf4d4b551";
	pa2m_afirmar(hash_quitar(NULL, clave) == NULL,
		     "No se puede quitar un elemento de un hash nulo");
}

void no_se_puede_quitar_un_elemento_con_clave_nula()
{
	hash_t *hash = hash_crear(4);
	char *clave = "0xf4d4b551";
	char *msj = "...........";
	hash = hash_insertar(hash, clave, msj, NULL);
	pa2m_afirmar(hash_quitar(hash, NULL) == NULL,
		     "No se puede quitar un elemento con una clave nula");
	hash_destruir(hash);
}

void se_puede_quitar_un_elemento_que_existe()
{
	hash_t *hash = hash_crear(10);

	char *nombres[] = { "Fran", "Otto", "Bianca", "Oli", "Mateo", "Rocco" };
	char *equipos[] = { "Ferrari", "Toyota",   "Glickenhaus",
			    "Porsche", "Cadillac", "Vanwall" };
	int tamanio = 6;

	for (int i = 0; i < tamanio; i++)
		hash = hash_insertar(hash, nombres[i], equipos[i], NULL);

	void *eliminado = hash_quitar(hash, "Oli");
	pa2m_afirmar(eliminado == equipos[3],
		     "Se elimino correctamente el valor esperado");
	pa2m_afirmar(hash_cantidad(hash) == tamanio - 1,
		     "La cantidad de elementos es correcta");

	hash_destruir_todo(hash, NULL);
}

void no_se_puede_quitar_un_elemento_que_no_existe()
{
	hash_t *hash = hash_crear(10);

	char *nombres[] = { "Fran", "Otto", "Bianca", "Mateo", "Rocco" };
	char *equipos[] = { "Ferrari", "Toyota", "Glickenhaus", "Porsche",
			    "Vanwall" };
	int tamanio = 5;

	for (int i = 0; i < tamanio; i++)
		hash = hash_insertar(hash, nombres[i], equipos[i], NULL);

	void *eliminado = hash_quitar(hash, "Oli");
	pa2m_afirmar(
		eliminado == NULL,
		"No se puede eliminar un elemento con una clave que no existe");
	pa2m_afirmar(hash_cantidad(hash) == tamanio,
		     "La cantidad de elementos no deberia cambiar");

	hash_destruir_todo(hash, NULL);
}

void no_se_puede_quitar_sobre_un_hash_vacio()
{
	hash_t *hash = hash_crear(3);
	char *clave = "0xf4d4b551";
	pa2m_afirmar(hash_quitar(hash, clave) == NULL,
		     "No se puede quitar un elemento de un hash vacio");
	hash_destruir(hash);
}

void no_se_puede_buscar_en_un_hash_nulo()
{
	char *clave = "0xf4d4b551";
	pa2m_afirmar(hash_obtener(NULL, clave) == NULL &&
			     !hash_obtener(NULL, clave),
		     "No se puede buscar en un hash nulo");
}

void no_se_puede_buscar_con_una_clave_nula()
{
	hash_t *hash = hash_crear(3);
	pa2m_afirmar(hash_obtener(hash, NULL) == NULL &&
			     !hash_obtener(hash, NULL),
		     "No se puede buscar con una clave nula");
	hash_destruir(hash);
}

void se_puede_buscar_un_elemento_que_existe()
{
	hash_t *hash = hash_crear(20);

	char *nombres[] = { "Fran",  "Otto", "Bianca", "Oli",
			    "Mateo", "Tomi", "Rocco",  "Max" };
	char *equipos[] = { "Ferrari",	"Toyota",  "Glickenhaus", "Porsche",
			    "Cadillac", "Vanwall", "Peugeot",	  "United" };
	int tamanio = 8;

	for (int i = 0; i < tamanio; i++)
		hash = hash_insertar(hash, nombres[i], equipos[i], NULL);

	char *buscado = hash_obtener(hash, "Fran");
	pa2m_afirmar(strcmp(buscado, "Ferrari") == 0 &&
			     hash_contiene(hash, "Fran"),
		     "Busco un elemento que existe y lo encuentro");

	hash_destruir(hash);
}

void no_se_puede_encontrar_un_elemento_que_no_existe()
{
	hash_t *hash = hash_crear(20);

	char *nombres[] = { "Fran",  "Otto", "Bianca", "Oli",
			    "Mateo", "Tomi", "Rocco",  "Max" };
	char *equipos[] = { "Ferrari",	"Toyota",  "Glickenhaus", "Porsche",
			    "Cadillac", "Vanwall", "Peugeot",	  "United" };
	int tamanio = 8;

	for (int i = 0; i < tamanio; i++)
		hash = hash_insertar(hash, nombres[i], equipos[i], NULL);

	char *buscado = hash_obtener(hash, "Leclerc");
	pa2m_afirmar(buscado == NULL && !hash_contiene(hash, "Leclerc"),
		     "Buscar un elemento que no existe devuelve NULL");

	hash_destruir(hash);
}

void no_se_busca_sobre_un_hash_vacio()
{
	hash_t *hash = hash_crear(20);
	pa2m_afirmar(hash_obtener(hash, "Fran") == NULL &&
			     !hash_contiene(hash, "Leclerc"),
		     "No puedo buscar un elemento en un hash vacio");

	hash_destruir(hash);
}

void no_puedo_buscar_un_elemento_que_estaba_en_el_hash()
{
	hash_t *hash = hash_crear(10);

	char *nombres[] = { "Fran", "Otto", "Bianca", "Oli", "Mateo", "Rocco" };
	char *equipos[] = { "Ferrari", "Toyota",   "Glickenhaus",
			    "Porsche", "Cadillac", "Vanwall" };
	int tamanio = 6;

	for (int i = 0; i < tamanio; i++)
		hash = hash_insertar(hash, nombres[i], equipos[i], NULL);

	hash_quitar(hash, "Oli");
	pa2m_afirmar(
		hash_obtener(hash, "Oli") == NULL &&
			!hash_contiene(hash, "Oli"),
		"Busco un elemento que estaba en el hash y no lo encuentro");
	hash_destruir_todo(hash, NULL);
}

bool sin_corte(const char *clave, void *valor, void *aux)
{
	return strcmp(clave, "Leclerc") != 0;
}

bool corte(const char *clave, void *valor, void *aux)
{
	return strcmp(clave, "Bianca") != 0;
}

void no_se_puede_iterar_con_un_hash_nulo()
{
	pa2m_afirmar(hash_con_cada_clave(NULL, sin_corte, NULL) == 0,
		     "No se puede iterar sobre un hash nulo");
}

void no_se_puede_iterar_con_una_funcion_nula()
{
	hash_t *hash = hash_crear(3);
	pa2m_afirmar(hash_con_cada_clave(hash, NULL, NULL) == 0,
		     "No se puede iterar con una funcion nula");
	hash_destruir(hash);
}

void se_puede_iterar_todos_los_elementos()
{
	hash_t *hash = hash_crear(20);

	char *nombres[] = { "Fran",  "Otto", "Bianca", "Oli",
			    "Mateo", "Tomi", "Rocco",  "Max" };
	char *equipos[] = { "Ferrari",	"Toyota",  "Glickenhaus", "Porsche",
			    "Cadillac", "Vanwall", "Peugeot",	  "United" };
	int tamanio = 8;

	for (int i = 0; i < tamanio; i++)
		hash = hash_insertar(hash, nombres[i], &equipos[i], NULL);

	pa2m_afirmar(hash_con_cada_clave(hash, sin_corte, NULL) == tamanio,
		     "Se recorrieron todos los elementos");

	hash_destruir(hash);
}

void se_puede_iterar_algunos_elementos()
{
	hash_t *hash = hash_crear(20);

	char *nombres[] = { "Fran",  "Otto", "Bianca", "Oli",
			    "Mateo", "Tomi", "Rocco",  "Max" };
	char *equipos[] = { "Ferrari",	"Toyota",  "Glickenhaus", "Porsche",
			    "Cadillac", "Vanwall", "Peugeot",	  "United" };
	int tamanio = 8;

	for (int i = 0; i < tamanio; i++)
		hash = hash_insertar(hash, nombres[i], &equipos[i], NULL);

	pa2m_afirmar(hash_con_cada_clave(hash, corte, NULL) == 7,
		     "Se recorrieron los elementos esperados");

	hash_destruir(hash);
}

void no_se_itera_sobre_un_hash_vacio()
{
	hash_t *hash = hash_crear(20);

	pa2m_afirmar(hash_con_cada_clave(hash, corte, NULL) == 0,
		     "No se itera ningun elemento de un hash vacio");

	hash_destruir(hash);
}

int main()
{
	pa2m_nuevo_grupo("Pruebas de Insercion");
	no_se_puede_insertar_en_un_hash_nulo();
	no_se_puede_insertar_con_una_clave_nula();
	se_puede_insertar_elementos_en_un_hash_sin_remplazos();
	se_puede_insertar_elementos_en_un_hash_con_remplazo();
	se_puede_insertar_elementos_nulos();

	pa2m_nuevo_grupo("Pruebas de Rehash");
	se_prueba_que_el_rehash_funcione_correctamente();

	pa2m_nuevo_grupo("Pruebas de Quitar");
	no_se_puede_quitar_un_elemento_de_un_hash_nulo();
	no_se_puede_quitar_un_elemento_con_clave_nula();
	se_puede_quitar_un_elemento_que_existe();
	no_se_puede_quitar_un_elemento_que_no_existe();
	no_se_puede_quitar_sobre_un_hash_vacio();

	pa2m_nuevo_grupo("Pruebas de Busqueda");
	no_se_puede_buscar_en_un_hash_nulo();
	no_se_puede_buscar_con_una_clave_nula();
	se_puede_buscar_un_elemento_que_existe();
	no_se_puede_encontrar_un_elemento_que_no_existe();
	no_se_busca_sobre_un_hash_vacio();
	no_puedo_buscar_un_elemento_que_estaba_en_el_hash();

	pa2m_nuevo_grupo("Pruebas de Iterador");
	no_se_puede_iterar_con_un_hash_nulo();
	no_se_puede_iterar_con_una_funcion_nula();
	se_puede_iterar_todos_los_elementos();
	se_puede_iterar_algunos_elementos();
	no_se_itera_sobre_un_hash_vacio();

	return pa2m_mostrar_reporte();
}