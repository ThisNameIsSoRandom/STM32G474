#ifndef LOGGING_H
#define LOGGING_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize logging system with SEGGER RTT and display startup banner
 */
void initLogging(void);

#ifdef __cplusplus
}
#endif

#endif /* LOGGING_H */