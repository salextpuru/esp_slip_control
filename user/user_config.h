#ifndef USER_CONFIG_H
#define USER_CONFIG_H

typedef enum {
	SIG_DO_NOTHING=0,
	/** Срока пришла в телнета буфер кольцевой */
	SIG_TELNET_RX=2,
	/** Срока уйти должна из буфера-кольца телнета */
	SIG_TELNET_TX } userSignals;

#endif /* USER_CONFIG_H */
