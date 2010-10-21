#! /usr/bin/python
# -*- coding: utf-8 -*-

import cgi
import cgitb
import re
import sys

cgitb.enable()

USER_AGENT = """Mozilla/5.0 (compatible; nwc-toolkit; http://code.google.com/p/nwc-toolkit/)"""

AVAILABLE_SCHEMES = frozenset(["http", "https", "ftp"])

RESPONSE_HEADER = """Content-Type: text/html; charset=utf-8\n\n"""

HTML_HEADER = """<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01//EN" "http://www.w3.org/TR/html4/strict.dtd">
<html lang="ja">
 <head>
  <meta http-equiv="Content-Type" content="text/html; charset=utf-8">
  <title>HTML テキスト抽出</title>
  <style type="text/css">
   <!--
    * {
     padding: 0px;
     margin: 0px;
    }
    a {
     color: inherit;
     text-decoration: none;
    }
    body {
     background: #FFF;
     color: #000;
     margin: 10px;
     padding: 10px;
    }
    table#header {
     background: #666;
     border-bottom: 2px solid #444;
     border-left: 2px solid #888;
     border-right: 2px solid #444;
     border-top: 2px solid #888;
     border-radius: 10px;
     -webkit-border-radius: 10px;
     -moz-border-radius: 10px;
     padding: 5px;
    }
    table#header a#logo {
     background: #DDD;
     border-bottom: 2px solid #AAA;
     border-left: 2px solid #444;
     border-right: 2px solid #AAA;
     border-top: 2px solid #444;
     border-radius: 5px;
     -webkit-border-radius: 5px;
     -moz-border-radius: 5px;
     color: #444;
     display: block;
     font-family: "Times New Roman";
     font-size: 200%;
     font-weight: bold;
     margin-left: 5px;
     margin-right: 20px;
     padding: 5px 10px;
     vertical-align: middle;
    }
    table#header td#title {
     border-bottom: 1px dashed #888;
     color: #FFF;
     font-size: 140%;
     margin-left: 0px;
     margin-right: 20px;
    }
    table#header td#subtitle a {
     color: #FFF;
     margin-left: 20px;
     margin-right: 20px;
    }
    ul#nav {
     color: #444;
     margin-top: 20px;
     padding: 0px;
     width: 100%;
    }
    ul#nav li {
     float: left;
     list-style: none;
     margin-left: 0px;
     margin-right: 10px;
     margin-bottom: 5px;
    }
    ul#nav li a {
     border-radius: 10px;
     -webkit-border-radius: 10px;
     -moz-border-radius: 10px;
     box-shadow: 0 1px 2px rgba(0,0,0,.5);
     -webkit-box-shadow: 0 1px 2px rgba(0,0,0,.5);
     -moz-box-shadow: 0 1px 2px rgba(0,0,0,.5);
     display: block;
     padding: 5px 15px;
    }
    ul#nav li.on a {
     background: #EEE;
    }
    ul#nav li.off a {
     background: #DDD;
    }
    ul#nav li a:hover {
     background: #EEE;
    }
    ul#nav li span {
     color: #222;
    }
    div#nav_end {
     clear: both;
    }
    div#body {
     background: #DDD;
     border-radius: 10px;
     -webkit-border-radius: 10px;
     -moz-border-radius: 10px;
     box-shadow: 0 1px 2px rgba(0,0,0,.5);
     -webkit-box-shadow: 0 1px 2px rgba(0,0,0,.5);
     -moz-box-shadow: 0 1px 2px rgba(0,0,0,.5);
     color: #444;
     margin: 0px;
     padding: 15px;
    }
    div#body hr {
     background: #888;
     border-color: #FFF;
     color: #888;
     margin-top: 10px;
     margin-bottom: 20px;
    }
    div#body input#url {
     padding: 2px 5px;
     width: 90%;
    }
    div#body input#file {
     background: #EEE;
     padding: 2px 5px;
    }
    div#body textarea#html {
     height: 50%;
     padding: 2px 5px;
     width: 90%;
    }
    div#body input#submit {
     background: #666;
     border: 0px;
     border-radius: 20px;
     -webkit-border-radius: 20px;
     -moz-border-radius: 20px;
     box-shadow: 0 1px 2px rgba(0,0,0,.2);
     -webkit-box-shadow: 0 1px 2px rgba(0,0,0,.2);
     -moz-box-shadow: 0 1px 2px rgba(0,0,0,.2);
     color: #FFF;
     padding: 5px 15px;
    }
    div#body input#submit:hover {
     background: #444;
    }
    div#body p.center {
     text-align: center;
     margin: 0px;
     margin-top: 20px;
     padding: 0px;
    }
    div#body table#text {
     border-collapse: separate;
     border-spacing: 5px 2px;
    }
    div#body table#text th {
     background: #EEE;
     border-radius: 5px;
     -webkit-border-radius: 5px;
     -moz-border-radius: 5px;
     line-height: 120%;
     margin-right: 5px;
     padding: 2px 5px;
     text-align: right;
     vertical-align: top;
    }
    div#body table#text td {
     background: #FFF;
     border-radius: 5px;
     -webkit-border-radius: 5px;
     -moz-border-radius: 5px;
     line-height: 120%;
     margin-left: 5px;
     padding: 2px 5px;
    }
   -->
  </style>
 </head>
 <body>
  <table id="header">
   <tr>
    <th rowspan="2"><a href="http://code.google.com/p/nwc-toolkit/" id="logo">NWC</a></th>
    <td id="title">HTML テキスト抽出</td>
   </tr>
   <tr>
    <td id="subtitle"><a href="http://code.google.com/p/nwc-toolkit/">─ Nihongo Web Corpus (NWC) Toolkit ─</a></td>
   </tr>
  </table>
"""

