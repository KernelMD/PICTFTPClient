/* 
 * File:   tftp_client.h
 * Author: Titov Denis <kernelmd@yandex.ru>
 * 
 * Created on 31.07.2016, 17:00
 * 
 * This file is part of PicTFTPClient.
 * 
 * PicTFTPClient is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * PicTFTPClient is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with PicTFTPClient.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef TFTP_CLIENT_H
#define	TFTP_CLIENT_H

#include <stdint.h>
#include <stddef.h>
#include "tftp_config.h"

enum TFTPOp {
    OP_NONE = 0,
    OP_READ,
    OP_WRITE
};

enum TFTPState {
    S_EUNKNOWN = 0,
    S_ENOFILE = 1,
    S_EACCESS = 2,
    S_ENOSPACE = 3,
    S_EWRONGOP = 4,
    S_EWRONGID = 5,
    S_EFILEEXISTS = 6,
    S_ENOUSER = 7,
    S_EWRONGOPTION = 8,
    S_NOTCONNECTED = 125,
    S_GOT = 126,
    S_ACK = 127  
};

typedef struct TFTPConn {
    /* Operation. By default -- OP_NONE */
    enum TFTPOp oper;
    /* URI of the file. As there is only one other host, it
     * becomes path to the file.*/
    char uri[MAX_URI_LEN];

    /* Current connection state. */
    enum TFTPState state;
    /* Timeout for the packages. */
    uint8_t timeout;

    /* Size of the file. */
    uint16_t tsize;
    /* Number of current block. */
    uint16_t seqnum;
    /* Buffer for block contents. */
    uint8_t block[BLK_SIZE];
    size_t blk_used;
} TFTPConn;

/* Initializes structure with default data. */
void tftpInit(TFTPConn* conn);

int tftpRead(TFTPConn* conn, uint8_t* dst, size_t len);
int tftpWrite(TFTPConn* conn, const uint8_t* src, size_t len);
/* Interrupts current transmission. */
void tftpReset(TFTPConn* conn);

int tftpRecieveData(TFTPConn* conn, const uint8_t* buf, size_t len);
int tftpTransmitData(TFTPConn* conn, uint8_t* buf, size_t len);

#endif	/* TFTP_CLIENT_H */

