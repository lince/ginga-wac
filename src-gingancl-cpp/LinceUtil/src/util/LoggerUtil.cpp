#include "utilconfig.h"

#ifdef HAVE_LOG4CXX
#include <log4cxx/logger.h>
#include <log4cxx/basicconfigurator.h>
#include <log4cxx/propertyconfigurator.h>
#include <log4cxx/consoleappender.h>
#include <log4cxx/patternlayout.h>
#include <log4cxx/file.h>
using namespace log4cxx;
#else
#include <time.h>
#include <iostream>
using namespace std;
#endif
#include "util/LoggerUtil.h"

namespace br {
    namespace ufscar {
        namespace lince {
                namespace util {

                        void LoggerUtil::configure() {
#ifdef HAVE_LOG4CXX
                            LoggerPtr rootLogger = Logger::getRootLogger();
                            
                            rootLogger->removeAllAppenders();
                            
                            rootLogger->addAppender(new ConsoleAppender(new PatternLayout("%d [%t] %-5p %c - %m%n")));
#endif
                        }

                        void LoggerUtil::configure(const char* xmlFilePath) {
#ifdef HAVE_LOG4CXX
                            PropertyConfigurator::configure(File(xmlFilePath));
#endif
                        }

                        HLoggerPtr LoggerUtil::getLogger(const char* id) {
#ifdef HAVE_LOG4CXX
                            return LoggerPtr(Logger::getLogger(id));
#else
                            return (HLoggerPtr)id;
#endif
                        }

                        void LoggerUtil::info(HLoggerPtr logger, const char* msg){
#ifdef HAVE_LOG4CXX
                            LOG4CXX_INFO(static_cast<Logger*>(logger), msg);
#else
                            stdLogOut(logger, msg);
#endif
                        }

                        void LoggerUtil::debug(HLoggerPtr logger, const char* msg){
#ifdef HAVE_LOG4CXX
                            LOG4CXX_DEBUG(static_cast<Logger*>(logger), msg);
#else
                            stdLogOut(logger, msg);
#endif
                        }

                        void LoggerUtil::warning(HLoggerPtr logger, const char* msg){
#ifdef HAVE_LOG4CXX
                            LOG4CXX_WARN(static_cast<Logger*>(logger), msg);
#else
                            stdLogOut(logger, msg);
#endif
                        }

                        void LoggerUtil::error(HLoggerPtr logger, const char* msg){
#ifdef HAVE_LOG4CXX
                            LOG4CXX_ERROR(static_cast<Logger*>(logger), msg);
#else
                            stdLogOut(logger, msg);
#endif
                        }

                        void LoggerUtil::stdLogOut(HLoggerPtr logger, const char* msg){
#ifndef HAVE_LOG4CXX
                            time_t t = time(NULL);
                            char timeNow[128];
                            strftime(timeNow, sizeof(timeNow), "%a, %d %b %Y %H:%M:%S GMT", gmtime(&t));
                            cout << timeNow << "\t" << ((const char*) logger) << " - " << msg << endl;
#endif
                        }

            }
        }
    }
}
