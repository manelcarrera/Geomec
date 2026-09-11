#pragma once

#include <QString>
#include <vector>
#include <string>

//FIXME: Code below is duplicated; taken from 'GlobalTetMesherMessage'
class CMesherDlgHelper
{

  static const size_t DIMENSIONS = 3;
  const size_t NORTHING = 0;
  const size_t EASTING = 1;
  const size_t DEPTH = 2;

  const QString COORDINATE_PREFIX = "( ";
  const QString COORDINATE_INFIX = ", ";
  const QString COORDINATE_POSTFIX = " )";

  const QString REGULAR_EXPRESSION_PREFIX = "\\( ?";
  const QString REGULAR_EXPRESSION_INFIX = ", ";
  const QString REGULAR_EXPRESSION_POSTFIX = " ?\\)";
  const QString REGULAR_EXPRESSION_DOUBLE = "-?[0-9]+[,.][0-9]+";

  const QString REGULAR_EXPRESSION_COORDINATE =	REGULAR_EXPRESSION_PREFIX +
                        
                          REGULAR_EXPRESSION_DOUBLE + REGULAR_EXPRESSION_INFIX +
                          REGULAR_EXPRESSION_DOUBLE + REGULAR_EXPRESSION_INFIX +
                          REGULAR_EXPRESSION_DOUBLE + 
        
                          REGULAR_EXPRESSION_POSTFIX;

public:

  bool appendMetresClause(	std::string& newMessage,
                const std::vector <std::string>& message)
  {
    QRegExp matchCoordinate(REGULAR_EXPRESSION_DOUBLE);
    bool coordinateMatched = false;

    for (std::vector <std::string>::const_iterator line = message.begin();
    !coordinateMatched && (line != message.end()); ++line)
    {
    coordinateMatched =
      coordinateMatched || (matchCoordinate.indexIn((*line).c_str()) != -1);
    }

    if (coordinateMatched)
    {
    newMessage += "\n\n";
    newMessage += "All coordinates are expressed in metres";
    }

    return coordinateMatched;
  }

  void save( const std::string& pathName, std::vector< std::string > v )
  {
    std::ofstream stream( pathName.c_str() );
    assert(stream.is_open());
    for( auto line: v )
      stream << line << std::endl;
    stream.close();
  }

  void save( const QString& pathName, std::vector< QString >& v )
  {
    std::ofstream stream(pathName.toStdString().c_str());
    assert(stream.is_open());
    for( auto line: v )
      stream << line.toStdString() << std::endl;
    stream.close();
  }

  void reformat_coordinates( std::vector< QString >& coordinates )
  {
    // waij TFS 73914
    for ( size_t ii= 0; ii< coordinates.size(); ++ii)
    {
      coordinates[ii].replace( COORDINATE_PREFIX,"");
      coordinates[ii].replace( COORDINATE_INFIX," ");
      coordinates[ii].replace( COORDINATE_POSTFIX,"");
    }
  }

  std::vector <QString> reorientateCoordinatesNED2END(std::string& message)
  {
    QRegExp matchCoordinate(REGULAR_EXPRESSION_COORDINATE);
    std::vector <QString> newCoordinates;
    int coordinateOffset = 0;

    while ((coordinateOffset =
    matchCoordinate.indexIn(message.c_str(), coordinateOffset)) != -1)
    {
    QString coordinate =
      message.substr(coordinateOffset, matchCoordinate.matchedLength()).c_str();
    QRegExp matchDouble(REGULAR_EXPRESSION_DOUBLE);
    QString NED[DIMENSIONS];
    size_t dimension = NORTHING;
    int doubleOffset = 0;

    while ((dimension != DIMENSIONS) &&
      ((doubleOffset = matchDouble.indexIn(coordinate, doubleOffset)) != -1))
    {
      NED[dimension++] =
      coordinate.mid(doubleOffset, matchDouble.matchedLength());
      doubleOffset += matchDouble.matchedLength();
    }

    assert(dimension == DIMENSIONS);

    newCoordinates.push_back(COORDINATE_PREFIX + NED[EASTING] +
      COORDINATE_INFIX + NED[NORTHING] + COORDINATE_INFIX + NED[DEPTH] +
      COORDINATE_POSTFIX);

    assert(newCoordinates.back().length() == matchCoordinate.matchedLength());

    message.replace(coordinateOffset, matchCoordinate.matchedLength(),
      newCoordinates.back().toStdString());
    coordinateOffset += matchCoordinate.matchedLength();
    }

    return newCoordinates;
  }

  std::string removeCoordinates(const std::vector <std::string>& message)
  {
    std::stringstream stream;
    QRegExp matchCoordinate(REGULAR_EXPRESSION_COORDINATE);

    for (std::vector <std::string>::const_iterator it = message.begin(); it != message.end(); ++it)
    {
    if (matchCoordinate.indexIn(it->c_str()) == -1)
      stream << *it << std::endl;
    }
    return stream.str();
  }
};