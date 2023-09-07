#include "utils.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


namespace Utility{
	
	std::string formatTimeStamp(uint64_t timestamp){
		std::ostringstream oss;
		oss << std::put_time(localtime(&timestamp), "%d/%m/%Y %H:%M:%S");
		return oss.str();
	}
	
	std::string formatTimeShort(double seconds) {

    int h((int) seconds / 3600);
    int min((int) seconds / 60 - h * 60);
    int sec((int) seconds - (h * 60 + min) * 60);

    std::ostringstream oss;
    if (h > 0) {
        oss << std::setfill('0') << std::setw(2) << h << ":";
    }
    //if (min > 0) {
        oss << std::setfill('0') << std::setw(2) << min << ":";
    //}
    oss << std::setfill('0') << std::setw(2) << sec;

    return oss.str();
	}
	
	urlschema parseUrl(std::string url){
	  urlschema res;
	  std::regex rgx("(?:([^:]*)://)?(?:([^:@]*)(?::([^@]*))?@)?(?:([^/:]*))?(?::([0-9]*))?/(/[^?#]*(=.*?/)/)?([^?#]*)?(?:([^#]*))?(?:#(.*))?");
      std::smatch match;
      if(std::regex_search(url, match, rgx)) {
         res.scheme = match[1];
		 res.user = match[2];
		 res.pass = match[3];
         res.server = match[4];
         res.port = match[5];
         res.path = match[8];
      }
	  return res;
	}
	
