#include "Xml.h"
#include "Printer.h"

#include <QFile>
#include <QString>
#include <QXmlStreamReader>

// modify XML
#include <QDomElement>
#include <QTextStream>

const std::string CXml::NOT_FOUND = "NOT_FOUND";

void CXml::toUpper(std::string &str) { std::transform(str.begin(), str.end(), str.begin(), ::toupper); }

void CXml::toUpper(TKeysV &str_v) {
  for (int i = 0; i < str_v.size(); i++)
    toUpper(str_v[i]);
}

CXml::CXml(std::string file, TKeysV keys, TKeysV optional_keys) : m_file(file) {
  toUpper(keys);
  toUpper(optional_keys);

  m_keys_v = keys;
  m_optional_keys_v = optional_keys;

  parse(false);
  replace_wild_cards();
}

std::string CXml::value(std::string key) {
  toUpper(key);
  return exist(key) ? m_map[key] : NOT_FOUND;
}

bool CXml::is(std::string key, std::string value, eKeySensitive key_sensitive) {
  if (exist(key)) {
    toUpper(key);
    std::string val = m_map[key];
    if (key_sensitive == Yes) {
      toUpper(val);
      toUpper(value);
    }
    return val == value;
  } else {
    return false;
  }
}

bool CXml::in(std::string key, TValuesV values_v, eKeySensitive key_sensitive) {
  for (int i = 0; i < values_v.size(); i++) {
    if (is(key, values_v[i], key_sensitive))
      return true;
  }
  return false;
}

bool CXml::exist(std::string key) {
  toUpper(key);
  return m_map.find(key) != m_map.end();
}

bool CXml::set(std::string key, std::string value) {
  toUpper(key);
  if (exist(key)) {
    m_map[key] = value;
    return true;
  }
  return false;
}

bool CXml::is_key(std::string key) {
  toUpper(key);
  bool is_key_ = std::find(m_keys_v.begin(), m_keys_v.end(), key) != m_keys_v.end();
  bool is_optional_key_ = std::find(m_optional_keys_v.begin(), m_optional_keys_v.end(), key) != m_optional_keys_v.end();

  return is_key_ || is_optional_key_;
}

bool CXml::verify() {
  for (TKeysV::const_iterator it = m_keys_v.begin(); it != m_keys_v.end(); it++)
    if (!exist(*it)) {
      Printer::instance()->error("cfg file verify: '%s'", m_file.c_str());
      return false;
    }

  return true;
}

std::string CXml::replace(const std::string k, const std::string v, const std::string value) {
  QString value_qs = QString::fromStdString(value);
  QString format = QString::asprintf("%%%s%%", k);
  value_qs.replace(format, QString::fromStdString(v));
  return value_qs.toStdString();
}

void CXml::replace_wild_cards() // FIXME: do it static public when someone need it
{
  int nK = (int)m_keys_v.size();
  int nV = (int)m_map.size();

  if (nK == nV) {
    for (int i = 0; i < nK; i++) {
      std::string k = m_keys_v[i];
      std::string v = value(k);

      if (v == NOT_FOUND) {
      } else {
        for (int j = 0; j < nK; j++) {
          std::string key = m_keys_v[j];
          if (key != k)
            m_map[key] = replace(k, v, m_map[key]);
        }
      }
    }
  }
}

void CXml::update(std::string file, std::string node, std::string value) {
  QFile xmlFile(QString::fromStdString(file));
  xmlFile.open(QIODevice::ReadWrite);

  QByteArray xmlData(xmlFile.readAll());

  QDomDocument doc;
  doc.setContent(xmlData);

  QDomNodeList nodes = doc.elementsByTagName(QString::fromStdString(node)); // Find elements with tag name "firstchild"

  if (!nodes.isEmpty()) {
    QDomElement el = nodes.at(0).toElement();
    if (!el.isNull()) {
      QDomNode n = el.firstChild();
      QDomText t = n.toText();
      if (!t.isNull()) {
        t.setData(QString::fromStdString(value));
      } else {
        QDomElement newNodeTag = doc.createElement(QString::fromStdString(node));
        QDomText newNodeText = doc.createTextNode(QString::fromStdString(value));
        newNodeTag.appendChild(newNodeText);

        el.parentNode().replaceChild(newNodeTag, el);
      }
    }
  }

  // Write changes to same file
  xmlFile.resize(0);
  QTextStream stream;
  stream.setDevice(&xmlFile);
  doc.save(stream, 4);

  xmlFile.close();
}

bool CXml::parse(bool allow_debug) {
  QFile xmlFile(QString::fromStdString(m_file));

  if (!xmlFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
    if (allow_debug)
      Printer::instance()->debug("Load XML File Problem - Couldn't open xmlfile.xml to load settings for download");
    return false;
  } else {
    QXmlStreamReader xmlReader(&xmlFile);

    while (!xmlReader.atEnd() && !xmlReader.hasError()) // Parse the XML until we reach end of it
    {
      QXmlStreamReader::TokenType token = xmlReader.readNext(); // Read next element

      switch (token) {
      case QXmlStreamReader::StartDocument:
        break;
      case QXmlStreamReader::StartElement: {
        std::string key = xmlReader.name().toString().toUpper().toStdString(); // FIXME: cumulative content

        // Printer::instance()->debug( "id: %s", key.c_str() );
        if (is_key(key)) {
          //
          // attributes are empty after calling 'readElementText()'
          // so get first the attributes and call then 'readElementText()'
          //
          QXmlStreamAttributes attr_v = xmlReader.attributes();
          QString value = xmlReader.readElementText();
          if (value.isEmpty()) {
            if (attr_v.size())
              value = attr_v[0].value().toString();
          }

          // IT_Printer::instance()->debug( "content: %s", value.c_str() );

          m_map[key] = value.toStdString();
        } else {
          // IT_Printer::instance()->debug( "content: NO" );
        }
        break;
      }
      }
    }

    if (xmlReader.hasError()) {
      if (allow_debug)
        Printer::instance()->debug("xmlFile.xml Parse Error : %s", xmlReader.errorString().toStdString().c_str());
      return false;
    }

    // close reader and flush file
    xmlReader.clear();
    xmlFile.close();
  }
  return true;
}

std::string CXml::get(std::string file, std::string key_) {
  QFile xmlFile(QString::fromStdString(file));

  std::string value = NOT_FOUND;

  if (!xmlFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
    return value;
  } else {
    QXmlStreamReader xmlReader(&xmlFile);

    while (!xmlReader.atEnd() && !xmlReader.hasError()) // Parse the XML until we reach end of it
    {
      QXmlStreamReader::TokenType token = xmlReader.readNext(); // Read next element
      if (token == QXmlStreamReader::StartElement) {
        std::string key = xmlReader.name().toString().toStdString();

        toUpper(key);
        toUpper(key_);

        if (key == key_) {
          value = xmlReader.readElementText().toStdString();
          break;
        }
      }
    }
    xmlReader.clear();
    xmlFile.close();
  }
  return value;
}