HTML_FOOTER = """ </body>
</html>
"""

NAVIGATOR = """   <ul id="nav">
    <li class="%s"><a href="?form=url"><span>URL</span> を入力</a></li>
    <li class="%s"><a href="?form=file"><span>ファイル</span>を入力</a></li>
    <li class="%s"><a href="?form=html"><span>HTML</span> を入力</a></li>
   </ul>
   <div id="nav_end"></div>
"""

URL_FORM = """  <div id="body">
   <form id="extract" method="get" action="">
    入力された URL から HTML をダウンロードしてテキストを抽出します．
    <hr>
    <p class="center">
     <input type="text" id="url" name="url" size="90%" />
    </p>
    <p class="center">
     <input type="submit" id="submit" value="テキスト抽出" />
    </p>
    </div>
   </form>
  </div>
"""

FILE_FORM = """  <div id="body">
   <form id="extract" method="post" action="" enctype="multipart/form-data">
    入力された HTML ファイルからテキストを抽出します．
    <hr>
    <p class="center">
     <input type="file" id="file" name="file" size="40" />
    </p>
    <p class="center">
     <input type="submit" id="submit" value="テキスト抽出" />
    </p>
    </div>
   </form>
  </div>
"""

HTML_FORM = """  <div id="body">
   <form id="extract" method="post" action="" enctype="multipart/form-data">
    入力された HTML からテキストを抽出します．
    <hr>
    <p class="center">
     <textarea id="html" name="html" rows="12"></textarea>
    </p>
    <p class="center">
     <input type="submit" id="submit" value="テキスト抽出" />
    </p>
    </div>
   </form>
  </div>
"""

ERROR_BODY = """  <div id="body">
   テキスト抽出に失敗しました．
   <hr>
   <p>
    %s
   </p>
  </div>
"""

TEXT_BODY = """  <div id=\"body\">
   テキスト抽出に成功しました．
   <hr>
   <table id=\"text\">
%s   </table>
  </div>
"""

def PrintResponseHeader():
  sys.stdout.write(RESPONSE_HEADER)

def PrintHtmlHeader():
  sys.stdout.write(HTML_HEADER)

def PrintHtmlFooter():
  sys.stdout.write(HTML_FOOTER)

def PrintForm(form_value):
  if form_value == "url":
    sys.stdout.write(NAVIGATOR % ("on", "off", "off"))
    sys.stdout.write(URL_FORM)
  elif form_value == "file":
    sys.stdout.write(NAVIGATOR % ("off", "on", "off"))
    sys.stdout.write(FILE_FORM)
  elif form_value == "html":
    sys.stdout.write(NAVIGATOR % ("off", "off", "on"))
    sys.stdout.write(HTML_FORM)
  else:
    sys.stdout.write(NAVIGATOR % ("on", "off", "off"))
    sys.stdout.write(URL_FORM)

