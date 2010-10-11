// Copyright 2010 Susumu Yata <syata@acm.org>

#include <nwc-toolkit/html-document.h>

#include <unordered_set>

#include <nwc-toolkit/character-encoding.h>
#include <nwc-toolkit/character-reference.h>
#include <nwc-toolkit/string-hash.h>

namespace nwc_toolkit {
namespace {

const std::unordered_set<String, StringHash> &GetBlockTagNameSet() {
  const char *tag_names[] = {
    "address", "article", "aside", "blockquote", "br", "caption", "center",
    "dd", "dialog", "dir", "div", "dl", "dt", "fieldset", "figure",
    "footer", "form", "frame", "h1", "h2", "h3", "h4", "h5", "h6",
    "header", "hr", "isindex", "legenda", "li", "menu", "multicol", "nav",
    "noframes", "noscript", "ol", "p", "pre", "section", "table", "tbody",
    "td", "tfoot", "th", "thead", "title", "tr", "ul", "xmp"
  };

  static bool is_initialized = false;
  static std::unordered_set<String, StringHash> tag_name_set;

  if (!is_initialized) {
    std::size_t num_keys = sizeof(tag_names) / sizeof(tag_names[0]);
    for (std::size_t i = 0; i < num_keys; ++i) {
      tag_name_set.insert(tag_names[i]);
    }
    is_initialized = true;
  }
  return tag_name_set;
}

}  // namespace

HtmlDocument::HtmlDocument()
    : body_(),
      src_encoding_(),
      content_type_(),
      parser_mode_(UNDEFINED_MODE),
      units_(),
      attributes_(),
      string_pool_(),
      temp_buf_(),
      symbol_stack_() {}

void HtmlDocument::Clear() {
  body_.Clear();
  src_encoding_.Clear();
  content_type_.Clear();
  ClearUnits();
}

bool HtmlDocument::Parse(const HtmlArchiveEntry &entry) {
  Clear();

  if (!entry.ExtractUnicodeBody(&body_, &src_encoding_)) {
    return false;
  }

  static const String UTF_8_BOM = "\xEF\xBB\xBF";

  String body = body_.str();
  while (body.StartsWith(UTF_8_BOM)) {
    body = body.SubString(UTF_8_BOM.length());
  }

  String path(entry.url().begin(), entry.url().FindFirstOf("?#").begin());
  entry.ExtractContentType(&content_type_);

  if ((content_type_.str() == "text/plain") || path.EndsWith(".txt")) {
    if (ParseAsPlainText(body)) {
      return true;
    }
  } else if (ParseAsXml(body) || ParseAsHtml(body)) {
    return true;
  }

  Clear();
  return false;
}

void HtmlDocument::ExtractText(StringBuilder *dest) {
  int mode_flags = 0;
  if (parser_mode() == PLAIN_TEXT_MODE) {
    mode_flags |= PLAINTEXT_MODE_FLAG;
  }

  for (std::size_t i = 0; i < num_units(); ++i) {
    const HtmlDocumentUnit &unit = this->unit(i);
    switch (unit.type()) {
      case HtmlDocumentUnit::TEXT_UNIT: {
        if ((mode_flags & INVISIBLE_MODE_FLAGS) != 0) {
          break;
        } else if (unit.is_cdata_section() ||
            ((mode_flags & (PLAIN_MODE_FLAGS | PRE_MODE_FLAGS)) != 0)) {
          AppendToText(unit.text_content(), KEEP_END_OF_LINE, dest);
        } else {
          AppendToText(unit.text_content(), REPLACE_END_OF_LINE, dest);
        }
        break;
      }
      case HtmlDocumentUnit::TAG_UNIT: {
        UpdateTextExtractorModeFlags(unit, &mode_flags);
        if (IsBlockTag(unit.tag_name())) {
          AppendEndOfLineToText(dest);
        }
        break;
      }
      case HtmlDocumentUnit::COMMENT_UNIT:
      case HtmlDocumentUnit::OTHER_UNIT:
      default: {
        break;
      }
    }
  }
  AppendEndOfLineToText(dest);
}

bool HtmlDocument::IsBlockTag(const String &tag_name) {
  static const std::unordered_set<String, StringHash> block_tag_name_set =
      GetBlockTagNameSet();
  return block_tag_name_set.find(tag_name) != block_tag_name_set.end();
}

void HtmlDocument::ClearUnits() {
  parser_mode_ = UNDEFINED_MODE;
  units_.clear();
  attributes_.clear();
  string_pool_.Clear();
  temp_buf_.Clear();
  symbol_stack_.clear();
}

bool HtmlDocument::ParseAsPlainText(const String &body) {
  ClearUnits();
  parser_mode_ = PLAIN_TEXT_MODE;

  AppendTextUnit(body, body, PLAIN_TEXT_FLAG);
  return true;
}

bool HtmlDocument::ParseAsXml(const String &body) {
  ClearUnits();
  parser_mode_ = XML_MODE;

  if (!body.StartsWith("<?xml", ToLower())) {
    return false;
  }

  String body_left = body;
  for (String avail = body_left; !avail.is_empty(); ) {
    String tag(avail.FindFirstOf('<').begin(), avail.end());

    static const CharTable TAG_NAME_BEGIN_TABLE("A-Za-z/!?");
    if (tag.length() > 1 && TAG_NAME_BEGIN_TABLE.Get(tag[1])) {
      String text_content(body_left.begin(), tag.begin());
      AppendTextUnit(text_content, text_content);

      if (tag.StartsWith("<!--")) {
        if (!ParseXmlCommentUnit(&tag)) {
          return false;
        }
      } else if (tag.StartsWith("<![CDATA[")) {
        if (!ParseXmlTextUnit(&tag)) {
          return false;
        }
      } else if (tag.StartsWith("<!") || tag.StartsWith("<?")) {
        if (!ParseXmlOtherUnit(&tag)) {
          return false;
        }
      } else if (!ParseXmlTagUnit(&tag)) {
        return false;
      }
      body_left.set_begin(tag.end());
    } else if (!tag.is_empty()) {
      return false;
    }
    avail.set_begin(tag.end());
  }
  AppendTextUnit(body_left, body_left);

  FixTagUnits();
  FixAttributes();
  return true;
}

bool HtmlDocument::ParseAsHtml(const String &body) {
  ClearUnits();
  parser_mode_ = HTML_MODE;

  String body_left = body;
  for (String avail = body; !avail.is_empty(); ) {
    String tag(avail.FindFirstOf('<').begin(), avail.end());

    static const CharTable TAG_NAME_BEGIN_TABLE("A-Za-z/!?");
    if (tag.length() > 1 && TAG_NAME_BEGIN_TABLE.Get(tag[1])) {
      String text_content(body_left.begin(), tag.begin());
      AppendTextUnit(text_content, text_content);

      if (tag.StartsWith("<!--")) {
        ParseHtmlCommentUnit(&tag);
      } else if (tag.StartsWith("<!") || tag.StartsWith("<?")) {
        ParseHtmlOtherUnit(&tag);
      } else {
        ParseHtmlTagUnit(&tag);
        const String &tag_name = units_.back().tag_name();
        if (tag_name == "script" || tag_name == "style" || tag_name == "xmp") {
          avail.set_begin(tag.end());
          ParseHtmlSpecialTag(avail, tag_name, &tag);
        } else if (tag_name == "plaintext") {
          avail.set_begin(tag.end());
          AppendTextUnit(avail, avail, PLAIN_TEXT_FLAG);
          body_left.Clear();
          avail.Clear();
          break;
        }
      }
      body_left.set_begin(tag.end());
    } else if (!tag.is_empty()) {
      tag = tag.SubString(0, 1);
    }
    avail.set_begin(tag.end());
  }
  AppendTextUnit(body_left, body_left);

  FixTagUnits();
  FixAttributes();
  return true;
}

bool HtmlDocument::ParseXmlTextUnit(String *tag) {
  static const String START_MARK = "<![CDATA[";
  static const String END_MARK = "]]>";

  String avail = tag->SubString(START_MARK.length());
  String tag_end = avail.Find(END_MARK);
  if (tag_end.is_empty()) {
    return false;
  }
  String text_content(avail.begin(), tag_end.begin());
  tag->set_end(tag_end.end());
  AppendTextUnit(*tag, text_content, CDATA_SECTION_FLAG);
  return true;
}

bool HtmlDocument::ParseXmlCommentUnit(String *tag) {
  static const String START_MARK = "<!--";
  static const String END_MARK = "-->";

  String avail = tag->SubString(START_MARK.length());
  String tag_end = avail.Find(END_MARK);
  if (tag_end.is_empty()) {
    return false;
  }
  String comment(avail.begin(), tag_end.begin());
  tag->set_end(tag_end.end());
  AppendCommentUnit(*tag, comment);
  return true;
}

bool HtmlDocument::ParseXmlTagUnit(String *tag) {
  static const CharTable BAD_NAME_TABLE("!\"#$%&'()*+,/;<=>?{|}@[\\]^`~");

  static const String TAG_START_MARK = "<";
  static const String END_TAG_START_MARK = "/";

  String avail = tag->SubString(TAG_START_MARK.length());
  int tag_unit_flags = 0;
  if (avail.StartsWith(END_TAG_START_MARK)) {
    tag_unit_flags |= END_TAG_FLAG;
    avail = avail.SubString(END_TAG_START_MARK.length());
  } else {
    tag_unit_flags |= START_TAG_FLAG;
  }
  std::size_t num_attributes_backup = attributes_.size();

  static const CharTable TAG_NAME_DELIM_TABLE(" \t\r\n/>");

  String tag_name_end = avail.FindFirstOf(TAG_NAME_DELIM_TABLE);
  if (tag_name_end.is_empty()) {
    return false;
  }
  String tag_name(avail.begin(), tag_name_end.begin());
  if (tag_name.Contains(BAD_NAME_TABLE)) {
    return false;
  }

  avail.set_begin(tag_name_end.begin());
  avail = avail.StripLeft();
  while (!avail.is_empty()) {
    if (avail[0] == '>') {
      avail = avail.SubString(1);
      break;
    } else if (avail[0] == '<') {
      return false;
    } else if (avail[0] == '/') {
      if (((tag_unit_flags & START_TAG_FLAG) == START_TAG_FLAG) &&
          (avail.length() > 1) && (avail[1] == '>')) {
        tag_unit_flags |= EMPTY_ELEMENT_TAG_FLAG;
        avail = avail.SubString(2);
        break;
      }
      return false;
    }

    static const CharTable ATTRIBUTE_NAME_DELIM_TABLE(" \t\r\n=/>");
    String attribute_name_end = avail.FindFirstOf(ATTRIBUTE_NAME_DELIM_TABLE);
    if (attribute_name_end.is_empty()) {
      return false;
    }
    String attribute_name(avail.begin(), attribute_name_end.begin());
    if (attribute_name.Contains(BAD_NAME_TABLE)) {
      return false;
    }

    avail.set_begin(attribute_name_end.begin());
    avail = avail.StripLeft();
    if (!avail.StartsWith("=")) {
      return false;
    }
    avail = avail.SubString(1).StripLeft();
    if (avail.is_empty() || ((avail[0] != '\'') && (avail[0] != '"'))) {
      return false;
    }
    String attribute_value = avail.SubString(1);
    String attribute_value_end = attribute_value.FindFirstOf(avail[0]);
    if (attribute_value_end.is_empty()) {
      return false;
    }
    attribute_value.set_end(attribute_value_end.begin());

    HtmlDocumentAttribute attribute;
    attribute.set_name(attribute_name);
    attribute.set_value(attribute_value);
    attributes_.push_back(attribute);

    avail.set_begin(attribute_value_end.end());
    avail = avail.StripLeft();
  }
  tag->set_end(avail.begin());

  std::size_t num_attributes = attributes_.size() - num_attributes_backup;
  AppendTagUnit(*tag, tag_name, num_attributes, tag_unit_flags);
  return true;
}

bool HtmlDocument::ParseXmlOtherUnit(String *tag) {
  static const String QUESTION_START_MARK = "<?";
  static const String QUESTION_END_MARK = "?>";

  if (tag->StartsWith(QUESTION_START_MARK)) {
    String avail = tag->SubString(QUESTION_START_MARK.length());
    String tag_end = avail.Find(QUESTION_END_MARK);
    if (tag_end.is_empty()) {
      return false;
    }
    tag->set_end(tag_end.end());
    AppendOtherUnit(*tag, *tag);
    return true;
  }

  static const String EXCLAMATION_START_MARK = "<!";

  String avail = tag->SubString(EXCLAMATION_START_MARK.length());
  symbol_stack_.clear();
  symbol_stack_.push_back('<');
  while (!symbol_stack_.empty()) {
    char symbol = symbol_stack_.back();
    switch (symbol) {
      case '<': {
        static const CharTable SYMBOL_TABLE("<>[\"'");
        String next_symbol = avail.FindFirstOf(SYMBOL_TABLE);
        if (next_symbol.is_empty()) {
          return false;
        } else if (next_symbol[0] == '>') {
          symbol_stack_.pop_back();
        } else {
          symbol_stack_.push_back(next_symbol[0]);
        }
        avail.set_begin(next_symbol.end());
        break;
      }
      case '[': {
        static const CharTable SYMBOL_TABLE("<[]\"'");
        String next_symbol = avail.FindFirstOf(SYMBOL_TABLE);
        if (next_symbol.is_empty()) {
          return false;
        } else if (next_symbol[0] == ']') {
          symbol_stack_.pop_back();
        } else {
          symbol_stack_.push_back(next_symbol[0]);
        }
        avail.set_begin(next_symbol.end());
        break;
      }
      case '"':
      case '\'': {
        String next_quote = avail.FindFirstOf(symbol);
        if (next_quote.is_empty()) {
          return false;
        }
        avail.set_begin(next_quote.end());
        symbol_stack_.pop_back();
        break;
      }
    }
  }
  tag->set_end(avail.begin());
  AppendOtherUnit(*tag, *tag);
  return true;
}

void HtmlDocument::ParseHtmlTagUnit(String *tag) {
  static const String TAG_START_MARK = "<";
  static const String END_TAG_START_MARK = "/";

  String avail = tag->SubString(TAG_START_MARK.length());
  int tag_unit_flags = 0;
  if (avail.StartsWith(END_TAG_START_MARK)) {
    tag_unit_flags |= END_TAG_FLAG;
    avail = avail.SubString(END_TAG_START_MARK.length());
  } else {
    tag_unit_flags |= START_TAG_FLAG;
  }
  std::size_t num_attributes_backup = attributes_.size();

  String tag_name_end;
  if (avail.StartsWith("<")) {
    static const CharTable TAG_NAME_DELIM_TABLE(" \t\r\n>/");
    tag_name_end = avail.FindFirstOf(TAG_NAME_DELIM_TABLE);
  } else {
    static const CharTable TAG_NAME_DELIM_TABLE(" \t\r\n<>/");
    tag_name_end = avail.FindFirstOf(TAG_NAME_DELIM_TABLE);
  }
  String tag_name(avail.begin(), tag_name_end.begin());

  avail.set_begin(tag_name_end.begin());
  avail = avail.StripLeft();
  while (!avail.is_empty()) {
    if (avail[0] == '>') {
      avail = avail.SubString(1);
      break;
    } else if (avail[0] == '<') {
      break;
    } else if (avail[0] == '/') {
      avail = avail.SubString(1);
      if (!avail.is_empty() && (avail[0] == '<' || avail[0] == '>')) {
        if ((tag_unit_flags & START_TAG_FLAG) == START_TAG_FLAG) {
          tag_unit_flags |= EMPTY_ELEMENT_TAG_FLAG;
        }
        if (avail[0] == '>') {
          avail = avail.SubString(1);
        }
        break;
      }
    }

    static const CharTable ATTRIBUTE_NAME_DELIM_TABLE(" \t\r\n=/<>");
    String attribute_name_end = avail.FindFirstOf(ATTRIBUTE_NAME_DELIM_TABLE);
    String attribute_name(avail.begin(), attribute_name_end.begin());

    avail = String(attribute_name_end.begin(), avail.end()).StripLeft();
    String attribute_value;
    if (avail.length() > 0 && avail[0] == '=') {
      avail = avail.SubString(1).StripLeft();
      if (avail.length() > 0 && (avail[0] == '\'' || avail[0] == '"')) {
        attribute_value = avail.SubString(1);
        String attribute_value_end = attribute_value.FindFirstOf(avail[0]);
        attribute_value.set_end(attribute_value_end.begin());
        avail.set_begin(attribute_value_end.end());
      } else {
        static const CharTable ATTRIBUTE_VALUE_DELIM_TABLE(" \t\r\n<>");
        String attribute_value_end =
            avail.FindFirstOf(ATTRIBUTE_VALUE_DELIM_TABLE);
        attribute_value = String(avail.begin(), attribute_value_end.begin());
        avail.set_begin(attribute_value_end.begin());
      }
    }

    HtmlDocumentAttribute attribute;
    attribute.set_name(attribute_name);
    attribute.set_value(attribute_value);
    attributes_.push_back(attribute);

    avail = avail.StripLeft();
  }
  tag->set_end(avail.begin());

  std::size_t num_attributes = attributes_.size() - num_attributes_backup;
  AppendTagUnit(*tag, tag_name, num_attributes, tag_unit_flags);
}

void HtmlDocument::ParseHtmlCommentUnit(String *tag) {
  static const String START_MARK = "<!--";
  static const String END_MARK = "-->";

  String avail = tag->SubString(START_MARK.length());
  String tag_end = avail.Find(END_MARK);
  String comment(avail.begin(), tag_end.begin());
  tag->set_end(tag_end.end());
  AppendCommentUnit(*tag, comment);
}

void HtmlDocument::ParseHtmlOtherUnit(String *tag) {
  // If a unit starts with "<![", it requires "]" and ">" in order.
  // Other units starting with "<!" or "<?" end with ">".
  static const String START_MARK = "<![";

  String avail = *tag;
  if (avail.StartsWith(START_MARK)) {
    avail = avail.SubString(START_MARK.length());
    String next_symbol = avail.FindFirstOf(']');
    avail.set_begin(next_symbol.end());
  } else {
    avail = avail.SubString(2);
  }

  String tag_end = avail.FindFirstOf('>');
  tag->set_end(tag_end.end());
  AppendOtherUnit(*tag, *tag);
}

void HtmlDocument::ParseHtmlSpecialTag(const String &body_left,
    const String &tag_name, String *tag) {
  for (String avail = body_left; !avail.is_empty(); ) {
    String start_mark = avail.Find("</").begin();
    if (start_mark.is_empty()) {
      break;
    }
    tag->Assign(start_mark.begin(), avail.end());
    avail = tag->SubString(2);
    if (avail.StartsWith(tag_name, ToLower())) {
      avail = avail.SubString(tag_name.length());
      if (avail.is_empty() || avail[0] == '>' || IsSpace()(avail[0])) {
        String text_content(body_left.begin(), tag->begin());
        AppendTextUnit(text_content, text_content, PLAIN_TEXT_FLAG);
        ParseHtmlTagUnit(tag);
        return;
      }
    }
  }
  AppendTextUnit(body_left, body_left, PLAIN_TEXT_FLAG);
}

void HtmlDocument::AppendTextUnit(const String &src,
    const String &text_content, int text_unit_flags) {
  if (src.is_empty()) {
    return;
  }

  HtmlDocumentUnit unit;
  unit.set_type(HtmlDocumentUnit::TEXT_UNIT);
  unit.set_src(src);
  unit.set_text_content(text_content);
  if ((text_unit_flags & (CDATA_SECTION_FLAG | PLAIN_TEXT_FLAG)) == 0) {
    temp_buf_.Clear();
    if (CharacterReference::Decode(text_content, &temp_buf_)) {
      unit.set_text_content(string_pool_.Append(temp_buf_.str()));
    }
  }
  if ((text_unit_flags & CDATA_SECTION_FLAG) == CDATA_SECTION_FLAG) {
    unit.set_cdata_section_flag();
  }
  units_.push_back(unit);
}

void HtmlDocument::AppendTagUnit(const String &src,
    const String &tag_name, std::size_t num_attributes, int tag_unit_flags) {
  static const HtmlDocumentAttribute DUMMY_ATTRIBUTE;

  HtmlDocumentUnit unit;
  unit.set_type(HtmlDocumentUnit::TAG_UNIT);
  unit.set_src(src);
  unit.set_tag_name(tag_name);
  unit.set_attributes(&DUMMY_ATTRIBUTE, num_attributes);
  if ((tag_unit_flags & START_TAG_FLAG) == START_TAG_FLAG) {
    unit.set_start_tag_flag();
  }
  if ((tag_unit_flags & END_TAG_FLAG) == END_TAG_FLAG) {
    unit.set_end_tag_flag();
  }
  if ((tag_unit_flags & EMPTY_ELEMENT_TAG_FLAG) == EMPTY_ELEMENT_TAG_FLAG) {
    unit.set_empty_element_tag_flag();
  }
  units_.push_back(unit);
}

void HtmlDocument::AppendCommentUnit(const String &src,
    const String &comment) {
  HtmlDocumentUnit unit;
  unit.set_type(HtmlDocumentUnit::COMMENT_UNIT);
  unit.set_src(src);
  unit.set_comment(comment);
  units_.push_back(unit);
}

void HtmlDocument::AppendOtherUnit(const String &src,
    const String &other_content) {
  HtmlDocumentUnit unit;
  unit.set_type(HtmlDocumentUnit::OTHER_UNIT);
  unit.set_src(src);
  unit.set_other_content(other_content);
  units_.push_back(unit);
}

void HtmlDocument::FixTagUnits() {
  std::size_t total_num_attributes = 0;
  for (std::size_t i = 0; i < num_units(); ++i) {
    if (units_[i].type() != HtmlDocumentUnit::TAG_UNIT) {
      continue;
    }
    temp_buf_.Assign(units_[i].tag_name(), ToLower());
    if (temp_buf_.str() != units_[i].tag_name()) {
      units_[i].set_tag_name(string_pool_.Append(temp_buf_.str()));
    }
    if (units_[i].num_attributes() > 0) {
      units_[i].set_attributes(&attributes_[total_num_attributes],
          units_[i].num_attributes());
      total_num_attributes += units_[i].num_attributes();
    }
  }
}

void HtmlDocument::FixAttributes() {
  for (std::size_t i = 0; i < attributes_.size(); ++i) {
    temp_buf_.Assign(attributes_[i].name(), ToLower());
    attributes_[i].set_name(string_pool_.Append(temp_buf_.str()));

    temp_buf_.Clear();
    if (CharacterReference::Decode(attributes_[i].value(), &temp_buf_)) {
      attributes_[i].set_value(string_pool_.Append(temp_buf_.str()));
    }
  }
}

void HtmlDocument::AppendEndOfLineToText(StringBuilder *text) {
  if (text->is_empty()) {
    return;
  } else if (IsSpace()((*text)[text->length() - 1])) {
    (*text)[text->length() - 1] = '\n';
  } else {
    text->Append('\n');
  }
}

void HtmlDocument::AppendToText(const String &str,
    EndOfLineHandler end_of_line_handler, StringBuilder *text) {
  for (std::size_t i = 0; i < str.length(); ++i) {
    if (IsSpace()(str[i])) {
      if (text->is_empty()) {
        continue;
      } else if ((end_of_line_handler == KEEP_END_OF_LINE) &&
          ((str[i] == '\r') || (str[i] == '\n'))) {
        AppendEndOfLineToText(text);
      } else if (!IsSpace()((*text)[text->length() - 1])) {
        text->Append(' ');
      }
    } else if (!IsCntrl()(str[i])) {
      text->Append(str[i]);
    }
  }
}

void HtmlDocument::UpdateTextExtractorModeFlags(
    const HtmlDocumentUnit &unit, int *mode_flags) {
  if (unit.is_empty_element_tag()) {
    if (unit.tag_name() == "plaintext") {
      *mode_flags |= PLAINTEXT_MODE_FLAG;
    }
  } else if (unit.is_start_tag()) {
    if (unit.tag_name() == "script") {
      *mode_flags |= SCRIPT_MODE_FLAG;
    } else if (unit.tag_name() == "style") {
      *mode_flags |= STYLE_MODE_FLAG;
    } else if (unit.tag_name() == "xmp") {
      *mode_flags |= XMP_MODE_FLAG;
    } else if (unit.tag_name() == "plaintext") {
      *mode_flags |= PLAINTEXT_MODE_FLAG;
    } else if (unit.tag_name() == "pre") {
      *mode_flags |= PRE_MODE_FLAG;
    } else if (unit.tag_name() == "listing") {
      *mode_flags |= LISTING_MODE_FLAG;
    }
  } else if (unit.is_end_tag()) {
    if (unit.tag_name() == "script") {
      *mode_flags &= ~SCRIPT_MODE_FLAG;
    } else if (unit.tag_name() == "style") {
      *mode_flags &= ~STYLE_MODE_FLAG;
    } else if (unit.tag_name() == "xmp") {
      *mode_flags &= ~XMP_MODE_FLAG;
    } else if (unit.tag_name() == "pre") {
      *mode_flags &= ~PRE_MODE_FLAG;
    } else if (unit.tag_name() == "listing") {
      *mode_flags &= ~LISTING_MODE_FLAG;
    }
  }
}

}  // namespace nwc_toolkit
