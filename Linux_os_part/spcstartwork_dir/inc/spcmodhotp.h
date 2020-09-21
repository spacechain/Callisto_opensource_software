#ifndef SPCMODHOTP_H
#define SPCMODHOTP_H

#define SECRETLEN	16

int hotp_getNewSecret(char *dest, uint32_t len, uint32_t seed);

int hotp_genHotp(char *secret, uint32_t counter, char *dest, uint32_t len);

int hotp_genNHotp(char *secret, uint32_t counter, uint32_t times, char *dest, uint32_t len);

#endif