def PrintError(error_message):
  sys.stdout.write(ERROR_BODY % (cgi.escape(error_message)))

def PrintText(text):
  import StringIO
  string_io = StringIO.StringIO()
  lines = text.splitlines()
  line_id = 0
  for line in lines:
    if not line:
      continue
    line_id = line_id + 1
    string_io.write("    <tr>\n")
    string_io.write("     <th>%s</th>\n" % str(line_id))
    string_io.write("     <td>%s</td>\n" % cgi.escape(line))
    string_io.write("    </tr>\n")
  sys.stdout.write(TEXT_BODY % (string_io.getvalue()))
  string_io.close()

def ExtractTextFromArchive(archive):
  import subprocess
  sub_process = subprocess.Popen(
      "nwc-toolkit-text-extractor --archive -n 1 --NFKC --filter",
      shell = True, stdin = subprocess.PIPE, stdout = subprocess.PIPE)
  sub_process.stdin.write(archive)
  sub_process.stdin.close()
  text = sub_process.stdout.read()
  sub_process.stdout.close()
  return_code = sub_process.wait()
  if return_code != 0:
    return None
  else:
    return text

def ExtractTextFromDocument(document):
  import subprocess
  sub_process = subprocess.Popen(
      "nwc-toolkit-text-extractor --single --NFKC --filter",
      shell = True, stdin = subprocess.PIPE, stdout = subprocess.PIPE)
  sub_process.stdin.write(document)
  sub_process.stdin.close()
  text = sub_process.stdout.read()
  sub_process.stdout.close()
  return_code = sub_process.wait()
  if return_code != 0:
    return None
  else:
    return text

def ExtractTextFromUrl(url_value):
  sys.stdout.write(NAVIGATOR % ("on", "off", "off"))

  import urlparse
  url_parts = urlparse.urlparse(url_value)
  url_scheme = url_parts.scheme.lower()
  if url_scheme not in AVAILABLE_SCHEMES:
    PrintError("指定された scheme（\"%s\"）には対応していません．" % (
        url_scheme))
    return

  import urllib
  class SimpleUrlOpener(urllib.FancyURLopener):
    version = USER_AGENT
    def __init__(self):
      urllib.FancyURLopener.__init__(self)
    def prompt_user_passwd(self, host, realm):
      return (None, None)
  url_opener = SimpleUrlOpener()
  try:
    response_handle = url_opener.open(url_value)
  except:
    PrintError("指定された URL（\"%s\"）からのダウンロードに失敗しました．" %(
        url_value))
    return
  response_url = response_handle.geturl()
  response_code = response_handle.getcode()
  response_header = str(response_handle.info())
  response_body = response_handle.read()
  archive = "%s\n%d\n%d\n%s%d\n%s" % (response_url, response_code,
      len(response_header), response_header, len(response_body), response_body)

  text = ExtractTextFromArchive(archive)
  if text == None:
    PrintError("HTML の解析に失敗しました．")
  else:
    PrintText(text)

def ExtractTextFromFile(file_value):
  sys.stdout.write(NAVIGATOR % ("off", "on", "off"))
  text = ExtractTextFromDocument(file_value)
  if text == None:
    PrintError("HTML の解析に失敗しました．")
  else:
    PrintText(text)

def ExtractTextFromHtml(html_value):
  sys.stdout.write(NAVIGATOR % ("off", "off", "on"))
  text = ExtractTextFromDocument(html_value)
  if text == None:
    PrintError("HTML の解析に失敗しました．")
  else:
    PrintText(text)

def main(argv):
  PrintResponseHeader()
  PrintHtmlHeader()
  field_storage = cgi.FieldStorage()
  if field_storage.has_key("url"):
    url_value = field_storage.getfirst("url")
    ExtractTextFromUrl(url_value)
  elif field_storage.has_key("file"):
    file_value = field_storage.getfirst("file")
    ExtractTextFromFile(file_value)
  elif field_storage.has_key("html"):
    html_value = field_storage.getfirst("html")
    ExtractTextFromHtml(html_value)
  else:
    form_value = field_storage.getvalue("form", "")
    PrintForm(form_value)
  PrintHtmlFooter()

if __name__ == "__main__":
	main(sys.argv)
