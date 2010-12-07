// Copyright 2010 Susumu Yata <syata@acm.org>

#include <nwc-toolkit/html-reducer.h>

#include <nwc-toolkit/character-reference.h>

namespace nwc_toolkit {

void HtmlReducer::Reduce(const HtmlDocument &src, StringBuilder *dest) {
  HtmlReducer reducer(dest);
  reducer.Reduce(src);
}

void HtmlReducer::Reduce(const HtmlDocument &src) {
  dest_->Clear();
  for (std::size_t i = 0; i < src.num_units(); ++i) {
    const nwc_toolkit::HtmlUnit &unit = src.unit(i);
    switch (unit.type()) {
      case nwc_toolkit::HtmlUnit::TEXT_UNIT: {
        ReduceText(unit);
        break;
      }
      case nwc_toolkit::HtmlUnit::TAG_UNIT: {
        ReduceTag(unit);
        break;
      }
      case nwc_toolkit::HtmlUnit::COMMENT_UNIT: {
        ReduceComment(unit);
        break;
      }
      case nwc_toolkit::HtmlUnit::OTHER_UNIT:
      default: {
        break;
      }
    }
  }
}

void HtmlReducer::ReduceText(const nwc_toolkit::HtmlUnit &unit) {
  if (is_code_) {
    return;
  }

  nwc_toolkit::String text_left = unit.text_content();
  int code_point;
  while (ExtractCodePoint(text_left, &text_left, &code_point)) {
    dest_->Append(ReduceCodePoint(code_point));
  }
}

void HtmlReducer::ReduceTag(const nwc_toolkit::HtmlUnit &unit) {
  dest_->Append('<');
  if (unit.is_end_tag()) {
    dest_->Append('/');
  }
  dest_->Append(unit.tag_name());
  for (std::size_t i = 0; i < unit.num_attributes(); ++i) {
    const nwc_toolkit::HtmlAttribute &attribute = unit.attribute(i);
    if ((attribute.name() == "class") || (attribute.name() == "id") ||
        (attribute.name() == "name")) {
      dest_->Append(' ').Append(attribute.name()).Append("=\"");
      nwc_toolkit::CharacterReference::Encode(attribute.value(), dest_,
          nwc_toolkit::CharacterReference::EncodeQuotes());
      dest_->Append('"');
    }
  }
  if (unit.is_empty_element_tag()) {
    dest_->Append(" /");
  }
  dest_->Append('>');

  if (is_code_) {
    is_code_ = false;
  } else if (unit.is_start_tag() && !unit.is_empty_element_tag() &&
      ((unit.tag_name() == "script") || (unit.tag_name() == "style"))) {
    is_code_ = true;
  }
}

void HtmlReducer::ReduceComment(const nwc_toolkit::HtmlUnit &unit) {
  nwc_toolkit::String comment = unit.comment().Strip();
  if ((comment == "google_ad_section_start") ||
      (comment == "google_ad_section_start(weight=ignore)") ||
      (comment == "google_ad_section_end")) {
    dest_->Append("<!-- ").Append(comment).Append(" -->");
  }
}

bool HtmlReducer::ExtractCodePoint(const String &str,
    String *str_left, int *code_point) {
  if (str.is_empty()) {
    return false;
  }
  *code_point = static_cast<unsigned char>(str[0]);
  std::size_t length = 1;
  if (*code_point >= 0x80) {
    while (*code_point & 0x40) {
      ++length;
      *code_point <<= 1;
    }
    *code_point = (*code_point & 0x3F) >> (length - 1);
    for (std::size_t i = 1; i < length; ++i) {
      *code_point <<= 6;
      *code_point |= static_cast<unsigned char>(str[i]) & 0x3F;
    }
  }
  *str_left = str.SubString(length);
  return true;
}

char HtmlReducer::ReduceCodePoint(int code_point) {
  switch (code_point) {
    case '\t':
    case ' ':
    case 0x00A0:
    case 0x202F:
    case 0x205F:
    case 0x3000:
    case 0xFEFF:
    case '\r': {
      return ' ';
    }
    case '\n': {
      return '\n';
    }
    case ',':
    case 0x3001:
    case 0xFF0C:
    case 0xFF64: {
      return ',';
    }
    case '.':
    case 0x3002:
    case 0xFF0E:
    case 0xFF61: {
      return '.';
    }
    case '!':
    case 0xFF01: {
      return '!';
    }
    case '?':
    case 0xFF1F: {
      return '?';
    }
  }
  if ((code_point >= 0x2000) && (code_point <= 0x200B)) {
    return ' ';
  } else if (((code_point >= '0') && (code_point <= '9')) ||
      ((code_point >= 0xFF10) && (code_point <= 0xFF19))) {
    return '0';
  } else if (((code_point >= 'A') && (code_point <= 'Z')) ||
      ((code_point >= 0xFF21) && (code_point <= 0xFF3A))) {
    return 'A';
  } else if (((code_point >= 'a') && (code_point <= 'z')) ||
      ((code_point >= 0xFF41) && (code_point <= 0xFF5A))) {
    return 'a';
  } else if ((code_point >= 0x3040) && (code_point <= 0x309F)) {
    return 'h';
  } else if (((code_point >= 0x30A0) && (code_point <= 0x30FF)) ||
      ((code_point >= 0x31F0) && (code_point <= 0x31FF)) ||
      ((code_point >= 0xFF66) && (code_point <= 0xFF9F))) {
    return 'k';
  } else if (((code_point >= 0x3400) && (code_point <= 0x4DBF)) ||
      ((code_point >= 0x4E00) && (code_point <= 0x9FFF)) ||
      ((code_point >= 0xF900) && (code_point <= 0xFAFF)) ||
      ((code_point >= 0x20000) && (code_point <= 0x2FA1F))) {
    return 'K';
  }
  return 'x';
}

}  // namespace nwc_toolkit