	std::string str_tolower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(), 
                   [](unsigned char c){ return std::tolower(c); }
                  );
    return s;
}


	bool compare(const remotefs_entry &a, const remotefs_entry &b) {

		if (a.isDir && !b.isDir) {
			return true;
		}
		if (!a.isDir && b.isDir) {
			return false;
		}
		return str_tolower(a.name) < str_tolower(b.name);
	}
	
	bool TxtLoadFromFile(std::string filename, GLuint* out_texture, int* out_width, int* out_height){
		int image_width = 0;
		int image_height = 0;
		unsigned char* image_data = stbi_load(filename.c_str(), &image_width, &image_height, NULL, 4);
		if (image_data == NULL)
			return false;
		
		GLuint id = 0;
		glGenTextures(1, &id);
		glBindTexture(GL_TEXTURE_2D, id);
		
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		
		//glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
    
		*out_texture = id;
		*out_width = image_width;
		*out_height = image_height;
		
		return true;
	}
	
	bool TxtLoadFromMemory(unsigned char* data,size_t image_size, GLuint* out_texture, int* out_width, int* out_height){
		int image_width = 0;
		int image_height = 0;
		int comp = 0;
		
		unsigned char* image_data = stbi_load_from_memory((const stbi_uc *)data, image_size, &image_width, &image_height, &comp, 0); 
		
		if (image_data == NULL){
			NXLOG::ERRORLOG("Failed to load IMG from memory\n");
			return false;
		}
		GLuint id = 0;
		glGenTextures(1, &id);
		glBindTexture(GL_TEXTURE_2D, id);
		
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		
		
		if(comp == 3)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image_width, image_height, 0, GL_RGB, GL_UNSIGNED_BYTE, image_data);
		else if(comp == 4)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);

		
		
		*out_texture = id;
		*out_width = image_width;
		*out_height = image_height;
		
		return true;
	}
	
	std::string humanSize(size_t bytes)
	{
		std::vector<std::string>suffix = {"B", "KB", "MB", "GB", "TB"};
	
		int i = 0;
		 double dblBytes = bytes;

		if (bytes > 1024) {
			for (i = 0; (bytes / 1024) > 0 && i<(int)suffix.size()-1; i++, bytes /= 1024)
				dblBytes = bytes / 1024.0;
			}

		char output[200];
		sprintf(output, "%.02lf %s", dblBytes, suffix[i].c_str());
		std::string ret = output;
		return ret;
	}
	
	bool startWith(const std::string &str, const std::string &value, bool sensitive) {
		if (sensitive) {
			return str.compare(0, value.length(), value) == 0;
		} else {
			std::string str_low = toLower(str);
			std::string val_low = toLower(value);
			return str_low.compare(0, val_low.length(), val_low) == 0;
		}
	}
	
	bool endsWith(const std::string &value, const std::string &ending, bool sensitive) {
		if (ending.size() > value.size()) return false;
		if (sensitive) {
			return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
		} else {
			std::string val_low = toLower(value);
			std::string end_low = toLower(ending);
			return std::equal(end_low.rbegin(), end_low.rend(), val_low.rbegin());
		}
	}
	
	
	std::string toLower(const std::string &str) {
		std::string ret = str;
		std::transform(ret.begin(), ret.end(), ret.begin(),[](unsigned char c) { return std::tolower(c); });
		return ret;
	}

	std::string toUpper(const std::string &str) {
		std::string ret = str;
		std::transform(ret.begin(), ret.end(), ret.begin(),[](unsigned char c) { return std::toupper(c); });
		return ret;
	}
	
	std::vector<std::string> getMediaExtensions() {
    return {
            ".8svx",
            ".aac",
            ".ac3",
            ".aif",
            ".asf",
            ".avi",
            ".dv",
            ".flv",
            ".m2ts",
            ".m2v",
            ".m4a",
            ".mkv",
            ".mov",
            ".mp3",
            ".mp4",
            ".mpeg",
            ".mpg",
            ".mts",
            ".ogg",
            ".rmvb",
            ".swf",
            ".ts",
            ".vob",
            ".wav",
            ".wma",
            ".wmv",
			".flac",
            ".m3u",
            ".m3u8",
			".webm",
			".jpg",
			".gif",
			".png"
    };
	}
	
	std::vector<lang_code_struct> getLanguages() {
    return {
			{"aa", "aar"},
			{"ab", "abk"},
			{"af", "afr"},
			{"ak", "aka"},
			{"sq", "alb"},
			{"am", "amh"},
			{"ar", "ara"},
			{"an", "arg"},
			{"hy", "arm"},
			{"as", "asm"},
			{"av", "ava"},
			{"ae", "ave"},
			{"ay", "aym"},
			{"az", "aze"},
			{"ba", "bak"},
			{"bm", "bam"},
			{"eu", "baq"},
			{"be", "bel"},
			{"bn", "ben"},
			{"bh", "bih"},
			{"bi", "bis"},
			{"bs", "bos"},
			{"br", "bre"},
			{"bg", "bul"},
			{"my", "bur"},
			{"ca", "cat"},
			{"ch", "cha"},
			{"ce", "che"},
			{"zh", "chi"},
			{"cu", "chu"},
			{"cv", "chv"},
			{"kw", "cor"},
			{"co", "cos"},
			{"cr", "cre"},
			{"cs", "cze"},
			{"da", "dan"},
			{"dv", "div"},
			{"nl", "dut"},
			{"dz", "dzo"},
			{"en", "eng"},
			{"eo", "epo"},
			{"et", "est"},
			{"ee", "ewe"},
			{"fo", "fao"},
			{"fj", "fij"},
			{"fi", "fin"},
			{"fr", "fre"},
			{"fr", "fre"},
			{"fy", "fry"},
			{"ff", "ful"},
			{"ka", "geo"},
			{"de", "ger"},
			{"gd", "gla"},
			{"ga", "gle"},
			{"gl", "glg"},
			{"gv", "glv"},
			{"el", "gre"},
			{"gn", "grn"},
			{"gu", "guj"},
			{"ht", "hat"},
			{"ha", "hau"},
			{"he", "heb"},
			{"hz", "her"},
			{"hi", "hin"},
			{"ho", "hmo"},
			{"hr", "hrv"},
			{"hu", "hun"},
			{"ig", "ibo"},
			{"is", "ice"},
			{"io", "ido"},
			{"ii", "iii"},
			{"iu", "iku"},
			{"ie", "ile"},
			{"ia", "ina"},
			{"id", "ind"},
			{"ik", "ipk"},
			{"it", "ita"},
			{"jv", "jav"},
			{"ja", "jpn"},
			{"kl", "kal"},
			{"kn", "kan"},
			{"ks", "kas"},
			{"kr", "kau"},
			{"kk", "kaz"},
			{"km", "khm"},
			{"ki", "kik"},
			{"rw", "kin"},
			{"ky", "kir"},
			{"kv", "kom"},
			{"kg", "kon"},
			{"ko", "kor"},
			{"kj", "kua"},
			{"ku", "kur"},
			{"lo", "lao"},
			{"la", "lat"},
			{"lv", "lav"},
			{"li", "lim"},
			{"ln", "lin"},
			{"lt", "lit"},
			{"lb", "ltz"},
			{"lu", "lub"},
			{"lg", "lug"},
			{"mk", "mac"},
			{"mh", "mah"},
			{"ml", "mal"},
			{"mi", "mao"},
			{"mr", "mar"},
			{"ms", "may"},
			{"mg", "mlg"},
			{"mt", "mlt"},
			{"mn", "mon"},
			{"na", "nau"},
			{"nv", "nav"},
			{"nr", "nbl"},
			{"nd", "nde"},
			{"ng", "ndo"},
			{"ne", "nep"},
			{"nn", "nno"},
			{"nb", "nob"},
			{"no", "nor"},
			{"ny", "nya"},
			{"oc", "oci"},
			{"oj", "oji"},
			{"or", "ori"},
			{"om", "orm"},
			{"os", "oss"},
			{"pa", "pan"},
			{"fa", "per"},
			{"pi", "pli"},
			{"pl", "pol"},
			{"pt", "por"},
			{"ps", "pus"},
			{"qu", "que"},
			{"rm", "roh"},
			{"ro", "rum"},
			{"ro", "rum"},
			{"rn", "run"},
			{"ru", "rus"},
			{"sg", "sag"},
			{"sa", "san"},
			{"si", "sin"},
			{"sk", "slo"},
			{"sk", "slo"},
			{"sl", "slv"},
			{"se", "sme"},
			{"sm", "smo"},
			{"sn", "sna"},
			{"sd", "snd"},
			{"so", "som"},
			{"st", "sot"},
			{"es", "spa"},
			{"sc", "srd"},
			{"sr", "srp"},
			{"ss", "ssw"},
			{"su", "sun"},
			{"sw", "swa"},
			{"sv", "swe"},
			{"ty", "tah"},
			{"ta", "tam"},
			{"tt", "tat"},
			{"te", "tel"},
			{"tg", "tgk"},
			{"tl", "tgl"},
			{"th", "tha"},
			{"bo", "tib"},
			{"ti", "tir"},
			{"to", "ton"},
			{"tn", "tsn"},
			{"ts", "tso"},
			{"tk", "tuk"},
			{"tr", "tur"},
			{"tw", "twi"},
			{"ug", "uig"},
			{"uk", "ukr"},
			{"ur", "urd"},
			{"uz", "uzb"},
			{"ve", "ven"},
			{"vi", "vie"},
			{"vo", "vol"},
			{"cy", "wel"},
			{"wa", "wln"},
			{"wo", "wol"},
			{"xh", "xho"},
			{"yi", "yid"},
			{"yo", "yor"},
			{"za", "zha"},
			{"zu", "zul"}
		  
		};
	
	
	}
	
	int getLanguagesIdx(std::string lang){
		for(int i=0;i<getLanguages().size();i++){
			if(getLanguages()[i].lang3 == lang){
				return i;
			}
		}
		return -1;
	}
	
	const std::string WHITESPACE = " \n\r\t\f\v";
 
	std::string ltrim(const std::string &s)
	{
		size_t start = s.find_first_not_of(WHITESPACE);
		return (start == std::string::npos) ? "" : s.substr(start);
	}
 
	std::string rtrim(const std::string &s)
	{
		size_t end = s.find_last_not_of(WHITESPACE);
		return (end == std::string::npos) ? "" : s.substr(0, end + 1);
	}
 
	std::string trim(const std::string &s) {
		return rtrim(ltrim(s));
	}
	
	std::string truncateLen(std::string path,int len){
		if(len>path.length())return path;
		std::string retpath = path.substr(0,len);
		return retpath + std::string("...");
	}
	
	void replace(std::string& subject, const std::string& search, const std::string& replace) {
	size_t pos = 0;
	while ((pos = subject.find(search, pos)) != std::string::npos) {
		subject.replace(pos, search.length(), replace);
		pos += replace.length();
		}
	}
	
	std::string scrapElement(std::string content, std::string get,std::string delim){
	std::string Element = "";
	if(content.length() <= 0)
	{return Element;}

	int val1 = 0, val2 = 0;
	val1 = content.find(get);

	if (val1 != -1)
	{
		std::string elmetTMP;
		if(delim.length())
			elmetTMP = delim;
		else{
			elmetTMP = content.substr(val1-1, 1);
			replace(elmetTMP, ">","<");
			replace(elmetTMP, "{","}");
			replace(elmetTMP, "[","]");
			replace(elmetTMP, "(",")");
		}
		val2 = content.find(elmetTMP, val1+get.length()+1);

		Element = content.substr(val1, val2 - val1);
		replace(Element, "\\", "");

		//std::cout << Element << std::endl;
	}
	return Element;
	}

