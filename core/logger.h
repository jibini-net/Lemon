#pragma once

#include <iostream>
#include <mutex>
#include <string>

////////////////////////////////////////////////////////////////////////////////
//                          Lemon 3D Graphics Engine                          //
//                    COPYRIGHT (c) 2021 by ZACH GOETHEL                      //
//  ------------------------------------------------------------------------  //
//  Use freely.  This code is published under the MIT permissive open-source  //
//  license.  For more information, see the license file included with this   //
//  repository.  Good luck, and enjoy!                                        //
//  ------------------------------------------------------------------------  //
////////////////////////////////////////////////////////////////////////////////

namespace lemon
{
	extern std::mutex* global_log_mutex;

	/**
	 * A rudimentary logger which provides various levels of severity for logged
	 * messages. This class provides a partial and very rudimentary implementation
	 * of a logger message pattern similar to that of Apache's Log4j.
	 * 
	 * Consult the default logger pattern in the implementation file for an example
	 * log message pattern.
	 * 
	 * @brief A simple logger for logging info and error messages.
	 * @author Zach Goethel
	 */
	class logger
	{
	private:
		/**
		 * @brief The name of this logger to help identify it in log messages.
		 */
		std::string name;

		/**
		 * @brief Pattern for formatting messages logged by this instance.
		 */
		std::string pattern;

	public:
		/**
		 * @brief Constructs a new named logger. Name the logger after the
		 * 		subsystem for which it logs to help trace messages.
		 * @param pattern Pattern for formatting messages logged by this instance.
		 * @param name The name of this logger to help identify it in log messages.
		 */
		logger(std::string pattern, std::string name);

		/**
		 * @brief Constructs a new named logger. Name the logger after the
		 * 		subsystem for which it logs to help trace messages.
		 * @param name The name of this logger to help identify it in log messages.
		 */
		logger(std::string name);

		/**
		 * @brief Logs a colored message to the terminal with severity.
		 * @param level The severity level of this log message.
		 * @param level_color Formatting characters for the console output.
		 * @param message Log message contents.
		 */
		void log(std::string level, std::string level_color, std::string message);

		/**
		 * @brief Logs a debug level message.
		 * @param message Message contents.
		 */
		void debug(std::string message);

		/**
		 * @brief Logs an info level message.
		 * @param message Message contents.
		 */
		void info(std::string message);

		/**
		 * @brief Logs a warning level message.
		 * @param message Message contents.
		 */
		void warn(std::string message);

		/**
		 * @brief Logs an error level message.
		 * @param message Message contents.
		 */
		void error(std::string message);
	};
}
