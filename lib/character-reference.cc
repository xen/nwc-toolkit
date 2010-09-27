// Copyright 2010 Susumu Yata <syata@acm.org>

#include <nwc-toolkit/character-reference.h>

#include <nwc-toolkit/darts.h>

namespace nwc_toolkit {
namespace {

// Returns a reference to the dictionary of character entity references.
const Darts::DoubleArray &GetEntityDic() {
  // List of character entity references and their code points.
  static const struct {
    const char *name;
    int code_point;
  } entities[] = {
    { "&AElig;", 198 }, { "&AMP;", 38 }, { "&Aacute;", 193 },
    { "&Acirc;", 194 }, { "&Agrave;", 192 }, { "&Alpha;", 913 },
    { "&Aring;", 197 }, { "&Atilde;", 195 }, { "&Auml;", 196 },
    { "&Beta;", 914 }, { "&COPY;", 169 }, { "&Ccedil;", 199 },
//    { "&Beta;", 914 }, { "&COPY;", 'c' }, { "&Ccedil;", 199 },
    { "&Chi;", 935 }, { "&Dagger;", 8225 }, { "&Delta;", 916 },
    { "&ETH;", 208 }, { "&Eacute;", 201 }, { "&Ecirc;", 202 },
    { "&Egrave;", 200 }, { "&Epsilon;", 917 }, { "&Eta;", 919 },
    { "&Euml;", 203 }, { "&GT;", 62 }, { "&Gamma;", 915 },
    { "&Iacute;", 205 }, { "&Icirc;", 206 }, { "&Igrave;", 204 },
    { "&Iota;", 921 }, { "&Iuml;", 207 }, { "&Kappa;", 922 },
    { "&LT;", 60 }, { "&Lambda;", 923 }, { "&Mu;", 924 },
    { "&Ntilde;", 209 }, { "&Nu;", 925 }, { "&OElig;", 338 },
    { "&Oacute;", 211 }, { "&Ocirc;", 212 }, { "&Ograve;", 210 },
    { "&Omega;", 937 }, { "&Omicron;", 927 }, { "&Oslash;", 216 },
    { "&Otilde;", 213 }, { "&Ouml;", 214 }, { "&Phi;", 934 },
    { "&Pi;", 928 }, { "&Prime;", 8243 }, { "&Psi;", 936 },
    { "&QUOT;", 34 }, { "&Rho;", 929 }, { "&Scaron;", 352 },
    { "&Sigma;", 931 }, { "&THORN;", 222 }, { "&Tau;", 932 },
    { "&Theta;", 920 }, { "&Uacute;", 218 }, { "&Ucirc;", 219 },
    { "&Ugrave;", 217 }, { "&Upsilon;", 933 }, { "&Uuml;", 220 },
    { "&Xi;", 926 }, { "&Yacute;", 221 }, { "&Yuml;", 376 },
    { "&Zeta;", 918 }, { "&aacute;", 225 }, { "&acirc;", 226 },
    { "&acute;", 180 }, { "&aelig;", 230 }, { "&agrave;", 224 },
    { "&alefsym;", 8501 }, { "&alpha;", 945 }, { "&amp;", 38 },
    { "&and;", 8743 }, { "&ang;", 8736 }, { "&apos;", 39 },
    { "&aring;", 229 }, { "&asymp;", 8776 }, { "&atilde;", 227 },
    { "&auml;", 228 }, { "&bdquo;", 8222 }, { "&beta;", 946 },
    { "&brvbar;", 166 }, { "&bull;", 8226 }, { "&cap;", 8745 },
    { "&ccedil;", 231 }, { "&cedil;", 184 }, { "&cent;", 162 },
    { "&chi;", 967 }, { "&circ;", 710 }, { "&clubs;", 9827 },
    { "&cong;", 8773 }, { "&copy;", 169 }, { "&crarr;", 8629 },
//    { "&cong;", 8773 }, { "&copy;", 'c' }, { "&crarr;", 8629 },
    { "&cup;", 8746 }, { "&curren;", 164 }, { "&dArr;", 8659 },
    { "&dagger;", 8224 }, { "&darr;", 8595 }, { "&deg;", 176 },
    { "&delta;", 948 }, { "&diams;", 9830 }, { "&divide;", 247 },
    { "&eacute;", 233 }, { "&ecirc;", 234 }, { "&egrave;", 232 },
    { "&empty;", 8709 }, { "&emsp;", 8195 }, { "&ensp;", 8194 },
    { "&epsilon;", 949 }, { "&equiv;", 8801 }, { "&eta;", 951 },
    { "&eth;", 240 }, { "&euml;", 235 }, { "&euro;", 8364 },
    { "&exist;", 8707 }, { "&forall;", 8704 }, { "&frac12;", 189 },
    { "&frac14;", 188 }, { "&frac34;", 190 }, { "&frasl;", 8260 },
    { "&gamma;", 947 }, { "&ge;", 8805 }, { "&gt;", 62 },
    { "&hArr;", 8660 }, { "&harr;", 8596 }, { "&hearts;", 9829 },
    { "&hellip;", 8230 }, { "&iacute;", 237 }, { "&icirc;", 238 },
    { "&iexcl;", 161 }, { "&igrave;", 236 }, { "&image;", 8465 },
    { "&infin;", 8734 }, { "&int;", 8747 }, { "&iota;", 953 },
    { "&iquest;", 191 }, { "&isin;", 8712 }, { "&iuml;", 239 },
    { "&kappa;", 954 }, { "&lArr;", 8656 }, { "&lambda;", 955 },
    { "&lang;", 9001 }, { "&laquo;", 171 }, { "&larr;", 8592 },
    { "&lceil;", 8968 }, { "&ldquo;", 8220 }, { "&le;", 8804 },
    { "&lfloor;", 8970 }, { "&lowast;", 8727 }, { "&loz;", 9674 },
    { "&lrm;", 8206 }, { "&lsaquo;", 8249 }, { "&lsquo;", 8216 },
    { "&lt;", 60 }, { "&macr;", 175 }, { "&mdash;", 8212 },
    { "&micro;", 181 }, { "&middot;", 183 }, { "&minus;", 8722 },
    { "&mu;", 956 }, { "&nabla;", 8711 }, { "&nbsp;", 160 },
//    { "&mu;", 956 }, { "&nabla;", 8711 }, { "&nbsp;", ' ' },
    { "&ndash;", 8211 }, { "&ne;", 8800 }, { "&ni;", 8715 },
    { "&not;", 172 }, { "&notin;", 8713 }, { "&nsub;", 8836 },
    { "&ntilde;", 241 }, { "&nu;", 957 }, { "&oacute;", 243 },
    { "&ocirc;", 244 }, { "&oelig;", 339 }, { "&ograve;", 242 },
    { "&oline;", 8254 }, { "&omega;", 969 }, { "&omicron;", 959 },
    { "&oplus;", 8853 }, { "&or;", 8744 }, { "&ordf;", 170 },
    { "&ordm;", 186 }, { "&oslash;", 248 }, { "&otilde;", 245 },
    { "&otimes;", 8855 }, { "&ouml;", 246 }, { "&para;", 182 },
    { "&part;", 8706 }, { "&permil;", 8240 }, { "&perp;", 8869 },
    { "&phi;", 966 }, { "&pi;", 960 }, { "&piv;", 982 },
    { "&plusmn;", 177 }, { "&pound;", 163 }, { "&prime;", 8242 },
    { "&prod;", 8719 }, { "&prop;", 8733 }, { "&psi;", 968 },
    { "&quot;", 34 }, { "&rArr;", 8658 }, { "&radic;", 8730 },
    { "&rang;", 9002 }, { "&raquo;", 187 }, { "&rarr;", 8594 },
    { "&rceil;", 8969 }, { "&rdquo;", 8221 }, { "&real;", 8476 },
    { "&reg;", 174 }, { "&rfloor;", 8971 }, { "&rho;", 961 },
    { "&rlm;", 8207 }, { "&rsaquo;", 8250 }, { "&rsquo;", 8217 },
    { "&sbquo;", 8218 }, { "&scaron;", 353 }, { "&sdot;", 8901 },
    { "&sect;", 167 }, { "&shy;", 173 }, { "&sigma;", 963 },
    { "&sigmaf;", 962 }, { "&sim;", 8764 }, { "&spades;", 9824 },
    { "&sub;", 8834 }, { "&sube;", 8838 }, { "&sum;", 8721 },
    { "&sup1;", 185 }, { "&sup2;", 178 }, { "&sup3;", 179 },
    { "&sup;", 8835 }, { "&supe;", 8839 }, { "&szlig;", 223 },
    { "&tau;", 964 }, { "&there4;", 8756 }, { "&theta;", 952 },
    { "&thetasym;", 977 }, { "&thinsp;", 8201 }, { "&thorn;", 254 },
    { "&tilde;", 732 }, { "&times;", 215 }, { "&trade;", 8482 },
    { "&uArr;", 8657 }, { "&uacute;", 250 }, { "&uarr;", 8593 },
    { "&ucirc;", 251 }, { "&ugrave;", 249 }, { "&uml;", 168 },
    { "&upsih;", 978 }, { "&upsilon;", 965 }, { "&uuml;", 252 },
    { "&weierp;", 8472 }, { "&xi;", 958 }, { "&yacute;", 253 },
    { "&yen;", 165 }, { "&yuml;", 255 }, { "&zeta;", 950 },
    { "&zwj;", 8205 }, { "&zwnj;", 8204 }
  };

  static bool initialized = false;
  static Darts::DoubleArray dic;

  if (!initialized) {
    std::size_t num_keys = sizeof(entities) / sizeof(entities[0]);
    const char *keys[sizeof(entities) / sizeof(entities[0])];
    int values[sizeof(entities) / sizeof(entities[0])];
    for (std::size_t i = 0; i < num_keys; ++i) {
      keys[i] = entities[i].name;
      values[i] = entities[i].code_point;
    }
    dic.build(num_keys, keys, NULL, values);
    initialized = true;
  }
  return dic;
}

}  // namespace

bool CharacterReference::Encode(const String &src, StringBuilder *dest) {
  bool modified = false;
  for (std::size_t i = 0; i < src.length(); ++i) {
    switch (src[i]) {
    case '<':
      dest->Append("&lt;");
      modified = true;
      break;
    case '>':
      dest->Append("&gt;");
      modified = true;
      break;
    case '&':
      dest->Append("&amp;");
      modified = true;
      break;
    default:
      dest->Append(src[i]);
      break;
    }
  }
  return modified;
}

bool CharacterReference::Encode(const String &src, StringBuilder *dest,
    EncodeQuotes) {
  bool modified = false;
  for (std::size_t i = 0; i < src.length(); ++i) {
    switch (src[i]) {
    case '<':
      dest->Append("&lt;");
      modified = true;
      break;
    case '>':
      dest->Append("&gt;");
      modified = true;
      break;
    case '&':
      dest->Append("&amp;");
      modified = true;
      break;
    case '"':
      dest->Append("&quot;");
      modified = true;
      break;
    default:
      dest->Append(src[i]);
      break;
    }
  }
  return modified;
}

bool CharacterReference::Decode(const String &src, StringBuilder *dest) {
  bool modified = false;
  for (String avail = src; !avail.IsEmpty(); ) {
    String amp = avail.FindFirstOf('&');
    dest->Append(String(avail.begin(), amp.begin()));

    String charref(amp.begin(), avail.end());
    if (!amp.IsEmpty()) {
      if (DecodeCharref(&charref, dest)) {
        modified = true;
      } else {
        dest->Append('&');
        charref = charref.SubString(0, 1);
      }
    }
    avail.set_begin(charref.end());
  }
  return modified;
}

bool CharacterReference::DecodeCharref(String *charref, StringBuilder *dest) {
  if (charref->length() < 3) {
    return false;
  } else if ((*charref)[1] == '#') {
    if ((*charref)[2] == 'x' || (*charref)[2] == 'X') {
      return DecodeHexadecimalCharacterReference(charref, dest);
    } else {
      return DecodeDecimalCharacterReference(charref, dest);
    }
  }
  return DecodeCharacterEntiryReference(charref, dest);
}

bool CharacterReference::DecodeHexadecimalCharacterReference(
    String *charref, StringBuilder *dest) {
  static const CharTable UPPER_TABLE("A-F");
  static const CharTable LOWER_TABLE("a-f");

  const String avail = charref->SubString(3);
  std::size_t index = 0;
  int code_point = 0;
  while (index < avail.length()) {
    if (IsDigit()(avail[index])) {
      code_point = (code_point * 16) + avail[index] - '0';
    } else if (UPPER_TABLE.Get(avail[index])) {
      code_point = (code_point * 16) + avail[index] - 'A' + 10;
    } else if (LOWER_TABLE.Get(avail[index])) {
      code_point = (code_point * 16) + avail[index] - 'a' + 10;
    } else {
      break;
    }

    if (code_point > MAX_CODE_POINT) {
      return false;
    }
    ++index;
  }
  if (index == 0 || index == avail.length() || avail[index] != ';') {
    return false;
  }

  *charref = charref->SubString(0, index + 4);
  EncodeCodePoint(code_point, dest);
  return true;
}

bool CharacterReference::DecodeDecimalCharacterReference(
    String *charref, StringBuilder *dest) {
  const String avail = charref->SubString(2);
  std::size_t index = 0;
  int code_point = 0;
  while (index < avail.length() && IsDigit()(avail[index])) {
    code_point = (code_point * 10) + avail[index] - '0';
    if (code_point > MAX_CODE_POINT) {
      return false;
    }
    ++index;
  }
  if (index == 0 || index == avail.length() || avail[index] != ';') {
    return false;
  }

  *charref = charref->SubString(0, index + 3);
  EncodeCodePoint(code_point, dest);
  return true;
}

bool CharacterReference::DecodeCharacterEntiryReference(
    String *charref, StringBuilder *dest) {
  static const Darts::DoubleArray &dic = GetEntityDic();

  Darts::DoubleArray::result_pair_type result = { 0, 0 };
  if (dic.commonPrefixSearch(charref->ptr(), &result, 1,
      charref->length()) == 0) {
    return false;
  }

  int code_point = result.value;
  *charref = charref->SubString(0, result.length);
  EncodeCodePoint(code_point, dest);
  return true;
}

void CharacterReference::EncodeCodePoint(int code_point, StringBuilder *dest) {
  static const int THRESHOLD_LIST[3] = { 1 << 11, 1 << 16, 1 << 21 };

  if (code_point < 0x80) {
    dest->Append(code_point);
    return;
  }

  std::size_t length = 0;
  for (std::size_t i = 0; i < 3; ++i) {
    if (code_point < THRESHOLD_LIST[i]) {
      length = i + 2;
      break;
    }
  }

  dest->Append((0x7F00 >> length) | (code_point >> (6 * (length - 1))));
  for (std::size_t i = 1; i < length; ++i) {
    dest->Append(0x80 | (((code_point >> (6 * (length - i - 1)))) & 0x3F));
  }
}

}  // namespace nwc_toolkit