std::string Nozomi_Link(std::string Link){
	curlDownloader jkObj;
	std::string jklink = jkObj.scrapeHtml(Link,"",false,"https://jkanime.net/",true);
	Link = "https://jkanime.net/" + scrapElement(jklink,"um2.php?", "\"");
    std::cout << "Secondkey: "<< Link << std::endl;
	std::string codetemp;
	//Get FirstKey
	std::string FirstKey = jkObj.scrapeHtml(Link,"",false,"https://jkanime.net/",true);
	codetemp = scrapElement(FirstKey,"name=\"data\" value=\"", "\"");
	replace(codetemp,"name=\"data\" value=\"","");
	FirstKey = codetemp;
	std::cout << "FirstKey: "<< FirstKey << std::endl;
	//Get SecondKey
	std::string data = "data=" + FirstKey;
	std::string SecondKey = jkObj.getRedirection("https://jkanime.net/gsplay/redirect_post.php",data,true,"https://jkanime.net/",true);
	std::cout << "Secondkey: "<< SecondKey << std::endl;
	//Get ThirdKey
	std::string second = "v=" + SecondKey;
	replace(second,"https://jkanime.net/gsplay/player.html#","");
	std::string ThirdKey = jkObj.scrapeHtml("https://jkanime.net/gsplay/api.php",second,true,"https://jkanime.net/",true);
	codetemp = scrapElement(ThirdKey,"https:", "\"");
	replace(codetemp,"\\","");
	ThirdKey = codetemp;
	std::cout << "ThirdKey: "<< ThirdKey << std::endl;
	//return URL
	return ThirdKey;
}


