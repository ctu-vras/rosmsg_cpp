#pragma once

#include <string>
#include <vector>

namespace ros
{
namespace message
{

/**
 * \brief Return the full recursive definition of the message type - the same as genpy generates.
 * \param messageType Type of the message.
 * \return The definition. If the definition could not be found, empty string is returned.
 */
std::string getFullDef(const std::string& messageType);

/**
 * \brief Return the MD5 sum of the message type - the same as genpy generates.
 * \param messageType Type of the message.
 * \return The MD5 sum. If the message could not be found, empty string is returned.
 */
std::string getMD5Sum(const std::string& messageType);

/**
 * \brief Tell whether the given message type has a header field.
 * \param messageType Type of the message.
 * \return Whether the given message type has a header field.
 */
bool hasHeader(const std::string& messageType);

/**
 * \brief Return the names of fields of the message type (non-recursive).
 * \param messageType Type of the message.
 * \return The names of the fields.
 */
std::vector<std::string> getFieldNames(const std::string& messageType);

/**
 * \brief Return the types of fields of the message type (non-recursive).
 * \param messageType Type of the message.
 * \return The types of the fields.
 */
std::vector<std::string> getFieldTypes(const std::string& messageType);

/**
 * \brief Get the path to the .msg file corresponding to the given message type.
 * \param messageType Type of the message.
 * \return Path to the .msg file corresponding to the given message type.
 */
std::string getMsgFile(const std::string& messageType);

/**
 * \brief Return the exact text from which the MD5 sum of the message is computed.
 * \param messageType Type of the message.
 * \return The MD5 sum source text.
 * \note This function might be slow (a few seconds at worst).
 */
std::string getMD5Text(const std::string& messageType);

}
}