//testing keyboard

SwkbdTextCheckResult Keyboard_ValidateText(char *string, size_t size) {
	if (strcmp(string, "") == 0) {
		strncpy(string, "Please write a URL!", size);
		return SwkbdTextCheckResult_Bad;
	}

	return SwkbdTextCheckResult_OK;
}

bool GetChargeStatue(void) {

        Result ret = 0;
	PsmChargerType chargetype;
        if (R_FAILED(ret = psmGetChargerType(&chargetype)))
            return false;
	if(chargetype >0)return true;
	return false;
}

std::string KeyboardCall (std::string hint, std::string text){
	Result ret = 0;
	SwkbdConfig swkbd;
	static char input_string[512];

	if (R_FAILED(ret = swkbdCreate(&swkbd, 64))) {
		swkbdClose(&swkbd);
		return "";
	}
	
	swkbdConfigMakePresetDefault(&swkbd);
	swkbdConfigSetInitialCursorPos (&swkbd, 1);
	swkbdConfigSetOkButtonText(&swkbd,"Search");
	swkbdConfigSetHeaderText(&swkbd, "NXMP Stream URL");
	swkbdConfigSetSubText(&swkbd, hint.c_str());
	swkbdConfigSetStringLenMax(&swkbd, 512);
	

	if (strlen(hint.c_str()) != 0)
		swkbdConfigSetGuideText(&swkbd, hint.c_str());

	if (strlen(text.c_str()) != 0)
		swkbdConfigSetInitialText(&swkbd, text.c_str());
	
	swkbdConfigSetTextCheckCallback(&swkbd, Keyboard_ValidateText);

	if (R_FAILED(ret = swkbdShow(&swkbd, input_string, sizeof(input_string)))) {
		swkbdClose(&swkbd);
		return "";
	}

	swkbdClose(&swkbd);

	char *buf = (char*)malloc(512);
	strcpy(buf, input_string);
	return std::string(buf);
	
}

		
}


void Utility::FontLoader(std::string fontpath,float fontSize,ImGuiIO &io,ImFont* fontSmallTmp){
	NXLOG::DEBUGLOG("Init Fonts\n");
      
	unsigned char *pixels = nullptr;
	int width = 0, height = 0, bpp = 0;
	ImFontConfig font_cfg = ImFontConfig();
		
	font_cfg.OversampleH = font_cfg.OversampleV = 1;
	font_cfg.PixelSnapH = true;
		
	font_cfg.OversampleH = font_cfg.OversampleV = 1;
	font_cfg.PixelSnapH = true;
	NXLOG::DEBUGLOG("Loading TTF\n");
	
	font_cfg.OversampleH = font_cfg.OversampleV = 1;
	
	io.Fonts->AddFontFromFileTTF("romfs:/DejaVuSans.ttf", fontSize, &font_cfg, io.Fonts->GetGlyphRangesCyrillic());
	font_cfg.MergeMode = true;
	io.Fonts->AddFontFromFileTTF("romfs:/NotoSansCJKjp-Medium.otf", fontSize, &font_cfg, io.Fonts->GetGlyphRangesJapanese());
	
	
	io.Fonts->Flags |= ImFontAtlasFlags_NoPowerOfTwoHeight;
	io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height, &bpp);
	//io.Fonts->GetTexDataAsAlpha8(&pixels, &width, &height, &bpp);
	
	io.Fonts->Build();
}